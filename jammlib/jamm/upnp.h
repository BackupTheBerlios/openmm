/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef JAMMUPNP_H
#define JAMMUPNP_H

#include <string>
#include <map>
#include <iostream>
#include <istream>
#include <fstream>
#include <ostream>
#include <sstream>

#include "Poco/SingletonHolder.h"
#include "Poco/NObserver.h"
#include "Poco/Observer.h"
#include "Poco/Thread.h"
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/NotificationCenter.h"
#include "Poco/URI.h"
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/AttrMap.h"
#include "Poco/DOM/Element.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/StreamCopier.h"

using Poco::SingletonHolder;
using Poco::NObserver;
using Poco::Observer;
using Poco::Thread;
using Poco::AutoPtr;
using Poco::UInt16;
using Poco::AbstractObserver;
using Poco::Notification;
using Poco::NotificationQueue;
using Poco::NotificationCenter;
using Poco::URI;
using Poco::UUID;
using Poco::UUIDGenerator;
using Poco::Timestamp;
using Poco::DateTime;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::DateTimeParser;
using Poco::Net::Socket;
using Poco::Net::DatagramSocket;
using Poco::Net::MulticastSocket;
using Poco::Net::NetworkInterface;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::Net::SocketInputStream;
using Poco::Net::SocketOutputStream;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::IPAddress;
using Poco::Net::MessageHeader;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPResponse;
using Poco::XML::DOMParser;
using Poco::XML::Document;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::Node;
using Poco::XML::NodeList;
using Poco::XML::NamedNodeMap;
using Poco::XML::AttrMap;
using Poco::XML::Element;
using Poco::XML::AutoPtr;
using Poco::XML::InputSource;
using std::istringstream;
using Poco::StreamCopier;


namespace Jamm {


/// SSDP - Simple Service Discovery Protocol
/// HTTP dialect transmitted over UDP Multicast
/// See "UPnP Device Architecture" for details

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";
static const std::string    SSDP_SEND_ADDRESS   = "239.255.255.250:0";
static const std::string    SSDP_ADDRESS        = "239.255.255.250";
static const UInt16         SSDP_PORT           = 1900;
static const UInt16         SSDP_CACHE_DURATION = 1800;
static const UInt16         SSDP_MIN_WAIT_TIME  = 1;
static const UInt16         SSDP_MAX_WAIT_TIME  = 120;


class SsdpMessage : public Notification
{
public:
    typedef enum {
        REQUEST_NOTIFY          = 1,
        REQUEST_NOTIFY_ALIVE    = 2,
        REQUEST_NOTIFY_BYEBYE   = 3,
        REQUEST_SEARCH          = 4,
        REQUEST_RESPONSE        = 5,
        SUBTYPE_ALIVE           = 6,
        SUBTYPE_BYEBYE          = 7,
        SSDP_ALL                = 8,
        UPNP_ROOT_DEVICES       = 9
    } TRequestMethod;
    
    SsdpMessage();
    // build sceletons for the different types of SSDP messages
    SsdpMessage(TRequestMethod requestMethod);
    
    // map the received HTTP header to an SsdpMessage object in memory
    SsdpMessage(const std::string& buf, const SocketAddress& sender = SocketAddress(SSDP_FULL_ADDRESS));
    ~SsdpMessage();
    
    // HTTP message envelope
    
    std::string toString();
    
    void setRequestMethod(TRequestMethod requestMethod);
    TRequestMethod getRequestMethod();
    
    // set and get the fields of the HTTP message header
    void setCacheControl(int duration = SSDP_CACHE_DURATION);  // duration of device advertisement in sec.
    int getCacheControl();
    
    void setNotificationType(const std::string& searchTarget);
    std::string getNotificationType();
    
    void setNotificationSubtype(TRequestMethod notificationSubtype);
    TRequestMethod getNotificationSubtype();
    
    void setSearchTarget(const std::string& searchTarget);
    std::string getSearchTarget();
    
    void setUniqueServiceName(const std::string& serviceName);
    std::string getUniqueServiceName();
    
    void setLocation(const URI& location);
    URI getLocation();
    
    void setHost();
    void setHttpExtensionNamespace();
    void setHttpExtensionConfirmed();
    bool getHttpExtensionConfirmed();
    
    void setServer(const std::string& productNameVersion);
    std::string getServerOperatingSystem();
    std::string getServerUpnpVersion();
    std::string getServerProductNameVersion();
    
    void setMaximumWaitTime(int waitTime = SSDP_MIN_WAIT_TIME);  // max time to delay response, between 1 and 120 seconds.
    int getMaximumWaitTime();
    
    void setDate();
    DateTime getDate();
    
    SocketAddress getSender();
    
private:
    void initMessageMap();
    
    TRequestMethod          m_requestMethod;
    TRequestMethod          m_notificationSubtype;
    MessageHeader           m_messageHeader;
    SocketAddress           m_sender;
    
    std::map<TRequestMethod,std::string> m_messageMap;
    std::map<std::string,TRequestMethod> m_messageConstMap;
};


/// This class provides an interface to a
/// UDP Multicast socket.
class SsdpSocket
{ 
public:
    SsdpSocket(const AbstractObserver& observer);
    ~SsdpSocket();
    
    void sendMessage(SsdpMessage& message, const SocketAddress& receiver = SocketAddress(SSDP_FULL_ADDRESS));
    
    NetworkInterface    m_interface;

private:
    IPAddress           m_ssdpAddress;
    UInt16              m_ssdpPort;
    MulticastSocket     m_ssdpSocket;
    SocketReactor       m_reactor;
    Thread              m_listenerThread;
    NotificationCenter  m_notificationCenter;
    char*               m_pBuffer;
    
    enum {
        BUFFER_SIZE = 65536 // Max UDP Packet size is 64 Kbyte.
            // Note that each SSDP message must fit into one UDP Packet.
    };
    
    void onReadable(const AutoPtr<ReadableNotification>& pNf);
};

class Device;
class DeviceRoot;

class Service {
public:
    Service(Device* device, NodeIterator rootNode);
    ~Service();

// private:
    URI                 m_descriptionUri;
    std::string         m_description;
//     istringstream       m_descriptionStream;
//     int                 m_descriptionLength;
    URI                 m_controlUri;
    URI                 m_eventUri;
    std::string         m_vendorDomain;
    std::string         m_serviceType;
    std::string         m_serviceVersion;
    Device*             m_device;
};

// TODO: possible request handler types:
//       RequestNotFoundRequestHandler
//       FileRequestHandler, MultiFileRequestHandler,
//       ActionResponseRequestHandler, StateVariableQueryRequestHandler,
//       EventSubscribeRequestHandler


class UpnpRequestHandler: public HTTPRequestHandler
{
public:
    virtual UpnpRequestHandler* create() = 0;
};


class RequestNotFoundRequestHandler: public UpnpRequestHandler
{
public:
    RequestNotFoundRequestHandler* create()
    {
        return new RequestNotFoundRequestHandler();
    }
    
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        std::cerr << "handle unknown request with HTTP 404 - not found error on request: " << request.getURI() << std::endl;
        response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    }
};


class DescriptionRequestHandler: public UpnpRequestHandler
	/// Return service or device description.
{
public:
    
    DescriptionRequestHandler(std::string& description):
        m_description(description)
    {
    }
    
    DescriptionRequestHandler* create()
    {
        // TODO: can we somehow avoid to make a copy of the RequestHandler on each request?
        return new DescriptionRequestHandler(m_description);
    }
    
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        std::cerr << "handle request: " << request.getURI() << std::endl;
        std::cerr << "sending contents of length: " << m_description.size() << std::endl;
        response.setContentLength(m_description.size());
        response.setContentType("text/xml");
        std::ostream& ostr = response.send();
        ostr << m_description;
    }
    
private:
    // TODO: only store a copy of the device/service description in Device/Service, not
    // in the RequestHandler. This segfaults, that's why a copy is stored here, for now ...
//     std::string&        m_description;
    std::string        m_description;
//     std::istream&       m_descriptionStream;
//     int                 m_length;
};


class Action : public Notification
{
};


class ActionResponse : public Notification
{
};


class ControlRequestHandler: public UpnpRequestHandler
	/// Return answer to an action request
{
public:
    
    ControlRequestHandler()
    {
    }
    
    ControlRequestHandler* create()
    {
        return new ControlRequestHandler();
    }
    
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        std::cerr << "handle request: " << request.getURI() << std::endl;
        // synchronous action handling: wait until handleAction() has finished. This must be done in under 30 sec,
        // otherwise it should return and an event should be sent on finishing the action request.
//         handleAction(new Action());
    }
    
    // TODO: send out a notification with Action as argument instead of implementing a callback
    virtual void handleAction(Action* action)
    {
    }
    
private:
};


class DeviceRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    DeviceRequestHandlerFactory()
    {
        registerRequestHandler(std::string(""), new RequestNotFoundRequestHandler());
    }
    
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
        std::cerr << "dispatch HTTP request: " << request.getURI() << std::endl;
        // TODO: if no request is registered for the query, return a default request
        // handler with error 404 or similar ...
        HTTPRequestHandler* res;
        std::map<std::string,UpnpRequestHandler*>::iterator i = m_requestHandlerMap.find(request.getURI());
        if (i != m_requestHandlerMap.end()) {
            return i->second->create();
        }
        else {
            return m_requestHandlerMap[""]->create();
        }
    }
    
    void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler)
    {
        std::cerr << "register request handler: " << Uri << std::endl;
        m_requestHandlerMap[Uri] = requestHandler;
    }
    
private:
    std::map<std::string,UpnpRequestHandler*> m_requestHandlerMap;
};


class HttpSocket
{
public:
    HttpSocket(NetworkInterface interface)
    {
        std::cerr << "HttpSocket()" << std::endl;
        
        m_pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory();
        
        // TODO: offer device description via HTTP, TCP/IP for download to controllers
        HTTPServerParams* pParams = new HTTPServerParams;
    //     pParams->setMaxQueued(maxQueued);
    //     pParams->setMaxThreads(maxThreads);
        // set-up a server socket on an available port
        ServerSocket socket(0);
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out
    //     socket.bind(m_ssdpSocket.m_interface.address());
        m_httpServerAddress = SocketAddress(interface.address(), socket.address().port());
        // set-up a HTTPServer instance
        // TODO: pass a map of all URI (devices and services) -> xml stream
        // for URI use the device/service name
        m_pHttpServer = new HTTPServer(m_pDeviceRequestHandlerFactory, socket, pParams);
        // start the HTTPServer
        m_pHttpServer->start();
        std::cerr << "started HTTP server on: " << m_httpServerAddress.toString() << std::endl;
    }
    
    ~HttpSocket()
    {
        m_pHttpServer->stop();
        delete m_pHttpServer;
    }
    
    
    SocketAddress                   m_httpServerAddress;
    DeviceRequestHandlerFactory*    m_pDeviceRequestHandlerFactory;
    
private:
    NotificationCenter              m_notificationCenter;
    
    HTTPServer*                     m_pHttpServer;
};


class Device {
public:
    Device();
    Device(DeviceRoot* rootDevice, NodeIterator rootNode);
    ~Device();
    
// protected:
    UUID                                m_uuid;
    std::string                         m_uuidDescription;
    std::string                         m_vendorDomain;
    std::string                         m_deviceType;
    std::string                         m_deviceVersion;
    std::vector<Service>                m_services;
    std::map<std::string,std::string>   m_deviceInfo;
    DeviceRoot*                         m_deviceRoot;
private:
    
};


class DeviceRoot {
public:
    DeviceRoot(std::string& description);
    ~DeviceRoot();
    
    void sendMessage(SsdpMessage& message, const SocketAddress& receiver = SocketAddress(SSDP_FULL_ADDRESS));
    void handleSsdpMessage(SsdpMessage* pNf);
    
    URI                             m_descriptionUri;  // for controller to download description
    SsdpSocket                      m_ssdpSocket;
    HttpSocket                      m_httpSocket;
    
private:
    
    Device                          m_rootDevice;
    std::vector<Device>             m_embeddedDevices;
    
    DescriptionRequestHandler*      m_descriptionRequestHandler;
    istringstream                   m_descriptionStream;
};


class Controller {
public:
    Controller();
    ~Controller();

private:
    void handleSsdpMessage(SsdpMessage* pNf);
    
    SsdpSocket m_ssdpSocket;
};

} // namespace Jamm

#endif
