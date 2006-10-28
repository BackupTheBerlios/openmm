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
#ifndef SVDRP_H
#define SVDRP_H

#include <qstring.h>
#include <qthread.h>
#include <qsocketdevice.h>

#include "tv.h"

// forward declaration
class Tv;
class TvChannel;
class EpgEntry;
class TvTimer;
class TvRec;

/**
@author Jörg Bakker
*/
class Svdrp : public QObject
{
Q_OBJECT
public:
    Svdrp(QString server, Q_UINT16 port);
    ~Svdrp();

    QString server() { return m_server; }
    Q_UINT16 port() { return m_port; }
    void getChannels(Tv *tv);
    void getEpg(Tv *tv);
    void getRecs(Tv *tv);
    void delRec(Tv *tv, TvRec *rec);
    void getTimers(Tv *tv);
    void setTimer(Tv *tv, TvTimer *timer);
    void delTimer(Tv *tv, TvTimer *timer);

private:
    QString m_server;
    Q_UINT16 m_port;
};


class SvdrpRequest : public QObject, private QThread
{
Q_OBJECT
public:
    SvdrpRequest(Svdrp *svdrp, QString request, Tv *tv);
    ~SvdrpRequest();

    void run();

public slots:
    void startRequest();

private:
    void writeToSocket(const QString& str);
    void processReply();

    QMutex m_requestMutex;

    Svdrp *m_svdrp;
    QString m_request;
    Tv *m_tv;
    TvTimer *m_timer;
    QSocketDevice *m_socket;
    QStringList m_reply;

    TvChannel *m_currentChannel;
    EpgEntry *m_currentEpgEntry;
};

#endif
