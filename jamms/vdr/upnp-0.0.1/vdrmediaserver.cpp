/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
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

#include <cerrno>
#include <cstring>
#include <dirent.h>

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
    m_liveTvPort = 3000;
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
        cerr << "VdrMediaServer::channelToDidl() new channel: " << (char*)objectId << endl;
        object->m_Title = channel->Name();
        object->m_ObjectClass.type = "object.item";
        object->m_ParentID = m_containerLiveTv->m_ObjectID;
        object->m_ObjectID = objectId;
        resource.m_Size = 0;
        resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_liveTvPort) + "/PES/") + objectId;
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
    
    objectId = rec->FileName() + NPT_String("/");
//     NPT_LOG_INFO(NPT_String(rec->Name()) + ": " + objectId);
    // check if item is already in itemCache
    ins = m_itemCache.insert(make_pair(objectId, object));
    if (ins.second) {
        cerr << "VdrMediaServer::recToDidl() new multistream: " << (char*)objectId << endl;
        MultiFileInputStream* m = new MultiFileInputStream(rec->FileName() + string("/"));
        m_recCache.insert(make_pair(objectId, NPT_InputStreamReference(m)));
        object->m_Title = rec->Name();
        object->m_ObjectClass.type = "object.item";
        object->m_ParentID = m_containerRecordings->m_ObjectID;
        object->m_ObjectID = objectId;
        m->GetSize(resource.m_Size);
        resource.m_Uri = NPT_String("http://" + m_localIp + ":" + itoa(m_recPort) + "/") + objectId;
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
    file_path = file_path.Right(file_path.GetLength() - 1);
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
    
    return ServeFile(response,
                    file_path,
                    start,
                    end,
                    !request.GetMethod().Compare("HEAD"));

    
failure:
    response.SetStatus(404, "File Not Found");
    return NPT_SUCCESS;
}


NPT_Result 
VdrMediaServer::ServeFile(NPT_HttpResponse& response,
                          NPT_String        file_path,
                          NPT_Position      start,
                          NPT_Position      end,
                          bool              request_is_head) 
{
    cerr << "VdrMediaServer::ServeFile() path: " << (char*)file_path << ", start: " << start << ", end: " << end << endl;
    NPT_LargeSize            total_len;
    NPT_InputStreamReference stream;
    NPT_File                 file(file_path);
    NPT_Result               result;
    
    // prevent hackers from accessing files outside of our root
    if ((file_path.Find("/..") >= 0) || (file_path.Find("\\..") >= 0)) {
        return NPT_FAILURE;
    }
    
    if (file_path.EndsWith("/")) {
        cerr << "VdrMediaServer::ServeFile() multistream: " << (const char*)file_path << endl;
        map<NPT_String, NPT_InputStreamReference>::iterator i = m_recCache.find((const char*)file_path);
        if (i != m_recCache.end()) {
            stream = (*i).second;
        }
        else {
            cerr << "VdrMediaServer::ServeFile() could not find multistream" << endl;
            response.SetStatus(404, "File Not Found");
            return NPT_SUCCESS;
        }
    }
    else {
        if (NPT_FAILED(result = file.Open(NPT_FILE_OPEN_MODE_READ)) || 
            NPT_FAILED(result = file.GetInputStream(stream))) {
            response.SetStatus(404, "File Not Found");
            return NPT_SUCCESS;
        }
    }
    
    if (/*NPT_FAILED(result = file.Open(NPT_FILE_OPEN_MODE_READ)) || 
        NPT_FAILED(result = file.GetInputStream(stream))        ||*/
        NPT_FAILED(result = stream->GetSize(total_len))) {
        // file didn't open
            response.SetStatus(404, "File Not Found");
            return NPT_SUCCESS;
        } else {
            NPT_HttpEntity* entity = new NPT_HttpEntity();
            entity->SetContentLength(total_len);
            response.SetEntity(entity);
            entity->SetContentType("video/mpeg");
        // request is HEAD, returns without setting a body
            if (request_is_head) return NPT_SUCCESS;
            
        // see if it was a byte range request
            if (start != (NPT_Position)-1 || end != (NPT_Position)-1) {
            // we can only support a range from an offset to the end of the resource for now
            // due to the fact we can't limit how much to read from a stream yet
                NPT_Position start_offset = (NPT_Position)-1, end_offset = total_len - 1, len;
                if (start == (NPT_Position)-1 && end != (NPT_Position)-1) {
                // we are asked for the last N=end bytes
                // adjust according to total length
                    if (end >= total_len) {
                        start_offset = 0;
                    } else {
                        start_offset = total_len-end;
                    }
                } else if (start != (NPT_Position)-1) {
                    start_offset = start;
                // if the end is specified but incorrect
                // set the end_offset in order to generate a bad response
                    if (end != (NPT_Position)-1 && end < start) {
                        end_offset = (NPT_Position)-1;
                    }
                }
                
            // in case the range request was invalid or we can't seek then respond appropriately
                if (start_offset == (NPT_Position)-1 || end_offset == (NPT_Position)-1 || 
                    start_offset > end_offset || NPT_FAILED(stream->Seek(start_offset))) {
                        response.SetStatus(416, "Requested range not satisfiable");
                    } else {
                        len = end_offset - start_offset + 1;
                        response.SetStatus(206, "Partial Content");
                        PLT_HttpHelper::SetContentRange(response, start_offset, end_offset, total_len);
                        
                        entity->SetInputStream(stream);
                        entity->SetContentLength(len);
                    }
        // no byte range request
            } else {
                entity->SetInputStream(stream); 
            }
            return NPT_SUCCESS;
        }
}


MultiFileInputStream::MultiFileInputStream(string path)
: m_totalSize(0)
{
    cerr << "MultiFileInputStream::MultiFileInputStream()" << endl;
    // TODO: open files later, when they are accessed and store filename in m_streams
    DIR* dir;
    struct dirent* dirent;
    dir = opendir(path.c_str());
    if (!dir)
    {
        cerr << "opendir() on " << path << " failed: " << strerror(errno) << endl;
        exit(1);
    }
    vector<string> fnames;
    while ((dirent = readdir(dir))) {
        string fname = string(dirent->d_name);
//         if (/*fname != "." && fname != ".." && */fname.rfind(".vdr") != string::npos && fname[0] == '0') {
        if (fname[0] == '0' && fname.substr(fname.size() - 4) == ".vdr") {
            fnames.push_back(fname);
        }
    }
    sort(fnames.begin(), fnames.end());
    for (vector<string>::iterator i = fnames.begin(); i != fnames.end(); ++i) {
        string fname = *i;
        struct stat statval;
        if (stat ((path + fname).c_str(), &statval) != 0) {
            cerr << "stat() failed on " + fname + ": " + strerror (errno) << endl;
        }
        if (S_ISREG (statval.st_mode)) {
            FILE* s = fopen((path + fname).c_str(), "rb");
            if (ferror(s)) {
                perror("MultiFileInputStream::MultiFileInputStream() opening file");
            }
            m_totalSize += statval.st_size;
            m_streams.insert(make_pair(m_totalSize, s));
            cerr << "MultiFileInputStream::MultiFileInputStream() file: " << path + fname << ", size: " << statval.st_size << endl;
        }
    }
    m_currentStream = m_streams.begin();
}


MultiFileInputStream::~MultiFileInputStream()
{
    cerr << "MultiFileInputStream::~MultiFileInputStream()" << endl;
    // close all files
   for (map<long long, FILE*>::iterator i = m_streams.begin(); i != m_streams.end(); ++i) {
       fclose((*i).second);
    }
    m_streams.clear();
}


NPT_Result
MultiFileInputStream::Read(void*  buffer,
                        NPT_Size  bytes_to_read,
                        NPT_Size* bytes_read)
{
    // obviously, if *bytes_read < bytes_to_read, same 4k packet is read again (from same offset with a seek?)
//     cerr << "MultiFileInputStream::Read()" << endl;
    NPT_Size bytes_read_attempt = 0;
    do {
        FILE* s = (*m_currentStream).second;
        bytes_read_attempt = fread((char*)buffer + bytes_read_attempt, 1, bytes_to_read, s);
        *bytes_read += bytes_read_attempt;
//         cerr << "MultiFileInputStream::Read() to_read: " << bytes_to_read << " read: " << *bytes_read << " FILE: " << s << endl;
        bytes_to_read -= bytes_read_attempt;
        if (feof(s)) {
            m_currentStream++;
            rewind((*m_currentStream).second);
            cerr << "****** MultiFileInputStream::Read() switch to next FILE: " << (*m_currentStream).second << " ******" << endl;
        }
        if (ferror(s)) {
            perror("MultiFileInputStream::Read error");
            clearerr(s);
            return NPT_FAILURE;
        }
    } while(bytes_to_read > 0);
    return NPT_SUCCESS;
}


// Possible implementation with m_streams.upper_bound ...
// NPT_Result
// MultiFileInputStream::Seek(NPT_Position offset)
// {
//     cerr << "MultiFileInputStream::Seek() to offset: " << offset << endl;
//     map<long long, FILE*>::iterator i = m_streams.begin();
//     i = m_streams.upper_bound(offset);
//     long long startOffset = (i == m_streams.begin()) ? 0 : (*(--i)).first;
//     if (fseek((*i).second, offset - startOffset, SEEK_SET)) {
//         perror("MultiFileInputStream::Seek()");
//         return NPT_FAILURE;
//     }
//     else {
//         m_currentStream = i;
//         return NPT_SUCCESS;
//     }
// }


NPT_Result
MultiFileInputStream::Seek(NPT_Position offset)
{
    cerr << "MultiFileInputStream::Seek() to offset: " << offset << endl;
    map<long long, FILE*>::iterator i = m_streams.begin();
    int counter = 0;
    long long startOffset = 0;
    while (i != m_streams.end() && (*i).first < offset) {
        cerr << "MultiFileInputStream::Seek() counter: " << counter << ", start: " << startOffset << ", end: " << (*i).first << endl;
        startOffset = (*i).first;
        ++i;
        ++counter;
    }
    if (fseek((*i).second, offset - startOffset, SEEK_SET)) {
        perror("MultiFileInputStream::Seek()");
        return NPT_FAILURE;
    }
    else {
        cerr << "MultiFileInputStream::Seek() currentStream: " << counter << endl;
        m_currentStream = i;
        return NPT_SUCCESS;
    }
}


NPT_Result
MultiFileInputStream::Tell(NPT_Position& offset)
{
    cerr << "MultiFileInputStream::Tell()" << endl;
    long curOffset = ftell((*m_currentStream).second);
    if (curOffset == -1) {
        perror("MultiFileInputStream::Tell()");
        return NPT_FAILURE;
    }
    offset = curOffset + (*m_currentStream).first;
    return NPT_SUCCESS;
}


NPT_Result
MultiFileInputStream::GetSize(NPT_LargeSize& size)
{
    cerr << "MultiFileInputStream::GetSize(): " << m_totalSize << endl;
    size = m_totalSize;
    return NPT_SUCCESS;
}


NPT_Result
MultiFileInputStream::GetAvailable(NPT_LargeSize& available)
{
    cerr << "MultiFileInputStream::GetAvailable()" << endl;
    NPT_Result res = Tell(available);
    available = m_totalSize - available;
    return res;
}
