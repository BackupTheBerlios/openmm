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

#include "PltMediaItem.h"
#include "PltDidl.h"
#include "PltUPnPHelper.h"

#include "vdrmediaserver.h"
#include "vdr/channels.h"
#include "vdr/recording.h"
#include "streamdev/server/setup.h"

NPT_SET_LOCAL_LOGGER("vdr.plugin.upnp");

VdrMediaServer::VdrMediaServer(const char*  friendly_name,
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
    m_localPort = StreamdevServerSetup.HTTPServerPort;
}

VdrMediaServer::~VdrMediaServer()
{
}


NPT_Result
VdrMediaServer::OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context)
{
    NPT_LOG_INFO_1("VDR BrowseMetadata Action on object_id = %s", object_id);
}


NPT_Result
VdrMediaServer::OnBrowseDirectChildren(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context)
{
    NPT_LOG_INFO_1("VDR BrowseDirectChildren Action on object_id = %s", object_id);
    NPT_String didl = didl_header;

    NPT_String filter;
    NPT_String startingInd;
    NPT_String reqCount;
    int numberReturned;
    int totalMatches;
    unsigned long start_index, req_count;

    NPT_CHECK_SEVERE(action->GetArgumentValue("Filter", filter));
    NPT_CHECK_SEVERE(action->GetArgumentValue("StartingIndex", startingInd));
    NPT_CHECK_SEVERE(action->GetArgumentValue("RequestedCount", reqCount));
    if (NPT_FAILED(startingInd.ToInteger(start_index)) ||
        NPT_FAILED(reqCount.ToInteger(req_count))) {
        return NPT_FAILURE;
    }

    if (NPT_String(object_id) == "0") {
        PLT_MediaContainer *containerLiveTv = new PLT_MediaContainer();
        containerLiveTv->m_Title = "Live TV";
        containerLiveTv->m_ObjectClass.type = "object.container";
        containerLiveTv->m_ChildrenCount = 0;
        containerLiveTv->m_ParentID = "0";
        containerLiveTv->m_ObjectID = "1";
    
        NPT_String tmp;
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*containerLiveTv, filter, tmp));
        didl += tmp;
    
        PLT_MediaContainer *containerRecordings = new PLT_MediaContainer();
        containerRecordings->m_Title = "Recordings";
        containerRecordings->m_ObjectClass.type = "object.container";
        containerRecordings->m_ChildrenCount = 0;
        containerRecordings->m_ParentID = "0";
        containerRecordings->m_ObjectID = "2";
    
        tmp = "";
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*containerRecordings, filter, tmp));
        didl += tmp;
        numberReturned = 2;
        totalMatches = 2;

    }
    else if (NPT_String(object_id) == "1") {
        numberReturned = 0;
        totalMatches = 0;

        for (cChannel *chan = Channels.First(); chan; chan = Channels.Next(chan)) {
            if (!chan) break;
            PLT_MediaItem object;
            PLT_MediaItemResource resource;
            NPT_String tmp;

            NPT_LOG_INFO(NPT_String(chan->Name()) + ": " + chan->GetChannelID().ToString());
            object.m_Title = chan->Name();
            object.m_ParentID = "1";
            object.m_ObjectID = chan->GetChannelID().ToString();
            resource.m_Size = 0;
            resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_localPort) + "/") + chan->GetChannelID().ToString();
            resource.m_ProtocolInfo = "http-get:*:video/mpeg:*";
            object.m_ObjectClass.type = "object.item.videoItem.movie";
            object.m_Resources.Add(resource);

            NPT_CHECK_SEVERE(PLT_Didl::ToDidl(object, filter, tmp));
            didl += tmp;
            numberReturned++;
            totalMatches++;
        }
    }
    else if (NPT_String(object_id) == "2") {
        numberReturned = 0;
        totalMatches = 0;

        for (cRecording *rec = Recordings.First(); rec; rec = Recordings.Next(rec)) {
            if (!rec) break;
            PLT_MediaItem object;
            PLT_MediaItemResource resource;
            NPT_String tmp;

            NPT_LOG_INFO(NPT_String(rec->Name()) + ": " + rec->FileName());
            object.m_Title = rec->Name();
            object.m_ParentID = "1";
            object.m_ObjectID = rec->FileName();
            resource.m_Size = 0;
            resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_localPort) + "/") + rec->FileName();
            resource.m_ProtocolInfo = "http-get:*:video/mpeg:*";
            object.m_ObjectClass.type = "object.item.videoItem.movie";
            object.m_Resources.Add(resource);

            NPT_CHECK_SEVERE(PLT_Didl::ToDidl(object, filter, tmp));
            didl += tmp;
            numberReturned++;
            totalMatches++;
        }
    }

    didl += didl_footer;

    NPT_LOG_INFO(didl);

    NPT_CHECK_SEVERE(action->SetArgumentValue("Result", didl));
    NPT_CHECK_SEVERE(action->SetArgumentValue("NumberReturned", itoa(numberReturned)));
    NPT_CHECK_SEVERE(action->SetArgumentValue("TotalMatches", itoa(totalMatches)));
    NPT_CHECK_SEVERE(action->SetArgumentValue("UpdateId", "1"));

    return NPT_SUCCESS;
}


NPT_Result
VdrMediaServer::OnSearch(PLT_ActionReference& action, const NPT_String& object_id, const NPT_String& searchCriteria, const NPT_HttpRequestContext& context)
{
}
