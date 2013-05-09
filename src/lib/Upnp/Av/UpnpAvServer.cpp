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
#include <Poco/StringTokenizer.h>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPClientSession.h>
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"

#include <map>
#include <list>
#include <vector>
#include <Poco/LineEndingConverter.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/TextConverter.h>
#include <Poco/URI.h>
#include <Poco/UUIDGenerator.h>

#include "UpnpAvServer.h"
#include "UpnpInternal.h"
#include "UpnpAvDevices.h"
#include "UpnpAvDescriptions.h"
//#include "UpnpAvServerPrivate.h"
#include "UpnpAvServerImpl.h"

namespace Omm {
namespace Av {


MediaServer::MediaServer(int port) :
_socket(Poco::Net::ServerSocket(port)),
_pSystemUpdateIdTimer(0),
_systemUpdateIdTimerInterval(0)
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
        new DevConnectionManagerServerImpl(this),
        new DevAVTransportServerImpl)
    );
}


MediaServer::~MediaServer()
{
    LOG(upnpav, debug, "delete media server " + getFriendlyName());

    delete _pHttpServer;
    delete _pDevContentDirectoryServerImpl->_pRoot;
    delete _pDevContentDirectoryServerImpl;
//    LOG(upnpav, information, "done");
}


void
MediaServer::setRoot(ServerContainer* pRoot)
{
    _pDevContentDirectoryServerImpl->_pRoot = pRoot;
}


void
MediaServer::start()
{
    LOG(upnpav, information, "start media server " + getFriendlyName());

    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    _pHttpServer = new Poco::Net::HTTPServer(new ItemRequestHandlerFactory(this), _socket, pParams);
    _pHttpServer->start();
    LOG(upnpav, information, "media server listening on: " + _socket.address().toString());
    startPollSystemUpdateId(true);
}


void
MediaServer::stop()
{
    LOG(upnpav, information, "stop media server " + getFriendlyName());

    startPollSystemUpdateId(false);
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


void
MediaServer::setSystemUpdateId(ui4 id)
{
    _pDevContentDirectoryServerImpl->_setSystemUpdateID(id);
}


void
MediaServer::setPollSystemUpdateIdTimer(long msec)
{
    LOG(upnpav, debug, "set poll system update id timer interval to: " + Poco::NumberFormatter::format(msec));

    _systemUpdateIdTimerInterval = msec;
}


void
MediaServer::startPollSystemUpdateId(bool start)
{
    if (_pSystemUpdateIdTimer) {
        LOG(upnpav, debug, "stop poll system update id timer ...");
        _pSystemUpdateIdTimer->stop();
        delete _pSystemUpdateIdTimer;
        _pSystemUpdateIdTimer = 0;
    }
    if (start && _systemUpdateIdTimerInterval) {
        LOG(upnpav, debug, "start poll system update id timer, polling every " + Poco::NumberFormatter::format(_systemUpdateIdTimerInterval) + " msec");
        _pSystemUpdateIdTimer = new Poco::Timer(0, _systemUpdateIdTimerInterval);
        Poco::TimerCallback<MediaServer> callback(*this, &MediaServer::pollSystemUpdateId);
        _pSystemUpdateIdTimer->start(callback);
    }
}


void
MediaServer::pollSystemUpdateId(Poco::Timer& timer)
{
    LOG(upnpav, debug, "poll system update id of server: \"" + getFriendlyName() + "\"");

    AbstractDataModel* pDataModel = _pServerContainer->getDataModel();
    if (pDataModel) {
        pDataModel->checkSystemUpdateId();
    }
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
    LOG(upnpav, debug, "handle media item request: " + request.getURI());

    response.setDate(Poco::Timestamp());

    std::ostringstream requestHeader;
    request.write(requestHeader);
    LOG(upnpav, debug, "request method: " + request.getMethod());
    LOG(upnpav, debug, "request header:\n" + requestHeader.str());

    Poco::StringTokenizer uri(request.getURI(), "$");
    std::string objectId = uri[0].substr(1);
    LOG(upnpav, debug, "objectId: " + objectId + ", resourceId: " + uri[1]);
    ServerObject* pObject = _pServer->_pServerContainer->getDescendant(objectId);
    if (!pObject) {
        LOG(upnpav, error, "item request handler could not find object with id: " + objectId);
        return;
    }

    ServerObjectResource* pResource;
    ServerObjectProperty* pProperty;
//    Poco::UInt64 resSize = 0;
    std::streamsize resSize = 0;
    LOG(upnpav, debug, "size of streamsize: " + Poco::NumberFormatter::format(sizeof(std::streamsize)));
    if (uri[1] == "i") {
        // object icon requested by controller
        LOG(upnpav, debug, "icon request: server creates icon property");
        pProperty = static_cast<ServerObjectProperty*>(pObject->getProperty(AvProperty::ICON));
        LOG(upnpav, debug, "icon request: server icon property created");
        // deliver icon
        if (pProperty) {
            // icon requested (handle other streamable properties as well, such as album art ...)
            try {
                std::istream* pIstr = pProperty->getStream();
                if (pIstr) {
                    LOG(upnpav, debug, "sending icon ...");
                    std::ostream& ostr = response.send();
                    std::streamsize numBytes = Poco::StreamCopier::copyStream(*pIstr, ostr);
                    LOG(upnpav, debug, "icon sent (" + Poco::NumberFormatter::format(numBytes) + " bytes transfered).");
                    delete pIstr;
                }
            }
            catch(Poco::Exception& e) {
                LOG(upnpav, debug, "delivering icon failed: " + e.displayText());
            }
        }
        return;
    }
    else {
        // resource requested by controller
        int resourceId = Poco::NumberParser::parse(uri[1]);
        pResource = static_cast<ServerObjectResource*>(pObject->getResource(resourceId));
        if (!pResource) {
            LOG(upnpav, error, "item request handler no resource for object with id: " + objectId);
            return;
        }
        LOG(upnpav, debug, "item request handler getting size");
        resSize = pResource->getSize();

        LOG(upnpav, debug, "item request handler getting prot info");
        std::string protInfoString = pResource->getProtInfo();
        ProtocolInfo protInfo(protInfoString);
        std::string mime = protInfo.getMimeString();
        std::string dlna = protInfo.getDlnaString();

        LOG(upnpav, debug, "item request handler writing response header");
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
            // setContentLength(int) argument type int is too small for big files
//            response.setContentLength(len);
            response.set("Content-Length", Poco::NumberFormatter::format(len));
        }
        std::ostringstream responseHeader;
        response.write(responseHeader);
        LOG(upnpav, debug, "response header:\n" + responseHeader.str());
        std::ostream& ostr = response.send();
#ifdef __DARWIN__
        signal(SIGPIPE, SIG_IGN); // fixes abort with "Program received signal SIGPIPE, Broken pipe." on Mac OSX when renderer stops the stream.
#endif
        try {
            std::istream* pIstr;
            // deliver resource
            pIstr = pResource->getStream();
            if (pIstr) {
                LOG(upnpav, debug, "sending stream ...");
//                std::streamsize numBytes = Poco::StreamCopier::copyStream(*pIstr, ostr);
                std::streamsize numBytes = copyStream(*pIstr, ostr, start, end);
                LOG(upnpav, debug, "stream sent (" + Poco::NumberFormatter::format(numBytes) + " bytes transfered).");
                // TODO: define different server resource streaming models:
                // exclusive access (interruptable by second request or not), unlimited number of streams per resource, ...
                // resource stream should always be deleted by model and freed by separate method in interface of model (freeStream(std::istream*))?
//                delete pIstr;
                pResource->freeStream(pIstr);
            }
            else {
                throw Poco::Exception("no stream available");
            }
        }
        catch(Poco::Exception& e) {
            LOG(upnpav, debug, "streaming aborted: " + e.displayText());
        }
    }
    else if (request.getMethod() == "HEAD") {
        if (resSize > 0 ) {
            response.setContentLength(resSize);
        }
        std::ostringstream responseHeader;
        response.write(responseHeader);
        LOG(upnpav, debug, "response header:\n" + responseHeader.str());
        response.send();
    }

    if (response.sent()) {
        LOG(upnpav, debug, "media item request finished: " + request.getURI());
    }
}


std::streamsize
ItemRequestHandler::copyStream(std::istream& istr, std::ostream& ostr, std::streamoff start, std::streamoff end)
{
    LOG(upnpav, debug, "copy stream, start: " + Poco::NumberFormatter::format(start) + ", end: " + Poco::NumberFormatter::format(end));

    if (start > 0) {
        istr.seekg(start);
        LOG(upnpav, debug, "copy stream jumped to position: " + Poco::NumberFormatter::format(istr.tellg()));
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
    if( istr.bad() ) {
       LOG(upnpav, error, "reading resource stream failed");
    }
    std::streamsize n = istr.gcount();
//    LOG(upnpav, debug, "copy stream read bytes: " + Poco::NumberFormatter::format(n));
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
            if( istr.bad() ) {
                LOG(upnpav, error, "reading resource stream failed");
            }
            n = istr.gcount();
        }
        else {
            n = 0;
            LOG(upnpav, error, "copy stream " + std::string(!istr ? "input" : "output") + " stream not ready");
        }
//        LOG(upnpav, debug, "copy stream read bytes: " + Poco::NumberFormatter::format(n));
    }
    LOG(upnpav, debug, "copy stream copied bytes: " + Poco::NumberFormatter::format(len));
    return len;
}


void
ItemRequestHandler::parseRange(const std::string& rangeValue, std::streamoff& start, std::streamoff& end)
{
    std::string range = rangeValue.substr(rangeValue.find('=') + 1);

    std::string::size_type delim = range.find('-');
    start = Poco::NumberParser::parse64(range.substr(0, delim));
    try {
        end = Poco::NumberParser::parse64(range.substr(delim + 1));
    }
    catch (Poco::Exception& e) {
//        LOG(upnpav, warning, "range end parsing: " + e.displayText());
    }
    LOG(upnpav, debug, "range: " + range + " (start: " + Poco::NumberFormatter::format((long)start)\
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


void
ServerObjectResource::freeStream(std::istream* pIstream)
{
    if (_pDataModel) {
        _pDataModel->freeStream(pIstream);
    }
}


void
ServerObjectResource::writeResource(const uri& sourceUri)
{
    LOG(upnpav, debug, "write resource of object with index name space: " + Poco::NumberFormatter::format(_pObject->_indexNamespace));
    if (_pObject->_indexNamespace == ServerObject::User) {
        // FIXME: always access the resource stream via getStream(), even on write operations (same in ServerContainer::initChild())
        std::string indexFileName = _pDataModel->getMetaDirPath() + _pDataModel->getModelClass() + "/" + _pDataModel->getBasePath() + "/" + getPrivateResourceUri();
        LOG(upnpav, debug, "server container, write resource to playlist index file: " + indexFileName);
        std::ofstream indexFile(indexFileName.c_str());

        LOG(upnpav, debug, "write resource, retrieve source from uri: " + sourceUri.toString());
        Poco::Net::HTTPClientSession session(sourceUri.getHost(), sourceUri.getPort());
        Poco::Net::HTTPRequest request("GET", sourceUri.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        LOG(upnpav, debug, "request header:\n" + requestHeader.str());

        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);

        // extract indices from resource URIs in playlist and write them to server stored playlist
        std::string resourceUri;
        while (std::getline(istr, resourceUri)) {
            Poco::URI res(resourceUri);
            std::string path = res.getPath();
            indexFile << path.substr(1, path.find("$") - 1) << std::endl;
        }
    }
}


std::string
ServerObjectResource::getPrivateResourceUri()
{
    return _privateUri;
}


void
ServerObjectResource::setPrivateResourceUri(const std::string& uri)
{
    _privateUri = uri;
}


ServerObject::ServerObject(MediaServer* pServer) :
_index(AbstractDataModel::INVALID_INDEX),
_pParent(0),
_pServer(pServer),
_pDataModel(0),
_indexNamespace(Data)
//_isVirtual(false)
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


void
ServerObject::setUniqueProperty(const std::string& name, const std::string& value)
{
//    LOG(upnpav, debug, "server container set unique property: " + name);
//    LOG(upnpav, debug, "server container data model: " + Poco::NumberFormatter::format(_pDataModel));

    if (_pDataModel) {
        LOG(upnpav, debug, "server container text converter: " + Poco::NumberFormatter::format(_pDataModel->getTextConverter()));
        std::string recodedValue;
        // convert encodings, if necessary
        if (_pDataModel->getTextConverter()) {
            LOG(upnpav, debug, "server container convert character encoding");
            _pDataModel->getTextConverter()->convert(value, recodedValue);
        }
        else {
            recodedValue = value;
        }
        // replace control characters (ASCII < 32) with space to prevent Poco::XMLWriter throwing
        // Poco::XMLException("invalid character token")
        for (std::string::iterator it = recodedValue.begin(); it != recodedValue.end(); ++it) {
            if (*it < 32) {
                *it = ' ';
            }
        }
        MemoryMediaObject::setUniqueProperty(name, recodedValue);
     }
    else {
        MemoryMediaObject::setUniqueProperty(name, value);
    }
}


ServerObject*
ServerObject::createChildObject()
{
    LOG(upnpav, debug, "server container create child object");

    ServerObject* pObject = new ServerObject(_pServer);
    pObject->_pDataModel = _pDataModel;

    return pObject;
}


ServerObjectResource*
ServerObject::createResource()
{
    LOG(upnpav, debug, "server container create resource");

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
        std::string prefix("");
        switch (_indexNamespace) {
            case Virtual:
                prefix = "v";
                break;
            case User:
                prefix = "u";
                break;
        }
        objectId = prefix + Poco::NumberFormatter::format(_index);
    }
    LOG(upnpav, debug, "server object id: " + objectId);
    return objectId;

    // data model based relationship between media objects
//    std::string objectId;
//    if (_index == AbstractDataModel::INVALID_INDEX) {
//        objectId = "0";
//    }
//    else {
//        objectId = getParentId() + "/" + Poco::NumberFormatter::format(_index);
//    }
//    LOG(upnpav, debug, "server object id: " + objectId);
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
//    LOG(upnpav, debug, "ServerObject::getParentObjectId()");

    std::string parentId;
    if (_index == AbstractDataModel::INVALID_INDEX) {
        parentId = "-1";
    }
    else {
        parentId = "0";
    }
    LOG(upnpav, debug, "server parent id: " + parentId);
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
//    LOG(upnpav, debug, "server object parent id: " + parentId);
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
//    LOG(upnpav, debug, "ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    return _index;
}


void
ServerObject::setIndex(ui4 index)
{
//    LOG(upnpav, debug, "ServerObject::setObjectNumber() objectId: " + Poco::NumberFormatter::format(id));
    _index = index;
}


void
ServerObject::setIndex(const std::string& index)
{
//    LOG(upnpav, debug, "ServerObject::setObjectId() from string: " + id);
    _index = Poco::NumberParser::parseUnsigned(index);
}


//bool
//ServerObject::isVirtual()
//{
//    return _isVirtual;
//}
//
//
//void
//ServerObject::setIsVirtual(bool isVirtual)
//{
//    _isVirtual = isVirtual;
//}


ui4
ServerObject::getParentIndex()
{
//    LOG(upnpav, debug, "ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    return _parentIndex;
}


ServerObject*
ServerObject::getParent()
{
//    LOG(upnpav, debug, "ServerObject::getParent()");
    return _pParent;
}


void
ServerObject::setParentIndex(ui4 index)
{
//    LOG(upnpav, debug, "ServerObject::getIndex index: " + Poco::NumberFormatter::format(_index));
    _parentIndex = index;
}


void
ServerObject::setParent(ServerObject* pParent)
{
//    LOG(upnpav, debug, "ServerObject::setParent()");
    _pParent = pParent;
}


//AbstractMediaObject*
//ServerObject::createChildObject()
//{
//    ServerObject* pObject = new ServerObject(_pServer);
//    pObject->setParent(this);
//    return pObject;
//}


MediaServer*
ServerObject::getServer()
{
    return _pServer;
}


void
ServerObject::destroy()
{
    if (_indexNamespace == User) {
        _pServer->_pServerContainer->removeUserObject(this);
    }
}


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
    LOG(upnpav, debug, "server item create resource");

    return new ServerObjectResource(this, _pDataModel);
}


const std::string ServerContainer::LAYOUT_FLAT = "Flat";
const std::string ServerContainer::LAYOUT_DIR_STRUCT = "DirStruct";
const std::string ServerContainer::LAYOUT_PROPERTY_GROUPS = "PropertyGroups";

const std::string ServerContainer::PROPERTY_GROUP_PROPERTY_NAME = "omm:groupPropName";
const std::string ServerContainer::PROPERTY_GROUP_PROPERTY_VALUE = "omm:groupPropValue";


ServerContainer::ServerContainer(MediaServer* pServer) :
ServerObject(pServer),
//_pDataModel(0),
_pObjectCache(0),
_pVirtualContainerCache(0),
_pUserObjectCache(0),
_layout(Flat),
//_groupPropertyName(AvProperty::CLASS),
_groupPropertyName(AvProperty::ARTIST),
_childrenPlaylistSize(0)
{
    setIsContainer(true);
    setIsRestricted(false);
    _pServer->_pServerContainer = this;
}


ServerContainer::~ServerContainer()
{
    if (_pDataModel) {
        _pDataModel->deInit();
        delete _pDataModel;
    }
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


void
ServerContainer::addUserObject(ServerObject* pChildObject)
{
    LOG(upnpav, debug, "server container add user object: " + pChildObject->getTitle());
    std::vector<ui4> indices;
    _pUserObjectCache->getIndices(indices);
    // TODO: don't use max + 1 as new free index
    ui4 maxIndex = 0;
    for (std::vector<ui4>::const_iterator it = indices.begin(); it != indices.end(); ++it) {
        maxIndex = std::max(maxIndex, *it);
    }
    pChildObject->setIndex(maxIndex + 1);
    pChildObject->setParent(this);
    pChildObject->setParentIndex(getIndex());
    // set resource uri of user object
    ServerObjectResource* pResource = new ServerObjectResource(pChildObject, getDataModel());
    // touch a resource file with a unique random filename.
    std::string resourceFileName = Poco::UUIDGenerator::defaultGenerator().createOne().toString();
    std::string resourceFilePath = getDataModel()->getMetaDirPath() + _pDataModel->getModelClass() + "/" + _pDataModel->getBasePath();
    std::ofstream resourceFile((resourceFilePath + "/" + resourceFileName).c_str());
    resourceFile.close();
    pResource->setUri(resourceFileName);
    // FIXME: only valid for playlists
    pResource->setProtInfo("http-get:*:audio/m3u:*");
    pChildObject->addResource(pResource);
    // FIXME: insertMediaObject() gets update id from data model, but for user objects, update id
    //        should be set to current timestamp or 0.
    _pUserObjectCache->insertMediaObject(pChildObject);
}


void
ServerContainer::removeUserObject(ServerObject* pChildObject)
{
    LOG(upnpav, debug, "server container remove user object: " + pChildObject->getTitle());
    _pUserObjectCache->removeMediaObjectForIndex(pChildObject->getIndex());
    std::string metaDir = getDataModel()->getMetaDirPath() + _pDataModel->getModelClass() + "/" + _pDataModel->getBasePath();
    ServerObjectResource* pResource = static_cast<ServerObjectResource*>(pChildObject->getResource());
    std::string resourceFile = pResource->getPrivateResourceUri();
    LOG(upnpav, debug, "server container remove resource: " + metaDir + "/" + resourceFile);
    Poco::File resource(metaDir + "/" + resourceFile);
    try {
        resource.remove();
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, debug, "server container removing resource " + metaDir + "/" + resourceFile + ": " + e.displayText());
    }
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
        _pObjectCache = new DatabaseCache("objcache", Data);
        _pObjectCache->_pServerContainer = this;
        _pObjectCache->addPropertiesForQuery(_pDataModel->getQueryProperties());

        // object cache may modify the proposed list of properties that can be queried (due to cache limitations)
        _searchCaps = _pObjectCache->getPropertiesForQuery();
        _sortCaps = _pObjectCache->getPropertiesForQuery();

        _pVirtualContainerCache = new DatabaseCache("vconcache", Virtual);
        _pVirtualContainerCache->_pServerContainer = this;
        _pVirtualContainerCache->addPropertiesForQuery(CsvList(PROPERTY_GROUP_PROPERTY_NAME, PROPERTY_GROUP_PROPERTY_VALUE));
    }
    _pUserObjectCache = new DatabaseCache("usrobj", User);
    _pUserObjectCache->_pServerContainer = this;
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


void
ServerContainer::setLayout(ServerContainer::Layout layout)
{
    _layout = layout;
}


void
ServerContainer::setLayout(const std::string& layout)
{
    if (layout == LAYOUT_FLAT) {
        _layout = Flat;
    }
    else if (layout == LAYOUT_DIR_STRUCT) {
        _layout = DirStruct;
    }
    else if (layout == LAYOUT_PROPERTY_GROUPS) {
        _layout = PropertyGroups;
    }
}


ServerContainer*
ServerContainer::createMediaContainer()
{
    LOG(upnpav, debug, "server container create media container");

    ServerContainer* pContainer = new ServerContainer(_pServer);
    pContainer->_pDataModel = _pDataModel;
    pContainer->_pObjectCache = _pObjectCache;
    pContainer->_pVirtualContainerCache = _pVirtualContainerCache;
    pContainer->_pUserObjectCache = _pUserObjectCache;
    pContainer->setIsContainer(true);
    pContainer->setClass(AvClass::className(AvClass::CONTAINER));

    return pContainer;
}


ServerItem*
ServerContainer::createMediaItem()
{
    LOG(upnpav, debug, "server container create media item");

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
    LOG(upnpav, debug, "server container, set base path to: " + basePath);
    if (_pObjectCache) {
        std::string cacheFileDir = getDataModel()->getCacheDirPath() + _pDataModel->getModelClass() + "/" + basePath;
        try {
            Poco::File(cacheFileDir).createDirectories();
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, error, "can not create cache file directory: " + cacheFileDir);
        }
        std::string cacheFilePath = cacheFileDir + "/objects";
        _pObjectCache->setCacheFilePath(cacheFilePath);
        _pVirtualContainerCache->setCacheFilePath(cacheFilePath);
//        updateCache();
    }
    std::string metaFileDir = getDataModel()->getMetaDirPath() + _pDataModel->getModelClass() + "/" + basePath;
    try {
            Poco::File(metaFileDir).createDirectories();
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, error, "can not create meta file directory: " + metaFileDir);
        }
    std::string metaFilePath = metaFileDir + "/objects";
    _pUserObjectCache->setCacheFilePath(metaFilePath);

    _pDataModel->setBasePath(basePath);
}


void
ServerContainer::updateCache()
{
    LOG(upnpav, debug, "server container, update cache started ...");
    if (_pObjectCache) {
        // removed objects
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginRemovedIndex(); it != _pDataModel->endRemovedIndex(); ++it) {
            LOG(upnpav, debug, "remove object from cache with index: " + Poco::NumberFormatter::format(*it));
            _pObjectCache->removeMediaObjectForIndex(*it);
        }
        // added objects
        for (AbstractDataModel::IndexIterator it = _pDataModel->beginAddedIndex(); it != _pDataModel->endAddedIndex(); ++it) {
            LOG(upnpav, debug, "add object to cache with index: " + Poco::NumberFormatter::format(*it));
            ServerObject* pObject = _pDataModel->getMediaObject(_pDataModel->getPath(*it));
            if (pObject) {
                pObject->setIndex(*it);
                _pObjectCache->insertMediaObject(pObject);
            }
            else {
                LOG(upnpav, error, "could not retrieve media object from data model");
            }
        }
        // modified objects
        std::map<ui4, ui4> cacheUpdateIds;
        if (_pDataModel->endCommonIndex() - _pDataModel->beginCommonIndex() > 1) {
            _pObjectCache->getUpdateIds(cacheUpdateIds);
        }

        for (AbstractDataModel::IndexIterator it = _pDataModel->beginCommonIndex(); it != _pDataModel->endCommonIndex(); ++it) {
            ui4 updateId = _pDataModel->getUpdateId(_pDataModel->getPath(*it));
            ui4 cacheUpdateId = cacheUpdateIds[*it];
            if (updateId != cacheUpdateId) {
                LOG(upnpav, debug, "update object in cache with index: " + Poco::NumberFormatter::format(*it));
                ServerObject* pObject = _pDataModel->getMediaObject(_pDataModel->getPath(*it));
                if (pObject) {
                    pObject->setIndex(*it);
                    _pObjectCache->updateMediaObject(pObject);
                }
            }
        }
        // TODO: also update user objects.
        _pObjectCache->updateVirtualObjects(_pVirtualContainerCache);
    }
    else {
        LOG(upnpav, debug, "no object cache, no update.");
    }
    LOG(upnpav, debug, "server container, update cache finished.");
}


bool
ServerContainer::cacheConsistent()
{
    LOG(upnpav, debug, "server container, check cache for consistency with data model ...");
    if (_pObjectCache) {
        std::vector<ui4> cacheIndices;
        _pObjectCache->getIndices(cacheIndices);
        if (cacheIndices.size() != getDataModel()->getIndexCount()) {
            LOG(upnpav, error, "cache size mismatch, object cache: " + Poco::NumberFormatter::format(cacheIndices.size()) + ", data model: " + Poco::NumberFormatter::format(getDataModel()->getIndexCount()));
            return false;
        }

        for (std::vector<ui4>::const_iterator it = cacheIndices.begin(); it != cacheIndices.end(); ++it) {
            if (!getDataModel()->hasIndex(*it)) {
                LOG(upnpav, error, "data model missing index: " + Poco::NumberFormatter::format(*it));
                return false;
            }
        }
    }
    LOG(upnpav, debug, "server container, cache and data model are consistent.");
    return true;
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
    LOG(upnpav, debug, "server container get descendant with object id relative to this container's id (in most cases \"0\"): " + objectId);

    std::string::size_type slashPos = objectId.find('/');
    if (slashPos != std::string::npos) {
        // child is a container
        ServerContainer* pChild = static_cast<ServerContainer*>(getChildForIndexString(objectId.substr(0, slashPos)));
        if (pChild == 0) {
            // child container is not a child of this container, we try the full path
            pChild = static_cast<ServerContainer*>(getChildForIndexString(objectId));
            if (pChild == 0) {
                // child container is not a child of this container
                LOG(upnpav, error, "retrieving child objectId of container, but no child container found");
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
        LOG(upnpav, debug, "server container get descendant is a child with index: " + objectId);
        if (objectId == "0") {
            return this;
        }
//        ServerItem* pChild = static_cast<ServerItem*>(getChildForIndexString(objectId));
        ServerObject* pChild = getChildForIndexString(objectId);
        if (pChild == 0) {
            // child item is not a child of this container
            LOG(upnpav, error, "no child item found");
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
        IndexNamespace indexNamespace = Data;
        switch (indexString.at(0)) {
            case 'v':
                indexNamespace = Virtual;
                break;
            case 'u':
                indexNamespace = User;
                break;
        }

        try {
            index = Poco::NumberParser::parseUnsigned(indexNamespace == Data ? indexString : indexString.substr(1));
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, error, "server container could no parse index string");
        }
        return getChildForIndex(index, true, indexNamespace);
    }
}


ServerObject*
ServerContainer::getChildForIndex(ui4 index, bool init, IndexNamespace indexNamespace)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    if (!_pDataModel) {
        // currently, only server containers with data models are handled
        LOG(upnpav, error, "server container without data model, can not retrieve child objects");
        return 0;
    }
    LOG(upnpav, debug, "server container _pVirtualContainerCache: " + Poco::NumberFormatter::format(_pVirtualContainerCache));
    LOG(upnpav, debug, "server container _pUserObjectCache: " + Poco::NumberFormatter::format(_pUserObjectCache));

    ServerObject* pObject = 0;
    if (indexNamespace == Virtual && _pVirtualContainerCache) {
        pObject = _pVirtualContainerCache->getMediaObjectForIndex(index);
//        pObject->_indexNamespace = Virtual;
    }
    else if (indexNamespace == User && _pUserObjectCache) {
        pObject = _pUserObjectCache->getMediaObjectForIndex(index);
//        pObject->_indexNamespace = User;
    }
//    else if (_pObjectCache && !updateCacheThreadIsRunning() && !cacheNeedsUpdate()) {
//    else if (_pObjectCache && !updateCacheThreadIsRunning()) {
    else if (_pObjectCache) {
        // get media object out of data base cache (column xml)
        pObject = _pObjectCache->getMediaObjectForIndex(index);
//        pObject->_indexNamespace = Virtual;
    }
    else {
        std::string path = _pDataModel->getPath(index);
        LOG(upnpav, debug, "server container, get child from data model with index: " + Poco::NumberFormatter::format(index) + ", path: " + path);
        pObject = _pDataModel->getMediaObject(path);
    }
    if (pObject) {
        pObject->_indexNamespace = indexNamespace;
        pObject = initChild(pObject, index, init);
    }
    return pObject;
}


ui4
ServerContainer::getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

     if (!_pDataModel) {
        // currently, only server containers with data models are handled
        LOG(upnpav, error, "server container without data model, can not retrieve child objects");
        return 0;
    }

    ui4 childCount = 0;
    if (_pUserObjectCache && getId() == "0" && (search == "" || search == "*")) {
        childCount += _pUserObjectCache->rowCount();
        // TODO: better criteria for blending in user objects at beginning of list (depending on number of user objects)
        if (offset == 0) {
            // FIXME: don't get all user objects but count user objects at maximum
            _pUserObjectCache->getBlockAtRow(children, this, 0, 0);
            count -= childCount;
        }
        else {
            offset -= childCount;
        }
    }

//    bool updateCache = cacheNeedsUpdate();
    if (_index == AbstractDataModel::INVALID_INDEX && _layout == PropertyGroups && _pVirtualContainerCache) {
        // parent container is root and we want to browse virtual child containers
        childCount += _pVirtualContainerCache->getBlockAtRow(children, this, offset, count, sort, search);
    }
//    else if (_pObjectCache && !updateCache) {
    else if (_pObjectCache) {
        childCount += _pObjectCache->getBlockAtRow(children, this, offset, count, sort, search);
    }
    else {
        childCount += _pDataModel->getBlockAtRow(children, this, offset, count, sort, search);
    }
    for (std::vector<ServerObject*>::iterator it = children.begin(); it != children.end(); ++it) {
        *it = initChild(*it, (*it)->getIndex());
    }
    return childCount;
}


std::stringstream*
ServerContainer::generateChildrenPlaylist()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serverLock);

    LOG(upnpav, debug, "generate children playlist, container objectID: " + getId() + ", indexNamespace: " + Poco::NumberFormatter::format(_indexNamespace));

//    switch (_indexNamespace) {
//        case Data:
//            if (_pObjectCache) {
//                int i = 0;
//                int maxLogEntries = 5;
//                std::string playlistLog;
//                std::vector<ui4> indices;
//                _pObjectCache->getIndices(indices);
//                for (std::vector<ui4>::iterator it = indices.begin(); it != indices.end(); ++it, ++i) {
//                    std::string playlistEntry = _pServer->getServerAddress() + "/" + Poco::NumberFormatter::format(*it) + "$0" + Poco::LineEnding::NEWLINE_LF;
//                    _childrenPlaylist << playlistEntry;
//                    _childrenPlaylistSize += playlistEntry.size();
//
//                    if (i == maxLogEntries) {
//                        playlistLog += "..." + Poco::LineEnding::NEWLINE_DEFAULT;
//                    }
//                    if (indices.size() < maxLogEntries || i < maxLogEntries) {
//                        playlistLog += playlistEntry;
//                    }
//                    else if (i >= indices.size() - maxLogEntries) {
//                        playlistLog += playlistEntry;
//                    }
//                }
//                LOG(upnpav, debug, "children playlist: " + Poco::LineEnding::NEWLINE_DEFAULT + playlistLog);
//            }
//            else if (_pDataModel) {
//            // solely AbstractDataModel based retrieval of indices:
//                int i = 0;
//                int maxLogEntries = 5;
//                std::string playlistLog;
//                for (AbstractDataModel::IndexIterator it = _pDataModel->beginIndex(); it != _pDataModel->endIndex(); ++it, ++i) {
//                    std::string playlistEntry = _pServer->getServerAddress() + "/" + Poco::NumberFormatter::format((*it).first) + "$0" + Poco::LineEnding::NEWLINE_LF;
//                    _childrenPlaylist << playlistEntry;
//                    _childrenPlaylistSize += playlistEntry.size();
//
//                    if (i == maxLogEntries) {
//                        playlistLog += "..." + Poco::LineEnding::NEWLINE_DEFAULT;
//                    }
//                    if (_pDataModel->getIndexCount() < maxLogEntries || i < maxLogEntries) {
//                        playlistLog += playlistEntry;
//                    }
//                    else if (i >= _pDataModel->getIndexCount() - maxLogEntries) {
//                        playlistLog += playlistEntry;
//                    }
//                }
//                LOG(upnpav, debug, "children playlist: " + Poco::LineEnding::NEWLINE_DEFAULT + playlistLog);
//            }
//            else {
//                // currently, only server containers with object caches are handled
//                LOG(upnpav, error, "server container without object cache or data model, can not generate playlist");
//                return 0;
//            }
//            break;
//
//        case Virtual:
//            break;

//        case User:
                int i = 0;
                int maxLogEntries = 5;
                std::string playlistLog;
                std::stringstream* pChildrenPlaylist = new std::stringstream;
                for (std::vector<ui4>::iterator it = _childrenPlaylistIndices.begin(); it != _childrenPlaylistIndices.end(); ++it, ++i) {
                    std::string playlistEntry = _pServer->getServerAddress() + "/" + Poco::NumberFormatter::format(*it) + "$0" + Poco::LineEnding::NEWLINE_LF;
                    *pChildrenPlaylist << playlistEntry;
                    _childrenPlaylistSize += playlistEntry.size();

                    if (i == maxLogEntries) {
                        playlistLog += "..." + Poco::LineEnding::NEWLINE_DEFAULT;
                    }
                    if (_childrenPlaylistIndices.size() < maxLogEntries || i < maxLogEntries) {
                        playlistLog += playlistEntry;
                    }
                    else if (i >= _childrenPlaylistIndices.size() - maxLogEntries) {
                        playlistLog += playlistEntry;
                    }
                }
                LOG(upnpav, debug, "children playlist: " + Poco::LineEnding::NEWLINE_DEFAULT + playlistLog);
//            break;
//    }

    return pChildrenPlaylist;
}


ServerObject*
ServerContainer::initChild(ServerObject* pObject, ui4 index, bool fullInit)
{
    LOG(upnpav, debug, "server container, init child with title: " + pObject->getTitle() + ", index: " + Poco::NumberFormatter::format(index) + ", namespace: " + Poco::NumberFormatter::format(pObject->_indexNamespace));

    // set index
    pObject->setIndex(index);

    // set parent index
    if (pObject->_indexNamespace == Virtual || pObject->_indexNamespace == User) {
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
        return pObject;
    }

    std::string privateResourceUri;
    if (AvClass::matchClass(pObject->getClass(), AvClass::ITEM, AvClass::PLAYLIST_ITEM)) {
        LOG(upnpav, debug, "server container, init child convert playlist item to playlist container");
        // create playlist container with list of indices that match the paths of m3u file (read in via getStream())
        ServerContainer* pContainer = createMediaContainer();
        pContainer->getProperty(AvProperty::CLASS)->setValue(AvClass::className(AvClass::CONTAINER, AvClass::PLAYLIST_CONTAINER));
        pContainer->setTitle(pObject->getTitle());
        pContainer->setIndex(pObject->getIndex());
        pContainer->_indexNamespace = pObject->_indexNamespace;
        ServerObjectResource* pResource = static_cast<ServerObjectResource*>(pObject->getResource());
        if (pResource) {
            privateResourceUri = pResource->getUri();
            std::string uri = pResource->getUri();
            if (uri == "") {
                std::istream* pStream = pResource->getStream();
                if (pStream) {
                    std::string parentPath = _pDataModel->getPath(pObject->getIndex());
                    if (parentPath != "") {
                        parentPath = parentPath.substr(0, parentPath.size() - pObject->getTitle().size());
                    }
                    std::string line;
                    while (std::getline(*pStream, line)) {
                        if (line[0] != '#') {
                            std::string path = parentPath == "" ? line : parentPath + line;
                            ui4 index = _pDataModel->getIndex(path);
                            pContainer->_childrenPlaylistIndices.push_back(index);
                            LOG(upnpav, debug, "index: " + Poco::NumberFormatter::format(index) + ", path: " + line);
                        }
                    }
                }
            }
            else {
                std::string indexFileName = getDataModel()->getMetaDirPath() + _pDataModel->getModelClass() + "/" + _pDataModel->getBasePath() + "/" + uri;
                LOG(upnpav, debug, "server container, init child get playlist indices from file: " + indexFileName);
                std::ifstream indexFile(indexFileName.c_str());
                std::string line;
                while (std::getline(indexFile, line)) {
                    ui4 index = Poco::NumberParser::parse(line);
                    pContainer->_childrenPlaylistIndices.push_back(index);
                    LOG(upnpav, debug, "index: " + Poco::NumberFormatter::format(index));
                }
            }
        }

        delete pObject;
        pObject = pContainer;
    }

    // add resources
    std::string objectId = pObject->getId();
    std::string resourceUri = _pServer->getServerAddress() + "/" + objectId;
//    if (pObject->isContainer()) {
    if (AvClass::matchClass(pObject->getClass(), AvClass::CONTAINER, AvClass::PLAYLIST_CONTAINER)) {
        LOG(upnpav, debug, "server container, init child create container playlist resource");
        // add playlist resource
        ServerObjectResource* pResource = pObject->createResource();
        pResource->setPrivateResourceUri(privateResourceUri);
        pResource->setUri(resourceUri + "$0");
        pResource->setProtInfo("http-get:*:audio/m3u:*");
        // resource is writable, set import URI to resource URI
        pResource->setAttribute(AvProperty::IMPORT_URI, pResource->getUri());
        pObject->addResource(pResource);
    }
    else if (!pObject->isContainer()) {
        LOG(upnpav, debug, "server container, init child create item resources");
        for (int r = 0; r < pObject->getResourceCount(); r++) {
            AbstractResource* pResource = pObject->getResource(r);
            if (pResource) {
                pResource->setUri(resourceUri + "$" + Poco::NumberFormatter::format(r));
            }
        }
    }

    return pObject;


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
    LOG(upnpav, debug, "ServerObjectWriter::writeChildren()");
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


DatabaseCache::DatabaseCache(const std::string& cacheTableName, ServerObject::IndexNamespace indexNamespace) :
_pSession(0),
_cacheTableName(cacheTableName),
_maxQueryPropertyCount(5),
_indexNamespace(indexNamespace)
{
    LOG(upnpav, debug, "database cache ctor");
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
    LOG(upnpav, debug, "database cache set cache file path: " + cacheFilePath);
    _cacheFilePath = cacheFilePath;
    _pSession = new Poco::Data::Session("SQLite", cacheFilePath);

    // FIXME: UNSIGNED INT(4) for index and parent index doesn't work with SQLite.
    std::string createTableString = "CREATE TABLE " + _cacheTableName + " (" + "idx INTEGER(8), paridx INTEGER(8), updid INTEGER(8)";
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        createTableString += ", " + getColumnName(*it) + " " + getColumnType(*it);
    }
    createTableString += ", xml VARCHAR)";

    try {
        *_pSession << createTableString, Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "database cache creating object cache table failed: " + e.displayText());
    }
    createDatabaseIndices();
}


ui4
DatabaseCache::rowCount()
{
    ui4 totalCount = 0;
    try {
        *_pSession << "SELECT COUNT(idx) FROM " + _cacheTableName, Poco::Data::into(totalCount), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "database cache get row count failed: " + e.displayText());
    }
    return totalCount;
}


ServerObject*
DatabaseCache::getMediaObjectForIndex(ui4 index, bool isVirtual)
{
    LOG(upnpav, debug, "database cache table " + _cacheTableName + " get object for index: " + Poco::NumberFormatter::format(index));

    std::vector<std::string> objectClass;
    std::vector<std::string> xml;
    try {
        *_pSession << "SELECT class, xml FROM " + _cacheTableName + " WHERE idx = :index", Poco::Data::use(index), Poco::Data::into(objectClass), Poco::Data::into(xml), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "database cache get object for index failed: " + e.displayText());
    }
    LOG(upnpav, debug, "database cache get xml for object with index: " + Poco::NumberFormatter::format(index));
    if (xml.size() == 1) {
        LOG(upnpav, debug, "database cache got xml: " + xml[0]);
        ServerObject* pObject;
        if (AvClass::matchClass(objectClass[0], AvClass::CONTAINER)) {
            pObject = _pServerContainer->createMediaContainer();
        }
        else {
            pObject = _pServerContainer->createMediaItem();
        }
        MediaObjectReader xmlReader;
        xmlReader.read(pObject, MediaObjectWriter::getXmlProlog() + xml[0]);
        return pObject;
    }
    else {
        LOG(upnpav, error, "database cache get object for index reading meta data failed.");
        return 0;
    }
}


ui4
DatabaseCache::getBlockAtRow(std::vector<ServerObject*>& block, ServerContainer* pParentContainer, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    LOG(upnpav, debug, "database cache table " + _cacheTableName + " get block at offset: " + Poco::NumberFormatter::format(offset) + ", count: " + Poco::NumberFormatter::format(count) + ", sort: " + sort + ", search: " + search);

    ui4 parentIndex = AbstractDataModel::INVALID_INDEX;
    ServerObject::IndexNamespace parentIndexNamespace = ServerObject::Data;
    if (pParentContainer) {
        parentIndex = pParentContainer->_index;
        parentIndexNamespace = pParentContainer->_indexNamespace;
    }

    LOG(upnpav, debug, "database cache parent index: " + Poco::NumberFormatter::format(parentIndex) + ", parent class: " + pParentContainer->getClass());

    // basic query statement
    Poco::Data::Statement select(*_pSession);
    std::string statement = "SELECT idx, class, xml FROM " + _cacheTableName;
    std::string whereClause = "";
    bool useParentIndex = false;
    bool virtualChildObjects = false;

    // present playlist item as playlist container
    if (AvClass::matchClass(pParentContainer->getClass(), AvClass::CONTAINER, AvClass::PLAYLIST_CONTAINER)) {
        LOG(upnpav, debug, "database cache parent children playlist size is: " + Poco::NumberFormatter::format(pParentContainer->_childrenPlaylistIndices.size()));
        for (ui4 r = offset; r < offset + count && r < pParentContainer->_childrenPlaylistIndices.size(); r++) {
            std::vector<std::string> xml;
            ui4 index = pParentContainer->_childrenPlaylistIndices[r];
            try {
                *_pSession << "SELECT xml FROM " + _cacheTableName + " WHERE idx = " + Poco::NumberFormatter::format(index),
                        Poco::Data::into(xml), Poco::Data::now;
            }
            catch (Poco::Exception& e) {
                LOG(upnpav, error, "database cache get object for index failed: " + e.displayText());
            }
            LOG(upnpav, debug, "database cache get xml for object with index: " + Poco::NumberFormatter::format(index));
            if (xml.size() == 1) {
                ServerObject* pObject = _pServerContainer->createMediaItem();
                MediaObjectReader xmlReader;
                xmlReader.read(pObject, MediaObjectWriter::getXmlProlog() + xml[0]);
                pObject->setIndex(index);
                block.push_back(pObject);
            }
            else {
                LOG(upnpav, error, "database cache get object for index reading meta data failed.");
                break;
            }
        }
        return pParentContainer->_childrenPlaylistIndices.size();
    }

    // handle search clause
    if (search != "*") {
        DatabaseCacheSearchCriteria searchCrit(this);
        try {
            whereClause += searchCrit.parse(search);
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, error, "database cache search error parsing search criteria: " + e.displayText());
        }
    }

    // handle layout of media container
    if (_pServerContainer->getLayout() == ServerContainer::Flat) {
        LOG(upnpav, debug, "database cache server container layout: Flat");
        whereClause += std::string(whereClause == "" ? "" : " AND") + " class <> \"" + AvClass::className(AvClass::CONTAINER) + "\"";
    }
    else if (_pServerContainer->getLayout() == ServerContainer::DirStruct && search == "*") {
        LOG(upnpav, debug, "database cache server container layout: DirStruct");
        useParentIndex = true;
        whereClause += std::string(whereClause == "" ? "" : " AND") + " paridx = :paridx";
    }
    else if (_pServerContainer->getLayout() == ServerContainer::PropertyGroups) {
        LOG(upnpav, debug, "database cache server container layout: PropertyGroups");
        if (parentIndex == AbstractDataModel::INVALID_INDEX) {
            LOG(upnpav, debug, "database cache parent container is root container");
            virtualChildObjects = true;
//            statement = "SELECT idx, class, xml FROM " + _cacheTableName + " WHERE prop0 = \"upnp:class\"";
            statement = "SELECT idx, class, xml FROM " + _cacheTableName + " WHERE prop0 = \"" + pParentContainer->_groupPropertyName + "\"";
        }
//        else if (pParentContainer->isVirtual()) {
        else {
            LOG(upnpav, debug, "database cache parent container is virtual");
            AbstractProperty* pGroupPropertyName = pParentContainer->getProperty(ServerContainer::PROPERTY_GROUP_PROPERTY_NAME);
            AbstractProperty* pGroupPropertyValue = pParentContainer->getProperty(ServerContainer::PROPERTY_GROUP_PROPERTY_VALUE);
            if (pGroupPropertyName && pGroupPropertyValue) {
                LOG(upnpav, debug, "database cache parent container group property name: " + pGroupPropertyName->getValue());
                whereClause += std::string(whereClause == "" ? "" : " AND") + " " + getColumnName(pGroupPropertyName->getValue()) + " = \"" + pGroupPropertyValue->getValue() + "\"";
            }
        }
    }
    if (whereClause != "") {
        statement += " WHERE " + whereClause;
    }

    // handle sort clause
    if (sort != "") {
        statement += " ORDER BY ";
        CsvList sortList(sort);
        for (CsvList::Iterator it = sortList.begin(); it != sortList.end(); ++it) {
            bool asc = ((*it)[0] == '+');
            statement += getColumnName((*it).substr(1)) + (asc ? " ASC" : " DESC");
            if (it != --sortList.end()) {
                statement.append(", ");
            }
        }
    }

    // get total count
    ui4 totalCount = 0;
    if (_totalCountQueryCache.find(whereClause) == _totalCountQueryCache.end()) {
        LOG(upnpav, debug, "database cache execute total count query");
        *_pSession << "SELECT COUNT(idx) FROM " + _cacheTableName + " WHERE " + whereClause, Poco::Data::into(totalCount), Poco::Data::now;
        LOG(upnpav, debug, "database cache total count query executed, number of objects: " + Poco::NumberFormatter::format(totalCount));
        _totalCountQueryCache[whereClause] = totalCount;
    }
    else {
        totalCount = _totalCountQueryCache[whereClause];
    }

    // UPnP AV CDS specs: if count == 0 then request all children, otherwise retrieve count media objects
    if (count) {
        statement += " LIMIT " + Poco::NumberFormatter::format(count);
    }
    else {
        statement += " LIMIT " + Poco::NumberFormatter::format(totalCount - offset);
    }
    // jump to given offset
    statement += " OFFSET " + Poco::NumberFormatter::format(offset);

    // execute query and return result
    LOG(upnpav, debug, "database cache parent index: " + Poco::NumberFormatter::format(parentIndex));
    LOG(upnpav, debug, "database cache execute query: " + statement);
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
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "database cache get block executing query and moving to offset failed: " + e.displayText());
    }
    LOG(upnpav, debug, "database cache query executed, creating " + Poco::NumberFormatter::format(recordSet.rowCount()) + " media objects");
    for (ui4 r = 0; r < recordSet.rowCount(); r++) {
        // get block
        try {
            ui4 index;
            std::string objectClass;
            std::string xml;
            index = recordSet["idx"].convert<ui4>();
            objectClass = recordSet["class"].convert<std::string>();
            xml = MediaObjectWriter::getXmlProlog() + recordSet["xml"].convert<std::string>();
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
                pObject->_indexNamespace = ServerObject::Virtual;
            }
            if (_indexNamespace == ServerObject::User) {
                pObject->_indexNamespace = ServerObject::User;
            }
            block.push_back(pObject);
            recordSet.moveNext();
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, warning, "database cache get block data for row " + Poco::NumberFormatter::format(r) + " failed: " + e.displayText());
        }
    }
    LOG(upnpav, debug, "database cache block retrieved");
    return totalCount;
}


void
DatabaseCache::getIndices(std::vector<ui4>& indices, const std::string& sort)
{
    // TODO: execute same query as in getBlockAtRow(), depending on _layout and sort.
    try {
//        *_pSession << "SELECT idx FROM " + _cacheTableName + " WHERE class <> \"" + AvClass::className(AvClass::CONTAINER) + "\"", Poco::Data::into(indices), Poco::Data::now;
        *_pSession << "SELECT idx FROM " + _cacheTableName, Poco::Data::into(indices), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "database cache get indices failed: " + e.displayText());
    }
}


void
DatabaseCache::getUpdateIds(std::map<ui4, ui4>& updateIds)
{
    std::vector< Poco::Tuple<ui4, ui4> > ids;
    try {
        *_pSession << "SELECT idx, updid FROM " + _cacheTableName, Poco::Data::into(ids), Poco::Data::now;
        for (std::vector< Poco::Tuple<ui4, ui4> >::const_iterator it = ids.begin(); it != ids.end(); ++it) {
            updateIds[it->get<0>()] = it->get<1>();
        }
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "database cache get update ids failed: " + e.displayText());
    }
}


void
DatabaseCache::insertMediaObject(ServerObject* pObject)
{
    LOG(upnpav, debug, "database cache insert media object with index: " + Poco::NumberFormatter::format(pObject->getIndex()));
    std::string xml;
    MediaObjectWriter xmlWriter(false);
    xmlWriter.write(xml, pObject);
    // chop off xml prolog:
    xml = xml.substr(MediaObjectWriter::getXmlProlog().size());

    std::string insertString = "INSERT INTO " + _cacheTableName + " (";
    std::string propColString = "idx, paridx, updid";
    std::string propValString = ":idx, :paridx, :updid";
    std::vector<std::string> props;
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        propColString += ", " + getColumnName(*it);
        propValString += ", :" + getColumnName(*it);
        AbstractProperty* pProperty = pObject->getProperty(*it);
        props.push_back(pProperty ? pProperty->getValue() : "");
    }
    insertString += propColString + ", xml) VALUES(" + propValString + ", :xml)";

    ui4 index = pObject->getIndex();
    ui4 parIndex = pObject->getParentIndex();
    AbstractDataModel* pDataModel = _pServerContainer->getDataModel();
    ui4 mod = pDataModel->getUpdateId(pDataModel->getPath(index));

    // this is somehow ugly and limits the number of properties, that can be queried.
    try {
        switch (props.size()) {
            case 2:
                *_pSession << insertString, Poco::Data::use(index), Poco::Data::use(parIndex), Poco::Data::use(mod),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(xml), Poco::Data::now;
                break;
            case 3:
                *_pSession << insertString, Poco::Data::use(index), Poco::Data::use(parIndex), Poco::Data::use(mod),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(xml), Poco::Data::now;
                break;
            case 4:
                *_pSession << insertString, Poco::Data::use(index), Poco::Data::use(parIndex), Poco::Data::use(mod),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(props[3]),
                        Poco::Data::use(xml), Poco::Data::now;
                break;
            case 5:
                *_pSession << insertString, Poco::Data::use(index), Poco::Data::use(parIndex), Poco::Data::use(mod),
                        Poco::Data::use(props[0]), Poco::Data::use(props[1]), Poco::Data::use(props[2]), Poco::Data::use(props[3]),
                        Poco::Data::use(props[4]), Poco::Data::use(xml), Poco::Data::now;
                break;
        }
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "database cache insert media object failed: " + e.displayText());
    }
}


void
DatabaseCache::updateMediaObject(ServerObject* pObject)
{
    // TODO: implement update media object with sql update.
    removeMediaObjectForIndex(pObject->getIndex());
    insertMediaObject(pObject);
}


void
DatabaseCache::removeMediaObjectForIndex(ui4 index)
{
    LOG(upnpav, debug, "database cache remove media object with index: " + Poco::NumberFormatter::format(index));
    try {
        *_pSession << "DELETE FROM " + _cacheTableName + " WHERE idx = " + Poco::NumberFormatter::format(index), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "database cache remove media object failed: " + e.displayText());
    }
}


void
DatabaseCache::addPropertiesForQuery(CsvList propertyList)
{
    std::size_t defaultPropCount = _propertyColumnNames.size();
    if (propertyList.getSize() > _maxQueryPropertyCount) {
        LOG(upnpav, warning, "database cache can only handle up to " + Poco::NumberFormatter::format(_maxQueryPropertyCount) +\
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
    clearQueryCache();

    // FIXME: virtual object update does not sync but rewrites everything
    return;

    ui4 index = 0;
    for (CsvList::Iterator it = _queryPropertyNames.begin(); it != _queryPropertyNames.end(); ++it) {
        if (*it == AvProperty::TITLE) {
            continue;
        }
        std::string columnName = getColumnName(*it);
        std::string statement = "SELECT " + columnName + " FROM " + _cacheTableName + " GROUP BY " + columnName + " ORDER BY " + columnName;
        LOG(upnpav, debug, "database cache execute query: " + statement);
        Poco::Data::Statement select(*_pSession);
        select << statement;

        Poco::Data::RecordSet recordSet(select);
        try {
            select.execute();
        }
        catch (Poco::Exception& e) {
            LOG(upnpav, warning, "database cache update virtual objects executing query failed: " + e.displayText());
        }
        bool more = recordSet.moveFirst();
        while (more) {
            std::string propVal = recordSet[columnName].convert<std::string>();
            ServerContainer* pContainer = _pServerContainer->createMediaContainer();
            pContainer->setTitle(propVal);
            pContainer->setIndex(index);
            pContainer->_indexNamespace = ServerObject::Virtual;
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


void
DatabaseCache::createDatabaseIndices()
{
    std::string indexedProps = "";
    for (int i = 0; i < _propertyColumnNames.size() - 2; ++i) {
        indexedProps += "prop" + Poco::NumberFormatter::format(i) + ",";
    }
    indexedProps += "title";
    try {
        *_pSession << "CREATE UNIQUE INDEX idx ON " + _cacheTableName + "(idx)", Poco::Data::now;
        *_pSession << "CREATE INDEX prop_idx ON " + _cacheTableName + "(" + indexedProps + ")", Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "database cache creating index on object cache table failed: " + e.displayText());
    }
}


void
DatabaseCache::clearQueryCache()
{
    _totalCountQueryCache.clear();
}


DatabaseCacheSearchCriteria::DatabaseCacheSearchCriteria(DatabaseCache* pDatabaseCache) :
_pDatabaseCache(pDatabaseCache)
{

}


std::string
DatabaseCacheSearchCriteria::translateProperty(const std::string& property)
{
    return _pDatabaseCache->getColumnName(property);
}


std::string
DatabaseCacheSearchCriteria::translateCompareExp(const std::string& property, const std::string& op, const std::string& val)
{
    if (property == Omm::Av::AvProperty::RES) {
        LOG(upnpav, debug, "database cache get object for resource: " + val);
        Poco::URI resUri(val);
        std::string resPath = resUri.getPath();
        LOG(upnpav, debug, "path: " + resPath);
        Poco::StringTokenizer uri(resPath, "$");
        std::string index = uri[0].substr(1);
        LOG(upnpav, debug, "index: " + index);
        const std::string indexCol("idx"); // funny, using "idx" in the return statement gives a random string (compiler bug?)
        return indexCol + space + op + space + index;
    }
    else {
        return SqlSearchCriteria::translateCompareExp(property, op, val);
    }
}


const ui4 AbstractDataModel::INVALID_INDEX = 0xffffffff;

AbstractDataModel::AbstractDataModel() :
_updateThreadRunnable(*this, &AbstractDataModel::updateThread),
_updateThreadRunning(false),
_pServerContainer(0),
//_indexBufferSize(50),
_pSourceEncoding(0),
_pTextConverter(0),
_publicSystemUpdateId(0),
_cacheSystemUpdateId(0),
_cacheSystemModId(0),
_checkMod(false)
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


std::string
AbstractDataModel::getBasePath()
{
    return _basePath.toString();
}


std::string
AbstractDataModel::getCacheDirPath()
{
    return _cacheDirPath.toString();
}


std::string
AbstractDataModel::getMetaDirPath()
{
    return _metaDirPath.toString();
}


void
AbstractDataModel::setBasePath(const std::string& path)
{
    _basePath = path;
    init();

    // TODO: avoid long and hidden paths in meta directory
//    Poco::Path path(basePath);
//    if (path.parent().toString() == Poco::Path(Util::Home::instance()->getConfigDirPath()).toString()) {
//        // if basePath points to a file in omm's config directory, don't repeat the config path
//        _basePath = path.getFileName();
//    }
//    else {
//        _basePath = basePath;
//    }

    // index map is in meta directory, because var directory should be deletable at any time
    // without loosing information. Indices must be persistant information, because object ids
    // are derived from them (playlists must be removed also, if index map is rebuild entirely)
    _indexFilePath = Poco::Path(getMetaDirPath() + getModelClass() + "/" + path, "index");
    readIndexMap();
    checkSystemUpdateId();
}


void
AbstractDataModel::setCacheDirPath(const std::string& path)
{
    _cacheDirPath = path;
}


void
AbstractDataModel::setMetaDirPath(const std::string& path)
{
    _metaDirPath = path;
}


void
AbstractDataModel::setCheckObjectModifications(bool check)
{
    _checkMod = check;
}


void
AbstractDataModel::checkSystemUpdateId()
{
//    getServerContainer()->cacheConsistent();

    if (updateThreadIsRunning()) {
        LOG(upnpav, debug, "update thread still running, don't check for data model changes.");
        return;
    }

    ui4 id = getSystemUpdateId(_checkMod);
    if (getCacheSystemUpdateId(_checkMod) == id) {
        LOG(upnpav, debug, "data model is current, nothing to do.");
    }
    else {
        LOG(upnpav, debug, "data model got new system update id: " + Poco::NumberFormatter::format(id));
        incPublicSystemUpdateId();
        setCacheSystemUpdateId(id, _checkMod);
        _updateThread.start(_updateThreadRunnable);
    }
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
        LOG(upnpav, error, "abstract data model, could not retrieve index from path: " + path);
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
        LOG(upnpav, error, "abstract data model, could not retrieve path from index: " + Poco::NumberFormatter::format(index));
        return "";
    }
}


ui4
AbstractDataModel::getIndexFromUri(const std::string& uri)
{
    Poco::URI resUri(uri);
    std::string resPath = resUri.getPath();
    return Poco::NumberParser::parse(Poco::StringTokenizer(resPath, "$")[0].substr(1));
}


AbstractDataModel::IndexMapIterator
AbstractDataModel::beginIndex()
{
    return _indexMap.begin();
}


AbstractDataModel::IndexMapIterator
AbstractDataModel::endIndex()
{
    return _indexMap.end();
}


AbstractDataModel::IndexIterator
AbstractDataModel::beginCommonIndex()
{
    return _commonIndices.begin();
}


AbstractDataModel::IndexIterator
AbstractDataModel::endCommonIndex()
{
    return _commonIndices.end();
}


AbstractDataModel::IndexIterator
AbstractDataModel::beginAddedIndex()
{
    return _addedIndices.begin();
}


AbstractDataModel::IndexIterator
AbstractDataModel::endAddedIndex()
{
    return _addedIndices.end();
}


AbstractDataModel::IndexIterator
AbstractDataModel::beginRemovedIndex()
{
    return _removedIndices.begin();
}


AbstractDataModel::IndexIterator
AbstractDataModel::endRemovedIndex()
{
    return _removedIndices.end();
}


void
AbstractDataModel::addPath(const std::string& path, const std::string& resourcePath)
{
//        LOG(upnpav, debug, "abstract data model add path: " + path + " with index: " + Poco::NumberFormatter::format(index));
    std::map<std::string, ui4>::iterator it = _pathMap.find(path);
    if (it == _pathMap.end()) {
        ui4 index = getNewIndex();
        if (index == INVALID_INDEX) {
            LOG(upnpav, error, "abstract data model max index reached, can not add path: " + path);
            return;
        }
        _pathMap[path] = index;
        _indexMap[index] = path;
        if (resourcePath != "") {
            _resourceMap.insert(std::pair<ui4, std::string>(index, resourcePath));
        }
        _addedIndices.push_back(index);
    }
    else {
        _commonIndices.push_back((*it).second);
    }
}


void
AbstractDataModel::removeIndex(ui4 index)
{
    std::map<ui4, std::string>::iterator pos = _indexMap.find(index);
    if (pos  != _indexMap.end()) {
        _indexMap.erase(pos);
        _pathMap.erase((*pos).second);
        _resourceMap.erase((*pos).first);
        _freeIndices.push(index);
    }
    else {
        LOG(upnpav, error, "abstract data model, could not erase index from index map: " + Poco::NumberFormatter::format(index));
    }
}


ui4
AbstractDataModel::getBlockAtRow(std::vector<ServerObject*>& block, ServerContainer* pParentContainer, ui4 offset, ui4 count, const std::string& sort, const std::string& search)
{
    // TODO: should be faster with a method getIndexBlock(), implemented with an additional std::vector<ui4> as a sorted index list
    // TODO: implement building sort indices and row filtering in memory without data base, currently sort and search are ignored
    LOG(upnpav, debug, "abstract data model search: " + search);
    if (search != "*") {
        Poco::StringTokenizer searchTokens(search, " ");
        if (searchTokens.count() == 3 && searchTokens[0] == "res") {
            ui4 index = getIndexFromUri(search);
            ServerObject* pObject = getMediaObject(getPath(index));
            pObject->setIndex(index);
            block.push_back(pObject);
            return 1;
        }
        else {
            return 0;
        }
    }
    else if (sort != "") {
        return 0;
    }
    if (count == 0) {
        // UPnP AV CDS specs, count == 0 then request all children
        count = getIndexCount();
    }

    // present playlist item as playlist container
    if (AvClass::matchClass(pParentContainer->getClass(), AvClass::CONTAINER, AvClass::PLAYLIST_CONTAINER)) {
        LOG(upnpav, debug, "abstract data model parent children playlist size is: " + Poco::NumberFormatter::format(pParentContainer->_childrenPlaylistIndices.size()));
        for (ui4 r = offset; r < offset + count && r < pParentContainer->_childrenPlaylistIndices.size(); r++) {
            ui4 index = pParentContainer->_childrenPlaylistIndices[r];
            ServerObject* pObject = getMediaObject(getPath(index));
            pObject->setIndex(index);
            block.push_back(pObject);
        }
        return pParentContainer->_childrenPlaylistIndices.size();
    }

    ui4 r = 0;
    for (IndexMapIterator it = beginIndex(); (it != endIndex()) && (r < offset + count); ++it) {
        if (r >= offset) {
            ServerObject* pObject = getMediaObject((*it).second);
            pObject->setIndex((*it).first);
            block.push_back(pObject);
        }
        r++;
    }
    return getIndexCount();
}


const Poco::TextEncoding*
AbstractDataModel::getTextEncoding()
{
    return _pSourceEncoding;
}


void
AbstractDataModel::setTextEncoding(const std::string& encoding)
{
    _pSourceEncoding = Poco::TextEncoding::find(encoding);
    _pTextConverter = new Poco::TextConverter(*_pSourceEncoding, _targetEncoding);
}


Poco::TextConverter*
AbstractDataModel::getTextConverter()
{
    return _pTextConverter;
}


void
AbstractDataModel::readIndexMap()
{
    if (!Poco::File(_indexFilePath).exists()) {
        LOG(upnpav, debug, "index map not present, not reading it");
        return;
    }
    LOG(upnpav, debug, "index map present, reading ...");
    std::ifstream indexMap(_indexFilePath.toString().c_str());
    std::string line;

    try {
        getline(indexMap, line);
        setPublicSystemUpdateId(Poco::NumberParser::parse(line));
        getline(indexMap, line);
        setCacheSystemUpdateId(Poco::NumberParser::parse(line), false);
        getline(indexMap, line);
        setCacheSystemUpdateId(Poco::NumberParser::parse(line), true);
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, debug, "could not parse update ids: " + e.displayText());
    }

    ui4 index = 0;
    ui4 lastIndex = 0;
    std::string path;
    while(getline(indexMap, line)) {
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
    // TODO: read resource map
    LOG(upnpav, debug, "index map read finished.");
}


void
AbstractDataModel::writeIndexMap()
{
    LOG(upnpav, debug, "index map writing to: " + _indexFilePath.toString() + " ...");
    std::ofstream indexMap(_indexFilePath.toString().c_str());
    indexMap << getPublicSystemUpdateId() << std::endl;
    indexMap << getCacheSystemUpdateId(false) << std::endl;
    indexMap << getCacheSystemUpdateId(true) << std::endl;

    for (std::map<ui4, std::string>::iterator it = _indexMap.begin(); it != _indexMap.end(); ++it) {
//        LOG(upnpav, debug, "abstract data model write index: " + Poco::NumberFormatter::format((*it).first) + ", path: " + (*it).second);
        indexMap << (*it).first << ' ' << (*it).second << std::endl;
    }
    // TODO: write resource map
    LOG(upnpav, debug, "index map write finished.");
}


ui4
AbstractDataModel::getPublicSystemUpdateId()
{
    return _publicSystemUpdateId;
}


void
AbstractDataModel::setPublicSystemUpdateId(ui4 id)
{
    _publicSystemUpdateId = id;
}


void
AbstractDataModel::incPublicSystemUpdateId()
{
    _publicSystemUpdateId++;
}


ui4
AbstractDataModel::getCacheSystemUpdateId(bool mod)
{
    if (mod) {
        return _cacheSystemModId;
    }
    else {
        return _cacheSystemUpdateId;
    }
}


void
AbstractDataModel::setCacheSystemUpdateId(ui4 id, bool mod)
{
    if (mod) {
        _cacheSystemModId = id;
    }
    else {
        _cacheSystemUpdateId = id;
    }
}


ui4
AbstractDataModel::getNewIndex()
{
    ui4 index;
//    if (!_freeIndices.empty() && !_pServerContainer->updateCacheThreadIsRunning()) {
    if (!_freeIndices.empty()) {
        index = _freeIndices.top();
        _freeIndices.pop();
    }
    else if (_indexMap.size() == 0) {
        index = 1;
    }
    else {
        index = _indexMap.rbegin()->first + 1;
    }
    return index;
}


void
AbstractDataModel::updateThread()
{
    _updateThreadLock.lock();
    _updateThreadRunning = true;
    _updateThreadLock.unlock();

    LOG(upnpav, debug, "update thread start ...");
    // clear temporary index lists
    _lastIndices.clear();
    _commonIndices.clear();
    _addedIndices.clear();
    _removedIndices.clear();

    // index diff calculation looks like an overkill, but indices have to be preserved
    // as they are meta data and identify a media object within a CDS over time.

    // save last index list
//    LOG(upnpav, debug, "save last index list ... ");
    for (IndexMapIterator it = beginIndex(); it != endIndex(); ++it) {
        _lastIndices.push_back((*it).first);
    }

    // when scanning the data model, addPath() is called.
    scan();

    // calculate indices to be removed
//    LOG(upnpav, debug, "calculate indices to be removed ...");
    for (IndexIterator it = _lastIndices.begin(); it != _lastIndices.end(); ++it) {
        IndexIterator pos = std::find(_commonIndices.begin(), _commonIndices.end(), *it);
        if (pos == _commonIndices.end()) {
            _removedIndices.push_back(*it);
        }
    }
    LOG(upnpav, debug, "last indices: " + Poco::NumberFormatter::format(_lastIndices.size()) +\
                                            ", common indices: " + Poco::NumberFormatter::format(_commonIndices.size()) +\
                                            ", added indices: " + Poco::NumberFormatter::format(_addedIndices.size()) +\
                                            ", removed indices: " + Poco::NumberFormatter::format(_removedIndices.size()));
    // remove index maps
//    LOG(upnpav, debug, "remove indices from maps ...");
    for (IndexIterator it = _removedIndices.begin(); it != _removedIndices.end(); ++it) {
        removeIndex(*it);
    }
//    LOG(upnpav, debug, "remove indices from maps finished, index map updated.");

    // save updated index map
    writeIndexMap();

    // update database cache (if available)
    _pServerContainer->updateCache();

    // trigger evented state variable SystemUpdateID
    _pServerContainer->getServer()->setSystemUpdateId(getPublicSystemUpdateId());

    LOG(upnpav, debug, "update thread finished.");

    _updateThreadLock.lock();
    _updateThreadRunning = false;
    _updateThreadLock.unlock();
}


bool
AbstractDataModel::updateThreadIsRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_updateThreadLock);
    LOG(upnpav, debug, "update thread is running: " + std::string(_updateThreadRunning ? "yes" : "no"));
    return _updateThreadRunning;
}


ServerObject*
SimpleDataModel::getMediaObject(const std::string& path)
{
//    LOG(upnpav, debug, "simple data model get media object for index: " + Poco::NumberFormatter::format(index) + " ...");
    LOG(upnpav, debug, "simple data model get media object for path: " + path + " ...");

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
