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

#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/StreamCopier.h>
#include <Poco/NObserver.h>
#include <Poco/Observer.h>
#include <Poco/Thread.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <Poco/NotificationCenter.h>
#include <Poco/URI.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/Path.h>
#include <Poco/Random.h>
#include <Poco/Timer.h>
#include <Poco/Mutex.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/MessageHeader.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>


// TODO: Variant: catch conversion errors with log message
// NOTE: Embedded Devices:
// -> HTTP server for controlling and eventing should move from DeviceRoot into Device
// -> only SSDP stuff should be in DeviceRoot
// -> this way ControlURL of same Service can be the same for all embedded devices
// -> albeit: BaseURL is device description (and thus DeviceRoot) related
// -> so nope: ControlURLs of embedded devices of same type must differ, as they are relative to BaseURL
// TODO: BaseURL is LOCATION in rootdevice SSDP message !!! All other URLs refer to this
// TODO: Find a proper interface for the readers and writers
// TODO: complete event messaging


// TODO: Error handling
// TODO: Variable query (only for Device)
// TODO: Service and Device version checking
// TODO: Presentation

namespace Jamm {

// TODO: make a typedef and extent Variant for remaining UPnP datatypes (see specs p.33)
typedef Poco::UInt8     ui1;
typedef Poco::UInt16    ui2;
typedef Poco::UInt32    ui4;
typedef Poco::Int8      i1;
typedef Poco::Int16     i2;
typedef Poco::Int32     i4;
typedef float           r4;
typedef double          r8;
typedef r8              number;
typedef Poco::URI       uri;

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    JAMM_VERSION        = "0.0.3";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";
static const std::string    SSDP_ADDRESS        = "239.255.255.250";
static const Poco::UInt16   SSDP_PORT           = 1900;
static const Poco::UInt16   SSDP_CACHE_DURATION = 1800;
static const Poco::UInt16   SSDP_MIN_WAIT_TIME  = 1;
static const Poco::UInt16   SSDP_MAX_WAIT_TIME  = 120;

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


class SsdpMessage : public Poco::Notification
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
    SsdpMessage(const std::string& buf, const Poco::Net::SocketAddress& sender = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
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
    
    void setLocation(const Poco::URI& location);
//     std::string getLocation();
    Poco::URI getLocation();
    
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
    Poco::DateTime getDate();
    
    Poco::Net::SocketAddress getSender();
    
private:
    void initMessageMap();
    
    TRequestMethod                      m_requestMethod;
    TRequestMethod                      m_notificationSubtype;
    std::map<std::string,std::string>   m_messageHeader;
    Poco::Net::SocketAddress                 m_sender;
    
    std::map<TRequestMethod,std::string> m_messageMap;
    std::map<std::string,TRequestMethod> m_messageConstMap;
};


class SsdpSocket
{ 
public:
    SsdpSocket(const Poco::Net::NetworkInterface& interface);
    ~SsdpSocket();
    
    const Poco::Net::NetworkInterface& getInterface() { return m_interface; }
    void setObserver(const Poco::AbstractObserver& observer);
    void init();
    
    void sendMessage(SsdpMessage& message, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    
private:
    void onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
    void onUnicastReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
    
    Poco::Net::NetworkInterface     m_interface;
    Poco::Net::MulticastSocket*     m_pSsdpSocket;
    Poco::Net::MulticastSocket*     m_pSsdpSenderSocket;
    Poco::Net::SocketReactor        m_reactor;
    Poco::Net::SocketReactor        m_unicastReactor;
    Poco::Thread                    m_listenerThread;
    Poco::Thread                    m_unicastListenerThread;
    Poco::NotificationCenter        m_notificationCenter;
    char*                           m_pBuffer;
    
    enum {
        BUFFER_SIZE = 65536 // Max UDP Packet size is 64 Kbyte.
            // Note that each SSDP message must fit into one UDP Packet.
    };
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
    void onTimer(Poco::Timer& timer);
    
    Poco::Random                        m_randomTimeGenerator;
    Poco::Timer                         m_sendTimer;
    SsdpSocket*                         m_socket;
    std::vector<SsdpMessage*>           m_ssdpMessages;
    int                                 m_repeat;
    long                                m_delay;
    bool                                m_continuous;
};


class Variant
{
public:
    Variant() : m_val("") {}
    Variant(const std::string& val) : m_val(val) {}
    
    // TODO: check if string formats are conform to specs (-> p. 33).
    void setValue(const std::string& val) {
//         std::clog << "Variant::setValue(const std::string& val), val: " << val << std::endl;
        m_val = val;
    }
    void setValue(bool val) { m_val = val ? "1" : "0"; }
    void setValue(ui1 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(ui2 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(ui4 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(i1 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(i2 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(i4 val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(float val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(double val) { m_val = Poco::NumberFormatter::format(val); }
    void setValue(Poco::URI val) { m_val = val.toString(); }
    
    const std::string& getValue() const {
        return m_val; 
    }
    void getValue(std::string& val) { 
        val = m_val; 
    }
    void getValue(bool& val) { val = (m_val == "1" || m_val == "true") ? true : false; }
    void getValue(ui1& val) { val = Poco::NumberParser::parse(m_val); }
    void getValue(ui2& val) { val = Poco::NumberParser::parse(m_val); }
    void getValue(ui4& val) { 
        if (m_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parseUnsigned(m_val);
        }
    }
    void getValue(i1& val) { 
        if (m_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parse(m_val);
        }
    }
    void getValue(i2& val) { val = Poco::NumberParser::parse(m_val); }
    void getValue(i4& val) {
        if (m_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parse(m_val);
        }
    }
    void getValue(float& val) { val = Poco::NumberParser::parse(m_val); }
    void getValue(double& val) { val = Poco::NumberParser::parse(m_val); }
    void getValue(Poco::URI& val) { val = Poco::URI(m_val); }
    
private:
    std::string     m_val;
};


template<class E>
class Container
{
public:
    typedef typename std::vector<E*>::iterator Iterator;
    
    E& get(std::string key)
    {
        return *m_pEntities.find(key)->second;
    }
    
    E& get(int index)
    {
        return *m_keys[index];
    }
    
    // TODO: make shure that only one entry exists in m_pEntities and m_keys for key
    void append(std::string key, E* pEntity)
    {
        m_pEntities[key] = pEntity;
        m_keys.push_back(pEntity);
    }
    
    void append(E* pEntity)
    {
        m_keys.push_back(pEntity);
    }
    
    void remove(std::string key)
    {   
        E* pEntity = m_pEntities.find(key)->second;
        m_pEntities.erase(key);
        m_keys.erase(find(m_keys.begin(), m_keys.end(), pEntity));
    }
    
    bool contains(const std::string& key)
    {
        return m_pEntities.find(key) != m_pEntities.end();
    }
    
    bool contains(E* pEntity)
    {
        return find(m_keys.begin(), m_keys.end(), pEntity) != m_keys.end();
    }
    
    template<typename T> T getValue(const std::string& key)
    {
        std::clog << "Container::getValue() key: " << key << std::endl;
        Variant* e = (*m_pEntities.find(key)).second;
        T res;
        if (e) {
            e->getValue(res);
        } else {
            std::clog << "Container::getValue() could not find key: " << key << std::endl;
        }
        std::clog << "Container::getValue() key: " << key << ", val: " << e->getValue() << std::endl;
        return res;
    }
    
    template<typename T> void setValue(const std::string& key, const T& val)
    {
        std::clog << "Container::setValue() key: " << key << std::endl;
        std::clog << "m_pEntities has: " << m_pEntities.size() << " entries" << std::endl;
        std::clog << "m_keys has: " << m_keys.size() << " entries" << std::endl;
        // FIXME: segfault here
        std::clog << "first pointer to Variant in m_keys is: " << m_keys[0] << std::endl;
        if (m_pEntities.find(key) == m_pEntities.end()) {
            std::clog << "Container::setValue() could not find key"<< std::endl;
            return;
        }
        std::clog << "Container::setValue() found key" << std::endl;
        Variant* e = (*m_pEntities.find(key)).second;
        std::clog << "Container::setValue() found Variant pointer: " << e << std::endl;
        if (e) {
            std::clog << "Container::setValue() found key: " << key << std::endl;
            e->setValue(val);
        }
        else {
            std::clog << "Container::setValue() pointer to Variant is invalid" << std::endl;
        }
        std::clog << "Container::setValue() key: " << key << ", val: " << e->getValue() << std::endl;
    }
    
    Iterator begin()
    {
        return m_keys.begin();
    }
    
    Iterator end()
    {
        return m_keys.end();
    }
        
private:
    std::map<std::string,E*>    m_pEntities;
    std::vector<E*>             m_keys;
};


class DescriptionReader 
{
public:
    DescriptionReader(std::string deviceDescriptionPath) : m_deviceDescriptionPath(deviceDescriptionPath) {}
    ~DescriptionReader();
    
    DeviceRoot* deviceRoot();
    
protected:
    virtual std::string& getDescription(const std::string& path) = 0;
    
    void releaseDescriptionDocument();
    
    Device* device();
    Service* service();
    Action* action();
    Argument* argument();
    StateVar* stateVar();
    
    std::string             m_deviceDescriptionPath;
    // TODO: replace m_nodeStack by argument Node* in the factory methods
    std::stack<Poco::XML::Node*>        m_nodeStack;
    std::stack<Poco::XML::Document*>    m_pDocStack;
    DeviceRoot*                         m_pDeviceRoot;
};


class UriDescriptionReader : public DescriptionReader
{
public:
    UriDescriptionReader(Poco::URI uri, const std::string& deviceDescriptionPath);
    
private:
    virtual std::string& getDescription(const std::string& path);
    Poco::URI m_uri;
};


class StringDescriptionReader : public DescriptionReader
{
public:
    StringDescriptionReader(std::map<std::string,std::string*>& stringMap, const std::string& deviceDescriptionPath);
    
private:
    virtual std::string& getDescription(const std::string& path);
    std::map<std::string,std::string*>*  m_pStringMap;
};


class ActionRequestReader
{
public:
    ActionRequestReader(const std::string& requestBody, Action* pActionTemplate);
    
    Action* action();
    
private:
    // TODO: replace m_nodeStack by Node* in action()
    std::stack<Poco::XML::Node*>            m_nodeStack;
    Poco::AutoPtr<Poco::XML::Document>      m_pDoc;
    Action*                                 m_pActionTemplate;
};


class ActionResponseReader
{
public:
    ActionResponseReader(const std::string& responseBody, Action* pActionTemplate);
    
    Action* action();
    
private:
    std::stack<Poco::XML::Node*>        m_nodeStack;
    Poco::AutoPtr<Poco::XML::Document>  m_pDoc;
    Action*                             m_pActionTemplate;
};


class DeviceDescriptionWriter
{
public:
    DeviceDescriptionWriter();
    
    void deviceRoot(DeviceRoot& deviceRoot);
    void write(std::string& description);
    
private:
    Poco::XML::Element* device(Device& device);
    Poco::AutoPtr<Poco::XML::Document>   m_pDoc;
};


class ServiceDescriptionWriter
{
public:
    ServiceDescriptionWriter(std::string& description) : m_pDescription(&description), m_pDoc(new Poco::XML::Document) {}
    
    void service(Service& service);
    
private:
    void stateVar(StateVar& stateVar);
    void action(Action& action);
    void argument(Argument& argument);
    
    std::string*                            m_pDescription;
    Poco::AutoPtr<Poco::XML::Document>      m_pDoc;
};


class SsdpNotifyAliveWriter
{
public:
    SsdpNotifyAliveWriter(SsdpMessageSet& generatedMessages) : m_res(&generatedMessages) {}
    
    void deviceRoot(const DeviceRoot& pDeviceRoot);
    void device(const Device& pDevice);
    void service(const Service& pService);

private:
    SsdpMessageSet*            m_res;
};


class SsdpNotifyByebyeWriter
{
public:
    SsdpNotifyByebyeWriter(SsdpMessageSet& generatedMessages) : m_res(&generatedMessages) {}
    
    void deviceRoot(const DeviceRoot& pDeviceRoot);
    void device(const Device& pDevice);
    void service(const Service& pService);
    
private:
    SsdpMessageSet*            m_res;
};


class ActionResponseWriter
{
public:
    ActionResponseWriter(std::string& responseBody);
    // TODO: couldn't cope with the const argument stuff here ...
    void action(Action& action);
private:
    std::string*    m_responseBody;
};


class ActionRequestWriter
{
public:
    void action(Action* action);
    void write(std::string& actionMessage);
    
private:
    Poco::AutoPtr<Poco::XML::Document>   m_pDoc;
};


class EventMessageWriter
{
public:
    EventMessageWriter();
    void write(std::string& eventMessage);
    void stateVar(const StateVar& stateVar);

private:
    Poco::AutoPtr<Poco::XML::Document>   m_pDoc;
    Poco::AutoPtr<Poco::XML::Element>    m_pPropertySet;
};


// TODO: possible request handler types:
//       RequestNotFoundRequestHandler
//       FileRequestHandler, MultiFileRequestHandler,
//       DescriptionRequestHandler
//       ControlRequestHandler, StateVariableQueryRequestHandler,
//       EventSubscribeRequestHandler

class UpnpRequestHandler: public Poco::Net::HTTPRequestHandler
{
public:
    virtual UpnpRequestHandler* create() = 0;
};


class RequestNotFoundRequestHandler: public UpnpRequestHandler
{
public:
    RequestNotFoundRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};


class DescriptionRequestHandler: public UpnpRequestHandler
	/// Return service or device description.
{
public:
    DescriptionRequestHandler(std::string& description);

    DescriptionRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    std::string*    m_pDescription;
};


class DeviceRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
    DeviceRequestHandlerFactory(HttpSocket* pHttpSocket);
    
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
    void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler);
    
private:
    std::map<std::string,UpnpRequestHandler*> m_requestHandlerMap;
    HttpSocket*                               m_pHttpSocket;
};


class HttpSocket
{
    friend class DeviceRoot;
    
public:
    HttpSocket(Poco::Net::NetworkInterface interface);
    ~HttpSocket();
    
    std::string getServerUri() { return "http://" + m_httpServerAddress.toString() + "/"; }
    void init();
    void startServer();
    void stopServer();
    
private:
    Poco::Net::NetworkInterface           m_interface;
    Poco::Net::SocketAddress              m_httpServerAddress;
    DeviceRequestHandlerFactory*          m_pDeviceRequestHandlerFactory;
    Poco::NotificationCenter              m_notificationCenter;
    Poco::Net::HTTPServer*                m_pHttpServer;
};


class VariableQuery : public Poco::Notification
{
};


class ControlRequestHandler: public UpnpRequestHandler
{
public:
//     ControlRequestHandler(DeviceRoot& deviceRoot);
    ControlRequestHandler(Service* service);
    
    ControlRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
//     DeviceRoot* m_deviceRoot;
    Service*    m_pService;
};


class EventRequestHandler: public UpnpRequestHandler
{
public:
    EventRequestHandler(Service* pService) : m_pService(pService) {}
    
    EventRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    Service*    m_pService;
};


class StateVar : public Variant
{
public:
    StateVar() : Variant() {}
    template<typename T> StateVar(const T& val) : Variant(val) {}
    
    std::string getName() const { return m_name; }
    const std::string& getType() const { return m_type; }
    void setName(std::string name) { m_name = name; }
    void setType(std::string type) { m_type = type; }
    void setDefaultValue(std::string defaultValue) { m_defaultValue = defaultValue; }
    void setSendEvents(std::string sendEvents) { m_sendEvents = (sendEvents=="yes") ? true : false; }
    bool getSendEvents() const { return m_sendEvents; }
    
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
    template<typename T> Argument(const T& val) : Variant(val) {}
    
    std::string getName() const { return m_name; }
    std::string getDirection() const { return m_in ? "in" : "out"; }
    const std::string& getRelatedStateVarName() const { return m_relatedStateVar; }
    StateVar* getRelatedStateVarReference() const;
    Action* getAction() const { return m_pAction; }
    
    void setName(std::string name) { m_name = name; }
    void setDirection(std::string direction) { m_in = (direction == "in") ? true : false; }
    void setRelatedStateVar(std::string relatedStateVar) { m_relatedStateVar = relatedStateVar; }
    void setAction(Action* pAction) { m_pAction = pAction; }
    
private:
    std::string     m_name;
    std::string     m_relatedStateVar;
    bool            m_in;
    Action*         m_pAction;
};


class Action : public Poco::Notification
{
public:
    Action() {}
    
    Action* clone();
    
    typedef Container<Argument>::Iterator ArgumentIterator;
    ArgumentIterator beginArgument() { return m_arguments.begin(); }
    ArgumentIterator endArgument() { return m_arguments.end(); }
    ArgumentIterator beginInArgument() { return m_inArguments.begin(); }
    ArgumentIterator endInArgument() { return m_inArguments.end(); }
    ArgumentIterator beginOutArgument() { return m_outArguments.begin(); }
    ArgumentIterator endOutArgument() { return m_outArguments.end(); }
    
    std::string getName() const { return m_actionName; }
    Service* getService() const { return m_pService; }
    template<typename T> const T getArgument(const std::string& name)
    {
        std::clog << "Action::getArgument() name: " << name << std::endl;
        return m_arguments.getValue<T>(name);
    }
    
    void setName(std::string name) { m_actionName = name; }
    void setService(Service* pService) { m_pService = pService; }
    template<typename T> void setArgument(std::string name, const T& val)
    {
        std::clog << "Action::setArgument() name: " << name << std::endl;
        m_arguments.setValue(name, val);
    }
    
    void appendArgument(Argument* pArgument);

private:
    std::string                         m_actionName;
    Service*                            m_pService;
    
    Container<Argument>                 m_arguments;
    Container<Argument>                 m_inArguments;
    Container<Argument>                 m_outArguments;
};


template<class ServiceClass>
class ActionThread : public Poco::Runnable
{
public:
    typedef void (ServiceClass::*Callback)(Action* pAction);
    
    ActionThread(ServiceClass* pObject, Callback method, Action* arg):
        m_pObject(pObject),
        m_method(method),
        m_arg(arg)
    {
    }
    
    void run()
    {
        (m_pObject->*m_method)(m_arg);
    }
    
    void start()
    {
        Poco::Thread t;
        t.start(*this);
    }
    
private:
    ServiceClass*   m_pObject;
    Callback        m_method;
    Action*         m_arg;
};


class Subscription
{
public:
    Subscription(std::string callbackUri);
    
    std::string getUuid() { return m_uuid.toString(); }
    
    Poco::Net::HTTPClientSession* getSession() { return m_pSession; }
    std::string getEventKey();
    
    void sendEventMessage(const std::string& eventMessage);
    void renew(int seconds);  // TODO: implement this
    void expire(Poco::Timer& timer);  // TODO: implement this
    
private:
//     HTTPRequest* newRequest();
    
    Poco::URI                       m_deliveryAddress;
    Poco::Net::HTTPClientSession*   m_pSession;
    Poco::UUID                      m_uuid;
    Poco::UInt32                    m_eventKey;
    std::string                     m_duration;
    Poco::Timer                     m_timer;
    Service*                        m_pService;
};


class Service {
public:
    Service() {}
    ~Service();
    
    std::string getServiceType() const { return m_serviceType; }
    std::string getDescriptionPath() const { return m_descriptionPath; }
    std::string& getDescription() const { return *m_pDescription; }
    std::string getControlPath() const { return m_controlPath; }
    std::string getEventPath() const { return m_eventPath; }
    DescriptionRequestHandler* getDescriptionRequestHandler() const { return m_pDescriptionRequestHandler; }
    ControlRequestHandler* getControlRequestHandler() const { return m_controlRequestHandler; }
    Device* getDevice() const { return m_pDevice; }
    Action* getAction(const std::string& actionName) { return &m_actions.get(actionName); }
    Subscription* getSubscription(const std::string& sid) { return &m_eventSubscriptions.get(sid); }
    StateVar* getStateVarReference(const std::string& key) { return &m_stateVars.get(key); }
    template<typename T> T getStateVar(const std::string& key);
    
    void setServiceType(std::string serviceType) { m_serviceType = serviceType; }
    void setDescriptionPath(std::string descriptionPath) { m_descriptionPath = descriptionPath; }
    void setDescription(std::string& description) { m_pDescription = &description; }
    void setDescriptionRequestHandler() { m_pDescriptionRequestHandler = new DescriptionRequestHandler(*m_pDescription); }
    void setControlPath(std::string controlPath) { m_controlPath = controlPath; }
    void setEventPath(std::string eventPath) { m_eventPath = eventPath; }
    void setDevice(Device* pDevice) { m_pDevice = pDevice; }
    template<typename T> void setStateVar(std::string key, const T& val);
    
    void addAction(Action* pAction);
    void addStateVar(StateVar* pStateVar);
    
    void initClient();
    void sendAction(Action* pAction);
    
    typedef Container<StateVar>::Iterator StateVarIterator;
    StateVarIterator beginStateVar() { return m_stateVars.begin(); }
    StateVarIterator endStateVar() { return m_stateVars.end(); }
    StateVarIterator beginEventedStateVar() { return m_eventedStateVars.begin(); }
    StateVarIterator endEventedStateVar() { return m_eventedStateVars.end(); }
    
    typedef Container<Action>::Iterator ActionIterator;
    ActionIterator beginAction() { return m_actions.begin(); }
    ActionIterator endAction() { return m_actions.end(); }
    
    typedef Container<Subscription>::Iterator SubscriptionIterator;
    SubscriptionIterator beginEventSubscription() { return m_eventSubscriptions.begin(); }
    SubscriptionIterator endEventSubscription() { return m_eventSubscriptions.end(); }
    
    void registerSubscription(Subscription* subscription);
    void unregisterSubscription(Subscription* subscription);
    
    // TODO: moderated event messaging
    // TODO: honor maximumRate and minimumDelta (specs p. 72)
    void sendEventMessage(StateVar& stateVar);
    void sendInitialEventMessage(Subscription* pSubscription);
    
private:
    Device*                                 m_pDevice;
    std::string                             m_vendorDomain;
    std::string                             m_serviceType;
    std::string                             m_serviceVersion;
    std::string*                            m_pDescription;
    std::string                             m_descriptionPath;
    DescriptionRequestHandler*              m_pDescriptionRequestHandler;
    std::string                             m_controlPath;
    ControlRequestHandler*                  m_controlRequestHandler;
    Poco::Net::HTTPClientSession*                m_pControlRequestSession;
    std::string                             m_eventPath;
    Container<Action>                       m_actions;
    Container<StateVar>                     m_stateVars;
    Container<StateVar>                     m_eventedStateVars;
    Container<Subscription>                 m_eventSubscriptions;
    
    Poco::FastMutex                         m_serviceLock;
};


// TODO: finish Device Property stuff ...
// class Property
// {
//     std::string getValue() { return m_value; }
//     void setValue(const std::string& val) { m_value = val; }
//     
// private:
//     std::string     m_value;
// };


class Device
{
public:
    Device();
    ~Device();
    
    typedef Container<Service>::Iterator ServiceIterator;
    ServiceIterator beginService() { return m_services.begin(); }
    ServiceIterator endService() { return m_services.end(); }
    
    typedef Container<std::string>::Iterator PropertyIterator;
    PropertyIterator beginProperty() { return m_properties.begin(); }
    PropertyIterator endProperty() { return m_properties.end(); }
    
    DeviceRoot* getDeviceRoot() const { return m_pDeviceRoot; }
    std::string getUuid() const { return m_uuid; }
    std::string getDeviceType() const { return m_deviceType; }
    Service* getService(std::string serviceType) { return &m_services.get(serviceType); }
    const std::string& getProperty(const std::string& name) { return m_properties.get(name); }
    
    void setDeviceRoot(DeviceRoot* pDeviceRoot) { m_pDeviceRoot = pDeviceRoot; }
    void setUuid(std::string uuid) { m_uuid = uuid; }
    void setDeviceType(std::string deviceType) { m_deviceType = deviceType; }
    void setProperty(const std::string& name, const std::string& val) { m_properties.get(name) = val; }
    
    void addProperty(const std::string& name, const std::string& val) { m_properties.append(name, new std::string(val)); }
    void addService(Service* pService);
    
private:
    DeviceRoot*                         m_pDeviceRoot;
    std::string                         m_uuid;
    std::string                         m_vendorDomain;
    std::string                         m_deviceType;
    std::string                         m_deviceVersion;
    Container<Service>                  m_services;
    Container<std::string>              m_properties;
};


class DeviceRootImplAdapter;

class DeviceRoot
{
public:
    DeviceRoot();
    ~DeviceRoot();
    
    typedef Container<Device>::Iterator DeviceIterator;
    DeviceIterator beginDevice() { return m_devices.begin(); }
    DeviceIterator endDevice() { return m_devices.end(); }
    
    typedef std::map<std::string,Service*>::iterator ServiceTypeIterator;
    ServiceTypeIterator beginServiceType() { return m_serviceTypes.begin(); }
    ServiceTypeIterator endServiceType() { return m_serviceTypes.end(); }
    
    
    /*const*/ Device* getDevice(std::string uuid) /*const*/ { return &m_devices.get(uuid); }
    Device* getRootDevice() const { return m_pRootDevice; }
    std::string& getDeviceDescription() const { return *m_pDeviceDescription; }
    const Poco::URI& getBaseUri() const { return m_baseUri; }
    const Poco::URI& getDescriptionUri() const { return m_descriptionUri; }
    Service* getServiceType(const std::string& serviceType);
    
    void setRootDevice(Device* pDevice) { m_pRootDevice = pDevice; }
    void setDeviceDescription(std::string& description) { m_pDeviceDescription = &description; }
    void setBaseUri() { m_baseUri = Poco::URI(m_httpSocket.getServerUri()); }
    void setBaseUri(const Poco::URI& baseUri) { m_baseUri = baseUri; }
    void setDescriptionUri(std::string descriptionPath) { m_descriptionUri = Poco::URI(m_httpSocket.getServerUri() + descriptionPath); }
    
    void addDevice(Device* pDevice) { m_devices.append(pDevice->getUuid(), pDevice); }
    void addServiceType(std::string serviceType, Service* pService) { m_serviceTypes[serviceType] = pService; }
    
    void print();
    
    void initDevice();
    void initController();
    void startSsdp();
    void startHttp();
    void stopSsdp();
    void stopHttp();
    
    void registerActionHandler(const Poco::AbstractObserver& observer)
    {
        m_httpSocket.m_notificationCenter.addObserver(observer);
    }
    
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler) 
    {
        m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler(path, requestHandler);
    }
    
    void sendMessage(SsdpMessage& message, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    void handleSsdpMessage(SsdpMessage* pNf);
    void postAction(Action* pAction) { m_httpSocket.m_notificationCenter.postNotification(pAction); }
    
    void setImplAdapter(DeviceRootImplAdapter* implAdapter) { m_pDeviceRootImplAdapter = implAdapter; }
    void initStateVars(const std::string& serviceType, Service* pThis);

    
private:
    Poco::URI                       m_baseUri;              // base URI for control URI and event URI
    Poco::URI                       m_descriptionUri;       // for controller to download description
    std::string*                    m_pDeviceDescription;
    Container<Device>               m_devices;
    Device*                         m_pRootDevice;
    std::map<std::string,Service*>  m_serviceTypes;
    SsdpSocket                      m_ssdpSocket;
    SsdpMessageSet                  m_ssdpNotifyAliveMessages;
    SsdpMessageSet                  m_ssdpNotifyByebyeMessages;
    HttpSocket                      m_httpSocket;
    DescriptionRequestHandler*      m_descriptionRequestHandler;
    DeviceRootImplAdapter*          m_pDeviceRootImplAdapter;
};


class DeviceRootImplAdapter
{
    friend class DeviceRoot;
    
public:
    DeviceRootImplAdapter();
    ~DeviceRootImplAdapter();
    
    void start();
    void stop();
    
    // TODO: implement Property setters ...
//     void setUuid(std::string uuid);
    void setFriendlyName(const std::string& friendlyName);
    
protected:
    virtual void actionHandler(Action* action) = 0;
    virtual void initStateVars(const std::string& serviceType, Service* pThis) = 0;
    
    std::map<std::string,std::string*>  m_descriptions;
    // m_deviceRoot is the link into the "dynamic-string-world".
    DeviceRoot*                         m_pDeviceRoot;
};


class Controller
{
public:
    Controller();
    ~Controller();

    void init();
    virtual void deviceAdded(DeviceRoot* pDeviceRoot) = 0;
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot) = 0;
    
private:
    void sendMSearch();
    void handleSsdpMessage(SsdpMessage* pMessage);
    void addDevice(DeviceRoot* pDevice);
    void removeDevice(const std::string& uuid);
    
    SsdpSocket              m_ssdpSocket;
    Container<DeviceRoot>   m_devices;
//     Poco::FastMutex     m_controllerLock;
};


class ControllerImplAdapter
{
protected:
    ControllerImplAdapter(Device* pDevice) : m_pDevice(pDevice) {}
    
    virtual void eventHandler(StateVar* stateVar) = 0;
    void init();
    
private:
    Controller*     m_pController;
    Device*         m_pDevice;
};


template<typename T>
T
Service::getStateVar(const std::string& key)
{
    // TODO: lock the m_stateVariables map because different threads could access it
    std::clog << "Service::getStateVar()" << std::endl;
    return m_stateVars.getValue<T>(key);
}


template<typename T>
void
Service::setStateVar(std::string key, const T& val)
{
    // TODO: lock the m_stateVariables map because different threads could access it
    std::clog << "Service::setStateVar() name: " << key << std::endl;
    // FIXME: segfault here
    std::clog << "service type: " << getServiceType() << std::endl;
    m_stateVars.setValue(key, val);
    if (m_stateVars.get(key).getSendEvents()) {
        std::clog << "Service::setStateVar() " << key << " sends event message" << std::endl;
        sendEventMessage(m_stateVars.get(key));
    }
    std::clog << "Service::setStateVar() finished" << std::endl;
}


class Urn
{
public:
    Urn(const std::string& urn);
    
    const std::string& getUrn() { return m_urn; }
    const std::string& getDomainName() { return m_domainName; }
    const std::string& getType() { return m_type; }
    const std::string& getTypeName() { return m_typeName; }
    const std::string& getVersion() { return m_version; }
    
private:
    std::string     m_urn;
    std::string     m_domainName;
    std::string     m_type;
    std::string     m_typeName;
    std::string     m_version;
};

} // namespace Jamm

#endif
