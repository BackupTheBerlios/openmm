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

#include "webmediaserver.h"

#include <platinum/PltMediaItem.h>
#include <platinum/PltDidl.h>
#include <platinum/PltUPnPHelper.h>
#include <platinum/PltHttpServer.h>

#include <iostream>
#include <fstream>

#include <cerrno>
#include <cstring>
#include <dirent.h>


NPT_SET_LOCAL_LOGGER("web.upnp");

WebMediaServer::WebMediaServer(const char*  friendly_name,
                bool         show_ip,
                const char*  uuid,
                NPT_UInt16   port)
: PLT_MediaServer(friendly_name, show_ip, uuid, port)
{
    // FIXME: hack for now: find the first valid non local ip address
    // to use in item resources. TODO: we should advertise all ips as
    // multiple resources instead.
    NPT_List<NPT_String> ips;
    PLT_UPnPMessageHelper::GetIPAddresses(ips);
//     if (ips.GetItemCount() == 0) return NPT_ERROR_INTERNAL;
    m_localIp = *ips.GetFirstItem();
//     m_localPort = StreamdevServerSetup.HTTPServerPort;
    
    m_containerRoot = new PLT_MediaContainer();
    // TODO: what to do, when browsing the root object "0"?
    m_containerRoot->m_Title = "";
    m_containerRoot->m_ObjectClass.type = "object.container";
    m_containerRoot->m_ChildrenCount = 0;
    m_containerRoot->m_ParentID = "0";
    m_containerRoot->m_ObjectID = "0";
    
/*    m_containerShout = new PLT_MediaContainer();
    m_containerShout->m_Title = "Shoutcast";
    m_containerShout->m_ObjectClass.type = "object.container";
    // let m_ChildrenCount be > 0, otherwise xbmc doesn't display the container
    m_containerShout->m_ChildrenCount = 1;
    m_containerShout->m_ParentID = m_containerRoot->m_ObjectID;
    m_containerShout->m_ObjectID = "1";*/
    
//     m_uriShout = URI("http://www.shoutcast.com/sbin/newxml.phtml");
    m_stationList.push_back(new RadioStation("SOMA FM - Groove Salad",
                                        "http://streamer-dtc-aa04.somafm.com:80/stream/1018"));
    m_stationList.push_back(new RadioStation("SOMA FM - Indie Pop Rocks (Lush)",
                                        "http://streamer-ntc-aa02.somafm.com:80/stream/1073"));
    m_stationList.push_back(new RadioStation("SOMA FM - Drone Zone",
                                        "http://streamer-dtc-aa01.somafm.com:80/stream/1032"));
    m_stationList.push_back(new RadioStation("Digitally Imported - Chillout",
                                        "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035"));
    m_stationList.push_back(new RadioStation("SWR DASDING",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3"));
    m_stationList.push_back(new RadioStation("SWR DASDING Lautstark",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3"));
    m_stationList.push_back(new RadioStation("SWR3",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3"));
    m_stationList.push_back(new RadioStation("MotorFM",
                                             "http://www.motorfm.de/stream-berlin"));
    m_stationList.push_back(new RadioStation("Freies Radio Stuttgart",
                                             "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    m_stationList.push_back(new RadioStation("HoRadS",
                                             "http://realserver3.hdm-stuttgart.de:8080/horads"));
}

WebMediaServer::~WebMediaServer()
{
}


NPT_Result
WebMediaServer::OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& /*context*/)
{
//     NPT_LOG_INFO_1("Web BrowseMetadata Action on object_id = %s", object_id);
    NPT_String didl, tmp;
    NPT_String filter;
    NPT_CHECK_SEVERE(action->GetArgumentValue("Filter", filter));
    
    if (NPT_String(object_id) == m_containerRoot->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerRoot, filter, tmp));
    }
    else {
        PLT_MediaItem* item;
        map<NPT_String, PLT_MediaItem*>::iterator i = m_itemCache.find(NPT_String(object_id));
        if (i == m_itemCache.end()) {
            NPT_LOG_INFO_1("Web BrowseMetadata NOT IN CACHE: object_id = %s", object_id);
            return NPT_FAILURE;
        }
        item = (*i).second;
        if (item == NULL){
            NPT_LOG_INFO_1("Web BrowseMetadata NULL OBJECT IN CACHE: object_id = %s", object_id);
            return NPT_FAILURE;
        }
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*item, filter, tmp));
    }
    didl = didl_header + tmp + didl_footer;
    NPT_LOG_INFO(didl);
    
    NPT_CHECK_SEVERE(action->SetArgumentValue("Result", didl));
    NPT_CHECK_SEVERE(action->SetArgumentValue("NumberReturned", "1"));
    NPT_CHECK_SEVERE(action->SetArgumentValue("TotalMatches", "1"));
    
    // update ID may be wrong here, it should be the one of the container?
    NPT_CHECK_SEVERE(action->SetArgumentValue("UpdateId", "1"));
    // TODO: We need to keep track of the overall updateID of the CDS
    
    return NPT_SUCCESS;
}


NPT_Result
WebMediaServer::OnBrowseDirectChildren(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& /*context*/)
{
//     NPT_LOG_INFO_1("Web BrowseDirectChildren Action on object_id = %s", object_id);
    std::cout << "Web BrowseDirectChildren Action on object_id = " << string(object_id) << std::endl;
    NPT_String didl = didl_header;

    NPT_String filter;
    NPT_String startingInd;
    NPT_String reqCount;
    int numberReturned = 0;
    int totalMatches = 0;
    unsigned long start_index, req_count;
    

    NPT_CHECK_SEVERE(action->GetArgumentValue("Filter", filter));
    NPT_CHECK_SEVERE(action->GetArgumentValue("StartingIndex", startingInd));
    NPT_CHECK_SEVERE(action->GetArgumentValue("RequestedCount", reqCount));
    if (NPT_FAILED(startingInd.ToInteger(start_index)) ||
        NPT_FAILED(reqCount.ToInteger(req_count))) {
        return NPT_FAILURE;
    }

    if (NPT_String(object_id) == m_containerRoot->m_ObjectID) {
        vector<RadioStation*>::iterator stationI;
        for (stationI = m_stationList.begin(); stationI != m_stationList.end(); stationI++) {
            PLT_MediaItem* object = new PLT_MediaItem();
            NPT_String objectId;
            PLT_MediaItemResource resource;
            pair<map<NPT_String, PLT_MediaItem*>::iterator, bool> ins;
            
            objectId = NPT_String((*stationI)->m_name.c_str());
            ins = m_itemCache.insert(make_pair(objectId, object));
            if (ins.second) {
                cerr << "WebMediaServer::audioToDidl() new channel: " << (char*)objectId << endl;
                object->m_Title = objectId;
                object->m_ObjectClass.type = "object.item";
                object->m_ParentID = "0";
                object->m_ObjectID = objectId;
                resource.m_Size = 0;
                resource.m_Uri = NPT_String((*stationI)->m_mrl.c_str());
                resource.m_ProtocolInfo = "http-get:*:audio/mpeg:*";
                object->m_ObjectClass.type = "object.item.audioItem.musicTrack";
                object->m_Resources.Add(resource);
            }
            else {
                delete object;
                object = (*ins.first).second;
            }
            
            NPT_String res;
            NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*object, filter, res));
            didl += res;
            numberReturned++;
            totalMatches++;
        }
    }
/*    else {
        // TODO: is that enough info for the leaves? look at the specs!
        PLT_MediaItem item;
        item.m_ObjectClass.type = "object.item";
        NPT_String tmp;
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(item, filter, tmp));
        didl += tmp;
        numberReturned = 1;
        totalMatches = 1;
    }*/
    didl += didl_footer;
    NPT_LOG_INFO(didl);

    NPT_CHECK_SEVERE(action->SetArgumentValue("Result", didl));
    stringstream stemp;
    stemp << numberReturned;
    NPT_CHECK_SEVERE(action->SetArgumentValue("NumberReturned", stemp.str().c_str()));
    stringstream stemp2;
    stemp2 << totalMatches;
    NPT_CHECK_SEVERE(action->SetArgumentValue("TotalMatches", stemp2.str().c_str()));
    NPT_CHECK_SEVERE(action->SetArgumentValue("UpdateId", "1"));

    return NPT_SUCCESS;
}


NPT_Result
WebMediaServer::OnSearch(PLT_ActionReference& /*action*/, const NPT_String& /*object_id*/, const NPT_String& /*searchCriteria*/, const NPT_HttpRequestContext& /*context*/)
{
    return NPT_SUCCESS;
}
