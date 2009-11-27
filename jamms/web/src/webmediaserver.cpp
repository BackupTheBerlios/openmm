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

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/Path.h>
#include "Poco/StreamCopier.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/AttrMap.h"
#include "Poco/DOM/Element.h"
#include "Poco/Exception.h"

#include <iostream>
#include <fstream>

#include <cerrno>
#include <cstring>
#include <dirent.h>


// www.shoutcast.com/sbin/newxml.phtml

using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Path;
using Poco::StreamCopier;
using Poco::XML::DOMParser;
using Poco::XML::Document;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::Node;
using Poco::XML::NamedNodeMap;
using Poco::XML::AttrMap;
using Poco::XML::Element;
using Poco::XML::AutoPtr;
using Poco::Exception;

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
    
    m_containerShout = new PLT_MediaContainer();
    m_containerShout->m_Title = "Shoutcast";
    m_containerShout->m_ObjectClass.type = "object.container";
    // let m_ChildrenCount be > 0, otherwise xbmc doesn't display the container
    m_containerShout->m_ChildrenCount = 1;
    m_containerShout->m_ParentID = m_containerRoot->m_ObjectID;
    m_containerShout->m_ObjectID = "1";
    
    m_uriShout = URI("http://www.shoutcast.com/sbin/newxml.phtml");
    
            // download the shoutcast genres
    string path(m_uriShout.getPathAndQuery());
    if (path.empty()) path = "/";
    
    session = new HTTPClientSession(m_uriShout.getHost(), m_uriShout.getPort());
    /*
    HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    session.sendRequest(req);
    HTTPResponse res;
    istream& rs = session.receiveResponse(res);
    cout << res.getStatus() << " " << res.getReason() << endl;
    StreamCopier::copyStream(rs, m_xmlShoutGenre);
    */
    
    ifstream rs("/etc/jamm/genrelist.xml");
    StreamCopier::copyStream(rs, m_xmlShoutGenre);
}

WebMediaServer::~WebMediaServer()
{
}


NPT_String
WebMediaServer::genreToDidl(NPT_String filter, string genre)
{
    PLT_MediaItem* object = new PLT_MediaItem();
    NPT_String objectId;
    PLT_MediaItemResource resource;
    pair<map<NPT_String, PLT_MediaItem*>::iterator, bool> ins;
    
    objectId = "g=" + NPT_String(genre.c_str());
//     NPT_LOG_INFO(NPT_String(channel->Name()) + ": " + objectId);
    // check if item is already in itemCache
    ins = m_itemCache.insert(make_pair(objectId, object));
    if (ins.second) {
        cerr << "WebServer::genreToDidl() new genre object_id: " << (char*)objectId << endl;
        object->m_Title = NPT_String(genre.c_str());
        object->m_ObjectClass.type = "object.container";
        object->m_ParentID = m_containerShout->m_ObjectID;
        object->m_ObjectID = objectId;
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
WebMediaServer::stationToDidl(NPT_String filter, string parent_id, string name, string id)
{
    PLT_MediaItem* object = new PLT_MediaItem();
    NPT_String objectId;
    PLT_MediaItemResource resource;
    pair<map<NPT_String, PLT_MediaItem*>::iterator, bool> ins;
    
    objectId = "s=" + NPT_String(id.c_str());
//     NPT_LOG_INFO(NPT_String(channel->Name()) + ": " + objectId);
    // check if item is already in itemCache
    ins = m_itemCache.insert(make_pair(objectId, object));
    if (ins.second) {
        cerr << "WebServer::stationToDidl() new station object_id: " << (char*)objectId << endl;
        object->m_Title = NPT_String(name.c_str());
        object->m_ObjectClass.type = "object.item";
        object->m_ParentID = NPT_String(parent_id.c_str());
        object->m_ObjectID = objectId;
        resource.m_Size = 0;
        
        string path("http://" + m_uriShout.getHost() + "/sbin/tunein-station.pls?id=" + id);
//         HTTPClientSession session(m_uriShout.getHost(), m_uriShout.getPort());
        /*
        HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
        session->sendRequest(req);
        HTTPResponse res;
        istream& rs = session->receiveResponse(res);
        cout << res.getStatus() << " " << res.getReason() << endl;
        
        string mrl = "";
        while (mrl.substr(0, 4) != "File") {
            std::getline(rs, mrl);
        }
        mrl = mrl.substr(mrl.find("=")+1);
        
        resource.m_Uri = NPT_String(mrl.c_str());
        */
        if (id == "3495") {
            resource.m_Uri = NPT_String("http://streamer-dtc-aa04.somafm.com:80/stream/1018");
        }
        else if (id == "46573") {
            resource.m_Uri = NPT_String("http://scfire-ntc-aa01.stream.aol.com:80/stream/1035");
        }
        else if (id == "131273") {
            resource.m_Uri = NPT_String("http://streamer-ntc-aa02.somafm.com:80/stream/1073");
        }
        else if (id == "") {
        }
        else {
            resource.m_Uri = NPT_String(path.c_str());
        }        
        resource.m_ProtocolInfo = "http-get:*:audio/mp3:*";
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
WebMediaServer::OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& /*context*/)
{
//     NPT_LOG_INFO_1("Web BrowseMetadata Action on object_id = %s", object_id);
    NPT_String didl, tmp;
    NPT_String filter;
    NPT_CHECK_SEVERE(action->GetArgumentValue("Filter", filter));
    
    if (NPT_String(object_id) == m_containerRoot->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerRoot, filter, tmp));
    }
    else if (NPT_String(object_id) == m_containerShout->m_ObjectID) {
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerShout, filter, tmp));
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
        NPT_String tmp;
        NPT_CHECK_SEVERE(PLT_Didl::ToDidl(*m_containerShout, filter, tmp));
        didl += tmp;
        tmp = "";
        numberReturned = 1;
        totalMatches = 1;

    }
    else if (NPT_String(object_id) == m_containerShout->m_ObjectID) {
        // parse the downloaded XML and generate media containers
        m_xmlShoutGenre.clear();
        m_xmlShoutGenre.seekg(0, std::ios::beg);
        InputSource strsource(m_xmlShoutGenre);
        DOMParser parser;
        AutoPtr<Document> pDoc = parser.parse(&strsource);
        NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
        Node* pNode = it.nextNode();
        int i = 0;
        while (pNode)
        {
//             std::cout << pNode->nodeName() << ":" << pNode->nodeValue() << std::endl;
            if (i >= start_index && pNode->hasAttributes()) {
                string genre = static_cast<Element*>(pNode)->getAttribute("name");
                if (genre == "Ambient" or genre == "Indie") {
                    didl += genreToDidl(filter, genre);
                    numberReturned++;
                }
            }
            if (numberReturned >= req_count) {
                break;
            }
            pNode = it.nextNode();
            i++;
        }
        
        totalMatches = i;
        NPT_LOG_INFO_1("totalMatches = %i", totalMatches);
    }
    else if (NPT_String(object_id).StartsWith("g=")) {
        // download the shoutcast stationlist for this genre
        string genre = string(object_id).substr(2);
        string path(m_uriShout.getPathAndQuery() + string("?genre=") + genre);
        
        pair<map<string, stringstream*>::iterator, bool> ins;
        stringstream* station = new stringstream();
        ins = m_stationCache.insert(make_pair(genre, station));
        
        if (ins.second) {
//             HTTPClientSession session(m_uriShout.getHost(), m_uriShout.getPort());
            if (genre == "Ambient") {
                ifstream rs("/etc/jamm/shoutcast_genre_Ambient.xml");
                StreamCopier::copyStream(rs, *station);
            }
            else if (genre == "Indie") {
                ifstream rs("/etc/jamm/shoutcast_genre_genre_Indie.xml");
                StreamCopier::copyStream(rs, *station);
            }
            else {
                HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
                session->sendRequest(req);
                HTTPResponse res;
                istream& rs = session->receiveResponse(res);
                cout << res.getStatus() << " " << res.getReason() << endl;
                StreamCopier::copyStream(rs, *station);
            }
        }
        else {
            delete station;
            station = (*ins.first).second;
        }
        
        // parse the downloaded XML and generate media items
        station->clear();
        station->seekg(0, std::ios::beg);
        InputSource strsource(*station);
        DOMParser parser;
        AutoPtr<Document> pDoc = parser.parse(&strsource);
        NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
        Node* pNode = it.nextNode();
        int i = 0;
        while (pNode)
        {
    //             std::cout << pNode->nodeName() << ":" << pNode->nodeValue() << std::endl;
            if (i >= start_index && pNode->hasAttributes()) {
                Element* e = static_cast<Element*>(pNode);
                string stationName = e->getAttribute("name");
                string stationId = e->getAttribute("id");
                if (stationId == "3495" || stationId == "46573" || stationId == "131273") {
                    didl += stationToDidl(filter, string(object_id), stationName, stationId);
                    numberReturned++;
                }
            }
            if (numberReturned >= req_count) {
                break;
            }
            pNode = it.nextNode();
            i++;
        }
        
        totalMatches = i;
        NPT_LOG_INFO_1("totalMatches = %i", totalMatches);
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
