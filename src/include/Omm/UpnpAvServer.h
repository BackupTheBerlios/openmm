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
class StreamingMediaObject;
class DevContentDirectoryServerImpl;
class ServerContainer;


class MediaItemServer
{
    friend class ItemRequestHandler;
    friend class StreamingMediaObject;

public:
    MediaItemServer(int port = 0);
    ~MediaItemServer();

    void start();
    void stop();
    // TODO: set a data model here. Initialization of data model should be
    // asynchronous, using update mechanism of UPnP AV directory service.

    Poco::UInt16 getPort() const;
    std::string getProtocol();

protected:
    // virtual bool initItemServer() { return true; }
    /// will be executed at start. If initializion takes a while, start(true)
    /// may be used for asynchronous initialization.

private:
    StreamingMediaObject*                       _pServerContainer;
    Poco::Net::ServerSocket                     _socket;
    Poco::Net::HTTPServer*                      _pHttpServer;
};


class ItemRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    ItemRequestHandler(MediaItemServer* pItemServer);

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    std::streamsize copyStream(std::istream& istr, std::ostream& ostr, std::streamoff start = 0, std::streamoff end = -1);
    void parseRange(const std::string& rangeValue, std::streamoff& start, std::streamoff& end);

    unsigned int _bufferSize;
    MediaItemServer*  _pItemServer;
};


class ItemRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    ItemRequestHandlerFactory(MediaItemServer* pItemServer);


    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
    MediaItemServer*  _pItemServer;
};


class MediaServer : public Device
    /// Used by ServerApplication
    /// Provides a meta data server that can be browsed by CDS implementation through root media object
{
public:
    MediaServer();
    virtual ~MediaServer();

    void setRoot(AbstractMediaObject* pRoot);
//    AbstractMediaObject* getRoot();

private:
    DevContentDirectoryServerImpl* _pDevContentDirectoryServerImpl;

//    AbstractMediaObject* _pRoot;
};


class StreamingProperty : public AbstractProperty
{
public:
    std::istream* getStream();
};


class StreamingPropertyImpl : public PropertyImpl
{
    friend class ItemRequestHandler;

public:
    StreamingPropertyImpl(StreamingMediaObject* pServer, AbstractMediaObject* pItem);

    virtual std::string getValue();
    // some properties can stream: icon, album art
    virtual std::istream* getStream() { return 0; }


protected:
    StreamingMediaObject*       _pServer;
    AbstractMediaObject*        _pItem;
};


class StreamingResource : public AbstractResource
{
    friend class ItemRequestHandler;

public:
    StreamingResource(PropertyImpl* pPropertyImpl, StreamingMediaObject* pServer, AbstractMediaObject* pItem);

    virtual std::string getValue();
    virtual std::string getAttributeName(int index);
    virtual std::string getAttributeValue(int index);
    virtual std::string getAttributeValue(const std::string& name);
    virtual int getAttributeCount();

protected:
    virtual std::streamsize getSize() { return -1; }
    virtual std::string getMime() { return "*"; }
    virtual std::string getDlna() { return "*"; }

    virtual bool isSeekable() = 0;
    virtual std::istream* getStream() = 0;

protected:
    StreamingMediaObject*       _pServer;
    AbstractMediaObject*        _pItem;
    int                         _id;
};


// TODO: this should be a ServerObject (automatic id assignment) with
// arbitrary implementation for accessing properties (see next todo brigde pattern)
// TODO: use bridge pattern for AbstractMediaObject, similar to AbstractProperty (really do so?)
// class StreamingMediaItem : public AbstractMediaObject
class StreamingMediaItem : public MemoryMediaObject
{
public:
    StreamingMediaItem(StreamingMediaObject* pServer);

protected:
    StreamingMediaObject*       _pServer;
};


class StreamingMediaObject : public AbstractMediaObject
{
    friend class ItemRequestHandler;
    friend class StreamingResource;
    friend class StreamingPropertyImpl;

public:
    StreamingMediaObject(int port = 0);
    ~StreamingMediaObject();

    virtual AbstractMediaObject* createChildObject();

protected:
    virtual std::istream* getIconStream();

private:
    std::string getServerAddress();
    std::string getServerProtocol();

    MediaItemServer*        _pItemServer;
//     AvStream::Transcoder*   _pTranscoder;
};


class AbstractDataModel
{
public:
    AbstractDataModel();

    static const ui4 INVALID_INDEX;

    void setServerContainer(ServerContainer* pServerContainer);
    ServerContainer* getServerContainer();

    // class property of container media object itself
    virtual std::string getContainerClass() { return AvClass::CONTAINER; }

    // data model cares only about one media object at a time
    // buffering / caching / optimized access is done internally at next layers
    // child media object creation / deletion
    // index and path
    // depending on the data domain, the bijective mapping between index and path
    // can be trivial and should override getIndex(), getPath(), and hasIndex().
    // otherwise a standard mapping is implemented here.
//    virtual void createIndexCache() {}
    virtual bool preserveIndexCache() { return false; }
    virtual bool useObjectCache() { return false; }
    /// decide if to use index cache, if no, implement next four methods
    virtual ui4 getIndexCount();
    virtual bool hasIndex(ui4 index);
    virtual ui4 getIndex(const std::string& path);
    virtual std::string getPath(ui4 index);

    typedef std::map<ui4, std::string>::const_iterator IndexIterator;
    IndexIterator beginIndex();
    IndexIterator endIndex();
//    ui4 getBlockAtRow(std::vector<AbstractMediaObject*>& block, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");
//    ui4 getIndex(ui4 indexNumber); // used only for special case with no sort and search criteria

    // add / remove index tells server about change of data
    // propagated via moderated event mechanism to controller
    // also, index can be added to / removed from index cache
//    virtual void addIndex(ui4 index, const std::string& path, bool cache = true);   // TODO: addPath() ?
//    virtual void removeIndex(ui4 index, bool cache = true);   // TODO: removePath() ?
    void addPath(const std::string& path);
    void removePath(const std::string& path);

    virtual bool isContainer(const std::string& path) { return false; }
    virtual std::string getParentPath(const std::string& path) { return ""; }
//    void addIndices(const std::vector<ui4>& indices);
//    void removeIndices(const std::vector<ui4>& indices);
//    void flushIndexBuffer();
//    void bufferIndex(Omm::ui4 index);

    // number of child media objects in container at one point in time consistent
    // the list of existing indices / paths (size of index list == child count)
    // (synchronized with child object creation / deletion)
    // TODO: methods below should be accessed via path ?
//    virtual ui4 getChildCount();   // TODO: getIndexCount() and put it in index section of class ?

    // meta data of object
//    virtual AbstractMediaObject* getMediaObject(ui4 index) { return 0; }
//    // stream data of object
    virtual bool isSeekable(ui4 index, const std::string& resourcePath = "") { return false; }
    virtual std::streamsize getSize(ui4 index) { return -1; }
    virtual std::istream* getStream(ui4 index, const std::string& resourcePath = "") { return 0; }

    virtual AbstractMediaObject* getMediaObject(const std::string& path) { return 0; }
    // stream data of object
    virtual std::streamsize getSize(const std::string& path) { return -1; }
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "") { return false; }
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "") { return 0; }

protected:
    void readIndexCache();
    void writeIndexCache();

    std::string                 _cacheFile;

private:
    ServerContainer*            _pServerContainer;
    std::map<ui4, std::string>  _indexCache;
    std::map<std::string, ui4>  _pathCache;
    std::stack<ui4>             _freeIndices;
    ui4                         _maxIndex;
    std::vector<Omm::ui4>       _indexBuffer;
    Omm::ui4                    _indexBufferSize;
};


class SimpleDataModel : public AbstractDataModel
{
public:
    virtual AbstractMediaObject* getMediaObject(ui4 index);
    virtual AbstractMediaObject* getMediaObject(const std::string& path);

     // properties
    virtual std::string getClass(ui4 index) { return AvClass::OBJECT; }
    virtual std::string getTitle(ui4 index) { return ""; }
    virtual std::string getOptionalProperty(ui4 index, const std::string& property) { return ""; }

    virtual std::string getClass(const std::string& path) { return AvClass::OBJECT; }
    virtual std::string getTitle(const std::string& path) { return ""; }
    virtual std::string getOptionalProperty(const std::string& path, const std::string& name) { return ""; }

    // resource(s), currently data model only supports one resource
    virtual std::string getMime(ui4 index) { return "*"; }
    virtual std::string getDlna(ui4 index) { return "*"; }

    virtual std::istream* getIconStream(ui4 index) { return 0; }

    virtual std::string getMime(const std::string& path) { return "*"; }
    virtual std::string getDlna(const std::string& path) { return "*"; }

    virtual std::istream* getIconStream(const std::string& path) { return 0; }
};


class ServerItemResource : public Omm::Av::StreamingResource
{
public:
    ServerItemResource(ServerContainer* pServer, Omm::Av::AbstractMediaObject* pItem);

    virtual bool isSeekable();
    virtual std::streamsize getSize();
    virtual std::istream* getStream();
};


class ServerItem : public Omm::Av::StreamingMediaItem
{
    friend class ServerContainer;

public:
    ServerItem(ServerContainer* pServer);
    virtual ~ServerItem();

    virtual ServerItemResource* createResource();
};


class ServerContainer : public StreamingMediaObject, public Util::ConfigurablePlugin
{
public:
    ServerContainer(int port = 0);

    void setDataModel(AbstractDataModel* pDataModel);
    AbstractDataModel* getDataModel();

    virtual AbstractMediaObject* createMediaContainer();
    virtual ServerItem* createMediaItem();
    virtual AbstractMediaObject* createChildObject();

    virtual bool isContainer();
    virtual int getPropertyCount(const std::string& name = "");
    virtual AbstractProperty* getProperty(int index);
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0);
    virtual void addProperty(AbstractProperty* pProperty);
    virtual AbstractProperty* createProperty();

    virtual ui4 getChildCount();
//    virtual ui4 getIndexForRow(ui4 row, const std::string& sort = "", const std::string& search = "");
    virtual AbstractMediaObject* getChildForIndex(ui4 index);
    virtual ui4 getChildrenAtRowOffset(std::vector<AbstractMediaObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");
    virtual bool singleRowInterface() { return false; }

//    virtual void addIndices(const std::vector<ui4>& indices) {}
//    virtual void removeIndices(const std::vector<ui4>& indices) {}

    virtual void setBasePath(const std::string& basePath);
    virtual void scan(bool on = true) {}

protected:
    void initObject(AbstractMediaObject* pObject, ui4 index);

    std::string                     _basePath;
    AbstractDataModel*              _pDataModel;

    AbstractProperty*               _pTitleProperty;
    AbstractProperty*               _pClassProperty;

private:
    Poco::FastMutex                 _serverLock;
};


class CachedServerContainer : public ServerContainer, public DatabaseCache
{
public:
    CachedServerContainer();

    virtual bool singleRowInterface() { return false; }
    virtual bool isSearchable() { return true; }

//    virtual void addIndices(const std::vector<ui4>& indices);
//    virtual void removeIndices(const std::vector<ui4>& indices);

    virtual CsvList* getSortCaps();
    virtual CsvList* getSearchCaps();

    virtual void scan(bool on = true);

private:
    void scanThread();
    virtual AbstractMediaObject* getChildForIndex(ui4 index);
    virtual ui4 getChildrenAtRowOffset(std::vector<AbstractMediaObject*>& children, ui4 offset, ui4 count, const std::string& sort = "", const std::string& search = "*");
    bool cacheNeedsUpdate();
    bool scanThreadIsRunning();

    CsvList             _searchCaps;
    CsvList             _sortCaps;

    Poco::Thread                                        _scanThread;
    Poco::RunnableAdapter<CachedServerContainer>        _scanThreadRunnable;
    bool                                                _scanThreadRunning;
    Poco::FastMutex                                     _scanThreadLock;
};





// --------------- deprecated --------------- //

//class TorchServerContainer : public ServerContainer
//{
//    friend class TorchItemResource;
//    friend class TorchItemPropertyImpl;
//
//public:
//    TorchServerContainer(int port = 0);
//    virtual ~TorchServerContainer();
//
//    void setDataModel(SimpleDataModel* pDataModel);
//    SimpleDataModel* getDataModel();
//
//private:
//    virtual AbstractMediaObject* getChildForIndex(ui4 index);
//    virtual AbstractMediaObject* getChildForRow(ui4 row);
//    // TODO: does getChildCount() make sense on server side? Is this total child count?
//    virtual ui4 getChildCount();
//
//    AbstractMediaObject*            _pChild;
//};


/*
class AbstractDataModel
{
public:
    // ------- mandatory interface to be implemented --------
    // deliver meta data for controller
    virtual ServerObject* getObject(const std::string& id) = 0;
    // count = 0 means fetch all children
    virtual std::vector<ServerObject*> getChildren(const std::string& id, ui4 start = 0, ui4 count = 0) = 0;

    // deliver media stream for renderer
    virtual void stream(const std::string& id, const std::string& res, std::ostream& stream) = 0;

    // ------- optional interface to be implemented --------
    // scan control
    virtual void startScan() {}
    virtual void stopScan() {}

    // cache control (size == 0 means unlimited cache size)
    virtual void activateCache(bool = true, ui4 size = 0) {}

    // filter
    virtual void filter(std::vector<std::string> properties) {};

    // sort
    virtual void sort(const std::string& property) {};

    // search for metadata
    // count = 0 means return all found objects
    virtual std::vector<ServerObject*> search(const std::string& name, const std::vector<std::string>& properties, ui4 start = 0, ui4 count = 0) {};

    // ------- callbacks  --------
    // update notification events are moderated by the server and not the data model
    // if no objectsHaveChanged notification appeared inbetween update events, only a system update event is triggered
    // also, changes on the same object inbetween events are handled by the server
    //
    // system data update notifications
    virtual void hasChanged() {}
    // container based update notifications
    virtual  std::vector<ServerObject*> objectsHaveChanged() {}
};
*/


} // namespace Av
} // namespace Omm

#endif
