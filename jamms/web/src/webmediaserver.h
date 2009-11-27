/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef WEBMEDIASERVER_H
#define WEBMEDIASERVER_H

#include <platinum/PltMediaServer.h>
#include <Poco/URI.h>
#include "Poco/SAX/InputSource.h"
#include "Poco/Net/HTTPClientSession.h"


#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstdio>
#include <sstream>

using namespace std;
using Poco::Net::HTTPClientSession;
using Poco::URI;
using Poco::XML::InputSource;


/**
	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/

class WebMediaServer : public PLT_MediaServer
{
public:
    WebMediaServer(const char*  friendly_name,
                bool         show_ip = false,
                const char*  uuid = NULL,
                NPT_UInt16   port = 0);

    ~WebMediaServer();

private:
    virtual NPT_Result OnBrowseMetadata(PLT_ActionReference& action,
                                        const char* object_id,
                                        const NPT_HttpRequestContext& context);
    virtual NPT_Result OnBrowseDirectChildren(PLT_ActionReference& action,
                                              const char* object_id,
                                              const NPT_HttpRequestContext& context);
    virtual NPT_Result OnSearch(PLT_ActionReference& action,
                                const NPT_String& object_id,
                                const NPT_String& searchCriteria,
                                const NPT_HttpRequestContext& context);
    NPT_String genreToDidl(NPT_String filter, string genre);
    NPT_String stationToDidl(NPT_String filter, string parent_id, string name, string id);
    
    
    NPT_String m_localIp;
    
    PLT_MediaContainer* m_containerRoot;
    PLT_MediaContainer* m_containerShout;
    
    URI m_uriShout;
    stringstream m_xmlShoutGenre;
    HTTPClientSession* session;
    // TODO: need to lock m_itemCache
    map<string, stringstream*> m_stationCache;
    map<NPT_String, PLT_MediaItem*> m_itemCache;
//     map<NPT_String, NPT_InputStreamReference> m_recCache;
};


#endif
