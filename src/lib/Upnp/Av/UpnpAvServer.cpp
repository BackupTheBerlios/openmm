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

#include "UpnpAvServer.h"
#include "UpnpInternal.h"
#include "UpnpAvDevices.h"
#include "UpnpAvDescriptions.h"
//#include "UpnpAvServerPrivate.h"
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

    StreamingResource* pResource;
    StreamingProperty* pProperty;
    std::streamsize resSize = 0;
    if (uri[1] == "i") {
        // object icon requested by controller
        Log::instance()->upnpav().debug("icon request: server creates icon property");
        pProperty = static_cast<StreamingProperty*>(pItem->getProperty(AvProperty::ICON));
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
        pResource = static_cast<StreamingResource*>(pItem->getResource(resourceId));
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


MediaServer::MediaServer()
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
}


void
MediaServer::setRoot(AbstractMediaObject* pRoot)
{
    _pDevContentDirectoryServerImpl->_pRoot = pRoot;
}


std::istream*
StreamingProperty::getStream()
{
    return static_cast<StreamingPropertyImpl*>(_pPropertyImpl)->getStream();
}


StreamingPropertyImpl::StreamingPropertyImpl(StreamingMediaObject* pServer, AbstractMediaObject* pItem) :
_pServer(pServer),
_pItem(pItem)
{

}


std::string
StreamingPropertyImpl::getValue()
{
//    Log::instance()->upnpav().debug("streaming property get resource string");

    std::string serverAddress = _pServer->getServerAddress();
    std::string relativeObjectId = _pItem->getId().substr(_pServer->getId().length()+1);
    std::string resourceId = "i";
    return serverAddress + "/" + relativeObjectId + "$" + resourceId;
}


StreamingResource::StreamingResource(PropertyImpl* pPropertyImpl, StreamingMediaObject* pServer, AbstractMediaObject* pItem) :
AbstractResource(pPropertyImpl),
_pServer(pServer),
_pItem(pItem),
_id(0)
{
}


std::string
StreamingResource::getValue()
{
//    Log::instance()->upnpav().debug("streaming resource get resource string ...");

    std::string serverAddress = _pServer->getServerAddress();

//    Log::instance()->upnpav().debug("streaming resource get relative object id ...");
    std::string relativeObjectId = _pItem->getId().substr(_pServer->getId().length()+1);
//    Log::instance()->upnpav().debug("streaming resource relative object id: " + relativeObjectId);
    std::string resourceId = Poco::NumberFormatter::format(_id);

//    Log::instance()->upnpav().debug("streaming resource get resource string returns: " + serverAddress + "/" + relativeObjectId + "$" + resourceId);
    return serverAddress + "/" + relativeObjectId + "$" + resourceId;
}


std::string
StreamingResource::getAttributeName(int index)
{
    if (index == 0) {
        return AvProperty::PROTOCOL_INFO;
    }
    else if (index == 1) {
        return AvProperty::SIZE;
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
    if (name == AvProperty::PROTOCOL_INFO) {
        return _pServer->getServerProtocol() + ":" + getMime() + ":" + getDlna();
    }
    else if (name == AvProperty::SIZE) {
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
    _pItemServer = new MediaItemServer(port);
    _pItemServer->_pServerContainer = this;
    _pItemServer->start();
}


//StreamingMediaObject::StreamingMediaObject(const StreamingMediaObject& object) :
//_pItemServer(object._pItemServer)
//{
//}


StreamingMediaObject::StreamingMediaObject(bool foo)
{
}


StreamingMediaObject::~StreamingMediaObject()
{
    if (_pItemServer) {
        _pItemServer->stop();
        delete _pItemServer;
        _pItemServer = 0;
    }
}


AbstractMediaObject*
StreamingMediaObject::createChildObject()
{
    return new StreamingMediaItem(this);
}


std::string
StreamingMediaObject::getServerAddress()
{
//    Log::instance()->upnpav().debug("streaming media object get server address ...");

    std::string address = Net::NetworkInterfaceManager::instance()->getValidIpAddress().toString();
    int port = _pItemServer->_socket.address().port();
//    Log::instance()->upnpav().debug("streaming media object g et server address returns: http://" + address + ":" + Poco::NumberFormatter::format(port));
    return "http://" + address + ":" + Poco::NumberFormatter::format(port);
}


std::string
StreamingMediaObject::getServerProtocol()
{
    return _pItemServer->getProtocol();
}


std::istream*
StreamingMediaObject::getIconStream()
{

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
AbstractDataModel::addPath(const std::string& path)
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
}


void
AbstractDataModel::removePath(const std::string& path)
{
    std::map<std::string, ui4>::iterator pos = _pathMap.find(path);
    if (pos  != _pathMap.end()) {
        _indexMap.erase((*pos).second);
        _pathMap.erase(pos);
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
    Log::instance()->upnpav().debug("abstract data model write index cache finished.");
}


//void
//AbstractDataModel::flushIndexBuffer()
//{
//    // TODO: don't add indices to server cache, server shall retrieve the media objects from the data model
////    addIndices(_indexBuffer);
//    _indexBuffer.clear();
//}


//void
//AbstractDataModel::bufferIndex(Omm::ui4 index)
//{
//    _indexBuffer.push_back(index);
//    if (_indexBuffer.size() > _indexBufferSize) {
//        flushIndexBuffer();
//    }
//}


AbstractMediaObject*
SimpleDataModel::getMediaObject(ui4 index)
{
    Log::instance()->upnpav().debug("simple data model get media object for index: " + Poco::NumberFormatter::format(index) + " ...");

    std::string path = getPath(index);
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

    ServerItemResource* pResource = pItem->createResource();
    pResource->setSize(getSize(path));
    // FIXME: add some parts of protinfo in server container / media server.
    pResource->setProtInfo("http-get:*:" + getMime(path) + ":" + getDlna(path));
    pItem->addResource(pResource);
    // TODO: add icon property
//        pItem->_icon = pItem->_path;

    return pItem;
}


AbstractMediaObject*
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

    ServerItemResource* pResource = pItem->createResource();
//    Omm::Av::MemoryResource* pResource = new Omm::Av::MemoryResource;
    pResource->setSize(getSize(path));
    // FIXME: add some parts of protinfo in server container / media server.
    pResource->setProtInfo("http-get:*:" + getMime(path) + ":" + getDlna(path));
    pItem->addResource(pResource);
    // TODO: add icon property
//        pItem->_icon = pItem->_path;

    return pItem;
}


ServerContainer::ServerContainer(int port) :
StreamingMediaObject(port)
{
}


//ServerContainer::ServerContainer(const ServerContainer& container) :
//StreamingMediaObject(container)
//{
//}


ServerContainer::ServerContainer(bool foo) :
StreamingMediaObject(foo)
{
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
    ServerContainer* pContainer = new ServerContainer;
    pContainer->_pItemServer = _pItemServer;
//    MemoryMediaObject* pContainer = new MemoryMediaObject;

    pContainer->setIsContainer(true);
    pContainer->setClass(AvClass::className(AvClass::CONTAINER));

    return pContainer;
}


ServerItem*
ServerContainer::createMediaItem()
{
    Log::instance()->upnpav().debug("server container create media item");

    ServerItem* pItem = new ServerItem(this);
    pItem->setParent(this);
    return pItem;
}


AbstractMediaObject*
ServerContainer::createChildObject()
{
    Log::instance()->upnpav().debug("server container create child object");

    // FIXME: doesn't work for containers stored in the data base cache.
    // for now, file server does add only indices from files, not directories
    ServerItem* pItem = new ServerItem(this);
    pItem->setParent(this);
    return pItem;
//    return new Omm::Av::MemoryMediaObject;
}


bool
ServerContainer::isContainer()
{
    return true;
}


int
ServerContainer::getPropertyCount(const std::string& name)
{
    // the server itself (not the media items) has two properties overall
    if (name == "") {
        return 2;
    }
    // and one property for title and class
    else if (name == AvProperty::TITLE || name == AvProperty::CLASS) {
        return 1;
    }
    else {
        return 0;
    }
}


AbstractProperty*
ServerContainer::getProperty(int index)
{
    if (index == 0) {
        return _pTitleProperty;
    }
    else if (index == 1) {
        return _pClassProperty;
    }
    else {
        return 0;
    }
}


AbstractProperty*
ServerContainer::getProperty(const std::string& name, int index)
{
    if (name == AvProperty::TITLE) {
        return _pTitleProperty;
    }
    else if (name == AvProperty::CLASS) {
        return _pClassProperty;
    }
    else {
        return 0;
    }
}


void
ServerContainer::addProperty(AbstractProperty* pProperty)
{
    if (pProperty->getName() == AvProperty::TITLE) {
        _pTitleProperty = pProperty;
    }
    else if (pProperty->getName() == AvProperty::CLASS) {
        _pClassProperty = pProperty;
    }
}


AbstractProperty*
ServerContainer::createProperty()
{
    return new MemoryProperty;
}


ui4
ServerContainer::getChildCount()
{
    if (!_pDataModel) {
        return 0;
    }
    return _pDataModel->getIndexCount();
}


//ui4
//ServerContainer::getIndexForRow(ui4 row, const std::string& sort, const std::string& search)
//{
//    // TODO: getIndexForRow handles only special case for now without sort and search queries
////    return _pDataModel->getIndex(row);
//}


AbstractMediaObject*
ServerContainer::getChildForIndex(ui4 index)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    std::string path = _pDataModel->getPath(index);
    AbstractMediaObject* pObject = _pDataModel->getMediaObject(path);
    initObject(pObject, index);
    return pObject;
}


ui4
ServerContainer::getChildrenAtRowOffset(std::vector<AbstractMediaObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    Log::instance()->upnpav().debug("server container, get children at row offset.");

    ui4 totalChildCount = 0;
    if (sort == "" && search == "*") {
        ui4 r = 0;
        // TODO: should be faster with method getIndexBlock() in AbstractDataModel, implemented there with an additional std::vector<ui4>
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); (it != _pDataModel->endIndex()) && (r < offset + count); ++it) {
            if (r >= offset) {
                AbstractMediaObject* pObject = _pDataModel->getMediaObject((*it).second);
//                if (!pObject->isContainer()) {
                    initObject(pObject, (*it).first);
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
ServerContainer::initObject(AbstractMediaObject* pObject, ui4 index)
{
    pObject->setIndex(index);
    std::string path = _pDataModel->getPath(index);
    std::string parentPath = _pDataModel->getParentPath(path);
    AbstractMediaObject* pParent;
    if (parentPath == "") {
        pParent = this;
    }
    else {
        pParent = _pDataModel->getMediaObject(parentPath);
    }
    pObject->setParent(pParent);
//    pObject->setParent(this);
}


CachedServerContainer::CachedServerContainer() :
_updateCacheThreadRunnable(*this, &CachedServerContainer::updateCacheThread),
_updateCacheThreadRunning(false)
{
    setContainer(this);

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


//void
//CachedServerContainer::addIndices(const std::vector<ui4>& indices)
//{
//    // insert indices and meta data into database cache
//    for (std::vector<ui4>::const_iterator it = indices.begin(); it != indices.end(); ++it) {
//        insertMediaObject(_pDataModel->getMediaObject(*it));
//    }
//}


//void
//CachedServerContainer::removeIndices(const std::vector<ui4>& indices)
//{
//    // TODO: implement removing of indices and meta data from database cache
//}


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


AbstractMediaObject*
CachedServerContainer::getChildForIndex(ui4 index)
{
    if (!_pDataModel) {
        return 0;
    }
    if (_pDataModel->useObjectCache() && !updateCacheThreadIsRunning()) {
        // get media object out of data base cache (column xml)
         AbstractMediaObject* pObject = DatabaseCache::getMediaObjectForIndex(index);
         if (pObject) {
             initObject(pObject, index);
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
CachedServerContainer::getChildrenAtRowOffset(std::vector<AbstractMediaObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
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
            for (std::vector<AbstractMediaObject*>::iterator it = children.begin(); it != children.end(); ++it) {
                initObject(*it, (*it)->getIndex());
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


ServerItemResource::ServerItemResource(ServerContainer* pServerContainer, AbstractMediaObject* pItem) :
StreamingResource(new MemoryPropertyImpl, pServerContainer, pItem)
{
}


bool
ServerItemResource::isSeekable()
{
//    Log::instance()->upnpav().debug("cached item is seekable");
    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
    if (!pDataModel) {
        return false;
    }
    return pDataModel->isSeekable(pDataModel->getPath(_pItem->getIndex()));
}


std::streamsize
ServerItemResource::getSize()
{
//    Log::instance()->upnpav().debug("cached item get size");
    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
    if (!pDataModel) {
        return 0;
    }
    return pDataModel->getSize(pDataModel->getPath(_pItem->getIndex()));
}


std::istream*
ServerItemResource::getStream()
{
//    Log::instance()->upnpav().debug("cached item get stream");
    AbstractDataModel* pDataModel = static_cast<ServerContainer*>(_pServer)->getDataModel();
    if (!pDataModel) {
        return 0;
    }
    return pDataModel->getStream(pDataModel->getPath(_pItem->getIndex()));
}


ServerItem::ServerItem(ServerContainer* pServer) :
StreamingMediaItem(pServer)
{
}


ServerItem::~ServerItem()
{
}


ServerItemResource*
ServerItem::createResource()
{
    Log::instance()->upnpav().debug("server item create resource");

    return new ServerItemResource(static_cast<ServerContainer*>(_pServer), this);
}


//TorchServerContainer::TorchServerContainer(int port) :
//ServerContainer(port),
//_pChild(new TorchItem(this))
//{
//    _pChild->setParent(this);
//}
//
//
//TorchServerContainer::~TorchServerContainer()
//{
//    delete _pDataModel;
//    delete _pChild;
//    delete _pTitleProperty;
//    delete _pClassProperty;
//}
//
//
//void
//TorchServerContainer::setDataModel(SimpleDataModel* pDataModel)
//{
//    ServerContainer::setDataModel(pDataModel);
//}
//
//
//SimpleDataModel*
//TorchServerContainer::getDataModel()
//{
//    return static_cast<SimpleDataModel*>(_pDataModel);
//}
//
//
//AbstractMediaObject*
//TorchServerContainer::getChildForIndex(ui4 index)
//{
//    _pChild->setIndex(index);
//    TorchItem* pTorchChild = static_cast<TorchItem*>(_pChild);
//    pTorchChild->_optionalProps.clear();
//    SimpleDataModel* pDataModel = getDataModel();
//
//    // FIXME: title property of child item should get it's title based on item's object number
//    pTorchChild->_pClassProp->setValue(pDataModel->getClass(index));
//    pTorchChild->_pTitleProp->setValue(pDataModel->getTitle(index));
//    std::string artist = pDataModel->getOptionalProperty(index, AvProperty::ARTIST);
//    if (artist != "") {
//        pTorchChild->_pArtistProp->setValue(artist);
//        pTorchChild->_optionalProps.push_back(pTorchChild->_pArtistProp);
//    }
//    std::string album = pDataModel->getOptionalProperty(index, AvProperty::ALBUM);
//    if (album != "") {
//        pTorchChild->_pAlbumProp->setValue(album);
//        pTorchChild->_optionalProps.push_back(pTorchChild->_pAlbumProp);
//    }
//    std::string track = pDataModel->getOptionalProperty(index, AvProperty::ORIGINAL_TRACK_NUMBER);
//    if (track != "") {
//        pTorchChild->_pTrackProp->setValue(track);
//        pTorchChild->_optionalProps.push_back(pTorchChild->_pTrackProp);
//    }
//    std::string genre = pDataModel->getOptionalProperty(index, AvProperty::GENRE);
//    if (genre != "") {
//        pTorchChild->_pGenreProp->setValue(genre);
//        pTorchChild->_optionalProps.push_back(pTorchChild->_pGenreProp);
//    }
//    std::string icon = pDataModel->getOptionalProperty(index, AvProperty::ICON);
//    if (icon != "") {
//        Log::instance()->upnpav().debug("torch server adds icon property: " + icon);
//        pTorchChild->_pIconProp->setValue(icon);
//        pTorchChild->_optionalProps.push_back(pTorchChild->_pIconProp);
//    }
//
//    return _pChild;
//}
//
//
//AbstractMediaObject*
//TorchServerContainer::getChildForRow(ui4 row)
//{
//    // TODO: special case here, row == index, no search, sort, add/remove items or similar possible
//    return getChildForIndex(row);
//}
//
//
//ui4
//TorchServerContainer::getChildCount()
//{
//    if (_pDataModel) {
//        return _pDataModel->getIndexCount();
//    }
//    else {
//        return 0;
//    }
//}
//
//
//TorchItemResource::TorchItemResource(TorchServerContainer* pServer, AbstractMediaObject* pItem) :
//StreamingResource(new MemoryPropertyImpl, pServer, pItem)
//{
//}
//
//
//bool
//TorchItemResource::isSeekable()
//{
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
////        return pDataModel->isSeekable(_pItem->getIndex());
//        return pDataModel->isSeekable(pDataModel->getPath(_pItem->getIndex()));
//    }
//    else {
//        return false;
//    }
//}
//
//
//std::istream*
//TorchItemResource::getStream()
//{
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
////        return pDataModel->getStream(_pItem->getIndex());
//        return pDataModel->getStream(pDataModel->getPath(_pItem->getIndex()));
//    }
//    else {
//        return 0;
//    }
//}
//
//
//std::streamsize
//TorchItemResource::getSize()
//{
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
////        return pDataModel->getSize(_pItem->getIndex());
//        return pDataModel->getSize(pDataModel->getPath(_pItem->getIndex()));
//    }
//    else {
//        return 0;
//    }
//}
//
//
//std::string
//TorchItemResource::getMime()
//{
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
////        return pDataModel->getMime(_pItem->getIndex());
//        return pDataModel->getMime(pDataModel->getPath(_pItem->getIndex()));
//    }
//    else {
//        return "*";
//    }
//}
//
//
//std::string
//TorchItemResource::getDlna()
//{
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
//        return pDataModel->getDlna(_pItem->getIndex());
//    }
//    else {
//        return "*";
//    }
//}
//
//
//TorchItemPropertyImpl::TorchItemPropertyImpl(TorchServerContainer* pServer, AbstractMediaObject* pItem) :
//StreamingPropertyImpl(pServer, pItem)
//{
//}
//
//
//void
//TorchItemPropertyImpl::setName(const std::string& name)
//{
//    _name = name;
//}
//
//
//void
//TorchItemPropertyImpl::setValue(const std::string& value)
//{
//    _value = value;
//}
//
//
//std::string
//TorchItemPropertyImpl::getName()
//{
////    Log::instance()->upnpav().debug("TorchItemPropertyImpl::getName() returns: " + _name);
//
//    return _name;
//}
//
//
//std::string
//TorchItemPropertyImpl::getValue()
//{
////    Log::instance()->upnpav().debug("TorchItemPropertyImpl::getValue() returns: " + _value);
//
//    if (_name == AvProperty::CLASS || _name == AvProperty::TITLE
//        || _name == AvProperty::ARTIST || _name == AvProperty::ALBUM || _name == AvProperty::ORIGINAL_TRACK_NUMBER
//        || _name == AvProperty::GENRE) {
//        return _value;
//    }
//    else if (_name == AvProperty::ICON) {
//        return StreamingPropertyImpl::getValue();
//    }
//}
//
//
//std::istream*
//TorchItemPropertyImpl::getStream()
//{
////    Log::instance()->upnpav().debug("TorchItemPropertyImpl::getStream()");
//
//    SimpleDataModel* pDataModel = static_cast<TorchServerContainer*>(_pServer)->getDataModel();
//    if (pDataModel) {
//        return pDataModel->getIconStream(_pItem->getIndex());
//    }
//    else {
//        return 0;
//    }
//}
//
//
//TorchItemProperty::TorchItemProperty(TorchServerContainer* pServer, Omm::Av::AbstractMediaObject* pItem) :
//AbstractProperty(new TorchItemPropertyImpl(pServer, pItem))
//{
//}
//
//
//TorchItem::TorchItem(TorchServerContainer* pServer) :
//StreamingMediaItem(pServer),
//_pClassProp(new TorchItemProperty(pServer, this)),
//_pTitleProp(new TorchItemProperty(pServer, this)),
//_pResource(new TorchItemResource(pServer, this)),
//_pArtistProp(new TorchItemProperty(pServer, this)),
//_pAlbumProp(new TorchItemProperty(pServer, this)),
//_pTrackProp(new TorchItemProperty(pServer, this)),
//_pGenreProp(new TorchItemProperty(pServer, this)),
//_pIconProp(new TorchItemProperty(pServer, this))
//{
////     std::clog << "TorchItem::TorchItem(pServer), pServer: " << pServer << std::endl;
//    _pClassProp->setName(AvProperty::CLASS);
//    _pTitleProp->setName(AvProperty::TITLE);
//
//    _pArtistProp->setName(AvProperty::ARTIST);
//    _pAlbumProp->setName(AvProperty::ALBUM);
//    _pTrackProp->setName(AvProperty::ORIGINAL_TRACK_NUMBER);
//    _pGenreProp->setName(AvProperty::GENRE);
//    _pIconProp->setName(AvProperty::ICON);
//}
//
//
//TorchItem::~TorchItem()
//{
//    delete _pClassProp;
//    delete _pTitleProp;
//    delete _pResource;
//
//    delete _pArtistProp;
//    delete _pAlbumProp;
//    delete _pTrackProp;
//    delete _pGenreProp;
//    delete _pIconProp;
//}
//
//
//int
//TorchItem::getPropertyCount(const std::string& name)
//{
////    Log::instance()->upnpav().debug("TorchItem::getPropertyCount(), name: " + name);
//
//    if (name == "") {
//        return 3 + _optionalProps.size();
//    }
//    else if (name == AvProperty::CLASS || name == AvProperty::TITLE || name == AvProperty::RES
//        || name == AvProperty::ARTIST || name == AvProperty::ALBUM || name == AvProperty::ORIGINAL_TRACK_NUMBER
//        || name == AvProperty::GENRE || name == AvProperty::ICON) {
//        return 1;
//    }
//    else {
//        return 0;
//    }
//}
//
//
//AbstractProperty*
//TorchItem::getProperty(int index)
//{
////    Log::instance()->upnpav().debug("TorchItem::getProperty(index), index: " + Poco::NumberFormatter::format(index));
//
//    if (index == 0) {
//        return _pClassProp;
//    }
//    else if (index == 1) {
//        return _pTitleProp;
//    }
//    else if (index == 2) {
//        return _pResource;
//    }
//    else {
//        return _optionalProps[index - 3];
//    }
//}
//
//
//AbstractProperty*
//TorchItem::getProperty(const std::string& name, int index)
//{
////    Log::instance()->upnpav().debug("TorchItem::getProperty(name, index), name: " + name + ", index: " + Poco::NumberFormatter::format(index));
//
//    if (name == AvProperty::CLASS) {
//        return _pClassProp;
//    }
//    else if (name == AvProperty::TITLE) {
//        return _pTitleProp;
//    }
//    else if (name == AvProperty::RES) {
//        return _pResource;
//    }
//    else {
//        for (int i = 0; i < _optionalProps.size(); i++) {
//            if (_optionalProps[i]->getName() == name) {
//                return _optionalProps[i];
//            }
//        }
//        return 0;
//    }
//}


} // namespace Av
} // namespace Omm
