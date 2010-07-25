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

namespace Omm {
namespace Av {


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


WebResource::WebResource(const std::string& resourceId, const std::string& protInfo, const std::string& privateUri) :
ServerResource(resourceId, protInfo, 0),
_privateUri(privateUri)
{
}


std::streamsize
WebResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    Poco::URI uri(_privateUri);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    session.setKeepAlive(true);
    session.setKeepAliveTimeout(Poco::Timespan(3, 0));
    Poco::Timespan timeout = session.getKeepAliveTimeout();
    Log::instance()->upnpav().debug("web resource server proxy timeout is: " + Poco::NumberFormatter::format(timeout.seconds() + "sec"));
    Poco::Net::HTTPRequest proxyRequest("GET", uri.getPath());
    proxyRequest.setKeepAlive(true);
    session.sendRequest(proxyRequest);
    std::stringstream requestHeader;
    proxyRequest.write(requestHeader);
    Log::instance()->upnpav().debug("proxy request header:\n" + requestHeader.str());
    
    Poco::Net::HTTPResponse proxyResponse;
    std::istream& istr = session.receiveResponse(proxyResponse);
    
    if (istr.peek() == EOF) {
        Log::instance()->upnpav().error("error reading data from web resource");
    }
    else {
        Log::instance()->upnpav().debug("success reading data from web resource");
    }
    
    Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(proxyResponse.getStatus()) + " " + proxyResponse.getReason());
    std::stringstream responseHeader;
    proxyResponse.write(responseHeader);
    Log::instance()->upnpav().debug("proxy response header:\n" + responseHeader.str());
    
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
    Log::instance()->upnpav().information("media item server listening on: " + _socket.address().toString());
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
    Log::instance()->upnpav().debug("handle media item request: " + request.getURI());
    
    std::ostringstream requestHeader;
    request.write(requestHeader);
    Log::instance()->upnpav().debug("request method: " + request.getMethod());
    Log::instance()->upnpav().debug("request header:\n" + requestHeader.str());
    
    Poco::StringTokenizer uri(request.getURI(), "$");
    std::string objectId = uri[0].substr(1);
    int resourceId = Poco::NumberParser::parse(uri[1]);
    Log::instance()->upnpav().debug("objectId: " + objectId + ", resourceId: " + uri[1]);
    
    AbstractMediaObject* pItem = _pItemServer->_pServerContainer->getObject(objectId);
    StreamingResource* pResource = static_cast<StreamingResource*>(pItem->getResource(resourceId));

    std::string resProtInfo = pResource->getProtInfo();
    Log::instance()->upnpav().debug("protInfo: " + resProtInfo);
    Poco::StringTokenizer prot(resProtInfo, ":");
    std::string mime = prot[2];
    std::string dlna = prot[3];
    Log::instance()->upnpav().debug("protInfo mime: " + mime + ", dlna: " + dlna);
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
    Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());

    if (request.getMethod() == "GET") {
        Log::instance()->upnpav().debug("sending stream ...");
        std::ostream& ostr = response.send();
        std::iostream::pos_type start = 0;
        if (request.has("Range")) {
            std::string rangeVal = request.get("Range");
            std::string range = rangeVal.substr(rangeVal.find('=') + 1);
            
            std::string::size_type delim = range.find('-');
            start = Poco::NumberParser::parse(range.substr(0, delim));
            Log::instance()->upnpav().debug("range: " + range + " (start: " + Poco::NumberFormatter::format(start) + ")");
        }
        std::streamsize numBytes = pResource->stream(ostr, start);
        Log::instance()->upnpav().debug("stream sent (" + Poco::NumberFormatter::format(numBytes) + " bytes transfered).");
    }
    else if (request.getMethod() == "HEAD") {
        response.send();
    }
    
    if (response.sent()) {
        Log::instance()->upnpav().debug("media item request finished: " + request.getURI());
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
UpnpAvServer::setRoot(AbstractMediaObject* pRoot)
{
    _pRoot = pRoot;
    static_cast<ContentDirectoryImplementation*>(_pContentDirectoryImpl)->_pRoot = _pRoot;
//     _pRoot->setObjectId("0");
}


AbstractMediaObject*
UpnpAvServer::getRoot()
{
    return _pRoot;
}


StreamingResource::StreamingResource(PropertyImpl* pPropertyImpl, StreamingMediaObject* pServer, AbstractMediaObject* pItem) :
AbstractResource(pPropertyImpl),
_pServer(pServer),
_pItem(pItem),
_id(0)
{
    std::clog << "StreamingResource::StreamingResource(pServer, pItem), pServer: " << pServer << ", pItem: " << pItem << std::endl;
}


std::string
StreamingResource::getValue()
{
    std::clog << "StreamingResourceImpl::getValue()" << std::endl;
    
    std::string serverAddress = _pServer->getServerAddress();
    std::string relativeObjectId = _pItem->getObjectId().substr(_pServer->getObjectId().length()+1);
    std::string resourceId = Poco::NumberFormatter::format(_id);
    return serverAddress + "/" + relativeObjectId + "$" + resourceId;
}


StreamingMediaObject::StreamingMediaObject(int port)
{
    std::clog << "StreamingMediaObject::StreamingMediaObject(port), port: " << port << std::endl;
    
    _pItemServer = new MediaItemServer(port);
    _pItemServer->_pServerContainer = this;
    _pItemServer->start();
}


StreamingMediaObject::~StreamingMediaObject()
{
    _pItemServer->stop();
    delete _pItemServer;
}


std::string
StreamingMediaObject::getServerAddress()
{
    std::clog << "StreamingMediaObject::getServerAddress()" << std::endl;
    
    std::string address = Omm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    int port = _pItemServer->_socket.address().port();
    return "http://" + address + ":" + Poco::NumberFormatter::format(port);
}



} // namespace Av
} // namespace Omm
