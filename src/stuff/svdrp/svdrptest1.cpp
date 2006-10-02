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

class SVDRP : public QObject
{
Q_OBJECT
public:
    SVDRP(QObject *parent = 0, const char *name = 0);

    ~SVDRP();

    void setServer(QString servername, int port);
    void connectServer();
    void disconnectServer();
    void getChannels();
    bool notConnected();

private:
    QString m_servername;
    int m_port;
    QSocket *m_connection;
    QString m_CmdListChannels;
    
protected slots:
    void hostFound();
    void connected();
    void closed();
    void readyRead();
    void error( int );
};


SVDRP::SVDRP(QObject *parent, const char *name)
    : QObject(parent, name)
{
    m_CmdListChannels = "LSTC\r\n";
    
    m_connection = new QSocket(this);
    
    connect( m_connection, SIGNAL( hostFound() ),
             this, SLOT( hostFound() ) );
    connect( m_connection, SIGNAL( connected() ),
             this, SLOT( connected() ) );
    connect( m_connection, SIGNAL( connectionClosed() ),
             this, SLOT( closed() ) );
    connect( m_connection, SIGNAL( readyRead() ),
             this, SLOT( readyRead() ) );
    connect( m_connection, SIGNAL( error( int ) ),
             this, SLOT( error( int ) ) );
}


SVDRP::~SVDRP()
{
    delete m_connection;
}


void
SVDRP::hostFound()
{
    qDebug("SVDRP::hostFound()");
}


void
SVDRP::connected()
{
    qDebug("SVDRP::connected()");
}


void
SVDRP::closed()
{
    qDebug("SVDRP::closed()");
}


void
SVDRP::readyRead()
{
    // new data arrived on the command socket

    // read the new data from the socket
    //QCString s;
    //s.resize( m_connection->bytesAvailable() + 1 );
    //m_connection->readBlock( s.data(), m_connection->bytesAvailable() );
    
    QString s;
    s = m_connection->readLine();
    
    qDebug("SVDRP::readyRead(), got: %s", s.latin1());
    
    //getChannels();
    //disconnectServer();

    // of the code of the server response was 200, we know that the
    // server is ready to get commands from us now
    //if ( s.left( 3 ) == "200" )
    //    connectionReady = TRUE;
}


void
SVDRP::error( int code )
{
    qDebug("Nntp::error(), error code: %i", code);
}


void
SVDRP::setServer(QString servername, int port)
{
    m_servername = servername;
    m_port = port;
}


void
SVDRP::connectServer()
{
    m_connection->connectToHost(m_servername, m_port);
    // TODO: check if connection is really established ...
    // TODO: if SVDRP is occupied, wait until connection is free
    //m_connection->flush();
//    qDebug("SVDRP::connectServer(), reply: %s", m_connection->readLine().latin1());
}


void
SVDRP::disconnectServer()
{
//    m_connection->flush();
    m_connection->writeBlock("quit\r\n", strlen("quit\r\n"));
    m_connection->close();
}


void
SVDRP::getChannels()
{
    qDebug("SVDRP::getChannels()");
    //connectServer();
    m_connection->writeBlock(m_CmdListChannels.latin1(), m_CmdListChannels.length());
    qDebug("SVDRP::getChannels(), reply: %s", m_connection->readLine().latin1());

    // read one line after the other
    while(m_connection->canReadLine())
    {
        QString s = m_connection->readLine();
        qDebug("SVDRP::getChannels(), read line from server: %s", s.latin1());
        // check if answer is 250
        if (s.left(3) != "250")
            break;
        // channel id starts from 5th character and ends at first space
        uint pos = s.find(' ', 4);
        QString id = s.mid(4, pos - 4);
        // channel name ends at first semicolon
        pos++;
        QString name = s.mid(pos, s.find(';', pos) - pos);
        qDebug("SVDRP::getChannels(), id: %s, name: %s", id.latin1(), name.latin1());
    }
    //disconnectServer();
}


bool
SVDRP::notConnected()
{
    return m_connection->state() != QSocket::Connected;
}


int
main (int argc, char **argv)
{
    QApplication a(argc, argv);
    SVDRP *testsvdrp = new SVDRP();
    
    testsvdrp->setServer("192.168.1.1", 2001);
    testsvdrp->connectServer();
    
    return a.exec();
    
    //while (testsvdrp->notConnected())
    //{
    //    sleep(1000);
    //}
    //testsvdrp->disconnectServer();
    //testsvdrp->getChannels();
}

#include "svdrptest.moc"
