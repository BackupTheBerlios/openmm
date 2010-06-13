/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
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

using namespace Omm;
using namespace Omm::Av;


ServerResource::ServerResource(const std::string& resourceId, const std::string& protInfo, ui4 size) :
Resource("", protInfo, size),
_resourceId(resourceId)
{
}


const std::string&
ServerResource::getResourceId()
{
    return _resourceId;
}


FileResource::FileResource(const std::string& resourceId, const std::string& protInfo, ui4 size, const std::string& privateUri) :
ServerResource(resourceId, protInfo, size),
_privateUri(privateUri)
{
}


std::streamsize
FileResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
//     std::string path = _pFileObjectSource->_basePath + "/" + _privateUri;
    std::string path = _privateUri;
    
    std::ifstream istr(path.c_str());
    if (seek > 0) {
        istr.seekg(seek);
    }
    return Poco::StreamCopier::copyStream(istr, ostr);
}


WebResource::WebResource(const std::string& resourceId, const std::string& protInfo, const std::string& privateUri) :
ServerResource(resourceId, protInfo, 0),
_privateUri(privateUri)
{
}


// FIXME: vdr streamdev-server does this frequently:
// 21:19:16.627 arthur[3581,12] D UPNP.AV sending stream: http://192.168.178.23:3000/TS/S19.2E-1-1107-17500 ...
// 21:19:16.628 arthur[3581,12] I UPNP.AV HTTP 409 Channel not available
// 21:19:16.628 arthur[3581,12] D UPNP.AV proxy response header:
// HTTP/1.0 409 Channel not available
// (409 = HTTP_CONFLICT)


std::streamsize
WebResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    Poco::URI uri(_privateUri);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    session.setKeepAlive(true);
    session.setKeepAliveTimeout(Poco::Timespan(3, 0));
    Poco::Timespan timeout = session.getKeepAliveTimeout();
    Log::instance()->upnpav().debug(Poco::format("web resource server proxy timeout is: %i sec", timeout.seconds()));
    Poco::Net::HTTPRequest proxyRequest("GET", uri.getPath());
    proxyRequest.setKeepAlive(true);
    session.sendRequest(proxyRequest);
    std::stringstream requestHeader;
    proxyRequest.write(requestHeader);
    Log::instance()->upnpav().debug(Poco::format("proxy request header:\n%s", requestHeader.str()));
    
    Poco::Net::HTTPResponse proxyResponse;
    std::istream& istr = session.receiveResponse(proxyResponse);
    
    if (istr.peek() == EOF) {
        Log::instance()->upnpav().error("error reading data from web resource");
    }
    else {
        Log::instance()->upnpav().debug("success reading data from web resource");
    }
    
    Log::instance()->upnpav().information(Poco::format("HTTP %s %s", Poco::NumberFormatter::format(proxyResponse.getStatus()), proxyResponse.getReason()));
    std::stringstream responseHeader;
    proxyResponse.write(responseHeader);
    Log::instance()->upnpav().debug(Poco::format("proxy response header:\n%s", responseHeader.str()));
    
    return Poco::StreamCopier::copyStream(istr, ostr);
}


MediaItemServer::MediaItemServer(int port) :
_socket(Poco::Net::ServerSocket(port))
{
}


MediaItemServer::~MediaItemServer()
{
    Log::instance()->upnpav().information("stopping media item server ...");
    _pHttpServer->stop();
    delete _pHttpServer;
    Log::instance()->upnpav().information("done");
}


void
MediaItemServer::start()
{
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    _pHttpServer = new Poco::Net::HTTPServer(new ItemRequestHandlerFactory(this), _socket, pParams);
    _pHttpServer->start();
    Log::instance()->upnpav().information(Poco::format("media item server listening on: %s", _socket.address().toString()));
}


void
MediaItemServer::stop()
{
    _pHttpServer->stop();
}


Poco::UInt16
MediaItemServer::getPort() const
{
    return _socket.address().port();
}


MediaServerContainer::MediaServerContainer(const std::string& title, const std::string& subClass, int port) :
MediaContainer(title, subClass)
{
    _pItemServer = new MediaItemServer(port);
    _pItemServer->_pServerContainer = this;
    _pItemServer->start();
    _port = _pItemServer->_socket.address().port();
    _address =  Omm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
}


MediaServerContainer::~MediaServerContainer()
{
    _pItemServer->stop();
    delete _pItemServer;
}


void
MediaServerContainer::appendChild(ServerObject* pChild)
{
    ServerObject::appendChild(pChild);
    
    // FIXME: writing the public uri should be moved somewhere else, probably into the XML writer of ServerObject
    // then appendChild() can be removed from MediaServerContainer
    for (MediaObject::ResourceIterator it = pChild->beginResource(); it != pChild->endResource(); ++it) {
        (*it)->setUri(getServerAddress() + "/" + (*it)->getUri());
    }
}


std::string
MediaServerContainer::getServerAddress()
{
    return "http://" + _address + ":" + Poco::NumberFormatter::format(_port);
}


ServerObject::ServerObject() :
MediaObject()
{
}


void
ServerObject::appendChild(ServerObject* pChild)
{
    MediaObject::appendChild(pChild);
    _childrenMap[pChild->_objectId] = pChild;
}


ServerObject*
ServerObject::getObject(const std::string& objectId)
{
//     std::clog << "ServerObject::getObject() objectId: " << objectId << std::endl;
    std::string::size_type slashPos = objectId.find('/');
    ServerObject* pChild;
    if (slashPos != std::string::npos) {
//         std::clog << "container id: " << objectId.substr(0, slashPos - 1) << std::endl;
        pChild = _childrenMap[objectId.substr(0, slashPos)];
        if (pChild == NULL) {
            Log::instance()->upnpav().error("child objectId of container, but no child container found");
            return NULL;
        }
        else {
            return pChild->getObject(objectId.substr(slashPos + 1));
        }
    }
    else {
//         std::clog << "item id: " << objectId << std::endl;
        pChild = _childrenMap[objectId];
        if (pChild == NULL) {
            Log::instance()->upnpav().error("no child item found");
            return NULL;
        }
        else {
            return pChild;
        }
    }
}


void
ServerObject::addResource(ServerResource* pResource)
{
    MediaObject::addResource(pResource);
    _resourceMap[pResource->getResourceId()] = pResource;
    pResource->setUri(_objectId + "$" + pResource->getResourceId());
    pResource->setProtInfo("http-get:*:" + pResource->getProtInfo());
}


ServerResource*
ServerObject::getResource(const std::string& resourceId)
{
    return _resourceMap[resourceId];
}


MediaContainer::MediaContainer() :
ServerObject()
{
    _isContainer = true;
}


MediaContainer::MediaContainer(const std::string& title, const std::string& subClass) :
ServerObject()
{
    _isContainer = true;
    setTitle(title);
    _properties["upnp:class"] = std::string("object.container" + (subClass == "" ? "" : "." + subClass));
}


MediaItem::MediaItem() :
ServerObject()
{
}


MediaItem::MediaItem(const std::string& objectId, const std::string& title, const std::string& subClass)
{
    _objectId = objectId;
    setTitle(title);
    _properties["upnp:class"] = std::string("object.item" + (subClass == "" ? "" : "." + subClass));
}


ItemRequestHandlerFactory::ItemRequestHandlerFactory(MediaItemServer* pItemServer) :
_pItemServer(pItemServer)
{
}


Poco::Net::HTTPRequestHandler*
ItemRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new ItemRequestHandler(_pItemServer);
}


ItemRequestHandler::ItemRequestHandler(MediaItemServer* pItemServer) :
_pItemServer(pItemServer)
{
}


void
ItemRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->upnpav().debug(Poco::format("handle media item request: %s", request.getURI()));
    
    std::ostringstream requestHeader;
    request.write(requestHeader);
    Log::instance()->upnpav().debug(Poco::format("request method: %s", request.getMethod()));
    Log::instance()->upnpav().debug(Poco::format("request header:\n%s", requestHeader.str()));
    
    Poco::StringTokenizer uri(request.getURI(), "$");
    std::string objectId = uri[0].substr(1);
    std::string resourceId = uri[1];
    Log::instance()->upnpav().debug(Poco::format("objectId: %s, resourceId: %s", objectId, resourceId));
    
    // TODO: check if pItem really is a MediaItem and not a MediaContainer?
    ServerObject* pItem = _pItemServer->_pServerContainer->getObject(objectId);
    ServerResource* pResource = pItem->getResource(resourceId);

    std::string resProtInfo = pResource->getProtInfo();
    Log::instance()->upnpav().debug(Poco::format("protInfo: %s", resProtInfo));
    Poco::StringTokenizer prot(resProtInfo, ":");
    std::string mime = prot[2];
    std::string dlna = prot[3];
    Log::instance()->upnpav().debug(Poco::format("protInfo mime: %s, dlna: %s", mime, dlna));
    ui4 resSize = pResource->getSize();
    
    response.setContentType(mime);
    response.set("transferMode.dlna.org", "Streaming");
    response.set("EXT", "");
    response.set("Server", Poco::Environment::osName() + "/"
                 + Poco::Environment::osVersion()
                 + " UPnP/" + UPNP_VERSION + " Omm/" + OMM_VERSION);
    response.setDate(Poco::Timestamp());
    
    if (pResource->isSeekable()) {
        response.set("Accept-Ranges", "bytes");
    }
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
        Log::instance()->upnpav().debug("sending stream ...");
        std::ostream& ostr = response.send();
        std::iostream::pos_type start = 0;
        if (request.has("Range")) {
            std::string rangeVal = request.get("Range");
            std::string range = rangeVal.substr(rangeVal.find('=') + 1);
            
            std::string::size_type delim = range.find('-');
            start = Poco::NumberParser::parse(range.substr(0, delim));
            Log::instance()->upnpav().debug(Poco::format("range: %s (start: %s)", range, Poco::NumberFormatter::format(start)));
        }
        std::streamsize numBytes = pResource->stream(ostr, start);
        Log::instance()->upnpav().debug(Poco::format("stream sent (%s bytes transfered).", Poco::NumberFormatter::format(numBytes)));
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
UpnpAvServer::setRoot(ServerObject* pRoot)
{
    _pRoot = pRoot;
    static_cast<ContentDirectoryImplementation*>(_pContentDirectoryImpl)->_pRoot = _pRoot;
    _pRoot->setObjectId("0");
}


ServerObject*
UpnpAvServer::getRoot()
{
    return _pRoot;
}
