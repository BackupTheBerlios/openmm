/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
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
#ifndef VDRMEDIASERVER_H
#define VDRMEDIASERVER_H

#include <platinum/PltMediaServer.h>

#include <vdr/channels.h>
#include <vdr/recording.h>

#include <map>
using namespace std;

/**
	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class VdrMediaServer : public PLT_MediaServer
{
public:
    VdrMediaServer(const char*  friendly_name,
                bool         show_ip = false,
                const char*  uuid = NULL,
                NPT_UInt16   port = 0);

    ~VdrMediaServer();

    // overridable methods
    virtual NPT_Result OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnBrowseDirectChildren(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnSearch(PLT_ActionReference& action, const NPT_String& object_id, const NPT_String& searchCriteria, const NPT_HttpRequestContext& context);

private:
    NPT_String channelToDidl(NPT_String filter, cChannel *channel);
    
    NPT_String m_localIp;
    int m_localPort;
    
    PLT_MediaContainer* m_containerRoot;
    PLT_MediaContainer* m_containerLiveTv;
    PLT_MediaContainer* m_containerRecordings;
    
    map<NPT_String, PLT_MediaItem*> m_itemCache;
};

#endif
