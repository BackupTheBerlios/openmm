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

#ifndef OMMUPNPAV_SERVER_H
#define OMMUPNPAV_SERVER_H

#include <stack>

#include <Poco/Path.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

// #include "AvStream.h"
#include "Upnp.h"
#include "UpnpAvObject.h"


namespace Omm {
namespace Av {

class MediaItemServer;
class ServerObject;
class ServerItem;
class DevContentDirectoryServerImpl;
class ServerContainer;
class AbstractDataModel;


class MediaServer : public Device
/// Used by ServerApplication
/// Provides a meta data server that can be browsed by CDS implementation through root media object,
/// and a http server for streaming server item resources
{
    friend class ItemRequestHandler;
    friend class ServerObject;
    friend class ServerContainer;

public:
    MediaServer(int port = 0);
    virtual ~MediaServer();

    void setRoot(ServerContainer* pRoot);

    void start();
    void stop();

//    std::string getProtocol();

    Poco::UInt16 getServerPort() const;
    std::string getServerAddress();
    std::string getServerProtocol();

private:
    DevContentDirectoryServerImpl*              _pDevContentDirectoryServerImpl;

    ServerContainer*                            _pServerContainer;
    Poco::Net::ServerSocket                     _socket;
    Poco::Net::HTTPServer*                      _pHttpServer;
};


class ItemRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    ItemRequestHandler(MediaServer* pItemServer);

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    std::streamsize copyStream(std::istream& istr, std::ostream& ostr, std::streamoff start = 0, std::streamoff end = -1);
    void parseRange(const std::string& rangeValue, std::streamoff& start, std::streamoff& end);

    unsigned int        _bufferSize;
    MediaServer*        _pServer;
};


class ItemRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    ItemRequestHandlerFactory(MediaServer* pItemServer);


    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
    MediaServer*        _pServer;
};


class ServerObjectProperty : public MemoryProperty
{
public:
    ServerObjectProperty(ServerItem* pItem, AbstractDataModel* pDataModel);

    virtual std::istream* getStream();

protected:
    ServerItem*                 _pItem;
    AbstractDataModel*          _pDataModel;
};


class ServerObjectResource : public MemoryResource
{
    friend class ItemRequestHandler;

public:
    ServerObjectResource(ServerItem* pItem, AbstractDataModel* pDataModel);

    virtual bool isSeekable();
    virtual std::streamsize getSize();
    virtual std::istream* getStream();

private:
    ServerItem*                 _pItem;
    AbstractDataModel*          _pDataModel;
    int                         _id;
};


class ServerObject : public MemoryMediaObject
{
    friend class ItemRequestHandler;
    friend class ServerObjectResource;
    friend class ServerObjectPropertyImpl;

public:
    ServerObject(MediaServer* pServer);
    ~ServerObject();

    virtual std::string getId();
    virtual std::string getParentId();
    virtual ui4 getIndex();
    void setIndex(const std::string& index);
    void setIndex(ui4 index);
    virtual ui4 getParentIndex();
    void setParentIndex(ui4 index);
    ServerObject* getParent();
    void setParent(ServerObject* pParent);
    virtual ui4 getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*") { return 0; }

    // this really is createChildItem(), not createChildObject()
    virtual AbstractMediaObject* createChildObject();

protected:
    virtual std::istream* getIconStream() {}

    ui4                         _index;
    ui4                         _parentIndex;
    ServerObject*               _pParent;
    MediaServer*                _pServer;
//     AvStream::Transcoder*   _pTranscoder;
};


class ServerItem : public Omm::Av::ServerObject
{
    friend class ServerContainer;

public:
    ServerItem(MediaServer* pServer, ServerContainer* pContainer = 0);
    virtual ~ServerItem();

    virtual ServerObjectResource* createResource();

private:
    ServerContainer*    _pContainer;
};


class ServerContainer : public ServerObject, public Util::ConfigurablePlugin
{
public:
    ServerContainer(MediaServer* pServer);

    void setDataModel(AbstractDataModel* pDataModel);
    AbstractDataModel* getDataModel();

    virtual ServerContainer* createMediaContainer();
    virtual ServerItem* createMediaItem();
    virtual AbstractMediaObject* createChildObject();

    void appendChild(AbstractMediaObject* pChild);
    void appendChildWithAutoIndex(AbstractMediaObject* pChild);

    ServerObject* getDescendant(const std::string& objectId);

    virtual ui4 getChildCount();
    virtual ServerObject* getChildForIndex(const std::string& index);
    virtual ServerObject* getChildForIndex(ui4 index);
    virtual ui4 getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");

    virtual void setBasePath(const std::string& basePath);
    virtual void updateCache(bool on = true) {}

protected:
    virtual void initChild(ServerObject* pObject, ui4 index);

    AbstractDataModel*              _pDataModel;

private:
    Poco::FastMutex                 _serverLock;
};


class ServerObjectWriter : public MediaObjectWriter2
{
public:
    void writeChildren(std::string& meta, const std::vector<ServerObject*>& children, const std::string& filter = "*");
};


class DatabaseCache : public AbstractMediaObjectCache
{
public:
    DatabaseCache();
    ~DatabaseCache();

    void setCacheFilePath(const std::string& cacheFilePath);

    ui4 rowCount();

    virtual ServerObject* getMediaObjectForIndex(ui4 index);
    virtual ui4 getBlockAtRow(std::vector<ServerObject*>& block, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");

protected:
    void insertMediaObject(ServerObject* pObject);
    void insertBlock(std::vector<ServerObject*>& block);

private:
    Poco::Data::Session*        _pSession;
    std::string                 _cacheFilePath;
};


class CachedServerContainer : public ServerContainer, public DatabaseCache
{
public:
    CachedServerContainer(MediaServer* pServer);

    virtual bool isSearchable() { return true; }

    // TODO: sort and search caps should be moved to data model
    virtual CsvList* getSortCaps();
    virtual CsvList* getSearchCaps();

    virtual void setBasePath(const std::string& basePath);
    virtual void updateCache(bool on = true);

private:
    virtual ServerObject* getChildForIndex(ui4 index);
    virtual ui4 getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");
    bool cacheNeedsUpdate();
    void updateCacheThread();
    bool updateCacheThreadIsRunning();
    virtual void initChild(ServerObject* pObject, ui4 index);

    CsvList                                             _searchCaps;
    CsvList                                             _sortCaps;

    Poco::Thread                                        _updateCacheThread;
    Poco::RunnableAdapter<CachedServerContainer>        _updateCacheThreadRunnable;
    bool                                                _updateCacheThreadRunning;
    Poco::FastMutex                                     _updateCacheThreadLock;
};


class AbstractDataModel
{
public:
    AbstractDataModel();

    static const ui4 INVALID_INDEX;

    void setServerContainer(ServerContainer* pServerContainer);
    ServerContainer* getServerContainer();
    void setBasePath(const std::string& basePath);
    std::string getBasePath();

    virtual std::string getServerClass() { return ""; }
    virtual void scan(bool recurse = true) {}
    // class property of container media object itself
//    virtual std::string getContainerClass() { return AvClass::CONTAINER; }

    // data model cares only about one media object at a time
    // buffering / caching / optimized access is done internally at next layers
    // child media object creation / deletion
    // index and path
    virtual bool preserveIndexCache() { return false; }
    virtual bool useObjectCache() { return false; }
    // decide if to use index cache, if no, implement next four methods
    // depending on the data domain, the bijective mapping between index and path
    // can be trivial and should override getIndex(), getPath(), and hasIndex().
    // otherwise a standard mapping is implemented here.
    virtual ui4 getIndexCount();
    virtual bool hasIndex(ui4 index);
    virtual ui4 getIndex(const std::string& path);
    virtual std::string getPath(ui4 index);

    typedef std::map<ui4, std::string>::const_iterator IndexIterator;
    IndexIterator beginIndex();
    IndexIterator endIndex();

    // add / remove path tells server about existence of objects
    // any change is propagated via moderated event mechanism to controller
    void addPath(const std::string& path, const std::string& resourcePath = "");
    void removePath(const std::string& path);

    virtual std::string getParentPath(const std::string& path) { return ""; }

    // meta data of object
    virtual ServerObject* getMediaObject(const std::string& path) { return 0; }
    // stream data of object
    virtual std::streamsize getSize(const std::string& path) { return -1; }
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "") { return false; }
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "") { return 0; }
    virtual std::istream* getIconStream(const std::string& path) { return 0; }

protected:
    void readIndexCache();
    void writeIndexCache();

    Poco::Path                                  _basePath;
    Poco::Path                                  _cacheDirPath;
    Poco::Path                                  _configDirPath;
    Poco::Path                                  _metaDirPath;
    Poco::Path                                  _indexFilePath;

private:
    ServerContainer*                            _pServerContainer;
    std::map<ui4, std::string>                  _indexMap;
    std::map<std::string, ui4>                  _pathMap;
    std::multimap<ui4, std::string>             _resourceMap;
    std::stack<ui4>                             _freeIndices;
    ui4                                         _maxIndex;
};


class SimpleDataModel : public AbstractDataModel
{
public:
    virtual ServerObject* getMediaObject(const std::string& path);

     // properties
    virtual std::string getClass(const std::string& path) { return AvClass::OBJECT; }
    virtual std::string getTitle(const std::string& path) { return ""; }
    virtual std::string getOptionalProperty(const std::string& path, const std::string& name) { return ""; }

    // resource(s), currently data model only supports one resource
    virtual std::string getMime(const std::string& path) { return "*"; }
    virtual std::string getDlna(const std::string& path) { return "*"; }
};


} // namespace Av
} // namespace Omm

#endif
