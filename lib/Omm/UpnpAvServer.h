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
class MediaServerContainer;


class ServerResource : public Resource
{
public:
    ServerResource(const std::string& resourceId, const std::string& protInfo, ui4 size);
    
    const std::string& getResourceId();
    virtual bool isSeekable() { return false; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek) { return 0; }
    
private:
    std::string     _resourceId;
};


class FileResource : public ServerResource
{
public:
    FileResource(const std::string& resourceId, const std::string& protInfo, ui4 size, const std::string& privateUri);
    
    virtual bool isSeekable() { return true; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
private:
    std::string         _privateUri;
};


class WebResource : public ServerResource
{
public:
    WebResource(const std::string& resourceId, const std::string& protInfo, const std::string& privateUri);
    
    virtual bool isSeekable() { return false; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
private:
    std::string         _privateUri;
};



class ServerObject : public MediaObject
{
public:
    ServerObject();
    
    void addResource(ServerResource* pResource);
    ServerResource* getResource(const std::string& resourceId);
    
    virtual void appendChild(ServerObject* pChild);
    ServerObject* getObject(const std::string& objectId);
    
private:
    std::map<std::string,ServerObject*>         _childrenMap;
    std::map<std::string,ServerResource*>       _resourceMap;
};


class MediaItem : public ServerObject
{
public:
    MediaItem();
    MediaItem(const std::string& objectId, const std::string& title, const std::string& subClass);
};


class MediaContainer : public ServerObject
{
public:
    MediaContainer();
    MediaContainer(const std::string& title, const std::string& subClass = "");
};


class MediaServerContainer : public MediaContainer
{
    friend class MediaItemServer;
    friend class ItemRequestHandler;
    
public:
    MediaServerContainer(const std::string& title, const std::string& subClass = "", int port = 0);
    ~MediaServerContainer();
    
    virtual void appendChild(ServerObject* pChild);
    
    std::string getServerAddress();
    
private:
    MediaItemServer*        _pItemServer;
//     AvStream::Transcoder*   _pTranscoder;
    
    // TODO: this should be fetched from _pItemServer
    int                 _port;
    std::string         _address;
};


class MediaItemServer
{
    friend class ItemRequestHandler;
    friend class MediaServerContainer;
    
public:
    MediaItemServer(int port = 0);
    ~MediaItemServer();
    
    void start();
    void stop();
    Poco::UInt16 getPort() const;
    
private:
    MediaServerContainer*                       _pServerContainer;
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


class UpnpAvServer : public MediaServer
{
public:
    UpnpAvServer();

    void setRoot(ServerObject* pRoot);
    ServerObject* getRoot();

private:
    ServerObject* _pRoot;
};


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


} // namespace Av
} // namespace Omm

#endif
