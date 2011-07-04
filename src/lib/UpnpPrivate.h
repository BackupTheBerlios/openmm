/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                                 |
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


#include <Poco/Net/MulticastSocket.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/NotificationQueue.h>
#include <Poco/NotificationCenter.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/SocketReactor.h>


#include <Poco/DateTime.h>

#include "Upnp.h"
#include "UpnpInternal.h"


namespace Omm {

static const std::string    SSDP_ADDRESS        = "239.255.255.250";
static const std::string    SSDP_LOOP_ADDRESS   = "127.255.255.255";
static const Poco::UInt16   SSDP_PORT           = 1900;
static const Poco::UInt16   SSDP_CACHE_DURATION = 1800;
static const Poco::UInt16   SSDP_MIN_WAIT_TIME  = 1;
static const Poco::UInt16   SSDP_MAX_WAIT_TIME  = 120;

class ControlRequestHandler;
class HttpSocket;


class SsdpSocket
{
    friend class DeviceContainer;
    friend class Controller;
    friend class Socket;

public:
    SsdpSocket();
    ~SsdpSocket();

    void addInterface(const std::string& name);
    void removeInterface(const std::string& name);
    void addObserver(const Poco::AbstractObserver& observer);
    void startListen();
    void stopListen();

    void sendMessage(SsdpMessage& message, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));

private:
    enum SocketMode {NotConfigured, Broadcast, Multicast};

    void init();
    void deinit();
    void setupSockets();
    void resetSockets();
    void setMulticast();
    void setBroadcast();

    void onReadable(Poco::Net::ReadableNotification* pNotification);

    SocketMode                      _mode;
    Poco::Net::MulticastSocket*     _pSsdpListenerSocket;
    Poco::Net::MulticastSocket*     _pSsdpSenderSocket;
    char*                           _pBuffer;

    static const int BUFFER_SIZE = 65536; // Max UDP Packet size is 64 Kbyte.
                 // Note that each SSDP message must fit into one UDP Packet.

    Poco::Net::SocketReactor                        _reactor;
    Poco::Thread                                    _listenerThread;
    Poco::NotificationCenter                        _notificationCenter;
};


class HttpSocket
{
    friend class Socket;

public:
    HttpSocket();
    ~HttpSocket();

    void init();
    void startServer();
    void stopServer();
    std::string getServerUri();

private:
    Poco::Net::HTTPServer*                _pHttpServer;
    Poco::UInt16                          _httpServerPort;
    UpnpRequestHandlerFactory*          _pDeviceRequestHandlerFactory;
    Poco::NotificationCenter              _notificationCenter;
};


class Socket
{
public:
    Socket();
    virtual ~Socket();

    void initSockets();
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler);
    void registerActionHandler(const Poco::AbstractObserver& observer);
    void registerSsdpMessageHandler(const Poco::AbstractObserver& observer);

    void startSsdp();
    void startHttp();

    void stopSsdp();
    void stopHttp();

    std::string getHttpServerUri();
    void postAction(Action* pAction);
    void sendSsdpMessage(SsdpMessage& ssdpMessage, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    void sendSsdpMessageSet(SsdpMessageSet& ssdpMessageSet, int repeat = 1, long delay = 0, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    void startSendSsdpMessageSet(SsdpMessageSet& ssdpMessageSet);
    void stopSendSsdpMessageSet(SsdpMessageSet& ssdpMessageSet);

private:
    void handleNetworkInterfaceChangedNotification(Net::NetworkInterfaceNotification* pNotification);
    void handleNetworkInterfaceChange(const std::string& interfaceName, bool added);

    SsdpSocket                      _ssdpSocket;
    HttpSocket                      _httpSocket;
};


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
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Action*                                 _pActionTemplate;
};


class ActionResponseReader
{
public:
    ActionResponseReader(const std::string& responseBody, Action* pActionTemplate);
    
    Action* action();
    
private:
    Poco::AutoPtr<Poco::XML::Document>  _pDoc;
    Action*                             _pActionTemplate;
};


class EventMessageReader
{
public:
    EventMessageReader(const std::string& responseBody, Service* pService);

    void stateVarValues();

private:
    void stateVar(Poco::XML::Node* pNode);

    Poco::AutoPtr<Poco::XML::Document>  _pDoc;
    Service*                            _pService;
};


class DescriptionWriter
{
public:
    DescriptionWriter();
    
    void deviceContainer(DeviceContainer& deviceContainer);
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


class UpnpRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
    UpnpRequestHandlerFactory(HttpSocket* pHttpSocket);

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
    void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler);

private:
    std::map<std::string,UpnpRequestHandler*> _requestHandlerMap;
    HttpSocket*                               _pHttpSocket;
};


// possible upnp request handler types:
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


class VariableQuery : public Poco::Notification
{
};


class Subscription
{
public:
    Subscription();
    ~Subscription();
    
    std::string getUuid();
    void setSid(const std::string& uuid);
    
    Poco::Net::HTTPClientSession* getSession() { return _pSession; }
    std::string getEventKey();

    void addCallbackUri(const std::string& uri);
    void sendEventMessage(const std::string& eventMessage);
    void renew(int seconds);  // TODO: implement this
    void expire(Poco::Timer& timer);  // TODO: implement this
    
private:
//     HTTPRequest* newRequest();
    
    std::vector<Poco::URI>          _callbackUris;
    Poco::URI*                      _pSessionUri;
    Poco::Net::HTTPClientSession*   _pSession;
    std::string                     _uuid;
    Poco::UInt32                    _eventKey;
    std::string                     _duration;
    Poco::Timer                     _timer;
    Service*                        _pService;
};


class EventMessageQueue
{
public:
    EventMessageQueue(Service* pService);
    ~EventMessageQueue();

    void queueStateVar(StateVar& stateVar);

private:
    void sendEventMessage(Poco::Timer& timer);
    
    Service*                _pService;
    std::set<StateVar*>     _stateVars;
    const int               _maxEventRate;  // event / _maxEventRate msec
    Poco::Timer*            _pModeratorTimer;
    bool                    _timerIsRunning;
    bool                    _firstStart;
    Poco::FastMutex         _lock;
};


class DeviceData
{
    friend class Device;
public:
    DeviceData();

    Device* getDevice();

    void setDeviceType(std::string deviceType);
    void setUuid(std::string uuid);
    void setDevice(Device* pDevice);

    void addProperty(const std::string& name, const std::string& val);
    void addService(Service* pService);

private:
    Device*                             _pDevice;
    std::string                         _uuid;
    std::string                         _deviceType;
    Container<Service>                  _services;
    Container<std::string>              _properties;
    std::vector<Icon*>                  _iconList;
};


} // namespace Omm

#endif
