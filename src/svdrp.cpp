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
SVDRP::getRecordings(JAMTV *tv)
{
    //qDebug("SVDRP::getRecordings()");

    SVDRPRequest request(this, "LSTR", tv);
    request.startRequest();
}


void
SVDRP::getTimers(JAMTV *tv)
{
    //qDebug("SVDRP::getTimers()");

    SVDRPRequest request(this, "LSTT", tv);
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
                //qDebug("SVDRPRequest::processReply(), new Channel: %s", (*it).latin1());
                QString signature = (*it).section(' ', 1, 1);
                //qDebug("SVDRPRequest::processReply(), Channel signature: %s", signature.latin1());
                m_currentChannel = m_tv->getChannelPointer(signature);
                //qDebug("SVDRPRequest::processReply(), currentChannel: %s", m_currentChannel->getName().latin1());
                // new channel (identify by channel name or by "signature"?)
            } else
            if ((*it)[4] == 'E')
            {
                //qDebug("SVDRPRequest::processReply(), new Event: %s", (*it).latin1());
                // new event
                m_currentEPGEntry = new EPGEntry((*it).section(' ', 1, 1), (time_t)(*it).section(' ', 2, 2).toUInt(), (time_t)(*it).section(' ', 3, 3).toUInt());
                // append EPGEntry to channel
                //qDebug("SVDRPRequest::processReply(), adding new EPG entry");
                //if (m_currentChannel != NULL)
                    m_currentChannel->appendEPGEntry(m_currentEPGEntry);
            } else
            if ((*it)[4] == 'T')
            {
                //qDebug("SVDRPRequest::processReply(), new Title, start: %s, %i", (*it).latin1(), (int)m_currentEPGEntry->getStartTime());
                // title
                m_currentEPGEntry->setTitle((*it).right((*it).length() - 6));
            } else
            if ((*it)[4] == 'S')
            {
                //qDebug("SVDRPRequest::processReply(), new Shorttext: %s", (*it).latin1());
                // short description
                m_currentEPGEntry->setShortText((*it).right((*it).length() - 6));
            } else
            if ((*it)[4] == 'D')
            {
                //qDebug("SVDRPRequest::processReply(), new Description: %s", (*it).latin1());
                // description
                m_currentEPGEntry->setDescription((*it).right((*it).length() - 6));
            }
        }
    }
}
