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

#ifndef JAMMUPNP_PRIVATE_H
#define JAMMUPNP_PRIVATE_H

#include "Upnp.h"

namespace Jamm {

static const std::string    SSDP_ADDRESS        = "239.255.255.250";
static const Poco::UInt16   SSDP_PORT           = 1900;
static const Poco::UInt16   SSDP_CACHE_DURATION = 1800;
static const Poco::UInt16   SSDP_MIN_WAIT_TIME  = 1;
static const Poco::UInt16   SSDP_MAX_WAIT_TIME  = 120;

class Argument;
class ControlRequestHandler;
class HttpSocket;
class Entity;
class EntityItem;


class NetworkInterfaceNotification : public Poco::Notification
{
public:
    NetworkInterfaceNotification(const std::string& interfaceName, bool added);
    
    std::string     m_interfaceName;
    bool            m_added;
};


// class NetworkInterfaceManager
// {
// public:
//     static NetworkInterfaceManager* instance();
//     // clients like DeviceRoot and Controller can register here
//     void registerInterfaceChangeHandler(const Poco::AbstractObserver& observer);
//     // some OS dependent hardware abstraction layer can add and remove devices here
//     void addInterface(const std::string& name);
//     void removeInterface(const std::string& name);
//     // this address can be announced for the HTTP servers to be reached at
//     const Poco::Net::IPAddress& getValidInterfaceAddress();
//     
// private:
//     NetworkInterfaceManager();
//     void findValidIpAddress();
//     
//     static NetworkInterfaceManager*     m_pInstance;
//     std::vector<std::string>            m_interfaceList;
//     Poco::Net::IPAddress                m_validIpAddress;
//     Poco::NotificationCenter            m_notificationCenter;
//     bool                                m_loopbackProvided;
//     Poco::Net::IPAddress                m_loopbackAddress;
// };


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
    SsdpMessage(const std::string& buf, const std::string& interface = "*", const Poco::Net::SocketAddress& sender = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
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
    
    void setLocation(const std::string& location);
    std::string getLocation();
    
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
    
    const std::string& getInterface();
    Poco::Net::SocketAddress getSender();
    
    
private:
    void initMessageMap();
    
    TRequestMethod                          m_requestMethod;
    TRequestMethod                          m_notificationSubtype;
    std::map<std::string,std::string>       m_messageHeader;
    std::string                             m_interface;
    Poco::Net::SocketAddress                m_sender;
    
    std::map<TRequestMethod,std::string>    m_messageMap;
    std::map<std::string,TRequestMethod>    m_messageConstMap;
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
    std::string* write();
    
private:
    Poco::XML::Element* device(Device& device);
    Poco::XML::Element* service(Service* pService);
    Poco::XML::Element* icon(Icon* pIcon);
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
//     DescriptionRequestHandler(std::string& description);
    DescriptionRequestHandler(std::string* pDescription);
    
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


class IconRequestHandler: public UpnpRequestHandler
{
public:
    IconRequestHandler(Icon* pIcon) : m_pIcon(pIcon) {}
    
    IconRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    Icon*    m_pIcon;
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

} // namespace Jamm

#endif
