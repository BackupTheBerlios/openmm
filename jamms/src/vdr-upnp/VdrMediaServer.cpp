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

#include <vdr/channels.h>
#include <vdr/recording.h>

#include "VdrMediaServer.h"


VdrMediaTree::VdrMediaTree()
{
    setTitle("Digital TV");
    
    m_pChannels = new VdrChannels;
    m_pChannels->setTitle("Live TV");
    appendChild("chan", m_pChannels);
    
    m_pRecordings = new VdrRecordings;
    m_pRecordings->setTitle("Recordings");
    appendChild("rec", m_pRecordings);
}


VdrMediaTree::~VdrMediaTree()
{
}


VdrChannels::VdrChannels() :
m_streamDevPort("3000")
{
    std::string localIp =  Jamm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    
    for (cChannel* pChan = Channels.First(); pChan != Channels.Last(); pChan = Channels.Next(pChan)) {
        if (!pChan || pChan->Vpid() == 0 || pChan->GetChannelID().ToString() == "0-0-0-0") {
            continue;
        }
        
        std::string objectId(pChan->GetChannelID().ToString());
        std::string title(pChan->Name());
        std::string resource = "http://" + localIp + ":" + m_streamDevPort + "/PES/" + objectId;
        appendChild(objectId, new Jamm::Av::MediaItem(title, resource));
    }
}


VdrRecordings::VdrRecordings()
{
    startHttpServer();
    setupTree();
}


void
VdrRecordings::setupTree()
{
    std::clog << "VdrRecordings::VdrRecordings() number of recordings: " << Recordings.Count() << std::endl;
    
    m_serverAddress =  Jamm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    
    for (cRecording* pRec = Recordings.First(); pRec/* != Recordings.Last()*/; pRec = Recordings.Next(pRec)) {
        std::clog << "*** Add Recording ***" << std::endl;
        if (!pRec) {
            continue;
        }
        std::clog << "Recording Name: " << pRec->Name() << std::endl;
        
        std::string objectId(pRec->Info()->ChannelID().ToString());
        objectId += "_" + Poco::NumberFormatter::format(pRec->start);
        std::clog << "Recording ObjectID: " << objectId << std::endl;
        
        std::string title(pRec->Name());
        std::string resource = "http://" +
            m_serverAddress + ":" +
            Poco::NumberFormatter::format(m_serverPort) + "/" + objectId;
        std::clog << "Recording resource: " << resource << std::endl;
        
        std::clog << "Recording file name: " << pRec->FileName() << std::endl;
        m_pFileServer->registerFile(objectId, std::string(pRec->FileName()) + "/001.vdr");
        
        appendChild(objectId, new Jamm::Av::MediaItem(title, resource));
    }
    
    std::clog << "VdrRecordings::VdrRecordings() finished" << std::endl;
    
//         MultiFileInputStream* m = new MultiFileInputStream(rec->FileName() + string("/"));
//         m_recCache.insert(make_pair(objectId, NPT_InputStreamReference(m)));
    
// //         resource.m_ProtocolInfo = "http-get:*:video/mpeg:*";
//         resource.m_ProtocolInfo = "http-get:*:video/mpeg:DLNA.ORG_PS=1;DLNA.ORG_CI=0;DLNA.ORG_OP=01;DLNA.ORG_PN=MPEG_ES_PAL;DLNA.ORG_FLAGS=01700000000000000000000000000000";
//         object->m_ObjectClass.type = "object.item.videoItem.movie";
}


void
VdrRecordings::startHttpServer()
{
    std::clog << "http file server starting ..." << std::endl;
    m_pFileServer = new Jamm::HttpFileServer;
    m_pFileServer->start();
    m_serverPort = m_pFileServer->getPort();
    std::clog << "http file server running on port: " << m_pFileServer->getPort() << std::endl;
}










// 
// NPT_Result 
// VdrMediaTree::ProcessHttpRequest(NPT_HttpRequest&              request,
//                                    const NPT_HttpRequestContext& context,
//                                    NPT_HttpResponse&             response)
// {
//     NPT_LOG_INFO_1("HttpRequest Path: %s", (char*)request.GetUrl().GetPath());
//     
//     if (request.GetUrl().GetPath().StartsWith("/vdr-rec")) {
//             return ProcessFileRequest(request, context, response);
//         }
//     
//     return PLT_MediaServer::ProcessHttpRequest(request, context, response);
// }
// 
// 
// NPT_Result 
// VdrMediaTree::ProcessFileRequest(NPT_HttpRequest&              request,
//                                    const NPT_HttpRequestContext& context,
//                                    NPT_HttpResponse&             response)
// {
//     NPT_LOG_INFO("ProcessFileRequest Received Request:");
//     PLT_LOG_HTTP_MESSAGE(NPT_LOG_LEVEL_INFO, &request);
//     
//     response.GetHeaders().SetHeader("Accept-Ranges", "bytes");
//     
//     if (request.GetMethod().Compare("GET") && request.GetMethod().Compare("HEAD")) {
//         response.SetStatus(500, "Internal Server Error");
//         return NPT_SUCCESS;
//     }
//     
//     // Extract uri path from url
// //     NPT_String uri_path = NPT_Uri::PercentDecode(request.GetUrl().GetPath());
// //     NPT_String uri_path = request.GetUrl().GetPath();
//     NPT_String file_path = request.GetUrl().GetPath();
//     file_path = file_path.Right(file_path.GetLength() - 1);
//     NPT_LOG_INFO_1("FileRequest file_path: %s", (char*)file_path);
//     
// /*    // extract file path from query
//     NPT_HttpUrlQuery query(request.GetUrl().GetQuery());
//     NPT_String file_path = query.GetField("path");*/
//     
//     // hack for XBMC support for 360, we urlencoded the ? to that the 360 doesn't strip out the query
//     // but then the query ends being parsed as part of the path
// /*    int index = uri_path.Find("path=");
//     if (index>0) file_path = uri_path.Right(uri_path.GetLength()-index-5);*/
//     if (file_path.GetLength() == 0){
//         goto failure;
//     }
//     
//     NPT_Position start, end;
//     PLT_HttpHelper::GetRange(request, start, end);
//     
//     return ServeFile(response,
//                     file_path,
//                     start,
//                     end,
//                     !request.GetMethod().Compare("HEAD"));
// 
//     
// failure:
//     response.SetStatus(404, "File Not Found");
//     return NPT_SUCCESS;
// }
// 
// 
// NPT_Result 
// VdrMediaTree::ServeFile(NPT_HttpResponse& response,
//                           NPT_String        file_path,
//                           NPT_Position      start,
//                           NPT_Position      end,
//                           bool              request_is_head) 
// {
//     cerr << "VdrMediaTree::ServeFile() path: " << (char*)file_path << ", start: " << start << ", end: " << end << endl;
//     NPT_LargeSize            total_len;
//     NPT_InputStreamReference stream;
//     NPT_File                 file(file_path);
//     NPT_Result               result;
//     
//     // prevent hackers from accessing files outside of our root
//     if ((file_path.Find("/..") >= 0) || (file_path.Find("\\..") >= 0)) {
//         return NPT_FAILURE;
//     }
//     
//     if (file_path.EndsWith("/")) {
//         cerr << "VdrMediaTree::ServeFile() multistream: " << (const char*)file_path << endl;
//         map<NPT_String, NPT_InputStreamReference>::iterator i = m_recCache.find((const char*)file_path);
//         if (i != m_recCache.end()) {
//             stream = (*i).second;
//         }
//         else {
//             cerr << "VdrMediaTree::ServeFile() could not find multistream" << endl;
//             response.SetStatus(404, "File Not Found");
//             return NPT_SUCCESS;
//         }
//     }
//     else {
//         if (NPT_FAILED(result = file.Open(NPT_FILE_OPEN_MODE_READ)) || 
//             NPT_FAILED(result = file.GetInputStream(stream))) {
//             response.SetStatus(404, "File Not Found");
//             return NPT_SUCCESS;
//         }
//     }
//     
//     if (/*NPT_FAILED(result = file.Open(NPT_FILE_OPEN_MODE_READ)) || 
//         NPT_FAILED(result = file.GetInputStream(stream))        ||*/
//         NPT_FAILED(result = stream->GetSize(total_len))) {
//         // file didn't open
//             response.SetStatus(404, "File Not Found");
//             return NPT_SUCCESS;
//         } else {
//             NPT_HttpEntity* entity = new NPT_HttpEntity();
//             entity->SetContentLength(total_len);
//             response.SetEntity(entity);
//             entity->SetContentType("video/mpeg");
//         // request is HEAD, returns without setting a body
//             if (request_is_head) return NPT_SUCCESS;
//             
//         // see if it was a byte range request
//             if (start != (NPT_Position)-1 || end != (NPT_Position)-1) {
//             // we can only support a range from an offset to the end of the resource for now
//             // due to the fact we can't limit how much to read from a stream yet
//                 NPT_Position start_offset = (NPT_Position)-1, end_offset = total_len - 1, len;
//                 if (start == (NPT_Position)-1 && end != (NPT_Position)-1) {
//                 // we are asked for the last N=end bytes
//                 // adjust according to total length
//                     if (end >= total_len) {
//                         start_offset = 0;
//                     } else {
//                         start_offset = total_len-end;
//                     }
//                 } else if (start != (NPT_Position)-1) {
//                     start_offset = start;
//                 // if the end is specified but incorrect
//                 // set the end_offset in order to generate a bad response
//                     if (end != (NPT_Position)-1 && end < start) {
//                         end_offset = (NPT_Position)-1;
//                     }
//                 }
//                 
//             // in case the range request was invalid or we can't seek then respond appropriately
//                 if (start_offset == (NPT_Position)-1 || end_offset == (NPT_Position)-1 || 
//                     start_offset > end_offset || NPT_FAILED(stream->Seek(start_offset))) {
//                         response.SetStatus(416, "Requested range not satisfiable");
//                     } else {
//                         len = end_offset - start_offset + 1;
//                         response.SetStatus(206, "Partial Content");
//                         PLT_HttpHelper::SetContentRange(response, start_offset, end_offset, total_len);
//                         
//                         entity->SetInputStream(stream);
//                         entity->SetContentLength(len);
//                     }
//         // no byte range request
//             } else {
//                 entity->SetInputStream(stream); 
//             }
//             return NPT_SUCCESS;
//         }
// }
// 
// 
// MultiFileInputStream::MultiFileInputStream(string path)
// : m_totalSize(0)
// {
//     cerr << "MultiFileInputStream::MultiFileInputStream()" << endl;
//     // TODO: open files later, when they are accessed and store filename in m_streams
//     DIR* dir;
//     struct dirent* dirent;
//     dir = opendir(path.c_str());
//     if (!dir)
//     {
//         cerr << "opendir() on " << path << " failed: " << strerror(errno) << endl;
//         return;
//     }
//     vector<string> fnames;
//     while ((dirent = readdir(dir))) {
//         string fname = string(dirent->d_name);
// //         if (/*fname != "." && fname != ".." && */fname.rfind(".vdr") != string::npos && fname[0] == '0') {
//         if (fname[0] == '0' && fname.substr(fname.size() - 4) == ".vdr") {
//             fnames.push_back(fname);
//         }
//     }
//     sort(fnames.begin(), fnames.end());
//     for (vector<string>::iterator i = fnames.begin(); i != fnames.end(); ++i) {
//         string fname = *i;
//         struct stat statval;
//         if (stat ((path + fname).c_str(), &statval) != 0) {
//             cerr << "stat() failed on " + fname + ": " + strerror (errno) << endl;
//         }
//         if (S_ISREG (statval.st_mode)) {
//             FILE* s = fopen((path + fname).c_str(), "rb");
//             if (ferror(s)) {
//                 perror("MultiFileInputStream::MultiFileInputStream() opening file");
//             }
//             m_totalSize += statval.st_size;
//             m_streams.insert(make_pair(m_totalSize, s));
//             cerr << "MultiFileInputStream::MultiFileInputStream() file: " << path + fname << ", size: " << statval.st_size << endl;
//         }
//     }
//     m_currentStream = m_streams.begin();
// }
// 
// 
// MultiFileInputStream::~MultiFileInputStream()
// {
//     cerr << "MultiFileInputStream::~MultiFileInputStream()" << endl;
//     // close all files
//    for (map<long long, FILE*>::iterator i = m_streams.begin(); i != m_streams.end(); ++i) {
//        fclose((*i).second);
//     }
//     m_streams.clear();
// }
// 
// 
// NPT_Result
// MultiFileInputStream::Read(void*  buffer,
//                         NPT_Size  bytes_to_read,
//                         NPT_Size* bytes_read)
// {
//     // obviously, if *bytes_read < bytes_to_read, same 4k packet is read again (from same offset with a seek?)
//     NPT_Size bytes_read_attempt = 0;
//     do {
//         FILE* s = (*m_currentStream).second;
//         bytes_read_attempt = fread((char*)buffer + bytes_read_attempt, 1, bytes_to_read, s);
//         *bytes_read += bytes_read_attempt;
//         bytes_to_read -= bytes_read_attempt;
//         if (feof(s)) {
//             m_currentStream++;
//             rewind((*m_currentStream).second);
//             cerr << "****** MultiFileInputStream::Read() switch to next FILE: " << (*m_currentStream).second << " ******" << endl;
//         }
//         if (ferror(s)) {
//             perror("MultiFileInputStream::Read error");
//             clearerr(s);
//             return NPT_FAILURE;
//         }
//     } while(bytes_to_read > 0);
//     return NPT_SUCCESS;
// }
// 
// 
// // Possible implementation with m_streams.upper_bound ...
// // NPT_Result
// // MultiFileInputStream::Seek(NPT_Position offset)
// // {
// //     cerr << "MultiFileInputStream::Seek() to offset: " << offset << endl;
// //     map<long long, FILE*>::iterator i = m_streams.begin();
// //     i = m_streams.upper_bound(offset);
// //     long long startOffset = (i == m_streams.begin()) ? 0 : (*(--i)).first;
// //     if (fseek((*i).second, offset - startOffset, SEEK_SET)) {
// //         perror("MultiFileInputStream::Seek()");
// //         return NPT_FAILURE;
// //     }
// //     else {
// //         m_currentStream = i;
// //         return NPT_SUCCESS;
// //     }
// // }
// 
// 
// NPT_Result
// MultiFileInputStream::Seek(NPT_Position offset)
// {
//     cerr << "MultiFileInputStream::Seek() to offset: " << offset << endl;
//     map<long long, FILE*>::iterator i = m_streams.begin();
//     int counter = 0;
//     long long startOffset = 0;
//     while (i != m_streams.end() && (*i).first < offset) {
//         cerr << "MultiFileInputStream::Seek() counter: " << counter << ", start: " << startOffset << ", end: " << (*i).first << endl;
//         startOffset = (*i).first;
//         ++i;
//         ++counter;
//     }
//     if (fseek((*i).second, offset - startOffset, SEEK_SET)) {
//         perror("MultiFileInputStream::Seek()");
//         return NPT_FAILURE;
//     }
//     else {
//         cerr << "MultiFileInputStream::Seek() currentStream: " << counter << endl;
//         m_currentStream = i;
//         return NPT_SUCCESS;
//     }
// }
// 
// 
// NPT_Result
// MultiFileInputStream::Tell(NPT_Position& offset)
// {
//     cerr << "MultiFileInputStream::Tell()" << endl;
//     long curOffset = ftell((*m_currentStream).second);
//     if (curOffset == -1) {
//         perror("MultiFileInputStream::Tell()");
//         return NPT_FAILURE;
//     }
//     offset = curOffset + (*m_currentStream).first;
//     return NPT_SUCCESS;
// }
// 
// 
// NPT_Result
// MultiFileInputStream::GetSize(NPT_LargeSize& size)
// {
//     cerr << "MultiFileInputStream::GetSize(): " << m_totalSize << endl;
//     size = m_totalSize;
//     return NPT_SUCCESS;
// }
// 
// 
// NPT_Result
// MultiFileInputStream::GetAvailable(NPT_LargeSize& available)
// {
//     cerr << "MultiFileInputStream::GetAvailable()" << endl;
//     NPT_Result res = Tell(available);
//     available = m_totalSize - available;
//     return res;
// }
