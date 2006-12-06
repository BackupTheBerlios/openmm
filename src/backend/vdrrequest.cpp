/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "vdrrequest.h"
#include "tvtimer.h"
#include "tvprogram.h"
#include "tvchannel.h"

#include <qdir.h>
#include <qregexp.h>


VdrRequest::VdrRequest(ListManager *listManager, List *list, QString request)
    : QThread()
{
    m_listManager = listManager;
    m_list = list;
    m_request = request;
    m_server = "192.168.178.10";  // TODO: set the following items with Setup class.
    m_svdrpPort = 2001;
    m_httpPort = 3000;
    m_videoDir = "/video";

    m_socket = new QSocketDevice;
    m_socket->setBlocking(true);

    QHostAddress server;
    server.setAddress(m_server);
}


VdrRequest::~VdrRequest()
{
    delete m_socket;
}


void
VdrRequest::startRequest()
{
    run();
}


void
VdrRequest::run()
{
    // TODO: does this reliably synchronize access to the VDR socket in order of request call in the main thread?
    QMutexLocker requestLocker(&m_requestMutex);
    const int MaxLen = 2048; // TODO: make this variable length (Epg description may be longer ...?)
    char line[MaxLen];

    if (!m_socket->connect(m_server, m_svdrpPort))
        return;

    m_socket->readLine(line, MaxLen);
    //qDebug("Connect Reply: %s", line);

    writeToSocket(m_request + "\r\n");

    do
    {
        m_socket->readLine(line, MaxLen);
        m_reply.append(QString::fromUtf8(line));
        //qDebug("Reply: %s", (QString(line).stripWhiteSpace()).latin1());
    } while (line[3] != ' ');

    writeToSocket("QUIT\r\n");
    m_socket->readLine(line, MaxLen);
    m_socket->close();

    processReply();
}


void
VdrRequest::writeToSocket(const QString &str)
{
    QByteArray ba;
    QTextOStream out(ba);
    out << str;

    int wrote = m_socket->writeBlock(ba.data(), ba.size());
    while (wrote < (int)ba.size())
        wrote += m_socket->writeBlock(ba.data() + wrote, ba.size() - wrote);
}


// this looks quite hacky. Basically, we desect the reply strings from SVDRP
// and package them into appropriate objects of subclasses of Title (TvChannel, TvTimer, ...)
// TODO: is there a way to clean this up, a bit?
void
VdrRequest::processReply()
{
    qDebug("VdrRequest::processReply()");
    if (m_request == "LSTC")
    {
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);  // it->find() is not possible ...? Inconsistency in Qt ...?
            QString id = (*it).mid(4, pos - 4);
            // channel name ends at first semicolon
            pos++;
            uint sigStart = (*it).find(';', pos);
            QString name = (*it).mid(pos, sigStart - pos);
            QString signature = (*it).section(':', 3, 3) + "-" + (*it).section(':', 10, 10) + "-" + 
                (*it).section(':', 11, 11) + "-" + (*it).section(':', 9, 9);
            TvChannel *tvChannel = new TvChannel(id, name, signature);
            Mrl *mrl = new Mrl("http://", "/PES/" + id, m_server + ":" + QString().setNum(m_httpPort), Mrl::TvVdr);
            tvChannel->setMrl(mrl);
            // take the SID as the unique channel identification number.
            int channelId = (*it).section(':', 9, 9).toInt();
            tvChannel->setId("ChannelId", channelId);
            m_list->addTitleEntry(tvChannel);
        }
    }
    else if (m_request == "LSTE")
    {
        TvProgram *program = 0;
        QString channelSignature = "";
        int channelId = 0;
        QString channelName = "";
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            //qDebug("VdrRequest::processReply() line: %s", (*it).latin1());
            if ((*it)[4] == 'C') {
                channelSignature = (*it).section(' ', 1, 1);
                channelId = channelSignature.section('-', 3, 3).toInt();
                channelName = (*it).section(' ', 2);
                //m_currentChannel = m_list->channelList()->getChannelPointer(signature);
            }
            else if ((*it)[4] == 'E') {
                program = new TvProgram((*it).section(' ', 1, 1), (time_t)(*it).section(' ', 2, 2).toUInt(),
                    (time_t)(*it).section(' ', 3, 3).toUInt());
                program->setText("Channel", channelName);
                program->setId("ChannelId", channelId);
            }
            else if ((*it)[4] == 'T') {
                // title
                program->setText("Name", (*it).right((*it).length() - 6).stripWhiteSpace());
            }
            else if ((*it)[4] == 'S') {
                // short description
                program->setText("Short Text", (*it).right((*it).length() - 6).stripWhiteSpace());
            }
            else if ((*it)[4] == 'D') {
                // description
                program->setText("Description", (*it).right((*it).length() - 6).stripWhiteSpace());
            }
            else if ((*it)[4] == 'e' && program) {
                    m_list->addTitleEntry(program);
            }
        }
    }
    else if (m_request == "LSTT")
    {
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);  // it->find() is not possible ...? Inconsistency in Qt ...?
            QString id = (*it).mid(4, pos - 4);
            pos++;
            int active = QString((*it)[pos]).toInt();
            QString channelId = (*it).section(':', 1, 1);
            QString day = (*it).section(':', 2, 2);
            QString start = (*it).section(':', 3, 3);
            QString end = (*it).section(':', 4, 4);
            int prio = (*it).section(':', 5, 5).toInt();
            int resist = (*it).section(':', 6, 6).toInt();
            QString title = (*it).section(':', 7, 7);
            TvTimer *tvTimer = new TvTimer(title, id, channelId, new TimerDay(day), new TimerTime(start), 
                new TimerTime(end), active, prio, resist);
            m_list->addTitleEntry(tvTimer);
        }
    }
    else if (m_request == "LSTR")
    {
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);  // it->find() is not possible ...? Inconsistency in Qt ...?
            QString id = (*it).mid(4, pos - 4);
            pos++;
            QString day = (*it).mid(pos, 8);
            QString start = (*it).mid(pos + 9, 6);
            QString name = (*it).right((*it).length() - pos - 16).stripWhiteSpace();
            //qDebug("SvdrpRequest::processReply() new TvRec: %s, %s, %s, %s", 
            //    id.latin1(), day.latin1(), start.latin1(), title.latin1());
            TvRec *tvRec = new TvRec(id, name, day, start);
            // no video directory specified, no infos about the recordings and no video files to play.
            if (m_videoDir != "") {
                // TODO: locate recording in video directory of VDR (if available).
                QString recDir = locateRecDir(tvRec);
                // TODO: retrieve extra info from recording directory (.vdr files, description)
                addRecFileInfo(tvRec, recDir);
            }
            m_list->addTitleEntry(tvRec);
        }
    }
    qDebug("VdrRequest::processReply() finished.");
}


QString
VdrRequest::locateRecDir(TvRec *tvRec)
{
    QDir videoDir(m_videoDir);
    QString vdrRecNamePath = tvRec->getText("Name").replace(' ', '_');
    vdrRecNamePath.replace(QRegExp("\\W"), "*");  // TODO: dangerous! All non-word characters are
                                                 // replaced by arbitrary character sequences.
                                                // maybe do a more exact RegExp matching: .{1,2}
    qDebug("VdrRequest::locateRecDir() vdrRecNamePath: %s", vdrRecNamePath.latin1());
    QStringList recList = videoDir.entryList(vdrRecNamePath);
    qDebug("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].latin1());
    vdrRecNamePath = recList[0];
    QDir d(m_videoDir + "/" + vdrRecNamePath);
    //recList = d.entryList("*.rec");
    //qDebug("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].latin1());

    QString vdrRecDirPath = "";
    recList = d.entryList("_");
    //qDebug("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].latin1());
    if (recList.count() > 0) {
        qDebug("VdrRequest::locateRecDir() this is a serial!");
        vdrRecNamePath += "/_";
        d = QDir(m_videoDir + "/" + vdrRecNamePath);
    }

    QStringList splitDate = QStringList::split(".", tvRec->getText("Day"));  // TODO: fix this!!
    QString date = "20" + splitDate[2] + "-" + splitDate[1] + "-" + splitDate[0];  // TODO: fix this!!
    QString start = tvRec->getText("Start").left(5);
    start[2] = '?';
    vdrRecDirPath = date + "." + start;
    recList = d.entryList(vdrRecDirPath + ".??.??.rec");
    qDebug("VdrRequest::locateRecDir() vdrRecDirPath: %s", vdrRecDirPath.latin1());
    qDebug("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].latin1());
    QString vdrRecPath = m_videoDir + "/" + vdrRecNamePath + "/" + recList[0];
    qDebug("VdrRequest::locateRecDir() vdrRecPath: %s", vdrRecPath.latin1());
    QDir recDir(vdrRecPath);
    recList = recDir.entryList("???.vdr");
    qDebug("VdrRequest::locateRecDir() recDir number of entries: %i, first recDir entry: %s", 
        recList.count(), recList[0].latin1());
    Mrl *mrl = new Mrl("file://", vdrRecPath);
    mrl->setFiles(recList);
    tvRec->setMrl(mrl);
    return vdrRecPath;
}


void
VdrRequest::addRecFileInfo(TvRec *tvRec, QString recDir)
{
    // TODO: add extra info to recording, like description text, ...
}
