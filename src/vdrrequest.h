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
#ifndef VDRREQUEST_H
#define VDRREQUEST_H

#include "listmanager.h"
#include "tvrec.h"

#include <qthread.h>
#include <qsocketdevice.h>


/**
Handles all details about the communication to vdr via svdrp protocol.

	@author Jörg Bakker <joerg@hakker.de>
*/
class VdrRequest : public QObject, private QThread
{
    Q_OBJECT

public:
    VdrRequest(ListManager *listManager, List *list, QString request);
    ~VdrRequest();

public slots:
    void startRequest();

private:
    void writeToSocket(const QString& str);
    void processReply();
    void run();

    // some helper functions to deal with the particular difficulties of VDR.
    QString locateRecDir(TvRec *tvRec);
    void addRecFileInfo(TvRec *tvRec, QString recDir);

    QMutex m_requestMutex;

    ListManager *m_listManager;
    List *m_list;
    QString m_request;
    QString m_server;
    Q_UINT16 m_svdrpPort;
    Q_UINT16 m_httpPort;
    QSocketDevice *m_socket;
    QStringList m_reply;
    QString m_videoDir;
};

#endif
