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
#include "otherutil.h"
#include "debug.h"

#include <qdir.h>
#include <qregexp.h>


VdrRequest::VdrRequest(ListManager *listManager, List *list, string request)
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
    const int MaxLen = 4096; // TODO: make this variable length (Epg description may be longer ...?)
    char line[MaxLen];

    if (!m_socket->connect(QString(m_server.c_str()), m_svdrpPort))
        return;

    m_socket->readLine(line, MaxLen);
    //TRACE("Connect Reply: %s", line);

    writeToSocket(m_request + "\r\n");

    do
    {
        m_socket->readLine(line, MaxLen);
        //m_reply.push_back(string::fromUtf8(line));
        m_reply.push_back(line);
        //TRACE("Reply: %s", (string(line).stripWhiteSpace()).c_str());
    } while (line[3] != ' ');

    writeToSocket("QUIT\r\n");
    m_socket->readLine(line, MaxLen);
    m_socket->close();

    processReply();
}


void
VdrRequest::writeToSocket(const string &str)
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
    //TRACE("VdrRequest::processReply()");
    if (m_request == "LSTC")
    {
        for ( vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            uint pos = (*it).find(' ', 4);
            string id = (*it).substr(4, pos - 4);
            // channel name ends at first semicolon
            pos++;
            uint sigStart = (*it).find(';', pos);
            string name = (*it).substr(pos, sigStart - pos);
            vector<string> s;
            OtherUtil::stringSplit((*it), ":", s);
            string signature = s[3] + "-" + s[10] + "-" + s[11] + "-" + s[9];
            TvChannel *tvChannel = new TvChannel(id, name, signature);
            char buf[8];
            sprintf(buf,"%i", m_httpPort);
            Mrl *mrl = new Mrl("http://", "/PES/" + id, m_server + ":" + string(buf), Mrl::TvVdr);
            tvChannel->setMrl(mrl);
            // take the SID as the unique channel identification number.
            int channelId = atoi(s[9].c_str());
            tvChannel->setId("ChannelId", channelId);
            m_list->addTitleEntry(tvChannel);
        }
    }
    else if (m_request == "LSTE")
    {
        TvProgram *program = 0;
        string channelSignature = "";
        int channelId = 0;
        string channelName = "";
        for ( vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            //TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            if ((*it)[4] == 'C') {
                int firstSpace = (*it).find(' ');
                int secondSpace = (*it).find(' ', firstSpace + 1);
                channelName = OtherUtil::s_trim((*it).substr(secondSpace));
                channelSignature = (*it).substr(firstSpace, secondSpace - firstSpace);
                channelId = atoi(channelSignature.substr(channelSignature.find_last_of('-') + 1).c_str());
                TRACE("VdrRequest::processReply(), channelName: %s, channelId: %i", channelName.c_str(), channelId);
            }
            else if ((*it)[4] == 'E') {
                vector<string> s;
                OtherUtil::stringSplit((*it), " ", s);
                program = new TvProgram(s[1], (time_t)atoi(s[2].c_str()), (time_t)atoi(s[3].c_str()));
                program->setText("Channel", channelName);
                program->setId("ChannelId", channelId);
            }
            else if ((*it)[4] == 'T') {
                program->setText("Name", OtherUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'S') {
                program->setText("Short Text", OtherUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'D') {
                program->setText("Description", OtherUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'e' && program) {
                m_list->addTitleEntry(program);
            }
        }
    }
    else if (m_request == "LSTT")
    {
        for ( vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
//             TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            uint pos = (*it).find(' ', 4);
            string id = (*it).substr(4, pos - 4);
            pos++;
            int active = atoi((*it).substr(pos, 1).c_str());
            vector<string> s;
            OtherUtil::stringSplit((*it), ":", s);
            // channelId=1,day=2,start=3,end=4,prio=5,resist=6,title=7
            TvTimer *tvTimer = new TvTimer(s[7], id, s[1], new TimerDay(s[2]), new TimerTime(s[3]), 
                new TimerTime(s[4]), active, atoi(s[5].c_str()), atoi(s[6].c_str()));
            m_list->addTitleEntry(tvTimer);
        }
    }
    else if (m_request == "LSTR")
    {
        for ( vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            uint pos = (*it).find(' ', 4);
            string id = (*it).substr(4, pos - 4);
            pos++;
            string day = (*it).substr(pos, 8);
            string start = (*it).substr(pos + 9, 6);
            string name = OtherUtil::s_trim((*it).substr(pos + 16));
            TRACE("SvdrpRequest::processReply() new TvRec: %s, %s, %s, %s", 
                id.c_str(), day.c_str(), start.c_str(), name.c_str());
            TvRec *tvRec = new TvRec(id, name, day, start);
            // no video directory specified, no infos about the recordings and no video files to play.
            if (m_videoDir != "") {
                // TODO: locate recording in video directory of VDR (if available).
//                 string recDir = locateRecDir(tvRec);
                // TODO: retrieve extra info from recording directory (.vdr files, description)
//                 addRecFileInfo(tvRec, recDir);
            }
            m_list->addTitleEntry(tvRec);
        }
    }
    TRACE("VdrRequest::processReply() finished.");
}


string
VdrRequest::locateRecDir(TvRec *tvRec)
{
    QDir videoDir(m_videoDir);
    string vdrRecNamePath = OtherUtil::s_replace(tvRec->getText("Name"), ' ', '_');
    //vdrRecNamePath.replace(QRegExp("\\W"), "*");  // TODO: dangerous! All non-word characters are
                                                 // replaced by arbitrary character sequences.
                                                // maybe do a more exact RegExp matching: .{1,2}
    TRACE("VdrRequest::locateRecDir() vdrRecNamePath: %s", vdrRecNamePath.c_str());
/*
    vector<string> recList = videoDir.entryList(vdrRecNamePath);
    TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].c_str());
    vdrRecNamePath = recList[0];
    QDir d(m_videoDir + "/" + vdrRecNamePath);
    //recList = d.entryList("*.rec");
    //TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].c_str());

    string vdrRecDirPath = "";
    recList = d.entryList("_");
    //TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].c_str());
    if (recList.count() > 0) {
        TRACE("VdrRequest::locateRecDir() this is a serial!");
        vdrRecNamePath += "/_";
        d = QDir(m_videoDir + "/" + vdrRecNamePath);
    }

    vector<string> splitDate = vector<string>::split(".", tvRec->getText("Day"));  // TODO: fix this!!
    string date = "20" + splitDate[2] + "-" + splitDate[1] + "-" + splitDate[0];  // TODO: fix this!!
    string start = tvRec->getText("Start").left(5);
    start[2] = '?';
    vdrRecDirPath = date + "." + start;
    recList = d.entryList(vdrRecDirPath + ".??.??.rec");
    TRACE("VdrRequest::locateRecDir() vdrRecDirPath: %s", vdrRecDirPath.c_str());
    TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.count(), recList[0].c_str());
    string vdrRecPath = m_videoDir + "/" + vdrRecNamePath + "/" + recList[0];
    TRACE("VdrRequest::locateRecDir() vdrRecPath: %s", vdrRecPath.c_str());
    QDir recDir(vdrRecPath);
    recList = recDir.entryList("???.vdr");
    TRACE("VdrRequest::locateRecDir() recDir number of entries: %i, first recDir entry: %s", 
        recList.count(), recList[0].c_str());
    Mrl *mrl = new Mrl("file://", vdrRecPath);
    mrl->setFiles(recList);
    tvRec->setMrl(mrl);
    return vdrRecPath;
*/
    return vdrRecNamePath;
}


void
VdrRequest::addRecFileInfo(TvRec *tvRec, string recDir)
{
    // TODO: add extra info to recording, like description text, ...
}
