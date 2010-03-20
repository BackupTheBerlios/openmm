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

#include <Poco/File.h>

#include "UpnpAvServer.h"
#include "UpnpAvServerImpl.h"

using namespace Jamm;
using namespace Jamm::Av;


MediaItemServer::MediaItemServer() :
m_socket(Poco::Net::ServerSocket(0))
{
    
}


MediaItemServer::~MediaItemServer()
{
    Log::instance()->upnpav().information("stopping media item server ...");
    m_pHttpServer->stop();
    delete m_pHttpServer;
    Log::instance()->upnpav().information("done");
}


void
MediaItemServer::start()
{
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    m_pHttpServer = new Poco::Net::HTTPServer(new FileRequestHandlerFactory(this), m_socket, pParams);
    m_pHttpServer->start();
    Log::instance()->upnpav().information(Poco::format("media item server listening on: %s", m_socket.address().toString()));
}


void
MediaItemServer::stop()
{
    m_pHttpServer->stop();
}


Poco::UInt16
MediaItemServer::getPort() const
{
    return m_socket.address().port();
}


// void
// MediaItemServer::registerFile(const std::string& uri, const std::string& path)
// {
//     m_uriPathMap["/" + uri] = path;
// }


void
MediaItemServer::registerMediaItem(const std::string& relObjectId, MediaItem* pMediaItem, const std::string& privateUri)
{
    m_itemMap["/" + relObjectId] = pMediaItem;
    m_privateUriMap["/" + relObjectId] = privateUri;
}


FileRequestHandlerFactory::FileRequestHandlerFactory(MediaItemServer* pFileServer) :
m_pFileServer(pFileServer)
{
}


Poco::Net::HTTPRequestHandler*
FileRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new FileRequestHandler(m_pFileServer);
}


FileRequestHandler::FileRequestHandler(MediaItemServer* pFileServer) :
m_pFileServer(pFileServer)
{
}


void
FileRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->upnpav().debug(Poco::format("handle media item request: %s", request.getURI()));
    
    std::ostringstream requestHeader;
    request.write(requestHeader);
    Log::instance()->upnpav().debug(Poco::format("request method: %s", request.getMethod()));
    Log::instance()->upnpav().debug(Poco::format("request header:\n%s", requestHeader.str()));
    
    MediaItem* pItem = m_pFileServer->m_itemMap[request.getURI()];
    Resource* pRes = pItem->getResource(0);
//     Poco::URI resUri(pRes->m_uri);
    Poco::URI resUri(m_pFileServer->m_privateUriMap[request.getURI()]);
    std::string resProtInfo = pRes->m_protInfo;
    Poco::StringTokenizer prot(resProtInfo, ":");
    std::string mime = prot[2];
    std::string dlna = prot[3];
    ui4 resSize = pRes->m_size;
    
    response.set("transferMode.dlna.org", "Streaming");
    response.set("EXT", "");
    response.set("Server", Poco::Environment::osName() + "/"
                 + Poco::Environment::osVersion()
                 + " UPnP/" + UPNP_VERSION + " Jamm/" + JAMM_VERSION);
    response.setDate(Poco::Timestamp());
    
    response.set("Accept-Ranges", "bytes");
    if (resSize > 0 ) {
        response.setContentLength(resSize);
    }
    if (request.has("getMediaInfo.sec")) {
        response.setContentType(mime);
    }
    if (request.has("getcontentFeatures.dlna.org")) {
        response.set("contentFeatures.dlna.org", dlna);
    }
    
    std::ostringstream responseHeader;
    response.write(responseHeader);
    Log::instance()->upnpav().debug(Poco::format("response header:\n%s", responseHeader.str()));
    
    if (request.getMethod() == "GET") {
//         std::istream istr;
        Log::instance()->upnpav().debug(Poco::format("sending stream: %s ...", resUri.toString()));
        std::ostream& ostr = response.send();
        if (resUri.getScheme() == "file") {
            Log::instance()->upnpav().debug(Poco::format("opening file: %s ...", resUri.getPath()));
            std::ifstream istr(resUri.getPath().c_str());
            if (request.has("Range")) {
                std::string rangeVal = request.get("Range");
                std::string range = rangeVal.substr(rangeVal.find('=') + 1);
                
                std::string::size_type delim = range.find('-');
                std::iostream::pos_type start = Poco::NumberParser::parse(range.substr(0, delim));
//             std::iostream::pos_type end = Poco::NumberParser::parse(range.substr(delim + 1));
                Log::instance()->upnpav().debug(Poco::format("range: %s (start: %s)", range, Poco::NumberFormatter::format(start)));
                if (start > 0) {
                    istr.seekg(start);
                }
            }
            Poco::StreamCopier::copyStream(istr, ostr);
        }
        else if (resUri.getScheme() == "http") {
            Poco::Net::HTTPClientSession session(resUri.getHost(), resUri.getPort());
            Poco::Net::HTTPRequest proxyRequest("GET", resUri.getPath());
            session.sendRequest(proxyRequest);
            Poco::Net::HTTPResponse proxyResponse;
            std::istream& istr = session.receiveResponse(proxyResponse);
            Log::instance()->upnpav().information(Poco::format("HTTP %s %s", Poco::NumberFormatter::format(proxyResponse.getStatus()), proxyResponse.getReason()));
            std::stringstream header;
            proxyResponse.write(header);
            Log::instance()->upnpav().debug(Poco::format("proxy response header:\n%s", header.str()));
            if (request.has("Range")) {
                proxyRequest.set("Range", request.get("Range"));
            }
            Poco::StreamCopier::copyStream(istr, ostr);
        }
        Log::instance()->upnpav().debug("stream sent.");
    }
    else if (request.getMethod() == "HEAD") {
        response.send();
    }
    
    if (response.sent()) {
        Log::instance()->upnpav().debug(Poco::format("media item request finished: %s", request.getURI()));
    }
}


UpnpAvServer::UpnpAvServer() :
MediaServer(
new ContentDirectoryImplementation,
new ConnectionManagerImplementation,
new AVTransportImplementation
)
{
}

void
UpnpAvServer::setRoot(MediaObject* pRoot)
{
    m_pRoot = pRoot;
    static_cast<ContentDirectoryImplementation*>(m_pContentDirectoryImpl)->m_pRoot = m_pRoot;
    m_pRoot->setObjectId("0");
    m_pRoot->setParentId("-1");
}


MediaObject*
UpnpAvServer::getRoot()
{
    return m_pRoot;
}
