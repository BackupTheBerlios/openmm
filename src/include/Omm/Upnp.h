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

#ifndef Upnp_INCLUDED
#define Upnp_INCLUDED

#include <string>
#include <map>
#include <stack>
#include <iostream>
#include <istream>
#include <fstream>
#include <ostream>
#include <sstream>

#ifdef POCO_VERSION_HEADER_FOUND
#include <Poco/Version.h>
#endif
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/StringTokenizer.h>
#include <Poco/StreamCopier.h>
#include <Poco/NObserver.h>
#include <Poco/Observer.h>
#include <Poco/Thread.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <Poco/NotificationCenter.h>
#include <Poco/URI.h>
#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/Path.h>
#include <Poco/Random.h>
#include <Poco/Timer.h>
#include <Poco/Mutex.h>
#include <Poco/ClassLoader.h>
#include <Poco/String.h>
#include <Poco/Environment.h>
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Message.h>
#include <Poco/Net/MediaType.h>
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
#include <Poco/Net/MediaType.h>
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
#include <Poco/SAX/SAXException.h>
#include <Poco/DOM/DOMException.h>

#include "Net.h"
#include "Util.h"
#include "Sys.h"

namespace Omm {

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
typedef Poco::Timestamp date;
typedef Poco::Timestamp dateTime;
typedef Poco::Timestamp time;

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    OMM_VERSION         = "0.1.0";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";

class DescriptionProvider;
class DeviceManager;
class Controller;
class UserInterface;
class DeviceContainer;
class Device;
class DeviceData;
class DevDeviceCode;
class CtlDeviceCode;
class Service;
class Action;
class Argument;
class StateVar;
class ControlRequestHandler;
class HttpSocket;
class Entity;
class EntityItem;
class UpnpRequestHandler;
class DescriptionRequestHandler;
class DeviceRequestHandlerFactory;
class ControlRequestHandler;
class SsdpSocket;
class SsdpMessage;
class SsdpMessageSet;
class Subscription;


class Log
{
public:
    static Log* instance();
    
    Poco::Logger& upnp();
    Poco::Logger& ssdp();
    Poco::Logger& http();
    Poco::Logger& desc();
    Poco::Logger& ctrl();
    Poco::Logger& event();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pUpnpLogger;
    Poco::Logger*   _pSsdpLogger;
    Poco::Logger*   _pHttpLogger;
    Poco::Logger*   _pDescriptionLogger;
    Poco::Logger*   _pControlLogger;
    Poco::Logger*   _pEventingLogger;
};


class Icon
{
    friend class IconRequestHandler;
    friend class DeviceServer;
    friend class DeviceContainer;
    friend class Device;
    friend class DeviceDescriptionWriter;
    
public:
    Icon(int width, int height, int depth, const std::string& mime, const std::string& uri = "");
    ~Icon();

    const std::string& getBuffer();
    std::string getIconRequestPath();
    void setIconRequestPath(const std::string& path);
    
private:
    void retrieve(const std::string& uri);
    
    int                     _width;
    int                     _height;
    int                     _depth;
    Poco::Net::MediaType    _mime;
    std::string             _buffer;
    std::string             _requestPath;
    std::string             _searchPath;
};


class Urn
{
public:
    Urn(const std::string& urn);

    const std::string& getUrn() { return _urn; }
    const std::string& getDomainName() { return _domainName; }
    const std::string& getType() { return _type; }
    const std::string& getTypeName() { return _typeName; }
    const std::string& getVersion() { return _version; }

private:
    std::string     _urn;
    std::string     _domainName;
    std::string     _type;
    std::string     _typeName;
    std::string     _version;
};


class Variant
{
public:
    Variant() : _val("") {}
    Variant(const std::string& val) : _val(val) {}
    
    // TODO: check if string formats are conform to specs (-> p. 33).
    void setValue(const std::string& val) {
//         std::clog << "Variant::setValue(const std::string& val), val: " << val << std::endl;
        _val = val;
    }
    void setValue(bool val) { _val = val ? "1" : "0"; }
    void setValue(ui1 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(ui2 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(ui4 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(i1 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(i2 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(i4 val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(float val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(double val) { _val = Poco::NumberFormatter::format(val); }
    void setValue(Poco::URI val) { _val = val.toString(); }
    
    const std::string& getValue() const {
        return _val; 
    }
    void getValue(std::string& val) { 
        val = _val; 
    }
    void getValue(bool& val) { val = (_val == "1" || _val == "true") ? true : false; }
    void getValue(ui1& val) { val = Poco::NumberParser::parse(_val); }
    void getValue(ui2& val) { val = Poco::NumberParser::parse(_val); }
    void getValue(ui4& val) { 
        if (_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parseUnsigned(_val);
        }
    }
    void getValue(i1& val) { 
        if (_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parse(_val);
        }
    }
    void getValue(i2& val) { val = Poco::NumberParser::parse(_val); }
    void getValue(i4& val) {
        if (_val == "") {
            val = 0;
        }
        else {
            val = Poco::NumberParser::parse(_val);
        }
    }
    void getValue(float& val) { val = Poco::NumberParser::parse(_val); }
    void getValue(double& val) { val = Poco::NumberParser::parse(_val); }
    void getValue(Poco::URI& val) { val = Poco::URI(_val); }
    
private:
    std::string     _val;
};


template<class E>
class Container
{
public:
    typedef typename std::vector<E*>::iterator Iterator;
    typedef typename std::map<std::string,E*>::iterator KeyIterator;
    
    E& get(std::string key)
    {
        return *_pEntities.find(key)->second;
    }
    
    E& get(int index)
    {
        return *_keys[index];
    }
    
    // TODO: make shure that only one entry exists in _pEntities and _keys for key
    void append(std::string key, E* pEntity)
    {
        _pEntities[key] = pEntity;
        _keys.push_back(pEntity);
    }
    
    void append(E* pEntity)
    {
        _keys.push_back(pEntity);
    }
    
    void remove(std::string key)
    {   
        E* pEntity = _pEntities.find(key)->second;
        _pEntities.erase(key);
        _keys.erase(find(_keys.begin(), _keys.end(), pEntity));
    }
    
    void remove(E* pEntity)
    {   
        _keys.erase(find(_keys.begin(), _keys.end(), pEntity));
    }
    
    bool contains(const std::string& key)
    {
        return _pEntities.find(key) != _pEntities.end();
    }
    
    bool contains(E* pEntity)
    {
        return find(_keys.begin(), _keys.end(), pEntity) != _keys.end();
    }
    
    int position(const std::string& key)
    {
        E* pEntity = _pEntities.find(key)->second;
        return find(_keys.begin(), _keys.end(), pEntity) - _keys.begin();
    }
    
    int position(E* pEntity)
    {
        return find(_keys.begin(), _keys.end(), pEntity) - _keys.begin();
    }
    
    int size()
    {
        return _keys.size();
    }
    
    template<typename T> T getValue(const std::string& key)
    {
//         std::clog << "Container::getValue() key: " << key << std::endl;
        Variant* e = (*_pEntities.find(key)).second;
        // FIXME: is this a proper check for "key not found in _pEntities"
        if (e) {
            T res;
            e->getValue(res);
//             std::clog << "Container::getValue() key: " << key << ", val: " << e->getValue() << std::endl;
            return res;
        } else {
//             std::cerr << "Container::getValue() could not find key: " << key << std::endl;
            // TODO: throw an exception
            return T();
        }
    }
    
    template<typename T> void setValue(const std::string& key, const T& val)
    {
//         std::clog << "Container::setValue() key: " << key << std::endl;
//         std::clog << "_pEntities has: " << _pEntities.size() << " entries" << std::endl;
//         std::clog << "_keys has: " << _keys.size() << " entries" << std::endl;
        if (_pEntities.find(key) == _pEntities.end()) {
            Log::instance()->upnp().error("could not set container value, key not found: " + key);
            return;
        }
//         std::clog << "Container::setValue() found key" << std::endl;
        Variant* e = (*_pEntities.find(key)).second;
//         std::clog << "Container::setValue() found Variant pointer: " << e << std::endl;
        if (e) {
//             std::clog << "Container::setValue() found key: " << key << std::endl;
            e->setValue(val);
        }
        else {
//             std::cerr << "Container::setValue() pointer to Variant is invalid" << std::endl;
        }
//         std::clog << "Container::setValue() key: " << key << ", val: " << e->getValue() << std::endl;
    }
    
    Iterator begin()
    {
        return _keys.begin();
    }
    
    Iterator end()
    {
        return _keys.end();
    }
    
    KeyIterator beginKey()
    {
        return _pEntities.begin();
    }
        
    KeyIterator endKey()
    {
        return _pEntities.end();
    }
    
    void clear()
    {
        _pEntities.clear();
        _keys.clear();
    }
    
private:
    std::map<std::string,E*>    _pEntities;
    std::vector<E*>             _keys;
};


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
    DeviceRequestHandlerFactory*          _pDeviceRequestHandlerFactory;
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


class DescriptionProvider
{
public:
    std::string& getDeviceDescription();
    std::string& getServiceDescription(const std::string& path);
    /// Argument path correspond to the SCPD fields of the device description,
    /// because usually the device description is retrieved first and then
    /// the service descriptions listed in the device description.

protected:
    std::string*                            _pDeviceDescription;
    std::map<std::string,std::string*>      _serviceDescriptions;
};


class DescriptionReader
{
public:
    DescriptionReader();
    ~DescriptionReader();

    void getDeviceDescription(const std::string& deviceDescriptionUri = "");
    /// Reads a device description from the URI given in deviceDescriptionUri.
    /// The service description URIs contained in the device description
    /// can be relative to the device description uri (depending on implementation
    /// of retrieveDescriptionUri).
    /// When device description location is unique, deviceDescriptionUri can be ""

    DeviceContainer* deviceContainer();
    /// Parses the device description, creates and returns a DeviceContainer, including
    /// the root device and all subdevices. This is used at the controller side
    /// when a new device pops up and is read by the controller. Also, the stub
    /// generator uses this to build the device tree and generate the stub code.
    /// The stub generator could also use rootDeviceData(), but this is more robust.
    DeviceData* rootDeviceData(Device* pDevice);
    /// Parses the device description, creates and returns the device data of
    /// the root device. Subdevices, if present, are ignored. This is used by
    /// the device side, when building the internal device tree from a memory description.
    
protected:
    virtual std::string& retrieveDescription(const std::string& descriptionUri) = 0;
    /// Retrieves the description from descriptionUri, which can be relative to _deviceDescriptionUri.
    std::string   _deviceDescriptionUri;
    /// Device description URI and base URI for service descriptions.

private:
    DeviceData* deviceData(Poco::XML::Node* pNode, Device* pDevice);
    Service* service(Poco::XML::Node* pNode);
    Action* action(Poco::XML::Node* pNode);
    Argument* argument(Poco::XML::Node* pNode);
    StateVar* stateVar(Poco::XML::Node* pNode);

    void parseDescription(const std::string& description);
    void releaseDescriptionDocument();
    /// release parsed xml document of root device / service description

    std::stack<Poco::XML::Document*>    _pDocStack;
    /// _pDocStack contains device description of root device and it's service
    /// descriptions. When descriptions are parsed, the parsed xml document
    /// is released and the document is popped from the stack.
};


class UriDescriptionReader : public DescriptionReader
{
private:
    virtual std::string& retrieveDescription(const std::string& relativeUri);
};


class MemoryDescriptionReader : public DescriptionReader
{
public:
    MemoryDescriptionReader(DescriptionProvider& deviceDescriptionProvider);

private:
    virtual std::string& retrieveDescription(const std::string& descriptionKey);

    DescriptionProvider& _deviceDescriptionProvider;
};


class DeviceManager : public Util::Startable
{
    friend class ControlRequestHandler;

public:
    DeviceManager(Socket* pNetworkListener);
    virtual ~DeviceManager();

    typedef Container<DeviceContainer>::Iterator DeviceContainerIterator;
    DeviceContainerIterator beginDeviceContainer();
    DeviceContainerIterator endDeviceContainer();

    void addDeviceContainer(DeviceContainer* pDevice);
    void removeDeviceContainer(const std::string& uuid);

    virtual void init();
    virtual void start();
    virtual void stop();

    std::string getHttpServerUri();
    void registerActionHandler(const Poco::AbstractObserver& observer);
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler);

protected:
    virtual void handleSsdpMessage(SsdpMessage* pMessage) {}
    virtual void deviceContainerAdded(DeviceContainer* pDeviceContainer) {}
    virtual void deviceContainerRemoved(DeviceContainer* pDeviceContainer) {}

    virtual void startSsdp();
    virtual void stopSsdp();

    void startHttp();
    void stopHttp();


    Container<DeviceContainer> _deviceContainers;
    Socket*                    _pSocket;

private:
    void postAction(Action* pAction);
};


class Controller : public DeviceManager
{
public:
    Controller();
    virtual ~Controller();

    virtual void start();

    void setUserInterface(UserInterface* pUserInterface);
    UserInterface* getUserInterface();

protected:
    UserInterface*                      _pUserInterface;

private:
    void sendMSearch();
    void handleSsdpMessage(SsdpMessage* pMessage);
//    void handleNetworkInterfaceChangedNotification(Net::NetworkInterfaceNotification* pNotification);
    void discoverDevice(const std::string& location);
    void addDeviceContainer(DeviceContainer* pDevice);
    void removeDeviceContainer(const std::string& uuid);
    void update();
};


class DeviceServer : public DeviceManager
{
public:
    DeviceServer();
    virtual ~DeviceServer();

    virtual void init();

private:
    virtual void startSsdp();
    virtual void stopSsdp();
    virtual void handleSsdpMessage(SsdpMessage* pMessage);
};


class DeviceContainer
{
    friend class DeviceManager;
    friend class DeviceServer;
    friend class DevDeviceCode;
    friend class Controller;

public:
    DeviceContainer();
    ~DeviceContainer();

    typedef Container<Device>::Iterator DeviceIterator;
    DeviceIterator beginDevice();
    DeviceIterator endDevice();

    typedef std::map<std::string,Service*>::iterator ServiceTypeIterator;
    ServiceTypeIterator beginServiceType();
    ServiceTypeIterator endServiceType();

    DeviceManager* getDeviceManager();
    /*const*/ Device* getDevice(std::string uuid) /*const*/;
    Device* getRootDevice() const;
    Controller* getController() const;
    std::string* getDeviceDescription() const;
    const std::string& getDescriptionUri() const;
    std::string getDescriptionPath() const;
    Service* getServiceType(const std::string& serviceType);

    void setDeviceManager(DeviceManager* pDeviceManager);
    void setRootDevice(Device* pDevice);
    void setDeviceDescription(std::string& description);
    void setDescriptionUri(const std::string uri);
    void addDevice(Device* pDevice);
    void addServiceType(Service* pService);

    void print();

    void initUuid();
    void initStateVars();
    void rewriteDescriptions();
    void initDeviceDescriptionHandler();
    void initDevice();
    void initController();

private:
    void writeSsdpMessages();

//     Poco::URI                       _baseUri;              // base URI for control URI and event URI
    std::string                     _stringDescriptionUri;            // for controller to download description
    Poco::URI                       _descriptionUri;
    std::string*                    _pDeviceDescription;
    DeviceManager*                  _pDeviceManager;
    Container<Device>               _devices;
    Device*                         _pRootDevice;
    std::map<std::string,Service*>  _serviceTypes;
    SsdpMessageSet*                 _pSsdpNotifyAliveMessages;
    SsdpMessageSet*                 _pSsdpNotifyByebyeMessages;
    DescriptionRequestHandler*      _descriptionRequestHandler;
    // TODO: remove _pController (only needed in Service::actionNetworkActivity())
    // replace it with a notification center in DeviceManager
    Controller*                     _pController;
};


class Device
{
    friend class DeviceDescriptionWriter;
    friend class DeviceContainer;
    friend class DeviceManager;
    friend class DeviceServer;

public:
    Device();
    virtual ~Device();

    typedef Container<Service>::Iterator ServiceIterator;
    ServiceIterator beginService();
    ServiceIterator endService();

    typedef Container<std::string>::KeyIterator PropertyIterator;
    PropertyIterator beginProperty();
    PropertyIterator endProperty();

    typedef std::vector<Icon*>::iterator IconIterator;
    IconIterator beginIcon();
    IconIterator endIcon();

    DeviceContainer* getDeviceContainer() const;
    DevDeviceCode* getDevDevice() const;
    std::string getUuid() const;
    std::string getDeviceType() const;
    const std::string& getFriendlyName();
    Service* getService(std::string serviceType);
    const std::string& getProperty(const std::string& name);

    void setDeviceContainer(DeviceContainer* pDeviceContainer);
    void setDeviceData(DeviceData* pDeviceData);
    void setDeviceCode(DevDeviceCode* pDevDevice);
    void setUuid(std::string uuid);
    void setRandomUuid();
    void setProperty(const std::string& name, const std::string& val);

    void addProperty(const std::string& name, const std::string& val);
    void addService(Service* pService);
    void addIcon(Icon* pIcon);

    void initStateVars();

private:
    DeviceContainer*                    _pDeviceContainer;
    DeviceData*                         _pDeviceData;
    DevDeviceCode*                      _pDevDeviceCode;
    CtlDeviceCode*                      _pCtlDeviceCode;
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


class DevDeviceCode
{
    friend class DeviceContainer;
    friend class DeviceServer;
    friend class Device;

public:
    DevDeviceCode();
    ~DevDeviceCode();

    std::string& deviceDescription();
    std::map<std::string,std::string*>& serviceDescriptions();

protected:
    virtual void actionHandler(Action* action) = 0;
    virtual void initStateVars(Service* pService) = 0;
//    virtual bool initDevice() { return true; }
    /// initDevice() can be implemented by the customer to execute code before UPnP device is started.
    /// if initialization takes a while to process, start the device in threaded mode with start(true).

    Device*                                 _pDevice;
};


class StateVar : public Variant
{
public:
    StateVar() : Variant() {}
    template<typename T> StateVar(const T& val) : Variant(val) {}

    std::string getName() const { return _name; }
    const std::string& getType() const { return _type; }
    void setName(std::string name) { _name = name; }
    void setType(std::string type) { _type = type; }
    void setDefaultValue(std::string defaultValue) { _defaultValue = defaultValue; }
    void setSendEvents(std::string sendEvents) { _sendEvents = (sendEvents=="yes") ? true : false; }
    bool getSendEvents() const { return _sendEvents; }

private:
    std::string     _name;
    std::string     _type;
    std::string     _defaultValue;
    bool            _sendEvents;
};


class Service {
public:
    Service() {}
    ~Service();

    typedef Container<StateVar>::Iterator StateVarIterator;
    StateVarIterator beginStateVar();
    StateVarIterator endStateVar();
    StateVarIterator beginEventedStateVar();
    StateVarIterator endEventedStateVar();

    typedef Container<Action>::Iterator ActionIterator;
    ActionIterator beginAction();
    ActionIterator endAction();

    typedef Container<Subscription>::Iterator SubscriptionIterator;
    SubscriptionIterator beginEventSubscription();
    SubscriptionIterator endEventSubscription();

    typedef std::vector<std::string>::iterator EventCallbackPathIterator;
    EventCallbackPathIterator beginEventCallbackPath();
    EventCallbackPathIterator endEventCallbackPath();

    std::string getServiceType() const;
    std::string getServiceId() const;
    std::string getDescriptionPath() const;
    std::string* getDescription() const;
    std::string getControlPath() const;
    std::string getEventSubscriptionPath() const;
    std::string getEventCallbackPath(int callbackPathIndex = 0);
    DescriptionRequestHandler* getDescriptionRequestHandler() const;
    ControlRequestHandler* getControlRequestHandler() const;
    Device* getDevice() const;
    Action* getAction(const std::string& actionName);
    Subscription* getSubscription(const std::string& sid);
    StateVar* getStateVarReference(const std::string& key);
    template<typename T> T getStateVar(const std::string& key);

    void setServiceType(std::string serviceType);
    void setServiceId(std::string serviceId);
    void setDescriptionPath(std::string descriptionPath);
    void setServiceDescription(std::string& description);
    void setDescriptionRequestHandler();
    void setControlPath(std::string controlPath);
    void setEventSubscriptionPath(std::string eventPath);
    void setDeviceData(DeviceData* pDeviceData);
    template<typename T> void setStateVar(std::string key, const T& val);

    void addAction(Action* pAction);
    void addStateVar(StateVar* pStateVar);
    void addEventCallbackPath(const std::string path);

    void initClient();
    void sendAction(Action* pAction);

    void sendSubscriptionRequest();
    void registerSubscription(Subscription* subscription);
    void unregisterSubscription(Subscription* subscription);

    // TODO: moderated event messaging
    // TODO: honor maximumRate and minimumDelta (specs p. 72)
    void sendEventMessage(StateVar& stateVar);
    void sendInitialEventMessage(Subscription* pSubscription);

private:
    void actionNetworkActivity(bool begin);

    DeviceData*                             _pDeviceData;
    std::string                             _vendorDomain;
    std::string                             _serviceType;
    std::string                             _serviceId;
    std::string                             _serviceVersion;
    std::string*                            _pDescription;
    std::string                             _descriptionPath;
    DescriptionRequestHandler*              _pDescriptionRequestHandler;
    Poco::URI                               _baseUri;
    std::string                             _controlPath;
    ControlRequestHandler*                  _controlRequestHandler;
    std::string                             _eventSubscriptionPath;
    std::vector<std::string>                _eventCallbackPaths;
    // PROPOSE: add EventRequestHandler* for Controller here??
    Container<Action>                       _actions;
    Container<StateVar>                     _stateVars;
    Container<StateVar>                     _eventedStateVars;
    Container<Subscription>                 _eventSubscriptions;

    Poco::FastMutex                         _serviceLock;
};


template<typename T>
T
Service::getStateVar(const std::string& key)
{
    // TODO: lock the _stateVariables map because different threads could access it
//     std::clog << "Service::getStateVar()" << std::endl;
    return _stateVars.getValue<T>(key);
}


template<typename T>
void
Service::setStateVar(std::string key, const T& val)
{
    // TODO: lock the _stateVariables map because different threads could access it
//    Log::instance()->upnp().debug("service set state var: " + key);
    // FIXME: segfault here
//    Log::instance()->upnp().debug("service type: " + getServiceType());
    _stateVars.setValue(key, val);
    if (_stateVars.get(key).getSendEvents()) {
//        Log::instance()->upnp().debug("state var " + key + " sends event message");
        sendEventMessage(_stateVars.get(key));
    }
//    Log::instance()->upnp().debug("service set state var: " + key + " finished.");
}


class Action : public Poco::Notification
{
public:
    Action() {}
    
    Action* clone();
    
    typedef Container<Argument>::Iterator ArgumentIterator;
    ArgumentIterator beginArgument() { return _arguments.begin(); }
    ArgumentIterator endArgument() { return _arguments.end(); }
    ArgumentIterator beginInArgument() { return _inArguments.begin(); }
    ArgumentIterator endInArgument() { return _inArguments.end(); }
    ArgumentIterator beginOutArgument() { return _outArguments.begin(); }
    ArgumentIterator endOutArgument() { return _outArguments.end(); }
    
    std::string getName() const { return _actionName; }
    Service* getService() const { return _pService; }
    template<typename T> const T getArgument(const std::string& name)
    {
//         std::clog << "Action::getArgument() name: " << name << std::endl;
        return _arguments.getValue<T>(name);
    }
    
    void setName(std::string name) { _actionName = name; }
    void setService(Service* pService) { _pService = pService; }
    template<typename T> void setArgument(std::string name, const T& val)
    {
//         std::clog << "Action::setArgument() name: " << name << std::endl;
        _arguments.setValue(name, val);
    }
    
    void appendArgument(Argument* pArgument);

private:
    std::string                         _actionName;
    Service*                            _pService;
    
    Container<Argument>                 _arguments;
    Container<Argument>                 _inArguments;
    Container<Argument>                 _outArguments;
};


template<class ServiceClass>
class ActionThread : public Poco::Runnable
{
public:
    typedef void (ServiceClass::*Callback)(Action* pAction);
    
    ActionThread(ServiceClass* pObject, Callback method, Action* arg):
        _pObject(pObject),
        _method(method),
        _arg(arg)
    {
    }
    
    void run()
    {
        (_pObject->*_method)(_arg);
    }
    
    void start()
    {
        Poco::Thread t;
        t.start(*this);
    }
    
private:
    ServiceClass*   _pObject;
    Callback        _method;
    Action*         _arg;
};


class Argument : public Variant
{
public:
    Argument() : Variant() {}
    template<typename T> Argument(const T& val) : Variant(val) {}

    std::string getName() const { return _name; }
    std::string getDirection() const { return _in ? "in" : "out"; }
    const std::string& getRelatedStateVarName() const { return _relatedStateVar; }
    StateVar* getRelatedStateVarReference() const;
    Action* getAction() const { return _pAction; }

    void setName(std::string name) { _name = name; }
    void setDirection(std::string direction) { _in = (direction == "in") ? true : false; }
    void setRelatedStateVar(std::string relatedStateVar) { _relatedStateVar = relatedStateVar; }
    void setAction(Action* pAction) { _pAction = pAction; }

private:
    std::string     _name;
    std::string     _relatedStateVar;
    bool            _in;
    Action*         _pAction;
};


class UserInterface
{
    friend class Controller;
    friend class Service;
    
public:
    // TODO: pass command line arguments to user interface gui-toolkit
    virtual int eventLoop() { return 0; }
    virtual void initGui() {}
    virtual void showMainWindow() {}
//     virtual void hideMainWindow() {};
    virtual Sys::Visual* getVisual() { return 0; }
    virtual void setFullscreen(bool fullscreen) {}
    virtual void resize(int width, int height) {}
    
protected:
    virtual void beginAddDevice(int position) {}
    virtual void beginRemoveDevice(int position) {}
    virtual void endAddDevice(int position) {}
    virtual void endRemoveDevice(int position) {}
    
    virtual void error(const std::string& message) {}
    virtual void beginNetworkActivity() {}
    virtual void endNetworkActivity() {}
};


class CtlDeviceCode
{
    friend class Controller;

public:
    CtlDeviceCode(Device* pDevice) : _pDevice(pDevice) {}

    Device* getDevice() const { return _pDevice; }

protected:
    virtual void eventHandler(StateVar* stateVar) {}
    void init();

    Device*         _pDevice;
};


} // namespace Omm

#endif
