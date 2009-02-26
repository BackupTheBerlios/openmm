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

#include "vdrmediaserver.h"
// #include "streamdev/server/setup.h"

#include <platinum/PltMediaItem.h>
#include <platinum/PltDidl.h>
#include <platinum/PltUPnPHelper.h>
#include <platinum/PltHttpServer.h>

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
//     m_localPort = StreamdevServerSetup.HTTPServerPort;
    m_localPort = 3000;
    m_recPort = GetPort();
    
    m_containerRoot = new PLT_MediaContainer();
    // TODO: what to do, when browsing the root object "0"?
    m_containerRoot->m_Title = "";
    m_containerRoot->m_ObjectClass.type = "object.container";
    m_containerRoot->m_ChildrenCount = 0;
    m_containerRoot->m_ParentID = "0";
    m_containerRoot->m_ObjectID = "0";
    
    m_containerLiveTv = new PLT_MediaContainer();
    m_containerLiveTv->m_Title = "Live TV";
    m_containerLiveTv->m_ObjectClass.type = "object.container";
    m_containerLiveTv->m_ChildrenCount = 0;
    m_containerLiveTv->m_ParentID = m_containerRoot->m_ObjectID;
    m_containerLiveTv->m_ObjectID = "1";
    
    m_containerRecordings = new PLT_MediaContainer();
    m_containerRecordings->m_Title = "Recordings";
    m_containerRecordings->m_ObjectClass.type = "object.container";
    m_containerRecordings->m_ChildrenCount = 0;
    m_containerRecordings->m_ParentID = m_containerRoot->m_ObjectID;
    m_containerRecordings->m_ObjectID = "2";
}

VdrMediaServer::~VdrMediaServer()
{
}


NPT_String
VdrMediaServer::channelToDidl(NPT_String filter, cChannel *channel)
{
    PLT_MediaItem* object = new PLT_MediaItem();
    NPT_String objectId;
    PLT_MediaItemResource resource;
    pair<map<NPT_String, PLT_MediaItem*>::iterator, bool> ins;
    
    objectId = channel->GetChannelID().ToString();
//     NPT_LOG_INFO(NPT_String(channel->Name()) + ": " + objectId);
    // check if item is already in itemCache
    ins = m_itemCache.insert(make_pair(objectId, object));
    if (ins.second) {
        object->m_Title = channel->Name();
        object->m_ObjectClass.type = "object.item";
        object->m_ParentID = m_containerLiveTv->m_ObjectID;
        object->m_ObjectID = objectId;
        resource.m_Size = 0;
        resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_localPort) + "/PES/") + objectId;
        resource.m_ProtocolInfo = "http-get:*:video/mpeg:*";
        object->m_ObjectClass.type = "object.item.videoItem.movie";
        object->m_Resources.Add(resource);
    }
    else {
        delete object;
        object = (*ins.first).second;
    }
    
    NPT_String res;
    NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*object, filter, res));
    return res;
}


NPT_String
VdrMediaServer::recToDidl(NPT_String filter, cRecording *rec)
{
    PLT_MediaItem* object = new PLT_MediaItem();
    NPT_String objectId;
    PLT_MediaItemResource resource;
    pair<map<NPT_String, PLT_MediaItem*>::iterator, bool> ins;
    
    objectId = rec->FileName();
//     NPT_LOG_INFO(NPT_String(rec->Name()) + ": " + objectId);
    // check if item is already in itemCache
    ins = m_itemCache.insert(make_pair(objectId, object));
    if (ins.second) {
        object->m_Title = rec->Name();
        object->m_ObjectClass.type = "object.item";
        object->m_ParentID = m_containerRecordings->m_ObjectID;
        object->m_ObjectID = objectId;
        resource.m_Size = 0;
        resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_recPort) + "/") + objectId + "/001.vdr";
        resource.m_ProtocolInfo = "http-get:*:video/mpeg:*";
        object->m_ObjectClass.type = "object.item.videoItem.movie";
        object->m_Resources.Add(resource);
    }
    else {
        delete object;
        object = (*ins.first).second;
    }
    
    NPT_String res;
    NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*object, filter, res));
    return res;
}


NPT_Result
VdrMediaServer::OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& /*context*/)
{
//     NPT_LOG_INFO_1("VDR BrowseMetadata Action on object_id = %s", object_id);
    NPT_String didl, tmp;
    NPT_String filter;
    NPT_CHECK_SEVERE(action->GetArgumentValue("Filter", filter));
    
    if (NPT_String(object_id) == m_containerRoot->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerRoot, filter, tmp));
    }
    else if (NPT_String(object_id) == m_containerLiveTv->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerLiveTv, filter, tmp));
    }
    else if (NPT_String(object_id) == m_containerRecordings->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerRecordings, filter, tmp));
    }
    else {
        PLT_MediaItem* item;
        map<NPT_String, PLT_MediaItem*>::iterator i = m_itemCache.find(NPT_String(object_id));
        if (i == m_itemCache.end()) {
            NPT_LOG_INFO_1("VDR BrowseMetadata NOT IN CACHE: object_id = %s", object_id);
            return NPT_FAILURE;
        }
        item = (*i).second;
        if (item == NULL){
            NPT_LOG_INFO_1("VDR BrowseMetadata NULL OBJECT IN CACHE: object_id = %s", object_id);
            return NPT_FAILURE;
        }
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*item, filter, tmp));
    }
    didl = didl_header + tmp + didl_footer;
//     NPT_LOG_INFO(didl);
    
    NPT_CHECK_SEVERE(action->SetArgumentValue("Result", didl));
    NPT_CHECK_SEVERE(action->SetArgumentValue("NumberReturned", "1"));
    NPT_CHECK_SEVERE(action->SetArgumentValue("TotalMatches", "1"));
    
    // update ID may be wrong here, it should be the one of the container?
    NPT_CHECK_SEVERE(action->SetArgumentValue("UpdateId", "1"));
    // TODO: We need to keep track of the overall updateID of the CDS
    
    return NPT_SUCCESS;
}


NPT_Result
VdrMediaServer::OnBrowseDirectChildren(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& /*context*/)
{
//     NPT_LOG_INFO_1("VDR BrowseDirectChildren Action on object_id = %s", object_id);
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
        NPT_String tmp;
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerLiveTv, filter, tmp));
        didl += tmp;
        tmp = "";
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerRecordings, filter, tmp));
        didl += tmp;
        numberReturned = 2;
        totalMatches = 2;

    }
    else if (NPT_String(object_id) == m_containerLiveTv->m_ObjectID) {
        for (cChannel *chan = Channels.GetByNumber(start_index, 1); chan; chan = Channels.Next(chan)) {
            if (!chan) {
                break;
            }
            NPT_LOG_INFO(NPT_String(chan->Name()));
            // TODO: need to convert it to string or is there something like a NULL-ChannelID?
            if (chan->GetChannelID().ToString() == "0-0-0-0") continue;
            didl += channelToDidl(filter, chan);
            numberReturned++;
            totalMatches = Channels.MaxNumber();
            NPT_LOG_INFO_1("totalMatches = %i", totalMatches);
            if (numberReturned >= req_count) {
                break;
            }
        }
    }
    else if (NPT_String(object_id) == m_containerRecordings->m_ObjectID) {
        for (cRecording *rec = Recordings.Get(start_index); rec; rec = Recordings.Next(rec)) {
            if (!rec) {
                break;
            }
            didl += recToDidl(filter, rec);
            numberReturned++;
            totalMatches = Recordings.Count();
            if (numberReturned >= req_count) {
                break;
            }
        }
    }
    else {
        // TODO: is that enough info for the leaves? look at the specs!
        PLT_MediaItem item;
        item.m_ObjectClass.type = "object.item";
        NPT_String tmp;
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(item, filter, tmp));
        didl += tmp;
        numberReturned = 1;
        totalMatches = 1;
    }
    didl += didl_footer;
//     NPT_LOG_INFO(didl);

    NPT_CHECK_SEVERE(action->SetArgumentValue("Result", didl));
    NPT_CHECK_SEVERE(action->SetArgumentValue("NumberReturned", itoa(numberReturned)));
    NPT_CHECK_SEVERE(action->SetArgumentValue("TotalMatches", itoa(totalMatches)));
    NPT_CHECK_SEVERE(action->SetArgumentValue("UpdateId", "1"));

    return NPT_SUCCESS;
}


NPT_Result
VdrMediaServer::OnSearch(PLT_ActionReference& /*action*/, const NPT_String& /*object_id*/, const NPT_String& /*searchCriteria*/, const NPT_HttpRequestContext& /*context*/)
{
    return NPT_SUCCESS;
}


NPT_Result 
VdrMediaServer::ProcessHttpRequest(NPT_HttpRequest&              request,
                                   const NPT_HttpRequestContext& context,
                                   NPT_HttpResponse&             response)
{
    NPT_LOG_INFO_1("HttpRequest Path: %s", (char*)request.GetUrl().GetPath());
    
    if (request.GetUrl().GetPath().StartsWith("//")) {
            return ProcessFileRequest(request, context, response);
        }
    
    return PLT_MediaServer::ProcessHttpRequest(request, context, response);
}


NPT_Result 
VdrMediaServer::ProcessFileRequest(NPT_HttpRequest&              request,
                                   const NPT_HttpRequestContext& context,
                                   NPT_HttpResponse&             response)
{
    NPT_LOG_INFO("ProcessFileRequest Received Request:");
    PLT_LOG_HTTP_MESSAGE(NPT_LOG_LEVEL_INFO, &request);
    
    response.GetHeaders().SetHeader("Accept-Ranges", "bytes");
    
    if (request.GetMethod().Compare("GET") && request.GetMethod().Compare("HEAD")) {
        response.SetStatus(500, "Internal Server Error");
        return NPT_SUCCESS;
    }
    
    // Extract uri path from url
//     NPT_String uri_path = NPT_Uri::PercentDecode(request.GetUrl().GetPath());
//     NPT_String uri_path = request.GetUrl().GetPath();
    NPT_String file_path = request.GetUrl().GetPath();
    NPT_LOG_INFO_1("FileRequest file_path: %s", (char*)file_path);
    
/*    // extract file path from query
    NPT_HttpUrlQuery query(request.GetUrl().GetQuery());
    NPT_String file_path = query.GetField("path");*/
    
    // hack for XBMC support for 360, we urlencoded the ? to that the 360 doesn't strip out the query
    // but then the query ends being parsed as part of the path
/*    int index = uri_path.Find("path=");
    if (index>0) file_path = uri_path.Right(uri_path.GetLength()-index-5);*/
    if (file_path.GetLength() == 0){
        goto failure;
    }
    
    NPT_Position start, end;
    PLT_HttpHelper::GetRange(request, start, end);
    
    return PLT_FileServer::ServeFile(response,
                                         /*m_Path +*/ file_path,
                                     start,
                                     end,
                                     !request.GetMethod().Compare("HEAD"));
    
    
/*    NPT_CHECK_LABEL_WARNING(ServeFile(request,
                                      context,
                                      response,
                                      uri_path,
                                      file_path),
                            failure);*/
    
failure:
    response.SetStatus(404, "File Not Found");
    return NPT_SUCCESS;
}


NPT_Result 
VdrMediaServer::ServeFile(NPT_HttpRequest&              request, 
                               const NPT_HttpRequestContext& /*context*/,
                               NPT_HttpResponse&             response,
                               NPT_String                    uri_path,
                               NPT_String                    file_path)
{
    // prevent hackers from accessing files outside of our root
    if ((file_path.Find("/..") >= 0) || (file_path.Find("\\..") >= 0)) {
        return NPT_FAILURE;
    }
    
/*    // File requested
    NPT_String path = "/";
    if (path.Compare(uri_path.Left(path.GetLength()), true) == 0) {*/
        
        NPT_Position start, end;
        PLT_HttpHelper::GetRange(request, start, end);
        
        return PLT_FileServer::ServeFile(response,
                                         /*m_Path +*/ file_path,
                                         start,
                                         end,
                                         !request.GetMethod().Compare("HEAD"));
//     } 
    
    return NPT_FAILURE;
}
