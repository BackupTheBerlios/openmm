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


// #include "AvStream.h"
#include "UpnpAvTypes.h"
#include "UpnpAvDevices.h"


namespace Omm {
namespace Av {

class MediaItemServer;
class StreamingMediaObject;


class MediaItemServer
{
    friend class ItemRequestHandler;
    friend class StreamingMediaObject;
    
public:
    MediaItemServer(int port = 0);
    ~MediaItemServer();
    
    void start();
    void stop();
    Poco::UInt16 getPort() const;
    std::string getProtocol();
    
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


/**
class UpnpAvServer
used by ServerApplication
provides a meta data server that can be browsed by CDS implementation through root media object
*/
class AvServer : public MediaServer
{
public:
    AvServer();

    void setRoot(AbstractMediaObject* pRoot);
    AbstractMediaObject* getRoot();

private:
    AbstractMediaObject* _pRoot;
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
    virtual ui4 getSize() { return 0; }
    virtual std::string getMime() { return "*"; }
    virtual std::string getDlna() { return "*"; }
    
    virtual bool isSeekable() = 0;
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek) = 0;
    
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
    
private:
    StreamingMediaObject*       _pServer;
};


class StreamingMediaObject : public AbstractMediaObject
{
    friend class ItemRequestHandler;
    friend class StreamingResource;
    
public:
    StreamingMediaObject(int port = 0);
    ~StreamingMediaObject();
    
    virtual AbstractMediaObject* createChildObject();
    
private:
    std::string getServerAddress();
    std::string getServerProtocol();

    MediaItemServer*        _pItemServer;
//     AvStream::Transcoder*   _pTranscoder;
};


class AbstractDataModel
{
public:
    virtual ui4 getChildCount() { return 0; }
    virtual std::string getClass(Omm::ui4 index) { return AvClass::OBJECT; }
    virtual std::string getTitle(ui4 index) { return ""; }
    
    virtual ui4 getSize(ui4 index) { return 0; }
    virtual std::string getMime(ui4 index) { return "*"; }
    virtual std::string getDlna(ui4 index) { return "*"; }
    virtual bool isSeekable(ui4 index) { return false; }
    virtual std::streamsize stream(ui4 index, std::ostream& ostr, std::iostream::pos_type seek) { return 0; }
};


class TorchServer : public StreamingMediaObject
{
    friend class TorchItemResource;
    
public:
    TorchServer(int port = 0);
    virtual ~TorchServer();
    
    void setDataModel(AbstractDataModel* pDataModel);
    
protected:
    AbstractDataModel*          _pDataModel;
    
private:
    virtual AbstractMediaObject* getChild(ui4 numChild);
    virtual ui4 getChildCount();
    virtual bool isContainer();
    virtual int getPropertyCount(const std::string& name = "");
    virtual AbstractProperty* getProperty(int index);
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0);
    virtual void addProperty(AbstractProperty* pProperty);
    virtual AbstractProperty* createProperty();

    AbstractProperty*           _pTitleProp;
    AbstractMediaObject*        _pChild;
};


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
