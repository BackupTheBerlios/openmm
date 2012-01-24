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
#include <list>
#include <vector>
#include <Poco/LineEndingConverter.h>

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
    Log::instance()->upnpav().information("media server listening on: " + _socket.address().toString());
}


void
MediaServer::stop()
{
    _pHttpServer->stop();
}


Poco::UInt16
MediaServer::getServerPort() const
{
    return _socket.address().port();
}


std::string
MediaServer::getServerAddress()
{
    std::string address = Net::NetworkInterfaceManager::instance()->getValidIpAddress().toString();
    return "http://" + address + ":" + Poco::NumberFormatter::format(getServerPort());
}


std::string
MediaServer::getServerProtocol()
{
    return "http-get:*";
}


ItemRequestHandlerFactory::ItemRequestHandlerFactory(MediaServer* pItemServer) :
_pServer(pItemServer)
{
}


Poco::Net::HTTPRequestHandler*
ItemRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new ItemRequestHandler(_pServer);
}


ItemRequestHandler::ItemRequestHandler(MediaServer* pItemServer) :
_bufferSize(8192),
_pServer(pItemServer)
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
    ServerObject* pObject = _pServer->_pServerContainer->getDescendant(objectId);
    if (!pObject) {
        Log::instance()->upnpav().error("item request handler could not find object with id: " + objectId);
        return;
    }

    ServerObjectResource* pResource;
    ServerObjectProperty* pProperty;
    std::streamsize resSize = 0;
    if (uri[1] == "i") {
        // object icon requested by controller
        Log::instance()->upnpav().debug("icon request: server creates icon property");
        pProperty = static_cast<ServerObjectProperty*>(pObject->getProperty(AvProperty::ICON));
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
        pResource = static_cast<ServerObjectResource*>(pObject->getResource(resourceId));
        if (!pResource) {
            Log::instance()->upnpav().error("item request handler no resource for object with id: " + objectId);
            return;
        }
        Log::instance()->upnpav().debug("item request handler getting size");
        resSize = pResource->getSize();

        Log::instance()->upnpav().debug("item request handler getting prot info");
        std::string protInfoString = pResource->getProtInfo();
        ProtocolInfo protInfo(protInfoString);
        std::string mime = protInfo.getMimeString();
        std::string dlna = protInfo.getDlnaString();

        Log::instance()->upnpav().debug("item request handler writing response header");
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
//                delete pIstr;
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


ServerObjectProperty::ServerObjectProperty(ServerItem* pItem, AbstractDataModel* pDataModel) :
_pObject(pItem),
_pDataModel(pDataModel)
{
}


std::istream*
ServerObjectProperty::getStream()
{
    std::string path = _pDataModel->getPath(_pObject->getIndex());
//    if (getName() == AvProperty::RES) {
//        return _pDataModel->getStream(path);
//    }
    if (getName() == AvProperty::ICON) {
        return _pDataModel->getIconStream(path);
    }
    else {
        return 0;
    }
}


ServerObjectResource::ServerObjectResource(ServerObject* pItem, AbstractDataModel* pDataModel) :
_pObject(pItem),
_pDataModel(pDataModel),
_id(0)
{
}


bool
ServerObjectResource::isSeekable()
{
    if (_pObject->isContainer()) {
        return false;
    }
    else {
        return _pDataModel->isSeekable(_pDataModel->getPath(_pObject->getIndex()));
    }
}


std::streamsize
ServerObjectResource::getSize()
{
    if (_pObject->isContainer()) {
//        return 0;
        return static_cast<ServerContainer*>(_pObject)->_childrenPlaylistSize;
    }
    else {
        return _pDataModel->getSize(_pDataModel->getPath(_pObject->getIndex()));
    }
}


std::istream*
ServerObjectResource::getStream()
{
    // TODO: create playlist stream for container resources
    if (_pObject->isContainer()) {
        return static_cast<ServerContainer*>(_pObject)->generateChildrenPlaylist();
    }
    else {
        return _pDataModel->getStream(_pDataModel->getPath(_pObject->getIndex()));
    }
}


ServerObject::ServerObject(MediaServer* pServer) :
_index(AbstractDataModel::INVALID_INDEX),
_pParent(0),
_pServer(pServer),
_pDataModel(0),
_isVirtual(false)
{
}


ServerObject::~ServerObject()
{
//    if (_pItemServer) {
//        _pItemServer->stop();
//        delete _pItemServer;
//        _pItemServer = 0;
//    }
}


ServerObject*
ServerObject::createChildObject()
{
    Log::instance()->upnpav().debug("server container create child object");

    ServerObject* pObject = new ServerObject(_pServer);
    pObject->_pDataModel = _pDataModel;

    return pObject;
}


ServerObjectResource*
ServerObject::createResource()
{
    Log::instance()->upnpav().debug("server container create resource");

    return new ServerObjectResource(this, _pDataModel);
}


std::string
ServerObject::getId()
{
    // flat model
    std::string objectId;
    if (_index == AbstractDataModel::INVALID_INDEX) {
        objectId = "0";
    }
    else {
        objectId = (_isVirtual ? "v" : "") + Poco::NumberFormatter::format(_index);
    }
    Log::instance()->upnpav().debug("server object id: " + objectId);
    return objectId;

    // data model based relationship between media objects
//    std::string objectId;
//    if (_index == AbstractDataModel::INVALID_INDEX) {
//        objectId = "0";
//    }
//    else {
//        objectId = getParentId() + "/" + Poco::NumberFormatter::format(_index);
//    }
//    Log::instance()->upnpav().debug("server object id: " + objectId);
//    return objectId;

    // media object tree based relationship
//    ServerObject* pParent = getParent();
//    if (pParent == 0) {
//        return "0";
//    }
//    else {
//        return pParent->getId() + "/" + Poco::NumberFormatter::format(getIndex());
//    }
}


std::string
ServerObject::getParentId()
{
//    Log::instance()->upnpav().debug("ServerObject::getParentObjectId()");

    std::string parentId;
    if (_index == AbstractDataModel::INVALID_INDEX) {
        parentId = "-1";
    }
    else {
        parentId = "0";
    }
    Log::instance()->upnpav().debug("server parent id: " + parentId);
    return parentId;

    // data model based relationship between media objects
//    std::string parentPath = _pDataModel->getParentPath(_pDataModel->getPath(_index));
//    std::string parentId = "0";
//    while (parentPath != "") {
////        ui4 parentIndex = _pDataModel->getIndex(parentPath);
////        parentId += "/" + Poco::NumberFormatter::format(parentIndex);
//        parentId += "/" + Poco::NumberFormatter::format(_parentIndex);
//        parentPath = _pDataModel->getParentPath(parentPath);
//    }
//    Log::instance()->upnpav().debug("server object parent id: " + parentId);
//    return parentId;

    // media object tree based relationship
//    AbstractMediaObject* pParent = getParent();
//    if (pParent) {
//        return pParent->getId();
//    }
//    else {
//        return "";
//    }
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


bool
ServerObject::isVirtual()
{
    return _isVirtual;
}


void
ServerObject::setIsVirtual(bool isVirtual)
{
    _isVirtual = isVirtual;
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


//AbstractMediaObject*
//ServerObject::createChildObject()
//{
//    ServerObject* pObject = new ServerObject(_pServer);
//    pObject->setParent(this);
//    return pObject;
//}


ServerItem::ServerItem(MediaServer* pServer) :
ServerObject(pServer)
{
}


ServerItem::~ServerItem()
{
}


ServerObjectResource*
ServerItem::createResource()
{
    Log::instance()->upnpav().debug("server item create resource");

    return new ServerObjectResource(this, _pDataModel);
}


const std::string ServerContainer::PROPERTY_GROUP_PROPERTY_NAME = "omm:groupPropName";
const std::string ServerContainer::PROPERTY_GROUP_PROPERTY_VALUE = "omm:groupPropValue";


ServerContainer::ServerContainer(MediaServer* pServer) :
ServerObject(pServer),
//_pDataModel(0),
_pObjectCache(0),
_pVirtualContainerCache(0),
_layout(Flat),
//_layout(DirStruct),
//_layout(PropertyGroups),
//_groupPropertyName(AvProperty::CLASS),
_groupPropertyName(AvProperty::ARTIST),
_childrenPlaylistSize(0),
_updateCacheThreadRunnable(*this, &ServerContainer::updateCacheThread),
_updateCacheThreadRunning(false)
{
    setIsContainer(true);
    _pServer->_pServerContainer = this;
}


void
ServerContainer::addPlaylistResource()
{
    std::string resourceUri = _pServer->getServerAddress() + "/0";
    AbstractResource* pResource = createResource();
    pResource->setUri(resourceUri + "$0");
    pResource->setProtInfo("http-get:*:audio/m3u:*");
    addResource(pResource);
}


AbstractDataModel*
ServerContainer::getDataModel()
{
    return _pDataModel;
}


void
ServerContainer::setDataModel(AbstractDataModel* pDataModel)
{
    _pDataModel = pDataModel;
    _pDataModel->setServerContainer(this);
    if (_pDataModel->useObjectCache()) {
        _pObjectCache = new DatabaseCache("objcache");
        _pObjectCache->_pServerContainer = this;
        _pObjectCache->addPropertiesForQuery(_pDataModel->getQueryProperties());

        // object cache may modify the proposed list of properties that can be queried (due to cache limitations)
        _searchCaps = _pObjectCache->getPropertiesForQuery();
        _sortCaps = _pObjectCache->getPropertiesForQuery();

        _pVirtualContainerCache = new DatabaseCache("vconcache");
        _pVirtualContainerCache->_pServerContainer = this;
        _pVirtualContainerCache->addPropertiesForQuery(CsvList(PROPERTY_GROUP_PROPERTY_NAME, PROPERTY_GROUP_PROPERTY_VALUE));
    }
}


ServerObjectCache*
ServerContainer::getObjectCache()
{
    return _pObjectCache;
}


void
ServerContainer::setObjectCache(ServerObjectCache* pObjectCache)
{
    _pObjectCache = pObjectCache;
}


ServerContainer::Layout
ServerContainer::getLayout()
{
    return _layout;
}


ServerContainer*
ServerContainer::createMediaContainer()
{
    Log::instance()->upnpav().debug("server container create media container");

    ServerContainer* pContainer = new ServerContainer(_pServer);
    pContainer->_pDataModel = _pDataModel;
    pContainer->_pObjectCache = _pObjectCache;
    pContainer->_pVirtualContainerCache = _pVirtualContainerCache;
    pContainer->setIsContainer(true);
    pContainer->setClass(AvClass::className(AvClass::CONTAINER));

    return pContainer;
}


ServerItem*
ServerContainer::createMediaItem()
{
    Log::instance()->upnpav().debug("server container create media item");

    ServerItem* pItem = new ServerItem(_pServer);
    pItem->_pDataModel = _pDataModel;

    return pItem;
}


bool
ServerContainer::isSearchable()
{
    return (_pObjectCache != 0);
}


CsvList*
ServerContainer::getSearchCaps()
{
    if (_searchCaps.getSize()) {
        return &_searchCaps;
    }
    else {
        return 0;
    }
}


CsvList*
ServerContainer::getSortCaps()
{
    if (_sortCaps.getSize()) {
        return &_sortCaps;
    }
    else {
        return 0;
    }
}


void
ServerContainer::setBasePath(const std::string& basePath)
{
    Log::instance()->upnpav().debug("server container, set base path to: " + basePath);
    _pDataModel->setBasePath(basePath);
    if (_pObjectCache) {
        std::string cacheFilePath = Util::Home::instance()->getCacheDirPath(_pDataModel->getModelClass() + "/" + basePath) + "/objects";
        _pObjectCache->setCacheFilePath(cacheFilePath);
        _pVirtualContainerCache->setCacheFilePath(cacheFilePath);
        updateCache();
    }
}


void
ServerContainer::updateCache(bool on)
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
ServerContainer::updateCacheThread()
{
    Log::instance()->upnpav().debug("server container, update cache thread started ...");
    if (!cacheNeedsUpdate()) {
        Log::instance()->upnpav().debug("database cache is current, nothing to do");
    }
    else {
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); it != _pDataModel->endIndex(); ++it) {
            if (!updateCacheThreadIsRunning()) {
                Log::instance()->upnpav().debug("stopping scan thread");
                break;
            }
            _pObjectCache->insertMediaObject(ServerContainer::getChildForIndex((*it).first, false));
        }
        _pObjectCache->updateVirtualObjects(_pVirtualContainerCache);
    }
    _updateCacheThreadLock.lock();
    _updateCacheThreadRunning = false;
    _updateCacheThreadLock.unlock();
    Log::instance()->upnpav().debug("server container, update cache thread finished.");
}


bool
ServerContainer::updateCacheThreadIsRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_updateCacheThreadLock);
    Log::instance()->upnpav().debug("server container, update cache thread is running: " + std::string(_updateCacheThreadRunning ? "yes" : "no"));
    return _updateCacheThreadRunning;
}


bool
ServerContainer::cacheNeedsUpdate()
{
    if (!_pObjectCache) {
        return false;
    }
    ui4 rows = _pObjectCache->rowCount();
    if (rows > _pDataModel->getIndexCount()) {
        Log::instance()->upnpav().error("server container, database cache not coherent.");
        return true;
    }
    else if (rows == _pDataModel->getIndexCount()) {
        Log::instance()->upnpav().debug("server container, database cache is current.");
        return false;
    }
    else {
        Log::instance()->upnpav().debug("server container, database cache needs update.");
        return true;
    }
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


ServerObject*
ServerContainer::getDescendant(const std::string& objectId)
{
    Log::instance()->upnpav().debug("server container get descendant with object id relative to this container's id (in most cases \"0\"): " + objectId);

    std::string::size_type slashPos = objectId.find('/');
    if (slashPos != std::string::npos) {
        // child is a container
        ServerContainer* pChild = static_cast<ServerContainer*>(getChildForIndexString(objectId.substr(0, slashPos)));
        if (pChild == 0) {
            // child container is not a child of this container, we try the full path
            pChild = static_cast<ServerContainer*>(getChildForIndexString(objectId));
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
        if (objectId == "0") {
            return this;
        }
//        ServerItem* pChild = static_cast<ServerItem*>(getChildForIndexString(objectId));
        ServerObject* pChild = getChildForIndexString(objectId);
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
ServerContainer::getChildForIndexString(const std::string& indexString)
{
    if (indexString == "") {
        return 0;
    }
    else {
        ui4 index = AbstractDataModel::INVALID_INDEX;
        bool isVirtual = (indexString.at(0) == 'v');
        try {
            index = Poco::NumberParser::parseUnsigned(isVirtual ? indexString.substr(1) : indexString);
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnpav().error("server container could no parse index string");
        }
        return getChildForIndex(index, true, isVirtual);
    }
}


ServerObject*
ServerContainer::getChildForIndex(ui4 index, bool init, bool isVirtual)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    if (!_pDataModel) {
        // currently, only server containers with data models are handled
        Log::instance()->upnpav().error("server container without data model, can not retrieve child objects");
        return 0;
    }

    ServerObject* pObject = 0;
    if (isVirtual && _pVirtualContainerCache) {
        pObject = _pVirtualContainerCache->getMediaObjectForIndex(index);
        pObject->_isVirtual = true;
    }
    else if (_pObjectCache && !updateCacheThreadIsRunning() && !cacheNeedsUpdate()) {
        // get media object out of data base cache (column xml)
         pObject = _pObjectCache->getMediaObjectForIndex(index);
         pObject->setIsVirtual(true);
    }
    else {
        std::string path = _pDataModel->getPath(index);
        Log::instance()->upnpav().debug("server container, get child from data model with index: " + Poco::NumberFormatter::format(index) + ", path: " + path);
        pObject = _pDataModel->getMediaObject(path);
    }
    if (pObject) {
        initChild(pObject, index, init);
    }
    return pObject;
}


ui4
ServerContainer::getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

     if (!_pDataModel) {
        // currently, only server containers with data models are handled
        Log::instance()->upnpav().error("server container without data model, can not retrieve child objects");
        return 0;
    }

    ui4 childCount = 0;
    bool updateCache = cacheNeedsUpdate();
    if (_index == AbstractDataModel::INVALID_INDEX && _layout == PropertyGroups && _pVirtualContainerCache) {
        // parent container is root and we want to browse virtual child containers
        childCount = _pVirtualContainerCache->getBlockAtRow(children, this, offset, count, sort, search);
    }
    else if (_pObjectCache && !updateCache) {
        childCount = _pObjectCache->getBlockAtRow(children, this, offset, count, sort, search);
    }
    else {
        childCount = _pDataModel->getBlockAtRow(children, offset, count, sort, search);
    }
    for (std::vector<ServerObject*>::iterator it = children.begin(); it != children.end(); ++it) {
        initChild(*it, (*it)->getIndex());
    }
    return childCount;
}


std::stringstream*
ServerContainer::generateChildrenPlaylist()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    if (_pObjectCache) {
        int i = 0;
        int maxLogEntries = 5;
        std::string playlistLog;
        std::vector<ui4> indices;
        _pObjectCache->getIndices(indices);
        for (std::vector<ui4>::iterator it = indices.begin(); it != indices.end(); ++it, ++i) {
            std::string playlistEntry = _pServer->getServerAddress() + "/" + Poco::NumberFormatter::format(*it) + "$0" + Poco::LineEnding::NEWLINE_LF;
            _childrenPlaylist << playlistEntry;
            _childrenPlaylistSize += playlistEntry.size();

            if (i == maxLogEntries) {
                playlistLog += "..." + Poco::LineEnding::NEWLINE_DEFAULT;
            }
            if (indices.size() < maxLogEntries || i < maxLogEntries) {
                playlistLog += playlistEntry;
            }
            else if (i >= indices.size() - maxLogEntries) {
                playlistLog += playlistEntry;
            }
        }
        Log::instance()->upnpav().debug("children playlist: " + Poco::LineEnding::NEWLINE_DEFAULT + playlistLog);
    }
    else if (_pDataModel) {
// solely AbstractDataModel based retrieval of indices:
        int i = 0;
        int maxLogEntries = 5;
        std::string playlistLog;
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); it != _pDataModel->endIndex(); ++it, ++i) {
            std::string playlistEntry = _pServer->getServerAddress() + "/" + Poco::NumberFormatter::format((*it).first) + "$0" + Poco::LineEnding::NEWLINE_LF;
            _childrenPlaylist << playlistEntry;
            _childrenPlaylistSize += playlistEntry.size();

            if (i == maxLogEntries) {
                playlistLog += "..." + Poco::LineEnding::NEWLINE_DEFAULT;
            }
            if (_pDataModel->getIndexCount() < maxLogEntries || i < maxLogEntries) {
                playlistLog += playlistEntry;
            }
            else if (i >= _pDataModel->getIndexCount() - maxLogEntries) {
                playlistLog += playlistEntry;
            }
        }
        Log::instance()->upnpav().debug("children playlist: " + Poco::LineEnding::NEWLINE_DEFAULT + playlistLog);
    }
    else {
        // currently, only server containers with object caches are handled
        Log::instance()->upnpav().error("server container without object cache or data model, can not generate playlist");
        return 0;
    }

    return &_childrenPlaylist;
}


void
ServerContainer::initChild(ServerObject* pObject, ui4 index, bool fullInit)
{
    Log::instance()->upnpav().debug("server container, init child with title: " + pObject->getTitle() + ", index: " + Poco::NumberFormatter::format(index));

    // set index
    pObject->setIndex(index);

    // set parent index
    if (_isVirtual) {
        pObject->setParentIndex(_index);
    }
    else {
        std::string path = _pDataModel->getPath(index);
        std::string parentPath = _pDataModel->getParentPath(path);
        if (parentPath == "") {
            pObject->setParentIndex(AbstractDataModel::INVALID_INDEX);
        }
        else {
            pObject->setParentIndex(_pDataModel->getIndex(parentPath));
        }
    }

    // for storing the object only in cache, stop decorating media object
    if (!fullInit) {
        return;
    }

    // add resources
    std::string objectId = pObject->getId();
    std::string resourceUri = _pServer->getServerAddress() + "/" + objectId;
    if (pObject->isContainer()) {
        Log::instance()->upnpav().debug("server container, init child create container resource");
        // add playlist resource
        AbstractResource* pResource = pObject->createResource();
        pResource->setUri(resourceUri + "$0");
        pResource->setProtInfo("http-get:*:audio/m3u:*");
        pObject->addResource(pResource);
    }
    else {
        Log::instance()->upnpav().debug("server container, init child create item resources");
        for (int r = 0; r < pObject->getResourceCount(); r++) {
            AbstractResource* pResource = pObject->getResource(r);
            if (pResource) {
                pResource->setUri(resourceUri + "$" + Poco::NumberFormatter::format(r));
            }
        }
    }


//    pObject->setId(pObject->getId());

//    ServerObject* pParent;
//    if (parentPath == "") {
//        pParent = this;
//    }
//    else {
//        pParent = _pDataModel->getMediaObject(parentPath);
//    }
//    pObject->setParent(pParent);
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


CsvList&
ServerObjectCache::getPropertiesForQuery()
{
    return _queryPropertyNames;
}


DatabaseCache::DatabaseCache(const std::string& cacheTableName) :
_pSession(0),
_cacheTableName(cacheTableName),
_maxQueryPropertyCount(5)
{
    Log::instance()->upnpav().debug("database cache ctor");
    Poco::Data::SQLite::Connector::registerConnector();

    _queryPropertyNames.append(AvProperty::CLASS);
    _propertyColumnNames[AvProperty::CLASS] = "class";
    _queryPropertyNames.append(AvProperty::TITLE);
    _propertyColumnNames[AvProperty::TITLE] = "title";

    _propertyColumnTypes[AvProperty::ORIGINAL_TRACK_NUMBER] = "INTEGER(2)";
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
    Log::instance()->upnpav().debug("database cache set cache file path: " + cacheFilePath);
    _cacheFilePath = cacheFilePath;
    _pSession = new Poco::Data::Session("SQLite", cacheFilePath);

    // FIXME: UNSIGNED INT(4) for index and parent index doesn't work with SQLite.
    std::string createTableString = "CREATE TABLE " + _cacheTableName + " (" + "idx INTEGER(8), paridx INTEGER(8)";
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        createTableString += ", " + getColumnName(*it) + " " + getColumnType(*it);
    }
    createTableString += ", xml VARCHAR)";

    try {
        *_pSession << createTableString, Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating object cache table failed: " + e.displayText());
    }
    try {
        *_pSession << "CREATE UNIQUE INDEX " + _cacheTableName + "_idx ON " + _cacheTableName + " (idx)", Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating index on object cache table failed: " + e.displayText());
    }
}


ui4
DatabaseCache::rowCount()
{
    Poco::Data::Statement select(*_pSession);
    std::string statement = "SELECT idx FROM " + _cacheTableName;
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
DatabaseCache::getMediaObjectForIndex(ui4 index, bool isVirtual)
{
    Log::instance()->upnpav().debug("database cache table " + _cacheTableName + " get object for index: " + Poco::NumberFormatter::format(index));

    std::vector<std::string> objectClass;
    std::vector<std::string> xml;
    try {
        *_pSession << "SELECT class, xml FROM " + _cacheTableName + " WHERE idx = :index", Poco::Data::use(index), Poco::Data::into(objectClass), Poco::Data::into(xml), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("database cache get object for index failed: " + e.displayText());
    }
    Log::instance()->upnpav().debug("database cache get xml for object with index: " + Poco::NumberFormatter::format(index));
    if (xml.size() == 1) {
        Log::instance()->upnpav().debug("database cache got xml: " + xml[0]);
        ServerObject* pObject;
        if (AvClass::matchClass(objectClass[0], AvClass::CONTAINER)) {
            pObject = _pServerContainer->createMediaContainer();
        }
        else {
            pObject = _pServerContainer->createMediaItem();
        }
        MediaObjectReader xmlReader;
        xmlReader.read(pObject, MediaObjectWriter2::getXmlProlog() + xml[0]);
        return pObject;
    }
    else {
        Log::instance()->upnpav().error("database cache get object for index reading meta data failed.");
        return 0;
    }
}


ui4
DatabaseCache::getBlockAtRow(std::vector<ServerObject*>& block, ServerContainer* pParentContainer, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Log::instance()->upnpav().debug("database cache table " + _cacheTableName + " get block at offset: " + Poco::NumberFormatter::format(offset) + ", count: " + Poco::NumberFormatter::format(count) + ", sort: " + sort + ", search: " + search);


    ui4 parentIndex = AbstractDataModel::INVALID_INDEX;
    bool parentIsVirtual = false;
    if (pParentContainer) {
        parentIndex = pParentContainer->_index;
        parentIsVirtual = pParentContainer->isVirtual();
    }

    Log::instance()->upnpav().debug("database cache parent index: " + Poco::NumberFormatter::format(parentIndex));

    Poco::Data::Statement select(*_pSession);
    std::string statement = "SELECT idx, class, xml FROM " + _cacheTableName;
    std::string whereClause = "";
    bool useParentIndex = false;
    bool virtualChildObjects = false;

    if (search != "*") {
        whereClause += search;
    }
    if (_pServerContainer->getLayout() == ServerContainer::Flat) {
        Log::instance()->upnpav().debug("database cache server container layout: Flat");
        whereClause += std::string(whereClause == "" ? "" : " AND") + " class <> \"" + AvClass::className(AvClass::CONTAINER) + "\"";
    }
    else if (_pServerContainer->getLayout() == ServerContainer::DirStruct && search == "*") {
        Log::instance()->upnpav().debug("database cache server container layout: DirStruct");
        useParentIndex = true;
        whereClause += std::string(whereClause == "" ? "" : " AND") + " paridx = :paridx";
    }
    else if (_pServerContainer->getLayout() == ServerContainer::PropertyGroups) {
        Log::instance()->upnpav().debug("database cache server container layout: PropertyGroups");
        if (parentIndex == AbstractDataModel::INVALID_INDEX) {
            Log::instance()->upnpav().debug("database cache parent container is root container");
            virtualChildObjects = true;
//            statement = "SELECT idx, class, xml FROM " + _cacheTableName + " WHERE prop0 = \"upnp:class\"";
            statement = "SELECT idx, class, xml FROM " + _cacheTableName + " WHERE prop0 = \"" + pParentContainer->_groupPropertyName + "\"";
        }
//        else if (pParentContainer->isVirtual()) {
        else {
            Log::instance()->upnpav().debug("database cache parent container is virtual");
            AbstractProperty* pGroupPropertyName = pParentContainer->getProperty(ServerContainer::PROPERTY_GROUP_PROPERTY_NAME);
            AbstractProperty* pGroupPropertyValue = pParentContainer->getProperty(ServerContainer::PROPERTY_GROUP_PROPERTY_VALUE);
            if (pGroupPropertyName && pGroupPropertyValue) {
                Log::instance()->upnpav().debug("database cache parent container group property name: " + pGroupPropertyName->getValue());
                whereClause += std::string(whereClause == "" ? "" : " AND") + " " + getColumnName(pGroupPropertyName->getValue()) + " = \"" + pGroupPropertyValue->getValue() + "\"";
            }
        }
    }
    if (whereClause != "") {
        statement += " WHERE " + whereClause;
    }
    Log::instance()->upnpav().debug("database cache execute query: " + statement);
    Log::instance()->upnpav().debug("database cache parent index: " + Poco::NumberFormatter::format(parentIndex));
    if (useParentIndex) {
        select << statement, Poco::Data::use(parentIndex);
    }
    else {
        select << statement;
    }

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
            ui4 index;
            std::string objectClass;
            std::string xml;
            index = recordSet["idx"].convert<ui4>();
            objectClass = recordSet["class"].convert<std::string>();
            xml = MediaObjectWriter2::getXmlProlog() + recordSet["xml"].convert<std::string>();
            // FIXME: if child object is a container, we need to create a ServerContainer, otherwise getChildAtRowOffset() does nothing
            ServerObject* pObject;
            if (AvClass::matchClass(objectClass, AvClass::CONTAINER)) {
                pObject = pParentContainer->createMediaContainer();
            }
            else {
                pObject = pParentContainer->createMediaItem();
            }
            MediaObjectReader xmlReader;
            xmlReader.read(pObject, xml);
            pObject->setIndex(index);
            if (virtualChildObjects) {
                pObject->setIsVirtual(true);
            }
            block.push_back(pObject);
            recordSet.moveNext();
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnpav().warning("database cache get block data for row " + Poco::NumberFormatter::format(r) + " failed: " + e.displayText());
        }
    }
    return recordSet.rowCount();
}


void
DatabaseCache::getIndices(std::vector<ui4>& indices, const std::string& sort)
{
    // TODO: execute same query as in getBlockAtRow(), depending on _layout and sort.
    try {
        *_pSession << "SELECT idx FROM " + _cacheTableName + " WHERE class <> \"" + AvClass::className(AvClass::CONTAINER) + "\"", Poco::Data::into(indices), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("database cache get indices failed: " + e.displayText());
    }
}


void
DatabaseCache::insertMediaObject(ServerObject* pObject)
{
    Log::instance()->upnpav().debug("database cache inserting media object with index: " + Poco::NumberFormatter::format(pObject->getIndex()));
    std::string xml;
    MediaObjectWriter2 xmlWriter(false);
    xmlWriter.write(xml, pObject);
    // chop off xml prolog:
    xml = xml.substr(MediaObjectWriter2::getXmlProlog().size());

    std::string insertString = "INSERT INTO " + _cacheTableName + " (";
    std::string propColString = "idx, paridx";
    std::string propValString = ":idx, :paridx";
    std::vector<std::string> props;
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        propColString += ", " + getColumnName(*it);
        propValString += ", :" + getColumnName(*it);
        AbstractProperty* pProperty = pObject->getProperty(*it);
        props.push_back(pProperty ? pProperty->getValue() : "");
    }
    insertString += propColString + ", xml) VALUES(" + propValString + ", :xml)";

    // this is somehow ugly and limits the number of properties, that can be queried.
    try {
        switch (props.size()) {
            case 2:
                *_pSession << insertString, Poco::Data::use(pObject->getIndex()), Poco::Data::use(pObject->getParentIndex()),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(xml), Poco::Data::now;
                break;
            case 3:
                *_pSession << insertString, Poco::Data::use(pObject->getIndex()), Poco::Data::use(pObject->getParentIndex()),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(xml), Poco::Data::now;
                break;
            case 4:
                *_pSession << insertString, Poco::Data::use(pObject->getIndex()), Poco::Data::use(pObject->getParentIndex()),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(props[3]),
                        Poco::Data::use(xml), Poco::Data::now;
                break;
            case 5:
                *_pSession << insertString, Poco::Data::use(pObject->getIndex()), Poco::Data::use(pObject->getParentIndex()),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(props[3]),
                        Poco::Data::use(props[4]), Poco::Data::use(xml), Poco::Data::now;
                break;
        }
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().debug("database cache inserting media object failed: " + e.displayText());
    }
}


void
DatabaseCache::addPropertiesForQuery(CsvList propertyList)
{
    std::size_t defaultPropCount = _propertyColumnNames.size();
    if (propertyList.getSize() > _maxQueryPropertyCount) {
        Log::instance()->upnpav().warning("database cache can only handle up to " + Poco::NumberFormatter::format(_maxQueryPropertyCount) +
                " properties for query (except for class and title, ignoring the rest. Adapt your data model respectively." );
    }
    std::size_t p = 0;
    for (CsvList::Iterator it = propertyList.begin(); it != propertyList.end() && p < _maxQueryPropertyCount - defaultPropCount; ++it, ++p) {
        _queryPropertyNames.append(*it);
        _propertyColumnNames[*it] = "prop" + Poco::NumberFormatter::format(p);
    }
}


void
DatabaseCache::updateVirtualObjects(ServerObjectCache* pVirtualObjectCache)
{
    ui4 index = 0;
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        if (*it == AvProperty::TITLE) {
            continue;
        }
        std::string columnName = getColumnName(*it);
        std::string statement = "SELECT " + columnName + " FROM " + _cacheTableName + " GROUP BY " + columnName + " ORDER BY " + columnName;
        Log::instance()->upnpav().debug("database cache execute query: " + statement);
        Poco::Data::Statement select(*_pSession);
        select << statement;

        Poco::Data::RecordSet recordSet(select);
        try {
            select.execute();
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnpav().warning("database cache update virtual objects executing query failed: " + e.displayText());
        }
        bool more = recordSet.moveFirst();
        while (more) {
            std::string propVal = recordSet[columnName].convert<std::string>();
            ServerContainer* pContainer = _pServerContainer->createMediaContainer();
            pContainer->setTitle(propVal);
            pContainer->setIndex(index);
            pContainer->setIsVirtual(true);
            pContainer->setParentIndex(AbstractDataModel::INVALID_INDEX);
            MemoryProperty* pGroupPropertyName = new MemoryProperty;
            pGroupPropertyName->setName(ServerContainer::PROPERTY_GROUP_PROPERTY_NAME);
            pGroupPropertyName->setValue(*it);
            pContainer->addProperty(pGroupPropertyName);
            MemoryProperty* pGroupPropertyValue = new MemoryProperty;
            pGroupPropertyValue->setName(ServerContainer::PROPERTY_GROUP_PROPERTY_VALUE);
            pGroupPropertyValue->setValue(propVal);
            pContainer->addProperty(pGroupPropertyValue);

            pVirtualObjectCache->insertMediaObject(pContainer);
            more = recordSet.moveNext();
            index++;
        }
    }
}


std::string
DatabaseCache::getColumnName(const std::string& propertyName)
{
    if (propertyName == Omm::Av::AvProperty::CLASS) {
        return "class";
    }
    else if (propertyName == Omm::Av::AvProperty::TITLE) {
        return "title";
    }
    else {
        std::map<std::string, std::string>::iterator pos = _propertyColumnNames.find(propertyName);
        if (pos == _propertyColumnNames.end()) {
            // leave the property name as is
            return propertyName;
        }
        else {
            return _propertyColumnNames[propertyName];
        }
    }
}


std::string
DatabaseCache::getColumnType(const std::string& propertyName)
{
    std::map<std::string, std::string>::iterator pos = _propertyColumnTypes.find(propertyName);
    if (pos != _propertyColumnTypes.end()) {
        return _propertyColumnTypes[propertyName];
    }
    else {
        return "VARCHAR";
    }
}


const ui4 AbstractDataModel::INVALID_INDEX = 0xffffffff;

AbstractDataModel::AbstractDataModel() :
_pServerContainer(0),
_maxIndex(1)
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
    _indexFilePath = Poco::Path(Util::Home::instance()->getMetaDirPath(getModelClass() + "/" + basePath), "index");
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
//        Log::instance()->upnpav().debug("abstract data model add path: " + path + " with index: " + Poco::NumberFormatter::format(index));
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


ui4
AbstractDataModel::getBlockAtRow(std::vector<ServerObject*>& block, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    // TODO: should be faster with a method getIndexBlock(), implemented with an additional std::vector<ui4> as a sorted index list
    // TODO: implement building sort indices and row filtering in memory without data base, currently sort and search are ignored
    if (sort != "" || search != "*") {
        return 0;
    }
    ui4 r = 0;
    for (IndexIterator it = beginIndex(); (it != endIndex()) && (r < offset + count); ++it) {
        if (r >= offset) {
            ServerObject* pObject = getMediaObject((*it).second);
            pObject->setIndex((*it).first);
            block.push_back(pObject);
        }
        r++;
    }
    return getIndexCount();
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

    ServerObjectResource* pResource = pItem->createResource();
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
