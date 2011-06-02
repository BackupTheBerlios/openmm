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

#ifndef UpnpPrivate_INCLUDED
#define UpnpPrivate_INCLUDED

#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/HTTPStreamFactory.h>

#include "Upnp.h"


namespace Omm {

static const std::string    SSDP_ADDRESS        = "239.255.255.250";
static const std::string    SSDP_LOOP_ADDRESS   = "127.255.255.255";
static const Poco::UInt16   SSDP_PORT           = 1900;
static const Poco::UInt16   SSDP_CACHE_DURATION = 1800;
static const Poco::UInt16   SSDP_MIN_WAIT_TIME  = 1;
static const Poco::UInt16   SSDP_MAX_WAIT_TIME  = 120;

class Argument;
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
    
    Poco::Net::SocketAddress getSender();
    
    
private:
    void initMessageMap();
    
    TRequestMethod                          _requestMethod;
    TRequestMethod                          _notificationSubtype;
    std::map<std::string,std::string>       _messageHeader;
    Poco::Net::SocketAddress                _sender;
    
    std::map<TRequestMethod,std::string>    _messageMap;
    std::map<std::string,TRequestMethod>    _messageConstMap;
};


class SsdpMessageSet
{
    friend class Socket;

public:
    SsdpMessageSet();
    ~SsdpMessageSet();

    void clear();
    void addMessage(SsdpMessage& message);

private:
    void send(SsdpSocket& socket, int repeat = 1, long delay = 0, Poco::UInt16 cacheDuration = SSDP_CACHE_DURATION, bool continuous = false, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    /// Sends message set "repeat" times delayed for up to "delay" millisecs (actual delay randomly chosen).
    /// If continuous is true, message set is sent out repeatatly with a delay of up to cacheDuration / 2 (actual delay randomly chosen).
    /// Receiver of message set is by default ssdp multicast address, but can be chosen in parameter "receiver".
    /// Note that send blocks when started with continuous = false and does not block with continuous = true.
    void startSendContinuous(SsdpSocket& socket, long delay = 100, Poco::UInt16 cacheDuration = SSDP_CACHE_DURATION);
    void stopSendContinuous();
    void onTimer(Poco::Timer& timer);

    Poco::FastMutex                     _sendTimerLock;
    Poco::Random                        _randomTimeGenerator;
    Poco::Timer                         _sendTimer;
    SsdpSocket*                         _pSsdpSocket;
    std::vector<SsdpMessage*>           _ssdpMessages;
    int                                 _repeat;
    long                                _delay;
    Poco::UInt16                        _cacheDuration;
    bool                                _continuous;
    bool                                _sendTimerIsRunning;
    Poco::Net::SocketAddress            _receiver;
};


class ActionRequestReader
{
public:
    ActionRequestReader(const std::string& requestBody, Action* pActionTemplate);
    
    Action* action();
    
private:
    // TODO: replace _nodeStack by Node* in action()
    std::stack<Poco::XML::Node*>            _nodeStack;
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Action*                                 _pActionTemplate;
};


class ActionResponseReader
{
public:
    ActionResponseReader(const std::string& responseBody, Action* pActionTemplate);
    
    Action* action();
    
private:
    std::stack<Poco::XML::Node*>        _nodeStack;
    Poco::AutoPtr<Poco::XML::Document>  _pDoc;
    Action*                             _pActionTemplate;
};


class DeviceDescriptionWriter
{
public:
    DeviceDescriptionWriter();
    
    void deviceContainer(DeviceContainer& deviceRoot);
    std::string* write();
    
private:
    Poco::XML::Element* device(Device& device);
    Poco::XML::Element* service(Service* pService);
    Poco::XML::Element* icon(Icon* pIcon);

    Poco::AutoPtr<Poco::XML::Document>   _pDoc;
};


class ServiceDescriptionWriter
{
public:
    ServiceDescriptionWriter();
    
    void service(Service& service);
    std::string* write();
    
private:
    Poco::XML::Element*  action(Action* pAction);
    Poco::XML::Element*  argument(Argument* pArgument);
    Poco::XML::Element*  stateVar(StateVar* pStateVar);
    
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
};


class SsdpNotifyAliveWriter
{
public:
    SsdpNotifyAliveWriter(SsdpMessageSet& generatedMessages) : _res(&generatedMessages) {}
    
    void deviceContainer(const DeviceContainer& pDeviceContainer);
    void device(const Device& pDevice);
    void service(const Service& pService);

private:
    SsdpMessageSet*            _res;
};


class SsdpNotifyByebyeWriter
{
public:
    SsdpNotifyByebyeWriter(SsdpMessageSet& generatedMessages) : _res(&generatedMessages) {}
    
    void deviceContainer(const DeviceContainer& pDeviceContainer);
    void device(const Device& pDevice);
    void service(const Service& pService);
    
private:
    SsdpMessageSet*            _res;
};


class ActionResponseWriter
{
public:
    ActionResponseWriter(std::string& responseBody);
    // TODO: couldn't cope with the const argument stuff here ...
    void action(Action& action);
private:
    std::string*    _responseBody;
};


class ActionRequestWriter
{
public:
    void action(Action* action);
    void write(std::string& actionMessage);
    
private:
    Poco::AutoPtr<Poco::XML::Document>   _pDoc;
};


class EventMessageWriter
{
public:
    EventMessageWriter();
    void write(std::string& eventMessage);
    void stateVar(const StateVar& stateVar);

private:
    Poco::AutoPtr<Poco::XML::Document>   _pDoc;
    Poco::AutoPtr<Poco::XML::Element>    _pPropertySet;
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
    DescriptionRequestHandler(std::string* pDescription);
    
    DescriptionRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    std::string*    _pDescription;
};


class DeviceRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
    DeviceRequestHandlerFactory(HttpSocket* pHttpSocket);
    
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
    void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler);
    
private:
    std::map<std::string,UpnpRequestHandler*> _requestHandlerMap;
    HttpSocket*                               _pHttpSocket;
};


class VariableQuery : public Poco::Notification
{
};


class ControlRequestHandler: public UpnpRequestHandler
{
public:
    ControlRequestHandler(Service* service);
    
    ControlRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    Service*    _pService;
};


class EventSubscriptionRequestHandler: public UpnpRequestHandler
{
public:
    EventSubscriptionRequestHandler(Service* pService) : _pService(pService) {}
    
    EventSubscriptionRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    Service*    _pService;
};


class EventNotificationRequestHandler: public UpnpRequestHandler
{
public:
    EventNotificationRequestHandler(Service* pService) : _pService(pService) {}

    EventNotificationRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    Service*    _pService;
};


class IconRequestHandler: public UpnpRequestHandler
{
public:
    IconRequestHandler(Icon* pIcon) : _pIcon(pIcon) {}
    
    IconRequestHandler* create();
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
    Icon*    _pIcon;
};


class Subscription
{
public:
    Subscription(std::string callbackUri);
    
    std::string getUuid() { return _uuid.toString(); }
    
    Poco::Net::HTTPClientSession* getSession() { return _pSession; }
    std::string getEventKey();
    
    void sendEventMessage(const std::string& eventMessage);
    void renew(int seconds);  // TODO: implement this
    void expire(Poco::Timer& timer);  // TODO: implement this
    
private:
//     HTTPRequest* newRequest();
    
    Poco::URI                       _deliveryAddress;
    Poco::Net::HTTPClientSession*   _pSession;
    Poco::UUID                      _uuid;
    Poco::UInt32                    _eventKey;
    std::string                     _duration;
    Poco::Timer                     _timer;
    Service*                        _pService;
};

} // namespace Omm

#endif
