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

/*
blocking network thread with Qt3
http://doc.trolltech.com/qq/qq09-networkthread.html
*/


#include <qapplication.h>
#include <qsocket.h>
#include <qstring.h>
#include <qthread.h>
#include <qsocketdevice.h>


class SVDRPThread : public QThread
{
public:
    void run();

private:
    void writeToSocket(const QString& str);

    QSocketDevice *socket;
};


class SVDRPRequest : public QObject
{
Q_OBJECT
public:
    SVDRPRequest();
    ~SVDRPRequest();

public slots:
    void startRequest() { thread.run(); }

private:
    SVDRPThread thread;
};


SVDRPRequest::SVDRPRequest()
    : QObject()
{

}


SVDRPRequest::~SVDRPRequest()
{

}


void
SVDRPThread::run()
{
    const int MaxLen = 256;
    char line[MaxLen];

    socket = new QSocketDevice;
    socket->setBlocking(true);
    QHostAddress server;
    server.setAddress("192.168.1.1");
    
    if (!socket->connect(server, 2001))
        return;

    socket->readLine(line, MaxLen);
    qDebug("Connect Reply: %s", line);

    writeToSocket("LSTC\r\n");
    
    QStringList reply;
    do
    {
        socket->readLine(line, MaxLen);
        reply.append(QString::fromUtf8(line));
        qDebug("Reply: %s", line);
/*        
        if (s.left(3) != "250")
            break;
        // channel id starts from 5th character and ends at first space
        uint pos = s.find(' ', 4);
        QString id = s.mid(4, pos - 4);
        // channel name ends at first semicolon
        pos++;
        QString name = s.mid(pos, s.find(';', pos) - pos);
        qDebug("SVDRP::getChannels(), id: %s, name: %s", id.latin1(), name.latin1());
*/
    } while (line[3] != ' ');

    writeToSocket("QUIT\r\n");
    socket->readLine(line, MaxLen);
    //socket->flush();

    socket->close();
    delete socket;
}


void
SVDRPThread::writeToSocket(const QString &str)
{
    QByteArray ba;
    QTextOStream out(ba);
    out << str;

    int wrote = socket->writeBlock(ba.data(), ba.size());
    while (wrote < (int)ba.size())
        wrote += socket->writeBlock(ba.data() + wrote, ba.size() - wrote);
}


int
main (int argc, char **argv)
{
    QApplication a(argc, argv);
    SVDRPRequest *request = new SVDRPRequest();
    
    request->startRequest();
    
    return a.exec();
}

#include "svdrptest.moc"
