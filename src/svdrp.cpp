/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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
#include "svdrp.h"


SVDRP::SVDRP(QString server, Q_UINT16 port)
 : QObject()
{
    m_server = server;
    m_port = port;
}


SVDRP::~SVDRP()
{
}


void
SVDRP::getChannels(JAMTV *tv)
{
    //qDebug("SVDRP::getChannels()");

    SVDRPRequest request(this, "LSTC", tv);
    request.startRequest();
}


void
SVDRP::getEPG(JAMTV *tv)
{
    //qDebug("SVDRP::getEPG()");

    SVDRPRequest request(this, "LSTE", tv);
    request.startRequest();
}


void
SVDRP::getRecs(JAMTV *tv)
{
    //qDebug("SVDRP::getRecs()");

    SVDRPRequest request(this, "LSTR", tv);
    request.startRequest();
}


void
SVDRP::delRec(JAMTV *tv, TVRec *rec)
{
    qDebug("SVDRP::delRec() %s", ("DELR " + rec->getId()).latin1());
    // FIX: deleting is disabled. Enable it for release version.
    SVDRPRequest request(this, "DELR " + rec->getId(), tv);
    request.startRequest();
}


void
SVDRP::getTimers(JAMTV *tv)
{
    //qDebug("SVDRP::getTimers()");

    SVDRPRequest request(this, "LSTT", tv);
    request.startRequest();
}


void
SVDRP::setTimer(JAMTV *tv, TVTimer *timer)
{
    QString reqParam = timer->getActiveStr() + ":" + timer->getChannelId() + ":" + timer->getDayStr() + ":" + 
                       timer->getStartStr() + ":" + timer->getEndStr() + ":" + timer->getPrioStr() + ":" + 
                       timer->getResistStr() + ":" + timer->getTitle() + ":";
    qDebug("SVDRP::setTimer() %s", ("NEWT " + reqParam).latin1());
    SVDRPRequest request(this, "NEWT " +  reqParam, tv);
    request.startRequest();
}


void
SVDRP::delTimer(JAMTV *tv, TVTimer *timer)
{
    qDebug("SVDRP::delTimer() %s", ("DELT " + timer->getId()).latin1());
    // FIX: deleting is disabled. Enable it for release version.
    SVDRPRequest request(this, "DELT " + timer->getId(), tv);
    request.startRequest();
}


SVDRPRequest::SVDRPRequest(SVDRP *svdrp, QString request, JAMTV *tv)
    : QThread()
{
    m_svdrp = svdrp;
    m_request = request;
    m_tv =  tv;

    m_socket = new QSocketDevice;
    m_socket->setBlocking(true);

    m_currentChannel = NULL;
    m_currentEPGEntry = NULL;

    QHostAddress server;
    server.setAddress(m_svdrp->server());
}


SVDRPRequest::~SVDRPRequest()
{
    delete m_socket;
}


void
SVDRPRequest::startRequest()
{
    run();
}


void
SVDRPRequest::run()
{
    // FIX: does this reliably synchronize access to the VDR socket in order of request call in the main thread?
    QMutexLocker requestLocker(&m_requestMutex);
    const int MaxLen = 2048; // FIX: make this variable length (EPG description may be longer ...?)
    char line[MaxLen];

    if (!m_socket->connect(m_svdrp->server(), m_svdrp->port()))
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
SVDRPRequest::writeToSocket(const QString &str)
{
    QByteArray ba;
    QTextOStream out(ba);
    out << str;

    int wrote = m_socket->writeBlock(ba.data(), ba.size());
    while (wrote < (int)ba.size())
        wrote += m_socket->writeBlock(ba.data() + wrote, ba.size() - wrote);
}


void
SVDRPRequest::processReply()
{
    // TODO: dispatch requests LSTC, etc ...
    if (m_request == "LSTC")
    {
        //qDebug("SVDRPRequest::processReply(), m_request = LSTC");
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);  // it->find() is not possible ...? Inconsistency in Qt ...?
            QString id = (*it).mid(4, pos - 4);
            // channel name ends at first semicolon
            pos++;
            uint sigStart = (*it).find(';', pos);
            QString name = (*it).mid(pos, sigStart - pos);
            QString signature = (*it).section(':', 3, 3) + "-" + (*it).section(':', 10, 10) + "-" + (*it).section(':', 11, 11) + "-" + (*it).section(':', 9, 9);
            //qDebug("SVDRPRequest::processReply(), adding channel: %s, %s", name.latin1(), signature.latin1());
            TVChannel *channel = new TVChannel(id, name, signature);
            m_tv->appendChannel(channel);
        }
        //FIX: send an event, that the channellist is constructed (for further EPG data insertion)
    } else
    if (m_request == "LSTE")
    {
        //qDebug("SVDRPRequest::processReply(), m_request = LSTE");
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            if ((*it)[4] == 'C')
            {
                QString signature = (*it).section(' ', 1, 1);
                m_currentChannel = m_tv->getChannelPointer(signature);
            } else
            if ((*it)[4] == 'E')
            {
                m_currentEPGEntry = new EPGEntry((*it).section(' ', 1, 1), (time_t)(*it).section(' ', 2, 2).toUInt(), (time_t)(*it).section(' ', 3, 3).toUInt());
                m_currentChannel->appendEPGEntry(m_currentEPGEntry);
            } else
            if ((*it)[4] == 'T')
            {
                // title
                m_currentEPGEntry->setTitle((*it).right((*it).length() - 6).stripWhiteSpace());
            } else
            if ((*it)[4] == 'S')
            {
                // short description
                m_currentEPGEntry->setShortText((*it).right((*it).length() - 6).stripWhiteSpace());
            } else
            if ((*it)[4] == 'D')
            {
                // description
                m_currentEPGEntry->setDescription((*it).right((*it).length() - 6).stripWhiteSpace());
            }
        }
    } else
    if (m_request == "LSTT")
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
            //qDebug("SVDRPRequest::processReply() new TVTimer: %s, %s, %s, %s, %s, %i, %i, %i, %s", 
            //    id.latin1(), channelId.latin1(), day.latin1(), start.latin1(), end.latin1(), active, prio, resist, title.latin1());
            TVTimer *tvTimer = new TVTimer(id, channelId, new TimerDay(day), new TimerTime(start), new TimerTime(end), active, prio, resist, title);
            m_tv->appendTimer(tvTimer);
        }
    }
    if (m_request == "LSTR")
    {
        for ( QStringList::Iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);  // it->find() is not possible ...? Inconsistency in Qt ...?
            QString id = (*it).mid(4, pos - 4);
            pos++;
            QString day = (*it).mid(pos, 8);
            QString start = (*it).mid(pos + 9, 6);
            QString title = (*it).right((*it).length() - pos - 16).stripWhiteSpace();
            //qDebug("SVDRPRequest::processReply() new TVRec: %s, %s, %s, %s", 
            //    id.latin1(), day.latin1(), start.latin1(), title.latin1());
            TVRec *tvRec = new TVRec(id, day, start, title);
            m_tv->appendRec(tvRec);
        }
    }
}
