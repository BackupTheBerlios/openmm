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

#include "UpnpAvTypes.h"
#include "UpnpAvDevices.h"


namespace Omm {
namespace Av {

class MediaItemServer;
class MediaServerContainer;


class Transcoder
{
};


class Tagger
{
};


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
    MediaServerContainer(const std::string& title, const std::string& subClass = "");
    ~MediaServerContainer();
    
    virtual void appendChild(ServerObject* pChild);
    
    std::string getServerAddress();
    
private:
    MediaItemServer*    _pItemServer;
    Transcoder*         _pTranscoder;
    
    // TODO: this should be fetched from _pItemServer
    int                 _port;
    std::string         _address;
};


class MediaItemServer
{
    friend class ItemRequestHandler;
    friend class MediaServerContainer;
    
public:
    MediaItemServer();
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

} // namespace Av
} // namespace Omm

#endif
