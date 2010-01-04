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
#include <stack>
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
#include "Poco/Path.h"
#include "Poco/DynamicAny.h"
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
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/AttrMap.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Attr.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/DOM/DocumentFragment.h"
#include "Poco/XML/XMLWriter.h"
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
using Poco::DynamicAny;
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
using Poco::XML::Element;
using Poco::XML::Attr;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;
using Poco::XML::DocumentFragment;

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


class Device;
class Service;
class Action;
class Argument;
class StateVar;
class DeviceRoot;
class ControlRequestHandler;
class HttpSocket;
class Entity;
class EntityItem;


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


/// EntityItem is like a variant.
/// it stores data and its type in strings and provides conversion methods.
/// it is an abstract type can't be instantiated directly.
class EntityItem /*: public Entity*/
{
public:
    virtual std::string id() { return m_name; }
    
    void setName(std::string name) { m_name = name; }
    std::string getName() { return m_name; }
    
    void setType(std::string type) { m_type = type; }
    std::string getType() { return m_type; }
    
    inline void setVal(const std::string& val) { m_val = val; }
    void setBool(bool val);
    
    inline std::string getVal() { return m_val; }
    bool getBool();
    
private:
    std::string m_name;
    std::string m_type;
    std::string m_val;
};


class Variant : public DynamicAny
{
public:
    Variant() : DynamicAny() {}
    Variant(const std::string& val) : DynamicAny(val.c_str()) {}
    template<typename T> Variant(const T& val) : DynamicAny(val) {}
};


template<class E>
class Container /*: public Entity*/
{
public:
    void append(std::string key, E* pEntity) { m_pEntities[key] = pEntity; m_keys.push_back(key); }
    void set(std::string key, std::string val) { m_pEntities[key] = new E(val); }
    template<typename T> void set(std::string key, const T& val) { m_pEntities[key] = new E(val); }
    E* get(std::string key) { return m_pEntities[key]; }
//     template<typename T> const T& get(std::string key) { return m_pEntities[key]->convert<T>(); }
//     std::string get(std::string key) { return m_pEntities[key]->convert<std::string>(); }
    
    typedef typename std::vector<std::string>::iterator Iterator;
    std::vector<std::string>::iterator begin() { return m_keys.begin(); }
    std::vector<std::string>::iterator end() { return m_keys.end(); }
    
private:
    std::map<std::string,E*>    m_pEntities;
    std::vector<std::string>    m_keys;
};


/// Reader Factory
class ReaderFactory
{
public:
    virtual DeviceRoot* deviceRoot() { return NULL; }
    virtual Device* device() { return NULL; }
    virtual Service* service() { return NULL; }
    virtual Action* action() { return NULL; }
    virtual Argument* argument() { return NULL; }
    virtual StateVar* stateVar() { return NULL; }
};


/// Reader Factory that reads in Xml Device and Service Descriptions
/// and generates the Entities for a complete DeviceRoot
class DescriptionReader : public ReaderFactory
{
public:
    DescriptionReader(URI uri, std::string deviceDescriptionPath);
    
    virtual DeviceRoot* deviceRoot();
    
private:
    std::string getDescription(std::string path);
    
    virtual Device* device();
    virtual Service* service();
    virtual Action* action();
    virtual Argument* argument();
    virtual StateVar* stateVar();
    
    URI                     m_uri;
    std::string             m_deviceDescriptionPath;
//     AutoPtr<Document>       m_pDoc;
    std::stack<Node*>       m_nodeStack;
    DeviceRoot*             m_pDeviceRoot;
};


class ActionRequestReader : public ReaderFactory
{
public:
    ActionRequestReader(const std::string& requestBody);
    
    virtual Action* action();
    
private:
//     virtual Argument* argument();
    
    std::stack<Node*>       m_nodeStack;
};


/// Writer Factory
class WriterFactory
{
    virtual void device(Device* pDevice) {}
    virtual void service(Service* pService) {}
    virtual void action(Action* pAction) {}
    virtual void argument(Argument* pArgument) {}
    virtual void stateVar(StateVar* pStateVar) {}
};


class ActionWriter : WriterFactory
{
    virtual void argument(Argument* pArgument);
};


class ActionResponseWriter : public ActionWriter
{
    virtual void action(Action* pAction);
};


/// Writer Factory for Controllers
class ActionRequestWriter : public ActionWriter
{
    virtual void action(Action* pAction);
};




// TODO: possible request handler types:
//       RequestNotFoundRequestHandler
//       FileRequestHandler, MultiFileRequestHandler,
//       DescriptionRequestHandler
//       ControlRequestHandler, StateVariableQueryRequestHandler,
//       EventSubscribeRequestHandler

class UpnpRequestHandler: public HTTPRequestHandler
{
public:
    virtual UpnpRequestHandler* create() = 0;
};


class RequestNotFoundRequestHandler: public UpnpRequestHandler
{
public:
    RequestNotFoundRequestHandler* create();
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
};


class DescriptionRequestHandler: public UpnpRequestHandler
	/// Return service or device description.
{
public:
    DescriptionRequestHandler(std::string& description);

    DescriptionRequestHandler* create();
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
    
private:
    // TODO: only store a copy of the device/service description in Device/Service, not
    // in the RequestHandler. This segfaults, that's why a copy is stored here, for now ...
//     std::string&        m_description;
    std::string        m_description;
//     std::istream&       m_descriptionStream;
//     int                 m_length;
};


class DeviceRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    DeviceRequestHandlerFactory(HttpSocket* pHttpSocket);
    
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
    void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler);
    
private:
    std::map<std::string,UpnpRequestHandler*> m_requestHandlerMap;
    HttpSocket*                               m_pHttpSocket;
};


class HttpSocket
{
public:
    HttpSocket(NetworkInterface interface);
    ~HttpSocket();
    
    std::string getServerUri() { return "http://" + m_httpServerAddress.toString() + "/"; }
    
    SocketAddress                   m_httpServerAddress;
    DeviceRequestHandlerFactory*    m_pDeviceRequestHandlerFactory;
    NotificationCenter              m_notificationCenter;
    
private:
    HTTPServer*                     m_pHttpServer;
};


class VariableQuery : public Notification
{
};


class ControlRequestHandler: public UpnpRequestHandler
	/// Return answer to an action request
{
public:
    ControlRequestHandler(HttpSocket* pHttpSocket);
    
    ControlRequestHandler* create();
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
    
private:
    HttpSocket* m_pHttpSocket;
};


class StateVar : public Variant
{
public:
    StateVar() : Variant() {}
    StateVar(const std::string& val) : Variant(val) {}
    template<typename T> StateVar(const T& val) : Variant(val) {}
//     template<typename T> const T& get(std::string key) { return m_stateVars.get(key); }
    
    std::string getName() { return m_name; }
    void setName(std::string name) { m_name = name; }
    void setType(std::string type) { m_type = type; }
    void setDefaultValue(std::string defaultValue) { m_defaultValue = defaultValue; }
    void setSendEvents(std::string sendEvents) { m_sendEvents = (sendEvents=="yes") ? true : false; }
    
private:
    std::string     m_name;
    std::string     m_type;
    std::string     m_defaultValue;
    bool            m_sendEvents;
};


class Argument : public Variant
{
public:
    Argument() : Variant() {}
    Argument(const std::string& val) : Variant(val) {}
    template<typename T> Argument(const T& val) : Variant(val) {}
    
    std::string getName() { return m_name; }
    void setName(std::string name) { m_name = name; }
    void setRelatedStateVar(std::string relatedStateVar) { m_relatedStateVar = relatedStateVar; }
    void setDirection(std::string direction) { m_in = (direction == "in") ? true : false; }
    
private:
    std::string     m_name;
    std::string     m_relatedStateVar;
    bool            m_in;
};


class Action : public Notification/*, public Entity*/
{
public:
    Action() {}
    
    void setName(std::string name) { m_actionName = name; }
    std::string getName() { return m_actionName; }
    
    void setServiceType(std::string serviceType) { m_serviceType = serviceType; }
    std::string getServiceType() { return m_serviceType; }
    
    void addArgument(Argument* pArgument) { m_arguments.append(pArgument->getName(), pArgument); }
    void setArgument(std::string name, std::string val);
    std::string getArgument(std::string name);
    template<typename T> void setArgument(std::string key, const T& val) { m_arguments.set(key, val); }
    template<typename T> const T& getArgument(std::string key) { return m_arguments.get(key)->convert<T>(); }
    
    
// obsolete
    int responseSize();
    std::string responseBody();
    
private:
    std::string                         m_actionName;
    std::string                         m_serviceType;
    
    MessageHeader                       m_messageHeader;
//     SoapMessage                         m_message;
    // TODO: save some space and store the argument values only once
    //       this information is also stored in the Service Description
    std::map<std::string,std::string>   m_argumentValues;
    
    Container<Argument>                 m_arguments;
    std::vector<std::string>            m_inArgumentNames;
    std::vector<std::string>            m_outArgumentNames;
    
    SocketAddress                       m_sender;
    SocketAddress                       m_receiver;
    std::string                         m_responseBody;
};


class Service {
public:
    Service() {}
    ~Service();
    
    void setServiceType(std::string serviceType) { m_serviceType = serviceType; }
    std::string getServiceType() { return m_serviceType; }
    void setDescriptionPath(std::string descriptionPath) { m_descriptionPath = descriptionPath; }
    std::string getDescriptionPath() { return m_descriptionPath; }
    void setDescription(std::string description) { m_description = description; }
    void setDevice(Device* pDevice) { m_pDevice = pDevice; }
    void addAction(Action* pAction) { m_actions.append(pAction->getName(), pAction); }
    void addStateVar(StateVar* pStateVar) { m_stateVars.append(pStateVar->getName(), pStateVar); }
    
    void setStateVar(std::string name, std::string val);
    std::string getStateVar(std::string name);
    template<typename T> void setStateVar(std::string key, const T& val) { m_stateVars.set(key, val); }
    template<typename T> const T& getStateVar(std::string key) { return m_stateVars.get(key)->convert<T>(); }
    
    void ssdpNotifyAlive();
    
private:
    std::string                             m_descriptionPath;
    URI                                     m_descriptionUri;
    std::string                             m_description;
    Device*                                 m_pDevice;
    URI                                     m_controlUri;
    ControlRequestHandler*                  m_controlRequestHandler;
    URI                                     m_eventUri;
    std::string                             m_vendorDomain;
    std::string                             m_serviceType;
    std::string                             m_serviceVersion;
    Container<Action>                       m_actions;
    Container<StateVar>                     m_stateVars;
};


class Device /*: Entity*/
{
public:
    Device();
    ~Device();
    
    void setDeviceRoot(DeviceRoot* pDeviceRoot) { m_pDeviceRoot = pDeviceRoot; }
    DeviceRoot* getDeviceRoot() { return m_pDeviceRoot; }
    std::string getUuid() { return m_uuid; }
    void setUuid(std::string uuid) { m_uuid = uuid; }
    void setDeviceType(std::string deviceType) { m_deviceType = deviceType; }
    void addService(Service* pService) { m_services.append(pService->getServiceType(), pService); }
    Service* getService(std::string serviceType) { return m_services.get(serviceType); }
    
    void ssdpNotifyAlive();
    
private:
    DeviceRoot*                         m_pDeviceRoot;
//     UUID                                m_uuid;
    std::string                         m_uuid;
    std::string                         m_vendorDomain;
    std::string                         m_deviceType;
    std::string                         m_deviceVersion;
    Container<Service>                  m_services;
};


class DeviceRoot /* : Entity */
{
public:
    DeviceRoot();
    ~DeviceRoot();
    
    void addDevice(Device* pDevice) { m_devices.append(pDevice->getUuid(), pDevice); }
    Device* getDevice(std::string uuid) { return m_devices.get(uuid); }
    void setRootDevice(Device* pDevice) { m_pRootDevice = pDevice; }
    void setDeviceDescription(std::string description) { m_deviceDescription = description; }
    std::string getDeviceDescription() { return m_deviceDescription; }
    URI getDescriptionUri() { return m_descriptionUri; }
    void setDescriptionUri(std::string descriptionPath) { m_descriptionUri = URI(m_httpSocket.getServerUri() + descriptionPath); }
    
    void ssdpNotifyAlive();
    void startSsdp();
    void startHttp();
    
    void registerActionHandler(const AbstractObserver& observer)
    { m_httpSocket.m_notificationCenter.addObserver(observer); }
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler) 
    { m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler(path, requestHandler); }
    
    void sendMessage(SsdpMessage& message, const SocketAddress& receiver = SocketAddress(SSDP_FULL_ADDRESS));
    void handleSsdpMessage(SsdpMessage* pNf);
    
private:
    URI                             m_descriptionUri;  // for controller to download description
    std::string                     m_deviceDescription;
    Container<Device>               m_devices;
    Device*                         m_pRootDevice;
    SsdpSocket                      m_ssdpSocket;
    HttpSocket                      m_httpSocket;
    DescriptionRequestHandler*      m_descriptionRequestHandler;
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
