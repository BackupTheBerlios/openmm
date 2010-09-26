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
#include "UpnpAvServerPrivate.h"
#include "UpnpAvServerImpl.h"

namespace Omm {
namespace Av {


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


std::string
MediaItemServer::getProtocol()
{
    return "http-get:*";
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
    std::string mime = "*";
    if (prot.count() >= 3) {
        mime = prot[2];
    }
    std::string dlna = "*";
    if (prot.count() >= 4) {
        dlna = prot[3];
    }
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
            Log::instance()->upnpav().debug("range: " + range + " (start: " + Poco::NumberFormatter::format((long unsigned int)start) + ")");
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


std::string
StreamingResource::getAttributeName(int index)
{
    if (index == 0) {
        return "protocolInfo";
    }
    else if (index == 1) {
        return "size";
    }
}


std::string
StreamingResource::getAttributeValue(int index)
{
    if (index == 0) {
        return _pServer->getServerProtocol() + ":" + getMime() + ":" + getDlna();
    }
    else if (index == 1) {
        return Poco::NumberFormatter::format(getSize());
    }
}


std::string
StreamingResource::getAttributeValue(const std::string& name)
{
    if (name == "protocolInfo") {
        return _pServer->getServerProtocol() + ":" + getMime() + ":" + getDlna();
    }
    else if (name == "size") {
        return Poco::NumberFormatter::format(getSize());
    }
}


int
StreamingResource::getAttributeCount()
{
    // protocolInfo and size
    return 2;
}


StreamingMediaItem::StreamingMediaItem(StreamingMediaObject* pServer)
{
    _pServer = pServer;
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


AbstractMediaObject*
StreamingMediaObject::createChildObject()
{
    return new StreamingMediaItem(this);
}


std::string
StreamingMediaObject::getServerAddress()
{
//     std::clog << "StreamingMediaObject::getServerAddress()" << std::endl;
    
    std::string address = Omm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    int port = _pItemServer->_socket.address().port();
    return "http://" + address + ":" + Poco::NumberFormatter::format(port);
}


std::string
StreamingMediaObject::getServerProtocol()
{
    return _pItemServer->getProtocol();
}


TorchServer::TorchServer(int port) :
StreamingMediaObject(port),
_pChild(new TorchItem(this))
{
    _pChild->setParent(this);
}


TorchServer::~TorchServer()
{
    delete _pDataModel;
    delete _pTitleProp;
    delete _pChild;
}


void
TorchServer::setDataModel(AbstractDataModel* pDataModel)
{
    _pDataModel = pDataModel;
}


Omm::Av::AbstractMediaObject*
TorchServer::getChild(Omm::ui4 numChild)
{
    std::clog << "TorchServer::getChild()" << std::endl;
    
    _pChild->setObjectNumber(numChild);
    // FIXME: title property of child item should get it's title based on item's object number
    static_cast<TorchItem*>(_pChild)->_pTitleProp->setValue(_pDataModel->getTitle(numChild));
    
    return _pChild;
}


Omm::ui4
TorchServer::getChildCount()
{
    if (_pDataModel) {
        return _pDataModel->getChildCount();
    }
    else {
        return 0;
    }
}


bool
TorchServer::isContainer()
{
    return true;
}


int
TorchServer::getPropertyCount(const std::string& name)
{
    // only one property overall and one title property in particular
    if (name == "" || name == "dc:title") {
        return 1;
    }
    else {
        return 0;
    }
}


Omm::Av::AbstractProperty*
TorchServer::getProperty(int index)
{
    return _pTitleProp;
}


Omm::Av::AbstractProperty*
TorchServer::getProperty(const std::string& name, int index)
{
    if (name == "dc:title") {
        return _pTitleProp;
    }
    else {
        return 0;
    }
}


void
TorchServer::addProperty(Omm::Av::AbstractProperty* pProperty)
{
    _pTitleProp = pProperty;
}


Omm::Av::AbstractProperty*
TorchServer::createProperty()
{
    return new Omm::Av::MemoryProperty;
}


TorchItemResource::TorchItemResource(TorchServer* pServer, Omm::Av::AbstractMediaObject* pItem) :
StreamingResource(new MemoryPropertyImpl, pServer, pItem)
{
}


bool
TorchItemResource::isSeekable()
{
    AbstractDataModel* pDataModel = static_cast<TorchServer*>(_pServer)->_pDataModel;
    if (pDataModel) {
        return pDataModel->isSeekable(_pItem->getObjectNumber());
    }
    else {
        return false;
    }
}


std::streamsize
TorchItemResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    AbstractDataModel* pDataModel = static_cast<TorchServer*>(_pServer)->_pDataModel;
    if (pDataModel) {
        return pDataModel->stream(_pItem->getObjectNumber(), ostr, seek);
    }
    else {
        return 0;
    }
}


Omm::ui4
TorchItemResource::getSize()
{
    AbstractDataModel* pDataModel = static_cast<TorchServer*>(_pServer)->_pDataModel;
    if (pDataModel) {
        return pDataModel->getSize(_pItem->getObjectNumber());
    }
    else {
        return 0;
    }
}


std::string
TorchItemResource::getMime()
{
    AbstractDataModel* pDataModel = static_cast<TorchServer*>(_pServer)->_pDataModel;
    if (pDataModel) {
        return pDataModel->getMime(_pItem->getObjectNumber());
    }
    else {
        return "*";
    }
}


std::string
TorchItemResource::getDlna()
{
    AbstractDataModel* pDataModel = static_cast<TorchServer*>(_pServer)->_pDataModel;
    if (pDataModel) {
        return pDataModel->getDlna(_pItem->getObjectNumber());
    }
    else {
        return "*";
    }
}


void
TorchItemPropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
TorchItemPropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


std::string
TorchItemPropertyImpl::getName()
{
    std::clog << "TorchItemPropertyImpl::getName() returns: " << _name << std::endl;

    return _name;
}


std::string
TorchItemPropertyImpl::getValue()
{
    std::clog << "TorchItemPropertyImpl::getValue() returns: " << _value << std::endl;

    if (_name == "dc:title") {
        return _value;
    }
}


TorchItemProperty::TorchItemProperty() :
AbstractProperty(new TorchItemPropertyImpl)
{
}


TorchItem::TorchItem(TorchServer* pServer) :
StreamingMediaItem(pServer),
_pTitleProp(new TorchItemProperty),
_pResource(new TorchItemResource(pServer, this))
{
    std::clog << "TorchItem::TorchItem(pServer), pServer: " << pServer << std::endl;
    _pTitleProp->setName("dc:title");
}


TorchItem::~TorchItem()
{
    delete _pTitleProp;
}


int
TorchItem::getPropertyCount(const std::string& name)
{
    std::clog << "TorchItem::getPropertyCount(name), name: " << name << std::endl;
    
    if (name == "") {
        return 2;
    }
    else if (name == "dc:title" || name == "res") {
        return 1;
    }
    else {
        return 0;
    }
}


Omm::Av::AbstractProperty*
TorchItem::getProperty(int index)
{
    std::clog << "TorchItem::getProperty(index), index: " << index << std::endl;
    
    if (index == 0) {
        return _pTitleProp;
    }
    else if (index == 1) {
        return _pResource;
    }
}


Omm::Av::AbstractProperty*
TorchItem::getProperty(const std::string& name, int index)
{
    std::clog << "TorchItem::getProperty(name, index), name: " << name << ", index: " << index << std::endl;

    if (name == "dc:title") {
        return _pTitleProp;
    }
    else if (name == "res") {
        return _pResource;
    }
    else {
        return 0;
    }
}


} // namespace Av
} // namespace Omm
