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
#include <Poco/Timer.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/TextEncoding.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/TextConverter.h>

// #include "AvStream.h"
#include "Upnp.h"
#include "UpnpAvObject.h"


namespace Omm {
namespace Av {

class MediaItemServer;
class ServerObject;
class ServerItem;
class ServerObjectCache;
class DevContentDirectoryServerImpl;
class ServerContainer;
class AbstractDataModel;


class MediaServer : public Device
/// Used by ServerApplication
/// Provides a meta data server that can be browsed by CDS implementation through root media object,
/// and a http server for streaming server object resources
{
    friend class ItemRequestHandler;
    friend class ServerObject;
    friend class ServerContainer;

public:
    MediaServer(int port = 0);
    virtual ~MediaServer();

    void setRoot(ServerContainer* pRoot);

    virtual void start();
    virtual void stop();

    Poco::UInt16 getServerPort() const;
    std::string getServerAddress();
    std::string getServerProtocol();

    void setSystemUpdateId(ui4 id);
    void setPollSystemUpdateIdTimer(long msec);

private:
    void startPollSystemUpdateId(bool start = true);
    void pollSystemUpdateId(Poco::Timer& timer);

    DevContentDirectoryServerImpl*              _pDevContentDirectoryServerImpl;

    ServerContainer*                            _pServerContainer;
    Poco::Net::ServerSocket                     _socket;
    Poco::Net::HTTPServer*                      _pHttpServer;
    Poco::Timer*                                _pSystemUpdateIdTimer;
    long                                        _systemUpdateIdTimerInterval;
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
    ServerObject*               _pObject;
    AbstractDataModel*          _pDataModel;
};


class ServerObjectResource : public MemoryResource
{
    friend class ItemRequestHandler;

public:
    ServerObjectResource(ServerObject* pItem, AbstractDataModel* pDataModel);

    virtual bool isSeekable();
    virtual std::streamsize getSize();
    virtual std::istream* getStream();
    void freeStream(std::istream* pIstream);

    void writeResource(const uri& sourceUri);
    std::string getPrivateResourceUri();
    void setPrivateResourceUri(const std::string& uri);

private:
    ServerObject*               _pObject;
    AbstractDataModel*          _pDataModel;
    int                         _id;
    std::string                 _privateUri;
};


class ServerObject : public MemoryMediaObject
{
    friend class ItemRequestHandler;
    friend class ServerObjectResource;
    friend class ServerObjectCache;
    friend class ServerContainer;
    friend class DatabaseCache;

public:
    enum IndexNamespace {Data, Virtual, User};

    ServerObject(MediaServer* pServer);
    ~ServerObject();

    // AbstractMediaObject
    virtual void setUniqueProperty(const std::string& name, const std::string& value);

    // factory method
    virtual ServerObject* createChildObject();
    virtual ServerObjectResource* createResource();

    // object id, index, row and path
    // index: fixed number and unique identifier for a server object
    // object id: 1. object path to object consisting of object ids of parent containers (index1/index1/ ...) (fixed hierarchical parent relationship, not flat)
    //            2. id within a server container's name space (index = 0 and relative object id = "0") with child objects (index = 1, 2, ... or for virtual objects v0, v1, ...)
    //            within a server container's (flat) name space, all objects have a unique index, no paths (i.e. no slashes),
    //            a child object of a subcontainer object (which can be virtual or not) has always the same and unique object id (no relative object path)
    //
    //            resource of an object has the same object path + resource suffix (e.g. "$resource number")
    //            reserved object id "-1" for parent id when there's no parent
    // row: contiguous number of a child in an object container in the context of search and sort criteria
    // path: unique string to identify an object in the servers domain (e.g. path to file, url to webradio station), don't confuse with object path above.
    virtual std::string getId();
    virtual std::string getParentId();
    virtual ui4 getIndex();
    void setIndex(const std::string& index);
    void setIndex(ui4 index);
//    bool isVirtual();
//    void setIsVirtual(bool isVirtual = true);

    // parent and descendants
    virtual ui4 getParentIndex();
    void setParentIndex(ui4 index);
    ServerObject* getParent();
    void setParent(ServerObject* pParent);
    virtual ui4 getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*") { return 0; }
    // this really is createChildItem(), not createChildObject()
//    virtual AbstractMediaObject* createChildObject();

    MediaServer* getServer();

protected:
    virtual std::istream* getIconStream() {}

    ui4                         _index;
    ui4                         _parentIndex;
    ServerObject*               _pParent;
    MediaServer*                _pServer;
    AbstractDataModel*          _pDataModel;
    IndexNamespace              _indexNamespace;
//    bool                        _isVirtual;
//     AvStream::Transcoder*   _pTranscoder;
};


class ServerItem : public Omm::Av::ServerObject
{
    friend class ServerContainer;

public:
    ServerItem(MediaServer* pServer);
    virtual ~ServerItem();

    virtual ServerObjectResource* createResource();
};


class ServerContainer : public ServerObject, public Util::ConfigurablePlugin
{
    friend class DatabaseCache;
    friend class ServerObjectResource;

public:
    ServerContainer(MediaServer* pServer);

    enum Layout {Flat, DirStruct, PropertyGroups};
    static const std::string  LAYOUT_FLAT;
    static const std::string  LAYOUT_DIR_STRUCT;
    static const std::string  LAYOUT_PROPERTY_GROUPS;

    static const std::string  PROPERTY_GROUP_PROPERTY_NAME;
    static const std::string  PROPERTY_GROUP_PROPERTY_VALUE;

    // factory methods
    virtual ServerContainer* createMediaContainer();
    virtual ServerItem* createMediaItem();

    void addPlaylistResource();
    void addUserObject(ServerObject* pChildObject);

    AbstractDataModel* getDataModel();
    void setDataModel(AbstractDataModel* pDataModel);

    ServerObjectCache* getObjectCache();
    void setObjectCache(ServerObjectCache* pObjectCache);

    Layout getLayout();
    void setLayout(Layout layout);
    void setLayout(const std::string& layout);

    // sort and search caps
    virtual bool isSearchable();
    virtual CsvList* getSortCaps();
    virtual CsvList* getSearchCaps();

    virtual void setBasePath(const std::string& basePath);
    virtual void updateCache();
    bool cacheConsistent();

    // appendChild*() methods are only needed for server containers without data model (not supported right now)
    void appendChild(AbstractMediaObject* pChild);
    void appendChildWithAutoIndex(AbstractMediaObject* pChild);

    ServerObject* getDescendant(const std::string& objectId);

    virtual ServerObject* getChildForIndexString(const std::string& indexString);
    virtual ServerObject* getChildForIndex(ui4 index, bool init = true, IndexNamespace indexNamespace = Data);
    virtual ui4 getChildrenAtRowOffset(std::vector<ServerObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");

    std::stringstream* generateChildrenPlaylist();

private:
    virtual ServerObject* initChild(ServerObject* pObject, ui4 index, bool fullInit = true);

//    AbstractDataModel*                                  _pDataModel;
    ServerObjectCache*                                  _pObjectCache;
    ServerObjectCache*                                  _pVirtualContainerCache;
    ServerObjectCache*                                  _pUserObjectCache;
    Layout                                              _layout;
    std::string                                         _groupPropertyName;
    std::size_t                                         _childrenPlaylistSize;
    std::vector<ui4>                                    _childrenPlaylistIndices;

    Poco::FastMutex                                     _serverLock;
    CsvList                                             _searchCaps;
    CsvList                                             _sortCaps;
};


class ServerObjectWriter : public MediaObjectWriter2
{
public:
    void writeChildren(std::string& meta, const std::vector<ServerObject*>& children, const std::string& filter = "*");
};


class ServerObjectCache : public AbstractMediaObjectCache
{
    friend class ServerContainer;

public:
    virtual void setCacheFilePath(const std::string& cacheFilePath) {}

    virtual ui4 rowCount() { return 0; }

    virtual ServerObject* getMediaObjectForIndex(ui4 index, bool isVirtual = false) { return 0; }
    virtual ui4 getBlockAtRow(std::vector<ServerObject*>& block, ServerContainer* pParentContainer, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*") { return 0; }
    virtual void getIndices(std::vector<ui4>& indices, const std::string& sort = "") {}
    virtual void getUpdateIds(std::map<ui4, ui4>& updateIds) {}

    virtual void insertMediaObject(ServerObject* pObject) {}
    virtual void updateMediaObject(ServerObject* pObject) {}
    virtual void insertBlock(std::vector<ServerObject*>& block) {}
    virtual void removeMediaObjectForIndex(ui4 index) {}

    virtual void addPropertiesForQuery(CsvList propertyList) {}
    virtual void updateVirtualObjects(ServerObjectCache* pVirtualObjectCache) {}

    CsvList& getPropertiesForQuery();

protected:
    ServerContainer*                    _pServerContainer;
    CsvList                             _queryPropertyNames;
};


class DatabaseCache : public ServerObjectCache
{
    friend class DatabaseCacheSearchCriteria;

public:
    DatabaseCache(const std::string& cacheTableName, ServerObject::IndexNamespace indexNamespace = ServerObject::Data);
    ~DatabaseCache();

    virtual void setCacheFilePath(const std::string& cacheFilePath);

    virtual ui4 rowCount();

    virtual ServerObject* getMediaObjectForIndex(ui4 index, bool isVirtual = false);
    virtual ui4 getBlockAtRow(std::vector<ServerObject*>& block, ServerContainer* pParentContainer, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");
    virtual void getIndices(std::vector<ui4>& indices, const std::string& sort = "");
    virtual void getUpdateIds(std::map<ui4, ui4>& updateIds);

    virtual void insertMediaObject(ServerObject* pObject);
    virtual void updateMediaObject(ServerObject* pObject);
    virtual void removeMediaObjectForIndex(ui4 index);

    virtual void addPropertiesForQuery(CsvList propertyList);
    virtual void updateVirtualObjects(ServerObjectCache* pVirtualObjectCache);

private:
    std::string getColumnName(const std::string& propertyName);
    std::string getColumnType(const std::string& propertyName);

    Poco::Data::Session*                _pSession;
    std::string                         _cacheFilePath;
    std::string                         _cacheTableName;
    int                                 _maxQueryPropertyCount;
    std::map<std::string, std::string>  _propertyColumnNames;
    std::map<std::string, std::string>  _propertyColumnTypes;
    ServerObject::IndexNamespace        _indexNamespace;
};


class DatabaseCacheSearchCriteria : public SqlSearchCriteria
{
public:
    DatabaseCacheSearchCriteria(DatabaseCache* pDatabaseCache);

protected:
    virtual std::string translateProperty(const std::string& property);
    virtual std::string translateCompareExp(const std::string& property, const std::string& op, const std::string& val);

private:
    DatabaseCache*      _pDatabaseCache;
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
    virtual void init() {}
    virtual std::string getModelClass() { return ""; }
    virtual CsvList getQueryProperties() { return CsvList(""); }

    virtual ui4 getSystemUpdateId(bool checkMod) { return 0; }
    virtual ui4 getUpdateId(const std::string& path) { return getSystemUpdateId(false); }
    /// get update id of particular object in data model, identified by path
    /// if not implemented by data model, one single update id for the whole model is assumed
    void setCheckObjectModifications(bool check = true);
    void checkSystemUpdateId();

    // data model cares only about one media object at a time
    // buffering / caching / optimized access is done internally at next layers
    // child media object creation / deletion
    // index and path
    virtual void scan() {}
    virtual bool preserveIndices() { return false; }
    /// keep index of a removed path (in seperate index map)
    /// this can be usefull for a DVD server with resume functionality
    virtual bool useObjectCache() { return false; }
    /// decide if to use object cache

    // next four methods can optionally be implemented to provide a different mapping
    // between index and path (TODO: try this ... what about newIndex() call in addPath()).
    // depending on the data domain, the bijective mapping between index and path
    // can be trivial and should override getIndex(), getPath(), and hasIndex().
    // otherwise a standard mapping is implemented here.
    virtual ui4 getIndexCount();
    virtual bool hasIndex(ui4 index);
    virtual ui4 getIndex(const std::string& path);
    virtual std::string getPath(ui4 index);

    typedef std::map<ui4, std::string>::const_iterator IndexMapIterator;
    IndexMapIterator beginIndex();
    IndexMapIterator endIndex();

    typedef std::vector<ui4>::const_iterator IndexIterator;
    IndexIterator beginCommonIndex();
    IndexIterator endCommonIndex();
    IndexIterator beginAddedIndex();
    IndexIterator endAddedIndex();
    IndexIterator beginRemovedIndex();
    IndexIterator endRemovedIndex();

    // add / remove path tells server about existence of objects
    // any change is propagated via moderated event mechanism to controller
    void addPath(const std::string& path, const std::string& resourcePath = "");
    void removeIndex(ui4 index);

    virtual std::string getParentPath(const std::string& path) { return ""; }

    // meta data of objects
    virtual ServerObject* getMediaObject(const std::string& path) { return 0; }
    virtual ui4 getBlockAtRow(std::vector<ServerObject*>& block, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");

    // stream data of object
    virtual std::streamsize getSize(const std::string& path) { return -1; }
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "") { return false; }
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "") { return 0; }
    virtual void freeStream(std::istream* pIstream) {}
    virtual std::istream* getIconStream(const std::string& path) { return 0; }

    const Poco::TextEncoding* getTextEncoding();
    void setTextEncoding(const std::string& encoding);
    Poco::TextConverter* getTextConverter();

protected:
    void readIndexMap();
    void writeIndexMap();

    ui4 getPublicSystemUpdateId();
    void setPublicSystemUpdateId(ui4 id);
    void incPublicSystemUpdateId();

    ui4 getCacheSystemUpdateId(bool mod = false);
    void setCacheSystemUpdateId(ui4 id, bool mod = false);

    ui4 getNewIndex();

    Poco::Path                                  _basePath;
    Poco::Path                                  _cacheDirPath;
    Poco::Path                                  _configDirPath;
    Poco::Path                                  _metaDirPath;
    Poco::Path                                  _indexFilePath;

private:
    void updateThread();
    bool updateThreadIsRunning();

    ServerContainer*                            _pServerContainer;
    std::map<ui4, std::string>                  _indexMap;
    std::map<std::string, ui4>                  _pathMap;
    std::multimap<ui4, std::string>             _resourceMap;
    std::stack<ui4>                             _freeIndices;

    ui4                                         _publicSystemUpdateId;
    ui4                                         _cacheSystemUpdateId;
    ui4                                         _cacheSystemModId;
    bool                                        _checkMod;

    std::vector<ui4>                            _lastIndices;
    std::vector<ui4>                            _commonIndices;
    std::vector<ui4>                            _addedIndices;
    std::vector<ui4>                            _removedIndices;

    Poco::Thread                                _updateThread;
    Poco::RunnableAdapter<AbstractDataModel>    _updateThreadRunnable;
    bool                                        _updateThreadRunning;
    Poco::FastMutex                             _updateThreadLock;

    Poco::TextEncoding::Ptr                     _pSourceEncoding;
    Poco::UTF8Encoding                          _targetEncoding;
    Poco::TextConverter*                        _pTextConverter;
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
