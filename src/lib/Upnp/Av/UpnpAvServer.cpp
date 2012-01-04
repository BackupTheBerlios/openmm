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

#include <sstream>
#include <fstream>

#include <Poco/File.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Exception.h>
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"

#include <map>

#include "UpnpAvServer.h"
#include "UpnpInternal.h"
#include "UpnpAvDevices.h"
#include "UpnpAvDescriptions.h"
//#include "UpnpAvServerPrivate.h"
#include "UpnpAvServerImpl.h"

namespace Omm {
namespace Av {


MediaServer::MediaServer(int port) :
_socket(Poco::Net::ServerSocket(port))
{
    MediaServerDescriptions mediaServerDescriptions;
    MemoryDescriptionReader descriptionReader(mediaServerDescriptions);
    descriptionReader.getDeviceDescription();
    setDeviceData(descriptionReader.rootDeviceData());

    // service implementations are owned by DevMediaServer (DevDeviceCode)
    // DevDeviceCode should be owned by super class Device

    // store a pointer to content directory implementation, so that the
    // root object can be set later.
    _pDevContentDirectoryServerImpl = new DevContentDirectoryServerImpl;

    setDevDeviceCode(new DevMediaServer(
        _pDevContentDirectoryServerImpl,
        new DevConnectionManagerServerImpl,
        new DevAVTransportServerImpl)
    );
}


MediaServer::~MediaServer()
{
    Log::instance()->upnpav().information("stopping media server ...");
    _pHttpServer->stop();
    delete _pHttpServer;
    Log::instance()->upnpav().information("done");
}


void
MediaServer::setRoot(ServerContainer* pRoot)
{
    _pDevContentDirectoryServerImpl->_pRoot = pRoot;
    start();
}


void
MediaServer::start()
{
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    _pHttpServer = new Poco::Net::HTTPServer(new ItemRequestHandlerFactory(this), _socket, pParams);
    _pHttpServer->start();
    Log::instance()->upnpav().information("media item server listening on: " + _socket.address().toString());
}


void
MediaServer::stop()
{
    _pHttpServer->stop();
}


Poco::UInt16
MediaServer::getPort() const
{
    return _socket.address().port();
}


//std::string
//MediaServer::getProtocol()
//{
//    return "http-get:*";
//}


std::string
MediaServer::getServerAddress()
{
//    Log::instance()->upnpav().debug("streaming media object get server address ...");

    std::string address = Net::NetworkInterfaceManager::instance()->getValidIpAddress().toString();
//    int port = _pItemServer->_socket.address().port();
//    Log::instance()->upnpav().debug("streaming media object g et server address returns: http://" + address + ":" + Poco::NumberFormatter::format(port));
    return "http://" + address + ":" + Poco::NumberFormatter::format(getPort());
}


std::string
MediaServer::getServerProtocol()
{
    return "http-get:*";
//    return getProtocol();
}


//MediaItemServer::MediaItemServer(int port) :
//_socket(Poco::Net::ServerSocket(port))
//{
//}


//MediaItemServer::~MediaItemServer()
//{
//    Log::instance()->upnpav().information("stopping media item server ...");
//    _pHttpServer->stop();
//    delete _pHttpServer;
//    Log::instance()->upnpav().information("done");
//}


//void
//MediaItemServer::start()
//{
//    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
//    _pHttpServer = new Poco::Net::HTTPServer(new ItemRequestHandlerFactory(this), _socket, pParams);
//    _pHttpServer->start();
//    Log::instance()->upnpav().information("media item server listening on: " + _socket.address().toString());
//}
//
//
//void
//MediaItemServer::stop()
//{
//    _pHttpServer->stop();
//}
//
//
//Poco::UInt16
//MediaItemServer::getPort() const
//{
//    return _socket.address().port();
//}
//
//
//std::string
//MediaItemServer::getProtocol()
//{
//    return "http-get:*";
//}


ItemRequestHandlerFactory::ItemRequestHandlerFactory(MediaServer* pItemServer) :
_pItemServer(pItemServer)
{
}


Poco::Net::HTTPRequestHandler*
ItemRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new ItemRequestHandler(_pItemServer);
}


ItemRequestHandler::ItemRequestHandler(MediaServer* pItemServer) :
_bufferSize(8192),
_pItemServer(pItemServer)
{
}


void
ItemRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->upnpav().debug("handle media item request: " + request.getURI());

    response.setDate(Poco::Timestamp());

    std::ostringstream requestHeader;
    request.write(requestHeader);
    Log::instance()->upnpav().debug("request method: " + request.getMethod());
    Log::instance()->upnpav().debug("request header:\n" + requestHeader.str());

    Poco::StringTokenizer uri(request.getURI(), "$");
    std::string objectId = uri[0].substr(1);
    Log::instance()->upnpav().debug("objectId: " + objectId + ", resourceId: " + uri[1]);
    AbstractMediaObject* pItem = _pItemServer->_pServerContainer->getDescendant(objectId);
    if (!pItem) {
        Log::instance()->upnpav().error("item request handler could not find object with id: " + objectId);
        return;
    }

    ServerResource* pResource;
    ServerObjectProperty* pProperty;
    std::streamsize resSize = 0;
    if (uri[1] == "i") {
        // object icon requested by controller
        Log::instance()->upnpav().debug("icon request: server creates icon property");
        pProperty = static_cast<ServerObjectProperty*>(pItem->getProperty(AvProperty::ICON));
        Log::instance()->upnpav().debug("icon request: server icon property created");
        // deliver icon
        if (pProperty) {
            // icon requested (handle other streamable properties as well, such as album art ...)
            try {
                std::istream* pIstr = pProperty->getStream();
                if (pIstr) {
                    Log::instance()->upnpav().debug("sending icon ...");
                    std::ostream& ostr = response.send();
                    std::streamsize numBytes = Poco::StreamCopier::copyStream(*pIstr, ostr);
                    Log::instance()->upnpav().debug("icon sent (" + Poco::NumberFormatter::format(numBytes) + " bytes transfered).");
                    delete pIstr;
                }
            }
            catch(Poco::Exception& e) {
                Log::instance()->upnpav().debug("delivering icon failed: " + e.displayText());
            }
        }
        return;
    }
    else {
        // resource requested by controller
        int resourceId = Poco::NumberParser::parse(uri[1]);
        pResource = static_cast<ServerResource*>(pItem->getResource(resourceId));
        resSize = pResource->getSize();

        std::string protInfoString = pResource->getProtInfo();
        ProtocolInfo protInfo(protInfoString);
        std::string mime = protInfo.getMimeString();
        std::string dlna = protInfo.getDlnaString();

        response.setContentType(mime);
        response.set("Mime-Version", "1.0");
        response.set("transferMode.dlna.org", "Streaming");
        response.set("EXT", "");
        response.set("Server", Poco::Environment::osName() + "/"
                     + Poco::Environment::osVersion()
                     + " UPnP/" + UPNP_VERSION + " Omm/" + OMM_VERSION);

        if (pResource->isSeekable()) {
            response.set("Accept-Ranges", "bytes");
        }
        if (request.has("getcontentFeatures.dlna.org")) {
            response.set("contentFeatures.dlna.org", dlna);
        }
    }


    if (request.getMethod() == "GET") {
        std::streamoff start = 0;
        std::streamoff end = -1;
        std::streamsize len = resSize;
        if (request.has("Range") && pResource->isSeekable()) {
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_PARTIAL_CONTENT, "Partial Content");
            std::string rangeValue = request.get("Range");
            parseRange(rangeValue, start, end);
            response.set("Content-Range", "bytes "
                        + Poco::NumberFormatter::format((long)start) + "-"
                        + (end == -1 ? "" : Poco::NumberFormatter::format((long)end)) + "/"
                        + Poco::NumberFormatter::format(pResource->getSize()));
            if (end >= 0) {
                len = end - start + 1;
            }
            else {
                len = resSize - start;
            }
            response.setContentLength(len);
        }
        std::ostringstream responseHeader;
        response.write(responseHeader);
        Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());
        std::ostream& ostr = response.send();
#ifdef __DARWIN__
        signal(SIGPIPE, SIG_IGN); // fixes abort with "Program received signal SIGPIPE, Broken pipe." on Mac OSX when renderer stops the stream.
#endif
        try {
            std::istream* pIstr;
            // deliver resource
            pIstr = pResource->getStream();
            if (pIstr) {
                Log::instance()->upnpav().debug("sending stream ...");
                std::streamsize numBytes = copyStream(*pIstr, ostr, start, end);
                Log::instance()->upnpav().debug("stream sent (" + Poco::NumberFormatter::format(numBytes) + " bytes transfered).");
                delete pIstr;
            }
            else {
                throw Poco::Exception("no stream available");
            }
        }
        catch(Poco::Exception& e) {
            Log::instance()->upnpav().debug("streaming aborted: " + e.displayText());
        }
    }
    else if (request.getMethod() == "HEAD") {
        if (resSize > 0 ) {
            response.setContentLength(resSize);
        }
        std::ostringstream responseHeader;
        response.write(responseHeader);
        Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());
        response.send();
    }

    if (response.sent()) {
        Log::instance()->upnpav().debug("media item request finished: " + request.getURI());
    }
}


std::streamsize
ItemRequestHandler::copyStream(std::istream& istr, std::ostream& ostr, std::streamoff start, std::streamoff end)
{
    if (start > 0) {
        istr.seekg(start);
    }

    char buffer[_bufferSize];
    std::streamsize len = 0;
    std::streamsize left = end;
    if (end >= 0) {
        left -= start;
        left++;
    }
    if (left >= 0 && left < _bufferSize) {
        istr.read(buffer, left);
    }
    else {
        istr.read(buffer, _bufferSize);
    }
    std::streamsize n = istr.gcount();
    while (n > 0)
    {
        len += n;
        if (end >= 0) {
            left -= n;
        }
        ostr.write(buffer, n);
        if (istr && ostr)
        {
            if (left >= 0 && left < _bufferSize) {
                istr.read(buffer, left);
            }
            else {
                istr.read(buffer, _bufferSize);
            }
            n = istr.gcount();
        }
        else n = 0;
    }
    return len;
}


void
ItemRequestHandler::parseRange(const std::string& rangeValue, std::streamoff& start, std::streamoff& end)
{
   std::string range = rangeValue.substr(rangeValue.find('=') + 1);

    std::string::size_type delim = range.find('-');
    start = Poco::NumberParser::parse(range.substr(0, delim));
    try {
        end = Poco::NumberParser::parse(range.substr(delim + 1));
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("range end parsing: " + e.displayText());
    }
    Log::instance()->upnpav().debug("range: " + range + " (start: " + Poco::NumberFormatter::format((long)start)
                                                      + ", end: " + (end == -1 ? "not defined" : Poco::NumberFormatter::format((long)end)) + ")");
}


std::istream*
ServerObjectProperty::getStream()
{
    return static_cast<ServerObjectPropertyImpl*>(_pPropertyImpl)->getStream();
}


ServerObjectPropertyImpl::ServerObjectPropertyImpl(MediaServer* pServer, ServerObject* pItem) :
_pServer(pServer),
_pItem(pItem)
{

}


std::string
ServerObjectPropertyImpl::getValue()
{
//    Log::instance()->upnpav().debug("streaming property get resource string");

    std::string serverAddress = _pServer->getServerAddress();
//    std::string relativeObjectId = _pItem->getId().substr(_pServer->getId().length() + 1);
    std::string resourceId = "i";
    return serverAddress + "/" + _pItem->getId() + "$" + resourceId;
//    return serverAddress + "/" + relativeObjectId + "$" + resourceId;
}


ServerResource::ServerResource(ServerItem* pItem, AbstractDataModel* pDataModel) :
//ServerResource::ServerResource(MediaServer* pServer, AbstractMediaObject* pItem, AbstractDataModel* pDataModel) :
//AbstractResource(pPropertyImpl),
//_pServer(pServer),
_pItem(pItem),
_pDataModel(pDataModel),
_id(0)
{
}


//std::string
//ServerResource::getValue()
//{
////    Log::instance()->upnpav().debug("streaming resource get resource string ...");
//
//    std::string serverAddress = _pServer->getServerAddress();
//
////    Log::instance()->upnpav().debug("streaming resource get relative object id ...");
////    std::string relativeObjectId = _pItem->getId().substr(_pServer->getId().length() + 1);
////    Log::instance()->upnpav().debug("streaming resource relative object id: " + relativeObjectId);
//    std::string resourceId = Poco::NumberFormatter::format(_id);
//
////    Log::instance()->upnpav().debug("streaming resource get resource string returns: " + serverAddress + "/" + relativeObjectId + "$" + resourceId);
//    return serverAddress + "/" + _pItem->getId() + "$" + resourceId;
////    return serverAddress + "/" + relativeObjectId + "$" + resourceId;
//}
//
//
//std::string
//ServerResource::getAttributeName(int index)
//{
//    if (index == 0) {
//        return AvProperty::PROTOCOL_INFO;
//    }
//    else if (index == 1) {
//        return AvProperty::SIZE;
//    }
//}
//
//
//std::string
//ServerResource::getAttributeValue(int index)
//{
//    if (index == 0) {
////        return _pServer->getServerProtocol() + ":" + getMime() + ":" + getDlna();
//        return _pServer->getServerProtocol() + ":" + _pItem->getMime() + ":" + _pItem->getDlna();
//    }
//    else if (index == 1) {
//        return Poco::NumberFormatter::format(getSize());
//    }
//}
//
//
//std::string
//ServerResource::getAttributeValue(const std::string& name)
//{
//    if (name == AvProperty::PROTOCOL_INFO) {
//        return _pServer->getServerProtocol() + ":" + getMime() + ":" + getDlna();
//    }
//    else if (name == AvProperty::SIZE) {
//        return Poco::NumberFormatter::format(getSize());
//    }
//}
//
//
//int
//ServerResource::getAttributeCount()
//{
//    // protocolInfo and size
//    return 2;
//}


bool
ServerResource::isSeekable()
{
//    Log::instance()->upnpav().debug("cached item is seekable");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return false;
//    }
    return _pDataModel->isSeekable(_pDataModel->getPath(_pItem->getIndex()));
}


std::streamsize
ServerResource::getSize()
{
//    Log::instance()->upnpav().debug("cached item get size");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return 0;
//    }
    return _pDataModel->getSize(_pDataModel->getPath(_pItem->getIndex()));
}


std::istream*
ServerResource::getStream()
{
//    Log::instance()->upnpav().debug("cached item get stream");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return 0;
//    }
    return _pDataModel->getStream(_pDataModel->getPath(_pItem->getIndex()));
}


ServerObject::ServerObject(MediaServer* pServer) :
_index(0),
_pParent(0),
_pServer(pServer)
{
//    _pServer->_pServerContainer = this;
}


//StreamingMediaObject::StreamingMediaObject(int port)
//{
//    _pItemServer = new MediaServer(port);
//    _pItemServer->_pServerContainer = this;
////    _pItemServer->start();
//}


//StreamingMediaObject::StreamingMediaObject(const StreamingMediaObject& object) :
//_pItemServer(object._pItemServer)
//{
//}


//StreamingMediaObject::StreamingMediaObject(StreamingMediaObject* pServer)
//{
//    _pServer = pServer;
//}


ServerObject::~ServerObject()
{
//    if (_pItemServer) {
//        _pItemServer->stop();
//        delete _pItemServer;
//        _pItemServer = 0;
//    }
}


//void
//StreamingMediaObject::startStreamingServer()
//{
//    _pItemServer->start();
//}


std::string
ServerObject::getId()
{
//    Log::instance()->upnpav().debug("ServerObject::getObjectId()");

    ServerObject* pParent = getParent();
    if (pParent == 0) {
        return "0";
    }
    else {
        return pParent->getId() + "/" + Poco::NumberFormatter::format(getIndex());
    }
}


std::string
ServerObject::getParentId()
{
//    Log::instance()->upnpav().debug("ServerObject::getParentObjectId()");

    AbstractMediaObject* pParent = getParent();
    if (pParent) {
        return pParent->getId();
    }
    else {
        return "";
    }
}


ui4
ServerObject::getIndex()
{
//    Log::instance()->upnpav().debug("ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    return _index;
}


void
ServerObject::setIndex(ui4 index)
{
//    Log::instance()->upnpav().debug("ServerObject::setObjectNumber() objectId: " + Poco::NumberFormatter::format(id));
    _index = index;
}


void
ServerObject::setIndex(const std::string& index)
{
//    Log::instance()->upnpav().debug("ServerObject::setObjectId() from string: " + id);
    _index = Poco::NumberParser::parseUnsigned(index);
}


ui4
ServerObject::getParentIndex()
{
//    Log::instance()->upnpav().debug("ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    return _parentIndex;
}


ServerObject*
ServerObject::getParent()
{
//    Log::instance()->upnpav().debug("ServerObject::getParent()");
    return _pParent;
}


void
ServerObject::setParentIndex(ui4 index)
{
//    Log::instance()->upnpav().debug("ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    _parentIndex = index;
}


void
ServerObject::setParent(ServerObject* pParent)
{
//    Log::instance()->upnpav().debug("ServerObject::setParent()");
    _pParent = pParent;
}


AbstractMediaObject*
ServerObject::createChildObject()
{
    ServerObject* pObject = new ServerObject(_pServer);
    pObject->setParent(this);
    return pObject;
//    return new StreamingMediaObject(this);
}


//std::string
//StreamingMediaObject::getServerAddress()
//{
////    Log::instance()->upnpav().debug("streaming media object get server address ...");
//
//    std::string address = Net::NetworkInterfaceManager::instance()->getValidIpAddress().toString();
//    int port = _pItemServer->_socket.address().port();
////    Log::instance()->upnpav().debug("streaming media object g et server address returns: http://" + address + ":" + Poco::NumberFormatter::format(port));
//    return "http://" + address + ":" + Poco::NumberFormatter::format(port);
//}
//
//
//std::string
//StreamingMediaObject::getServerProtocol()
//{
//    return _pItemServer->getProtocol();
//}


//std::istream*
//StreamingMediaObject::getIconStream()
//{
//
//}


//ServerResource::ServerResource(ServerContainer* pServer, AbstractMediaObject* pItem) :
//StreamingResource(new MemoryPropertyImpl, pServer, pItem)
//{
//}


//bool
//ServerResource::isSeekable()
//{
////    Log::instance()->upnpav().debug("cached item is seekable");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return false;
//    }
//    return pDataModel->isSeekable(pDataModel->getPath(_pItem->getIndex()));
//}
//
//
//std::streamsize
//ServerResource::getSize()
//{
////    Log::instance()->upnpav().debug("cached item get size");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return 0;
//    }
//    return pDataModel->getSize(pDataModel->getPath(_pItem->getIndex()));
//}
//
//
//std::istream*
//ServerResource::getStream()
//{
////    Log::instance()->upnpav().debug("cached item get stream");
//    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
//    if (!pDataModel) {
//        return 0;
//    }
//    return pDataModel->getStream(pDataModel->getPath(_pItem->getIndex()));
//}


ServerItem::ServerItem(MediaServer* pServer, ServerContainer* pContainer) :
ServerObject(pServer),
_pContainer(pContainer)
{
}


ServerItem::~ServerItem()
{
}


ServerResource*
ServerItem::createResource()
{
    Log::instance()->upnpav().debug("server item create resource");

    return new ServerResource(this, _pContainer->getDataModel());
//    return new ServerResource(static_cast<ServerContainer*>(_pServer), this);
}


ServerContainer::ServerContainer(MediaServer* pServer) :
ServerObject(pServer)
{
    setIsContainer(true);
    _pServer->_pServerContainer = this;
}


void
ServerContainer::setDataModel(AbstractDataModel* pDataModel)
{
    _pDataModel = pDataModel;
    _pDataModel->setServerContainer(this);
}


AbstractDataModel*
ServerContainer::getDataModel()
{
    return _pDataModel;
}


ServerContainer*
ServerContainer::createMediaContainer()
{
    Log::instance()->upnpav().debug("server container create media container");

//    ServerContainer* pContainer = new ServerContainer(*this);
    ServerContainer* pContainer = new ServerContainer(_pServer);
    pContainer->_pServer = _pServer;
//    MemoryMediaObject* pContainer = new MemoryMediaObject;

    pContainer->setIsContainer(true);
    pContainer->setClass(AvClass::className(AvClass::CONTAINER));

    return pContainer;
}


ServerItem*
ServerContainer::createMediaItem()
{
    Log::instance()->upnpav().debug("server container create media item");

    ServerItem* pItem = new ServerItem(_pServer, this);
    pItem->setParent(this);
    return pItem;
}


AbstractMediaObject*
ServerContainer::createChildObject()
{
    Log::instance()->upnpav().debug("server container create child object");

    return createMediaItem();
}


void
ServerContainer::appendChild(AbstractMediaObject* pChild)
{
// TODO: server implementation of appendChild()
////    pChild->setIndex(getChildCount());
////    pChild->setIndex(index);
//    pChild->setParent(this);
//    appendChildImpl(pChild);
}


void
ServerContainer::appendChildWithAutoIndex(AbstractMediaObject* pChild)
{
// TODO: server implementation of appendChild(), replace appendChildWithAutoIndex()
//    pChild->setIndex(getChildCount());
//    appendChild(pChild);
}


ui4
ServerContainer::getChildCount()
{
    if (!_pDataModel) {
        return 0;
    }
    return _pDataModel->getIndexCount();
}


ServerObject*
ServerContainer::getDescendant(const std::string& objectId)
{
    Log::instance()->upnpav().debug("server container get descendant with (relative) objectId: " + objectId);

    // TODO: what about this object, not only child objects?

    std::string::size_type slashPos = objectId.find('/');
    if (slashPos != std::string::npos) {
        // child is a container
        ServerContainer* pChild = static_cast<ServerContainer*>(getChildForIndex(objectId.substr(0, slashPos)));
        if (pChild == 0) {
            // child container is not a child of this container, we try the full path
            pChild = static_cast<ServerContainer*>(getChildForIndex(objectId));
            if (pChild == 0) {
                // child container is not a child of this container
                Log::instance()->upnpav().error("retrieving child objectId of container, but no child container found");
                return 0;
            }
            else {
                // object id of child contains slashes (subtree's implementation is a faked tree with only item objects).
                return pChild;
            }
        }
        else {
            // recurse into child container
            return pChild->getDescendant(objectId.substr(slashPos + 1));
        }
    }
    else {
        // child is an item
        Log::instance()->upnpav().debug("server container get descendant is a child with index: " + objectId);
        ServerItem* pChild = static_cast<ServerItem*>(getChildForIndex(objectId));
        if (pChild == 0) {
            // child item is not a child of this container
            Log::instance()->upnpav().error("no child item found");
            return 0;
        }
        else {
            // return child item and stop recursion
            return pChild;
        }
    }
}


ServerObject*
ServerContainer::getChildForIndex(const std::string& index)
{
    return getChildForIndex(Poco::NumberParser::parseUnsigned(index));
}


ServerObject*
ServerContainer::getChildForIndex(ui4 index)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    std::string path = _pDataModel->getPath(index);
    Log::instance()->upnpav().debug("server container, get children for index: " + Poco::NumberFormatter::format(index) + ", path: " + path);
    ServerObject* pObject = _pDataModel->getMediaObject(path);
    initChild(pObject, index);
    return pObject;
}


ui4
ServerContainer::getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    Log::instance()->upnpav().debug("server container, get children at row offset.");

    ui4 totalChildCount = 0;
    if (sort == "" && search == "*") {
        ui4 r = 0;
        // TODO: should be faster with method getIndexBlock() in AbstractDataModel, implemented there with an additional std::vector<ui4>
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); (it != _pDataModel->endIndex()) && (r < offset + count); ++it) {
            if (r >= offset) {
                ServerObject* pObject = _pDataModel->getMediaObject((*it).second);
//                if (!pObject->isContainer()) {
                    initChild(pObject, (*it).first);
    //                pObject->setIndex((*it).first);
                    children.push_back(pObject);
//                }
//                else {
//                    delete pObject;
//                }
            }
            r++;
        }
        totalChildCount = _pDataModel->getIndexCount();
    }
    else {
        // TODO: implement building sort indices and row filtering in memory without data base.
    }
    return totalChildCount;
}


void
ServerContainer::setBasePath(const std::string& basePath)
{
    _pDataModel->setBasePath(basePath);
    updateCache();
}


void
ServerContainer::initChild(ServerObject * pObject, ui4 index)
{
    Log::instance()->upnpav().debug("server container, init child with title: " + pObject->getTitle());

    pObject->setIndex(index);
    std::string path = _pDataModel->getPath(index);
    std::string parentPath = _pDataModel->getParentPath(path);
    if (parentPath == "") {
        pObject->setParentIndex(AbstractDataModel::INVALID_INDEX);
    }
    else {
        pObject->setParentIndex(_pDataModel->getIndex(parentPath));
    }
//    AbstractMediaObject* pParent;
//    if (parentPath == "") {
//        pParent = this;
//    }
//    else {
//        pParent = _pDataModel->getMediaObject(parentPath);
//    }
//    pObject->setParent(pParent);

//    Log::instance()->upnpav().debug("streaming resource get resource string ...");

    std::string serverAddress = _pServer->getServerAddress();

//    Log::instance()->upnpav().debug("streaming resource get relative object id ...");
    std::string relativeObjectId = pObject->getId().substr(getId().length() + 1);
//    Log::instance()->upnpav().debug("streaming resource relative object id: " + relativeObjectId);
//    std::string resourceId = Poco::NumberFormatter::format(_id);

//    Log::instance()->upnpav().debug("streaming resource get resource string returns: " + serverAddress + "/" + relativeObjectId + "$" + resourceId);
//    std::string resourceUri = serverAddress + "/" + pObject->getId() + "$0";
    std::string resourceUri = serverAddress + "/" + relativeObjectId + "$0";
//    return serverAddress + "/" + relativeObjectId + "$" + resourceId;

    pObject->getResource(0)->setUri(resourceUri);
}


void
ServerObjectWriter::writeChildren(std::string& meta, const std::vector<ServerObject*>& children, const std::string& filter)
{
    Log::instance()->upnpav().debug("ServerObjectWriter::writeChildren()");
    writeMetaDataHeader();
    for (std::vector<ServerObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        ServerObjectWriter writer;
        writer.writeMetaData(_pDidl, *it);
    }
    writeMetaDataClose(meta);
}


DatabaseCache::DatabaseCache() :
_pSession(0)
{
    Poco::Data::SQLite::Connector::registerConnector();

}


DatabaseCache::~DatabaseCache()
{
    if (_pSession) {
        delete _pSession;
    }
    Poco::Data::SQLite::Connector::unregisterConnector();
}


void
DatabaseCache::setCacheFilePath(const std::string& cacheFilePath)
{
    _cacheFilePath = cacheFilePath;
    _pSession = new Poco::Data::Session("SQLite", cacheFilePath);
    try {
        *_pSession << "CREATE TABLE objcache (idx INTEGER(4), paridx INTEGER(4), class VARCHAR(30), title VARCHAR, artist VARCHAR, album VARCHAR, track INTEGER(2), xml VARCHAR)",
                Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating object cache table failed: " + e.displayText());
    }
    try {
        *_pSession << "CREATE UNIQUE INDEX idx ON objcache (idx)", Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating index on object cache table failed: " + e.displayText());
    }
}


ui4
DatabaseCache::rowCount()
{
    Poco::Data::Statement select(*_pSession);
    std::string statement = "SELECT idx FROM objcache";
    select << statement;
    Poco::Data::RecordSet recordSet(select);
    try {
        select.execute();
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache get row count failed: " + e.displayText());
    }
    return recordSet.rowCount();
}


ServerObject*
DatabaseCache::getMediaObjectForIndex(ui4 index)
{
    Log::instance()->upnpav().debug("database cache get object for index: " + Poco::NumberFormatter::format(index));

    std::vector<std::string> xml;
    try {
        *_pSession << "SELECT xml FROM objcache WHERE idx = :index", Poco::Data::use(index), Poco::Data::into(xml), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache get object for index failed: " + e.displayText());
    }
    if (xml.size() == 1) {
        ServerObject* pObject = new ServerObject(0);
//        if (_pContainer) {
//            pObject = _pContainer->createChildObject();
//        }
//        else {
//            Log::instance()->upnpav().error("database cache could not create child object");
//            return 0;
//        }
        MediaObjectReader xmlReader;
        xmlReader.read(pObject, xml[0]);
//        pObject->setIndex(index);
        return pObject;
    }
    else {
        Log::instance()->upnpav().warning("database cache get object for index reading meta data failed.");
        return 0;
    }
}


ui4
DatabaseCache::getBlockAtRow(std::vector<ServerObject*>& block, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Log::instance()->upnpav().debug("database cache get block at offset: " + Poco::NumberFormatter::format(offset) + ", count: " + Poco::NumberFormatter::format(count));

    ui4 index;
    std::string xml;
    Poco::Data::Statement select(*_pSession);
    std::string statement = "SELECT idx, xml FROM objcache";
    if (search != "*") {
        statement += " WHERE " + search;
    }
//    statement += " ORDER BY artist, album, track, title";
    select << statement;
    Poco::Data::RecordSet recordSet(select);
    try {
        select.execute();
        // move to offset
        recordSet.moveFirst();
        for (ui4 r = 0; r < offset; r++) {
            recordSet.moveNext();
        }
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache get block executing query and moving to offset failed: " + e.displayText());
    }
    for (ui4 r = 0; r < count; r++) {
        // get block
        try {
            index = recordSet["idx"].convert<ui4>();
            xml = recordSet["xml"].convert<std::string>();
            ServerObject* pObject = new ServerObject(0);
//            if (_pContainer) {
//                pObject = _pContainer->createChildObject();
//            }
//            else {
//                Log::instance()->upnpav().error("database cache could not create child object");
//                return recordSet.rowCount();
//            }
            MediaObjectReader xmlReader;
            xmlReader.read(pObject, xml);
//            if (!pObject->isContainer()) {
                pObject->setIndex(index);
                block.push_back(pObject);
//            }
//            else {
//                delete pObject;
//            }
            recordSet.moveNext();
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnpav().warning("database cache get block data for row " + Poco::NumberFormatter::format(r) + " failed: " + e.displayText());
        }
    }
    return recordSet.rowCount();
}


//void
//DatabaseCache::doScan(bool on)
//{
//
//}


void
DatabaseCache::insertMediaObject(ServerObject* pObject)
{
    Log::instance()->upnpav().debug("database cache inserting media object with index: " + Poco::NumberFormatter::format(pObject->getIndex()));
    std::string xml;
    MediaObjectWriter2 xmlWriter;
    xmlWriter.write(xml, pObject);
    std::string artist;
    AbstractProperty* pProperty = pObject->getProperty(AvProperty::ARTIST);
    if (pProperty) {
        artist = pProperty->getValue();
    }
    std::string album;
    pProperty = pObject->getProperty(AvProperty::ALBUM);
    if (pProperty) {
        album = pProperty->getValue();
    }
    std::string track;
    pProperty = pObject->getProperty(AvProperty::ORIGINAL_TRACK_NUMBER);
    if (pProperty) {
        track = pProperty->getValue();
    }
    try {
        *_pSession << "INSERT INTO objcache (idx, paridx, class, title, artist, album, track, xml) VALUES(:idx, :paridx, :class, :title, :artist, :album, :track, :xml)",
                Poco::Data::use(pObject->getIndex()),
                Poco::Data::use(pObject->getParentIndex()),
                Poco::Data::use(pObject->getClass()),
                Poco::Data::use(pObject->getTitle()),
                Poco::Data::use(artist),
                Poco::Data::use(album),
                Poco::Data::use(track),
                Poco::Data::use(xml),
                Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().debug("database cache inserting media object failed: " + e.displayText());
    }
}


void
DatabaseCache::insertBlock(std::vector<ServerObject*>& block)
{
    Log::instance()->upnpav().debug("database cache inserting media object block");
    typedef Poco::Tuple<ui4, ui4, std::string, std::string, std::string, std::string, int, std::string> MediaObject;
    std::vector<MediaObject> tupleBlock;
    for (std::vector<ServerObject*>::iterator it = block.begin(); it != block.end(); ++it) {
        ServerObject* pObject = *it;
        std::string xml;
        MediaObjectWriter2 xmlWriter;
        xmlWriter.write(xml, pObject);
        std::string artist;
        AbstractProperty* pProperty = pObject->getProperty(AvProperty::ARTIST);
        if (pProperty) {
            artist = pProperty->getValue();
        }
        std::string album;
        pProperty = pObject->getProperty(AvProperty::ALBUM);
        if (pProperty) {
            album = pProperty->getValue();
        }
        Variant trackVariant;
        pProperty = pObject->getProperty(AvProperty::ORIGINAL_TRACK_NUMBER);
        if (pProperty) {
            trackVariant.setValue(pProperty->getValue());
        }
        int track;
        trackVariant.getValue(track);

        tupleBlock.push_back(MediaObject(pObject->getIndex(), pObject->getParentIndex(), pObject->getClass(), pObject->getTitle(), artist, album, track, xml));
    }

    try {
        *_pSession << "INSERT INTO objcache (idx, paridx, class, title, artist, album, track, xml) VALUES(:idx, :paridx, :class, :title, :artist, :album, :track, :xml)",
                Poco::Data::use(tupleBlock),
                Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().debug("database cache inserting media object failed: " + e.displayText());
    }
}


CachedServerContainer::CachedServerContainer(MediaServer* pServer) :
ServerContainer(pServer),
_updateCacheThreadRunnable(*this, &CachedServerContainer::updateCacheThread),
_updateCacheThreadRunning(false)
{
//    setContainer(this);

    _searchCaps.append(AvProperty::CLASS);
    _searchCaps.append(AvProperty::TITLE);
    _searchCaps.append(AvProperty::ARTIST);
    _searchCaps.append(AvProperty::ALBUM);
    _searchCaps.append(AvProperty::ORIGINAL_TRACK_NUMBER);

    _sortCaps.append(AvProperty::CLASS);
    _sortCaps.append(AvProperty::TITLE);
    _sortCaps.append(AvProperty::ARTIST);
    _sortCaps.append(AvProperty::ALBUM);
    _sortCaps.append(AvProperty::ORIGINAL_TRACK_NUMBER);
}


CsvList*
CachedServerContainer::getSearchCaps()
{
    if (_searchCaps.getSize()) {
        return &_searchCaps;
    }
    else {
        return 0;
    }
}


CsvList*
CachedServerContainer::getSortCaps()
{
    if (_sortCaps.getSize()) {
        return &_sortCaps;
    }
    else {
        return 0;
    }
}


void
CachedServerContainer::setBasePath(const std::string& basePath)
{
    Log::instance()->upnpav().debug("cached server container, set base path to: " + basePath);
    setCacheFilePath(Util::Home::instance()->getCacheDirPath(basePath) + _pDataModel->getServerClass() + "/objects");
    ServerContainer::setBasePath(basePath);
}


void
CachedServerContainer::updateCache(bool on)
{
    _updateCacheThreadLock.lock();
    _updateCacheThreadRunning = on;
    _updateCacheThreadLock.unlock();
    if (on) {
        _updateCacheThread.start(_updateCacheThreadRunnable);
    }
    else {
        _updateCacheThread.join();
    }
}


void
CachedServerContainer::updateCacheThread()
{
    Log::instance()->upnpav().debug("cached server container, update cache thread started ...");
    if (!cacheNeedsUpdate()) {
        Log::instance()->upnpav().debug("database cache is current, nothing to do");
    }
    else {
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); it != _pDataModel->endIndex(); ++it) {
            if (!updateCacheThreadIsRunning()) {
                Log::instance()->upnpav().debug("stopping scan thread");
                break;
            }
            DatabaseCache::insertMediaObject(ServerContainer::getChildForIndex((*it).first));
        }
    }
    _updateCacheThreadLock.lock();
    _updateCacheThreadRunning = false;
    _updateCacheThreadLock.unlock();
    Log::instance()->upnpav().debug("cached server container, update cache thread finished.");
}


ServerObject*
CachedServerContainer::getChildForIndex(ui4 index)
{
    Log::instance()->upnpav().debug("cached server container, get children for index: " + Poco::NumberFormatter::format(index));

    if (!_pDataModel) {
        return 0;
    }
    if (_pDataModel->useObjectCache() && !updateCacheThreadIsRunning()) {
        // get media object out of data base cache (column xml)
         ServerObject* pObject = DatabaseCache::getMediaObjectForIndex(index);
         if (pObject) {
             initChild(pObject, index);
//             pObject->setIndex(index);
//             pObject->setParent(this);
         }
         else {
             // media object is not yet in the data base cache
             pObject = ServerContainer::getChildForIndex(index);
//             DatabaseCache::insertMediaObject(pObject);
         }
         return pObject;
    }
    else {
        return ServerContainer::getChildForIndex(index);
    }
}


ui4
CachedServerContainer::getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    if (!_pDataModel) {
        return 0;
    }
    ui4 totalChildCount = 0;
    if (_pDataModel->useObjectCache() && !updateCacheThreadIsRunning()) {
        if (cacheNeedsUpdate() && sort == "" && search == "*") {
            // if no query result and no sort or search queries, we can serve a block from the index cache of size "count" starting at "offset"
            totalChildCount = ServerContainer::getChildrenAtRowOffset(children, offset, count);
            // fill the cache at little bit at this occasion (... too slow, left out)
//            for (std::vector<AbstractMediaObject*>::iterator it = children.begin(); it != children.end(); ++it) {
//                DatabaseCache::insertMediaObject(*it);
//            }
            // block insertion seems to be even slower than single object insertion
//            DatabaseCache::insertBlock(children);
        }
        else {
            totalChildCount = DatabaseCache::getBlockAtRow(children, offset, count, sort, search);
            for (std::vector<ServerObject*>::iterator it = children.begin(); it != children.end(); ++it) {
                initChild(*it, (*it)->getIndex());
            }
        }
    }
    else {
        totalChildCount = ServerContainer::getChildrenAtRowOffset(children, offset, count, sort, search);
    }
    return totalChildCount;
}


bool
CachedServerContainer::cacheNeedsUpdate()
{
    ui4 rows = rowCount();
    if (rows > _pDataModel->getIndexCount()) {
        Log::instance()->upnpav().error("cached server container, database cache not coherent.");
        return true;
    }
    else if (rows == _pDataModel->getIndexCount()) {
        Log::instance()->upnpav().debug("cached server container, database cache is current.");
        return false;
    }
    else {
        Log::instance()->upnpav().debug("cached server container, database cache needs update.");
        return true;
    }
}


bool
CachedServerContainer::updateCacheThreadIsRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_updateCacheThreadLock);
    Log::instance()->upnpav().debug("cached server container, update cache thread is running: " + std::string(_updateCacheThreadRunning ? "yes" : "no"));
    return _updateCacheThreadRunning;
}


void
CachedServerContainer::initChild(ServerObject* pObject, ui4 index)
{
    Log::instance()->upnpav().debug("cached server container, init child with title: " + pObject->getTitle());
    pObject->setIndex(index);
    std::string path = _pDataModel->getPath(index);
    std::string parentPath = _pDataModel->getParentPath(path);
//    if (parentPath == "") {
//        pObject->setParentIndex(AbstractDataModel::INVALID_INDEX);
//    }
//    else {
//        pObject->setParentIndex(_pDataModel->getIndex(parentPath));
//    }
    ServerObject* pParent;
    if (parentPath == "") {
        pParent = this;
    }
    else {
        pParent = _pDataModel->getMediaObject(parentPath);
    }
    pObject->setParent(pParent);

//    Log::instance()->upnpav().debug("streaming resource get resource string ...");

    std::string serverAddress = _pServer->getServerAddress();

//    Log::instance()->upnpav().debug("streaming resource get relative object id ...");
    std::string relativeObjectId = pObject->getId().substr(getId().length() + 1);
//    Log::instance()->upnpav().debug("streaming resource relative object id: " + relativeObjectId);
//    std::string resourceId = Poco::NumberFormatter::format(_id);

//    Log::instance()->upnpav().debug("streaming resource get resource string returns: " + serverAddress + "/" + relativeObjectId + "$" + resourceId);
//    std::string resourceUri = serverAddress + "/" + pObject->getId() + "$0";
    std::string resourceUri = serverAddress + "/" + relativeObjectId + "$0";
//    return serverAddress + "/" + relativeObjectId + "$" + resourceId;

    pObject->getResource(0)->setUri(resourceUri);
}


const ui4 AbstractDataModel::INVALID_INDEX = 0xffffffff;

AbstractDataModel::AbstractDataModel() :
_pServerContainer(0),
_maxIndex(0)
//_indexBufferSize(50)
{
}


void
AbstractDataModel::setServerContainer(ServerContainer* pServerContainer)
{
    _pServerContainer = pServerContainer;
}


ServerContainer*
AbstractDataModel::getServerContainer()
{
    return _pServerContainer;
}


void
AbstractDataModel::setBasePath(const std::string& basePath)
{
    _basePath = basePath;
    _indexFilePath = Poco::Path(Util::Home::instance()->getMetaDirPath(basePath), "index");
    Omm::Av::Log::instance()->upnpav().debug("data model scan ...");
    scan(true);
    Omm::Av::Log::instance()->upnpav().debug("data model scan finished.");
}


std::string
AbstractDataModel::getBasePath()
{
    return _basePath.toString();
}


ui4
AbstractDataModel::getIndexCount()
{
    return _indexMap.size();
}


bool
AbstractDataModel::hasIndex(ui4 index)
{
    return _indexMap.find(index) != _indexMap.end();
}


ui4
AbstractDataModel::getIndex(const std::string& path)
{
    // check if path is not in the cache
    std::map<std::string, ui4>::const_iterator pos = _pathMap.find(path);
    if (pos  != _pathMap.end()) {
        return (*pos).second;
    }
    else {
        Log::instance()->upnpav().error("abstract data model, could not retrieve index from path: " + path);
        return INVALID_INDEX;
    }
}


std::string
AbstractDataModel::getPath(ui4 index)
{
    std::map<ui4, std::string>::const_iterator pos = _indexMap.find(index);
    if (pos  != _indexMap.end()) {
        return (*pos).second;
    }
    else {
        Log::instance()->upnpav().error("abstract data model, could not retrieve path from index: " + Poco::NumberFormatter::format(index));
        return "";
    }
}


AbstractDataModel::IndexIterator
AbstractDataModel::beginIndex()
{
    return _indexMap.begin();
}


AbstractDataModel::IndexIterator
AbstractDataModel::endIndex()
{
    return _indexMap.end();
}


void
AbstractDataModel::addPath(const std::string& path, const std::string& resourcePath)
{
    ui4 index;
    // got a free index lying around?
    if (!_freeIndices.empty()) {
        index = _freeIndices.top();
        _freeIndices.pop();
    }
    else {
        index = _maxIndex++;
        if (index == INVALID_INDEX) {
            Log::instance()->upnpav().error("abstract data model max index reached, can not add path: " + path);
            return;
        }
        Log::instance()->upnpav().debug("abstract data model add path: " + path + " with index: " + Poco::NumberFormatter::format(index));
    }
    // create a new index
    _pathMap[path] = index;
    _indexMap[index] = path;
    if (resourcePath != "") {
        _resourceMap.insert(std::pair<ui4, std::string>(index, resourcePath));
    }
}


void
AbstractDataModel::removePath(const std::string& path)
{
    std::map<std::string, ui4>::iterator pos = _pathMap.find(path);
    if (pos  != _pathMap.end()) {
        _indexMap.erase((*pos).second);
        _pathMap.erase(pos);
        _resourceMap.erase((*pos).second);
    }
    else {
        Log::instance()->upnpav().error("abstract data model, could not erase path from index cache: " + path);
    }
}


void
AbstractDataModel::readIndexCache()
{
    if (!Poco::File(_indexFilePath).exists()) {
        Omm::Av::Log::instance()->upnpav().debug("index cache not present, not reading it");
        return;
    }
    Omm::Av::Log::instance()->upnpav().debug("index cache present, reading ...");
    std::ifstream indexCache(_indexFilePath.toString().c_str());
    std::string line;
    ui4 index = 0;
    ui4 lastIndex = 0;
    _maxIndex = 0;
    std::string path;
    while(getline(indexCache, line)) {
        std::string::size_type pos = line.find(' ');
        index = Poco::NumberParser::parse(line.substr(0, pos));
        path = line.substr(pos + 1);
        _indexMap[index] = path;
        _pathMap[path] = index;
        for (ui4 i = lastIndex + 1; i < index; i++) {
            _freeIndices.push(i);
        }
        lastIndex = index;
    }
    _maxIndex = index;
    // TODO: read resource map
    Omm::Av::Log::instance()->upnpav().debug("index cache reading finished.");
}


void
AbstractDataModel::writeIndexCache()
{
    if (Poco::File(_indexFilePath).exists()) {
        // TODO: check if index cache needs update
        return;
    }
    Log::instance()->upnpav().debug("abstract data model write index cache to: " + _indexFilePath.toString() + " ...");
    std::ofstream indexCache(_indexFilePath.toString().c_str());
    for (std::map<ui4, std::string>::iterator it = _indexMap.begin(); it != _indexMap.end(); ++it) {
//        Log::instance()->upnpav().debug("abstract data model write index: " + Poco::NumberFormatter::format((*it).first) + ", path: " + (*it).second);
        indexCache << (*it).first << ' ' << (*it).second << std::endl;
    }
    // TODO: write resource map
    Log::instance()->upnpav().debug("abstract data model write index cache finished.");
}


ServerObject*
SimpleDataModel::getMediaObject(const std::string& path)
{
//    Log::instance()->upnpav().debug("simple data model get media object for index: " + Poco::NumberFormatter::format(index) + " ...");
    Log::instance()->upnpav().debug("simple data model get media object for path: " + path + " ...");

//    Omm::Av::MemoryMediaObject* pItem = new Omm::Av::MemoryMediaObject;
//    std::string path = getPath(index);
    ServerItem* pItem = getServerContainer()->createMediaItem();

    pItem->setClass(getClass(path));
    pItem->setTitle(getTitle(path));

    std::string artist = getOptionalProperty(path, AvProperty::ARTIST);
    if (artist != "") {
        pItem->setUniqueProperty(AvProperty::ARTIST, artist);
    }
    std::string album = getOptionalProperty(path, AvProperty::ALBUM);
    if (album != "") {
        pItem->setUniqueProperty(AvProperty::ALBUM, album);
    }
    std::string track = getOptionalProperty(path, AvProperty::ORIGINAL_TRACK_NUMBER);
    if (track != "") {
        pItem->setUniqueProperty(AvProperty::ORIGINAL_TRACK_NUMBER, track);
    }
    std::string genre = getOptionalProperty(path, AvProperty::GENRE);
    if (genre != "") {
        pItem->setUniqueProperty(AvProperty::GENRE, genre);
    }

    ServerResource* pResource = pItem->createResource();
//    Omm::Av::MemoryResource* pResource = new Omm::Av::MemoryResource;
    pResource->setSize(getSize(path));
    // FIXME: add some parts of protinfo in server container / media server.
    pResource->setProtInfo("http-get:*:" + getMime(path) + ":" + getDlna(path));
    pItem->addResource(pResource);
    // TODO: add icon property
//        pItem->_icon = pItem->_path;

    return pItem;
}


} // namespace Av
} // namespace Omm
