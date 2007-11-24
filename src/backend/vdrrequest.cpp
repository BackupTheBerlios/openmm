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
#include "stringutil.h"
#include "directory.h"
#include "debug.h"


VdrRequest::VdrRequest(ListManager *listManager, List *list, string request)
{
    m_listManager = listManager;
    m_list = list;
    m_request = request;
    m_server = "192.168.178.11";  // TODO: set the following items with Setup class.
    m_svdrpPort = 2001;
    m_httpPort = 3000;
    m_videoDir = "/video";

    m_socket = new NetSocket;
}


VdrRequest::~VdrRequest()
{
    delete m_socket;
}


void
VdrRequest::run()
{
    TRACE("VdrRequest::run()");
    // TODO: does this reliably synchronize access to the VDR socket in order of request call in the main thread?
//     MutexLocker requestLocker(&m_requestMutex);
    string line;

    m_reply.clear();
    if (!m_socket->open(m_server, m_svdrpPort)) {
        TRACE("VdrRequest::run() failed to open connection");
        return;
    }
    line = m_socket->readLine();
//     TRACE("Connect Reply: %s", line.c_str());
    m_socket->writeLine(m_request);
    do
    {
        line = m_socket->readLine();
        m_reply.push_back(line);
//         TRACE("Reply: %s", line.c_str());
    } while (line[3] != ' ');  // indicates the last line -> end of answer from server!

    m_socket->writeLine("QUIT");
    line = m_socket->readLine();  // one line answer from server after QUIT request.
    m_socket->close();

    processReply();
}


// this looks quite hacky. Basically, we desect the reply strings from SVDRP
// and package them into appropriate objects of subclasses of Title (TvChannel, TvTimer, ...)
// TODO: is there a way to clean this up, a bit?
void
VdrRequest::processReply()
{
    TRACE("VdrRequest::processReply()");
    if (m_request == "LSTC")
    {
        for ( vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it )
        {
            // TODO: sometimes empty lines appear
            if ((*it).size() == 0) {
                continue;
            }
            TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            uint pos = (*it).find(' ', 4);
            string id = (*it).substr(4, pos - 4);
            TRACE("VdrRequest::processReply() substr() gets id: %s", id.c_str());
            // channel name ends at first semicolon
            pos++;
            uint sigStart = (*it).find(';', pos);
            string name = (*it).substr(pos, sigStart - pos);
            TRACE("VdrRequest::processReply() substr() gets name: %s", name.c_str());
            vector<string> s;
            StringUtil::s_split((*it), ":", s);
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
        for (vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it)
        {
            //TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            if ((*it)[4] == 'C') {
                int firstSpace = (*it).find(' ');
                int secondSpace = (*it).find(' ', firstSpace + 1);
                channelName = StringUtil::s_trim((*it).substr(secondSpace));
                channelSignature = (*it).substr(firstSpace, secondSpace - firstSpace);
                channelId = atoi(channelSignature.substr(channelSignature.find_last_of('-') + 1).c_str());
                TRACE("VdrRequest::processReply(), channelName: %s, channelId: %i", channelName.c_str(), channelId);
            }
            else if ((*it)[4] == 'E') {
                vector<string> s;
                StringUtil::s_split((*it), " ", s);
                program = new TvProgram(s[1], (time_t)atoi(s[2].c_str()), (time_t)atoi(s[3].c_str()));
                program->setText("Channel", channelName);
                program->setId("ChannelId", channelId);
            }
            else if ((*it)[4] == 'T') {
                program->setText("Name", StringUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'S') {
                program->setText("Short Text", StringUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'D') {
                program->setText("Description", StringUtil::s_trim((*it).substr(6)));
            }
            else if ((*it)[4] == 'e' && program) {
                m_list->addTitleEntry(program);
            }
        }
    }
    else if (m_request == "LSTT")
    {
        for (vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it)
        {
//             TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            if ((*it).substr(0, 3) == "550")
            {
                TRACE("VdrRequest::processReply() no timers present");
                break;
            }
            uint pos = (*it).find(' ', 4);
            string id = (*it).substr(4, pos - 4);
            pos++;
            int active = atoi((*it).substr(pos, 1).c_str());
            vector<string> s;
            StringUtil::s_split((*it), ":", s);
            // channelId=1,day=2,start=3,end=4,prio=5,resist=6,title=7
            TvTimer *tvTimer = new TvTimer(s[7], id, s[1], new TvTimerDay(s[2]), new TvTimerTime(s[3]), 
                new TvTimerTime(s[4]), active, atoi(s[5].c_str()), atoi(s[6].c_str()));
            m_list->addTitleEntry(tvTimer);
        }
    }
    else if (m_request == "LSTR")
    {
        for (vector<string>::iterator it = m_reply.begin(); it != m_reply.end(); ++it)
        {
//             TRACE("VdrRequest::processReply() line: %s", (*it).c_str());
            uint pos = (*it).find(' ', 4);
            if (pos == string::npos) {
                continue;
            }
            string id = (*it).substr(4, pos - 4);
            pos++;
            string day = (*it).substr(pos, 8);
            string start = (*it).substr(pos + 9, 6);
            string name = StringUtil::s_trim((*it).substr(pos + 16));
            TRACE("SvdrpRequest::processReply() new TvRec: %s, %s, %s, %s", 
                id.c_str(), day.c_str(), start.c_str(), name.c_str());
            TvRec *tvRec = new TvRec(id, name, day, start);
            // no video directory specified, no infos about the recordings and no video files to play.
            if (m_videoDir != "") {
                // TODO: locate recording in video directory of VDR (if available).
                string recDir = locateRecDir(tvRec);
                // TODO: retrieve extra info from recording directory (.vdr files, description)
                addRecFileInfo(tvRec, recDir);
            }
            m_list->addTitleEntry(tvRec);
        }
    }
    TRACE("VdrRequest::processReply() finished.");
}

/*
    This is *butt* ugly code, cause this is a real hack anyway ... the directories where the recorded files
    are located should be provided by the SVDRP protocol (and the recordings should be streamed by vdr.streamdev ...)
*/

// TODO: there's still a bug, not all paths are found (Ripley's Game, Wer wird Million�? - Prominentenspecial,
//       all serials of Das perfekte Dinner~)
string
VdrRequest::locateRecDir(TvRec *tvRec)
{
    Directory videoDir(m_videoDir);
    string vdrRecNamePath = StringUtil::s_replace(tvRec->getText("Name"), ' ', '_');
//     TRACE("VdrRequest::locateRecDir() vdrRecNamePath: %s", vdrRecNamePath.c_str());

    vector<string> recList = videoDir.entryList(vdrRecNamePath, true);
    if (recList.size() == 0) {
        return "";
    }
//     TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.size(), recList[0].c_str());
    vdrRecNamePath = recList[0];
    Directory d(m_videoDir + "/" + vdrRecNamePath);

    string vdrRecDirPath = "";
    recList = d.entryList("_", false);
    if (recList.size() > 0) {
        TRACE("VdrRequest::locateRecDir() this is a serial!");
        vdrRecNamePath += "/_";
        d = Directory(m_videoDir + "/" + vdrRecNamePath);
    }

    vector<string> splitDate;
    StringUtil::s_split(tvRec->getText("Day"), ".", splitDate);  // TODO: fix this!!
    string date = "20" + splitDate[2] + "-" + splitDate[1] + "-" + splitDate[0];  // TODO: fix this!!
    string start = tvRec->getText("Start").substr(0, 5);
    start[2] = '?';
    vdrRecDirPath = date + "." + start;
    recList = d.entryList(vdrRecDirPath + ".??.??.rec", false);
//     TRACE("VdrRequest::locateRecDir() vdrRecDirPath: %s", vdrRecDirPath.c_str());
//     TRACE("VdrRequest::locateRecDir() recList size: %i, first recList entry: %s", recList.size(), recList[0].c_str());
    string vdrRecPath = m_videoDir + "/" + vdrRecNamePath + "/" + recList[0];
    TRACE("VdrRequest::locateRecDir() vdrRecPath: %s", vdrRecPath.c_str());
    Directory recDir(vdrRecPath);
    recList = recDir.entryList("???.vdr", false);
    TRACE("VdrRequest::locateRecDir() recDir number of entries: %i, first recDir entry: %s", 
        recList.size(), recList[0].c_str());
    Mrl *mrl = new Mrl("file://", vdrRecPath);
    mrl->setFiles(recList);
    tvRec->setMrl(mrl);
    return vdrRecPath;
}


void
VdrRequest::addRecFileInfo(TvRec *tvRec, string recDir)
{
    // TODO: add extra info to recording, like description text, ...
    // marks.vdr contains the time marks from noad (format: 0:06:00.25, each mark on a seperate line)
    // to convert file position to time, file number (and vice versa), use index.vdr 
    // (see http://www.vdr-wiki.de/wiki/index.php/Index.vdr)
    // problem: index.vdr is too big (~ 1 MB) to read it into memory for each recording.
}
