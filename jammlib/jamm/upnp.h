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
#include "Poco/Random.h"
#include "Poco/Timer.h"
#include <Poco/Mutex.h>
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
using Poco::Random;
using Poco::Timer;
using Poco::TimerCallback;
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


// TODO: Argument stuff right done
// TODO: StateVariable stuff right done
// TODO: Event messages
// TODO: Stubs generator
// TODO: Controller stuff
// TODO: M-SEARCH
// TODO: Variable query
// TODO: Error handling
// TODO: Presentation
// TODO: Service and Device version checking

namespace Jamm {

/// SSDP - Simple Service Discovery Protocol
/// HTTP dialect transmitted over UDP Multicast
/// See "UPnP Device Architecture" for details

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    JAMM_VERSION        = "Jamm/0.0.3";
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
    
    void setRequestMethod(TRequestMethod requestMethod);
    TRequestMethod getRequestMethod();
    
    // HTTP message envelope
    std::string toString();
    
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


// class SsdpMessage : UpnpMessage
// {
// };


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


class SsdpMessageSet
{
public:
    SsdpMessageSet();
    ~SsdpMessageSet();
    
    void addMessage(SsdpMessage& message);
    void send(SsdpSocket& socket, int repeat, long delay, bool continuous);
    void stop();
    
private:
    void onTimer(Timer& timer);
    
    Random                              m_randomTimeGenerator;
    Timer                               m_sendTimer;
    SsdpSocket*                         m_socket;
    std::vector<SsdpMessage*>           m_ssdpMessages;
    int                                 m_repeat;
    long                                m_delay;
    bool                                m_continuous;
//     Poco::FastMutex                     m_sendLock;
};


class Variant : public DynamicAny
{
public:
    Variant() : DynamicAny() {}
    Variant(const std::string& val) : DynamicAny(val.c_str()) {}
    template<typename T> Variant(const T& val) : DynamicAny(val) {}
    
    template<typename T> T getValue() { return convert<T>(); }
    template<typename T> void setValue(const T& val) { std::cerr << "Variant::setValue() val: " << val << std::endl; *this = val; std::cerr << "Variant::setValue() finished" << std::endl; }
};


template<class E>
class Container /*: public Entity*/
{
public:
    E& get(std::string key) { return *m_pEntities.find(key)->second; }
    void append(std::string key, E* pEntity) { m_pEntities[key] = pEntity; m_keys.push_back(key); }

//     std::string getValue(const std::string& key) { return m_pEntities[key]->getValue(); }
    template<typename T> T getValue(const std::string& key) { return m_pEntities[key]->convert<T>(); }
    
//     void setValue(const std::string& key, const std::string& val) { m_pEntities[key]->setValue(val); }
    template<typename T> void setValue(const std::string& key, const T& val) { std::cerr << "Container::setValue() key: " << key << "val: " << val << std::endl; *m_pEntities[key] = val; }
    
    
    
    // this gave me lot of headaches. See also:
    // http://stackoverflow.com/questions/1832704/default-assignment-operator-in-inner-class-with-reference-members
//     
    // should be implemented with standard STL iterator stuff ...?
    class Iterator {
    public:
        Iterator(Container& C, std::vector<std::string>::iterator i) : c(&C), pos(i) { }
//         void operator = (const Iterator& val) { c = val.c; pos = val.pos; }
        inline bool operator == (const Iterator& val) { return pos == val.pos; }
        inline bool operator != (const Iterator& val) { return pos != val.pos; }
        inline Iterator& operator ++ () { ++pos; return *this; }
//         inline E& operator * () { return *c->m_pEntities[*pos]; }
        inline E& operator * () { return *c->m_pEntities.find(*pos)->second; }
//         inline E& operator * () const { return *c->m_pEntities.find(*pos)->second; }
        
    protected:
        Container* c;
        std::vector<std::string>::iterator pos;
    };
    
    inline Iterator begin() { return Iterator(*this, m_keys.begin()); }
    inline Iterator end() { return Iterator(*this, m_keys.end()); }
//     inline Iterator begin() const { return Iterator(*this, m_keys.begin()); }
//     inline Iterator end() const { return Iterator(*this, m_keys.end()); }
    
//     class ConstIterator /*: public Iterator*/ {
//     public:
//         ConstIterator(Container& C, std::vector<std::string>::const_iterator i) : c(&C), pos(i) { }
//         inline bool operator == (const ConstIterator& val) { return pos == val.pos; }
//         inline bool operator != (const ConstIterator& val) { return pos != val.pos; }
//         inline ConstIterator& operator ++ () { ++pos; return *this; }
//         inline E& operator * () const { return *c->m_pEntities.find(*pos)->second; }
//         
//     private:
//         const Container* c;
//         std::vector<std::string>::const_iterator pos;
//     };
//     
//     inline ConstIterator constBegin() { return ConstIterator(*this, m_keys.begin()); }
//     inline ConstIterator constEnd() { return ConstIterator(*this, m_keys.end()); }
    
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
    ~DescriptionReader();
    
    virtual DeviceRoot* deviceRoot();
    
private:
    std::string getDescription(std::string path);
    void releaseDescriptionDocument();
    
    virtual Device* device();
    virtual Service* service();
    virtual Action* action();
    virtual Argument* argument();
    virtual StateVar* stateVar();
    
    URI                     m_uri;
    std::string             m_deviceDescriptionPath;
//     AutoPtr<Document>       m_pDoc;
    std::stack<Node*>       m_nodeStack;
    std::stack<Document*>   m_pDocStack;
    DeviceRoot*             m_pDeviceRoot;
};


class ActionRequestReader : public ReaderFactory
{
public:
    ActionRequestReader(const std::string& requestBody);
    
    virtual Action* action();
    
private:
    std::stack<Node*>       m_nodeStack;
    AutoPtr<Document>       m_pDoc;
};


/// Writer Factory
class WriterFactory
{
public:
    virtual void deviceRoot(const DeviceRoot& pDeviceRoot) {}
    virtual void device(const Device& pDevice) {}
    virtual void service(const Service& pService) {}
    virtual void action(const Action& pAction) {}
    virtual void argument(const Argument& pArgument) {}
    virtual void stateVar(const StateVar& pStateVar) {}
};


class SsdpNotifyAliveWriter : public WriterFactory
{
public:
    SsdpNotifyAliveWriter(SsdpMessageSet& generatedMessages) : m_res(&generatedMessages) {}
//     ~SsdpNotifyAliveWriter;
    
    virtual void deviceRoot(const DeviceRoot& pDeviceRoot);
    virtual void device(const Device& pDevice);
    virtual void service(const Service& pService);

private:
    SsdpMessageSet*            m_res;
};


class SsdpNotifyByebyeWriter : public WriterFactory
{
public:
    SsdpNotifyByebyeWriter(SsdpMessageSet& generatedMessages) : m_res(&generatedMessages) {}
    
    virtual void deviceRoot(const DeviceRoot& pDeviceRoot);
    virtual void device(const Device& pDevice);
    virtual void service(const Service& pService);
    
private:
    SsdpMessageSet*            m_res;
};


class ActionWriter : public WriterFactory
{
public:
    virtual void argument(const Argument& argument);
};


class ActionResponseWriter : public ActionWriter
{
public:
    ActionResponseWriter(std::string& responseBody);
    // TODO: couldn't cope with the const argument stuff here ...
    virtual void action(Action& action);
private:
    std::string*    m_responseBody;
};


/// Writer Factory for Controllers
class ActionRequestWriter : public ActionWriter
{
public:
    virtual void action(const Action& action);
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


class EventRequestHandler: public UpnpRequestHandler
	/// Return answer to an action request
{
public:
    EventRequestHandler(HttpSocket* pHttpSocket);
    
    EventRequestHandler* create();
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
    
private:
    HttpSocket* m_pHttpSocket;
};


class StateVar : public /*Variant*/ DynamicAny
{
public:
    StateVar() : DynamicAny() {}
    StateVar(const std::string& val) : DynamicAny(val.c_str()) {}
//     template<typename T> StateVar(const T& val) : DynamicAny(val) {}
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


class Argument : public /*Variant*/ DynamicAny
{
public:
    Argument() : DynamicAny() {}
    Argument(const std::string& val) : DynamicAny(val.c_str()) {}
//     template<typename T> Argument(const T& val) : DynamicAny(val) {}
    
    std::string getName() { return m_name; }
    std::string getDirection() { return m_in ? "in" : "out"; }
    std::string getRelatedStateVar() { return m_relatedStateVar; }
    
    void setName(std::string name) { m_name = name; }
    void setDirection(std::string direction) { m_in = (direction == "in") ? true : false; }
    void setRelatedStateVar(std::string relatedStateVar) { m_relatedStateVar = relatedStateVar; }
    
private:
    std::string     m_name;
    std::string     m_relatedStateVar;
    bool            m_in;
};


class Action : public Notification/*, public Entity*/
{
public:
    Action() {}
    
    typedef Container<Argument>::Iterator InArgumentIterator;
    InArgumentIterator beginInArgument() { return m_inArguments.begin(); }
    InArgumentIterator endInArgument() { return m_inArguments.end(); }
    
    typedef Container<Argument>::Iterator OutArgumentIterator;
    OutArgumentIterator beginOutArgument() { return m_outArguments.begin(); }
    OutArgumentIterator endOutArgument() { return m_outArguments.end(); }
    
    std::string getName() const { return m_actionName; }
    std::string getServiceType() const { return m_serviceType; }
//     std::string getArgument (const std::string& name);
//     template<typename T> const T& getArgument(const std::string& key) { return m_arguments.get(key).convert<T>(); }
    template<typename T> const T& getArgument(const std::string& name) { return m_arguments.getValue<T>(name); }
    
    void setName(std::string name) { m_actionName = name; }
    void setServiceType(std::string serviceType) { m_serviceType = serviceType; }
    void setArgument(std::string name, std::string val);
    template<typename T> void setArgument(std::string name, const T& val) { m_arguments.setValue(name, val); }
    
    void appendArgument(Argument* pArgument);

private:
    std::string                         m_actionName;
    std::string                         m_serviceType;
    
    MessageHeader                       m_messageHeader;
    
    Container<Argument>                 m_arguments;
    Container<Argument>                 m_inArguments;
    Container<Argument>                 m_outArguments;
    
    SocketAddress                       m_sender;
    SocketAddress                       m_receiver;
    std::string                         m_responseBody;
};


class Service {
public:
    Service() {}
    ~Service();
    
    std::string getServiceType() const { return m_serviceType; }
    std::string getDescriptionPath() const { return m_descriptionPath; }
    std::string getControlPath() const { return m_controlPath; }
    std::string getEventPath() const { return m_eventPath; }
    DescriptionRequestHandler* getDescriptionRequestHandler() const { return m_pDescriptionRequestHandler; }
    ControlRequestHandler* getControlRequestHandler() const { return m_controlRequestHandler; }
    Device* getDevice() const { return m_pDevice; }
//     std::string getStateVar(const std::string& name) /*const*/;
//     template<typename T> T getStateVar(const std::string& key) { return m_stateVars.get(key).convert<T>(); }
    template<typename T> T getStateVar(const std::string& key) { return m_stateVars.getValue<T>(key); }
    
    void setServiceType(std::string serviceType) { m_serviceType = serviceType; }
    void setDescriptionPath(std::string descriptionPath) { m_descriptionPath = descriptionPath; }
    void setDescription(std::string description) { m_description = description; }
    void setDescriptionRequestHandler() { m_pDescriptionRequestHandler = new DescriptionRequestHandler(m_description); }
    void setControlPath(std::string controlPath) { m_controlPath = controlPath; }
    void setEventPath(std::string eventPath) { m_eventPath = eventPath; }
    void setDevice(Device* pDevice) { m_pDevice = pDevice; }
//     void setStateVar(std::string name, std::string val);
    template<typename T> void setStateVar(std::string key, const T& val) { m_stateVars.setValue(key, val); }
    
    void addAction(Action* pAction) { m_actions.append(pAction->getName(), pAction); }
    void addStateVar(StateVar* pStateVar) { m_stateVars.append(pStateVar->getName(), pStateVar); }
    
private:
    Device*                                 m_pDevice;
    std::string                             m_vendorDomain;
    std::string                             m_serviceType;
    std::string                             m_serviceVersion;
    std::string                             m_descriptionPath;
    std::string                             m_description;
    DescriptionRequestHandler*              m_pDescriptionRequestHandler;
    std::string                             m_controlPath;
    ControlRequestHandler*                  m_controlRequestHandler;
    std::string                             m_eventPath;
    Container<Action>                       m_actions;
    Container<StateVar>                     m_stateVars;
};


class Device /*: Entity*/
{
public:
    Device();
    ~Device();
    
    typedef Container<Service>::Iterator ServiceIterator;
    ServiceIterator beginService() { return m_services.begin(); }
    ServiceIterator endService() { return m_services.end(); }
    
    DeviceRoot* getDeviceRoot() const { return m_pDeviceRoot; }
    std::string getUuid() const { return m_uuid; }
    std::string getDeviceType() const { return m_deviceType; }
    Service& getService(std::string serviceType) { return m_services.get(serviceType); }
    
    void setDeviceRoot(DeviceRoot* pDeviceRoot) { m_pDeviceRoot = pDeviceRoot; }
    void setUuid(std::string uuid) { m_uuid = uuid; }
    void setDeviceType(std::string deviceType) { m_deviceType = deviceType; }
    
    void addService(Service* pService) { m_services.append(pService->getServiceType(), pService); }
    
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
    
    typedef Container<Device>::Iterator DeviceIterator;
    DeviceIterator beginDevice() { return m_devices.begin(); }
    DeviceIterator endDevice() { return m_devices.end(); }
    
    /*const*/ Device& getDevice(std::string uuid) /*const*/ { return m_devices.get(uuid); }
    Device* getRootDevice() const { return m_pRootDevice; }
    std::string& getDeviceDescription() { return m_deviceDescription; }
    const URI& getDescriptionUri() const { return m_descriptionUri; }
    
    void setRootDevice(Device* pDevice) { m_pRootDevice = pDevice; }
    void setDeviceDescription(std::string description) { m_deviceDescription = description; }
    void setDescriptionUri(std::string descriptionPath) { m_descriptionUri = URI(m_httpSocket.getServerUri() + descriptionPath); }
    
    void addDevice(Device* pDevice) { m_devices.append(pDevice->getUuid(), pDevice); }
    
    void init();
    void startSsdp();
    void stopSsdp();
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
    SsdpMessageSet                  m_ssdpNotifyAliveMessages;
    SsdpMessageSet                  m_ssdpNotifyByebyeMessages;
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
