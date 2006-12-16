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
#include "thread.h"
#include "netsocket.h"


/**
Handles the communication to vdr via svdrp protocol.

	@author Jörg Bakker <joerg@hakker.de>
*/
class VdrRequest : public QObject, public Thread
{
    Q_OBJECT

public:
    VdrRequest(ListManager *listManager, List *list, string request);
    ~VdrRequest();

private:
    void processReply();
    void run();

    // some helper functions to deal with the particular difficulties of VDR.
    string locateRecDir(TvRec *tvRec);
    void addRecFileInfo(TvRec *tvRec, string recDir);

    Mutex            m_requestMutex;

    ListManager     *m_listManager;
    List            *m_list;
    string           m_request;
    string           m_server;
    unsigned int     m_svdrpPort;
    unsigned int     m_httpPort;
    NetSocket       *m_socket;
    vector<string>   m_reply;
    string           m_videoDir;
};

#endif
