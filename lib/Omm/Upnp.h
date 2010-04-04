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

#ifndef OMMUPNP_H
#define OMMUPNP_H

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
#include <Poco/StringTokenizer.h>
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
#include <Poco/ClassLoader.h>
#include <Poco/String.h>
#include <Poco/Environment.h>
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Format.h>
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

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    OMM_VERSION        = "0.1.0";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";

class Device;
class DeviceRoot;
class Service;
class Action;
class Argument;
class StateVar;
class DeviceRoot;
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
class NetworkInterfaceNotification;


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


template<class C>
class PluginLoader
{
public:
    PluginLoader() :
        _pPluginLoader(new Poco::ClassLoader<C>),
        _pluginPath(":/usr/lib/omm:/usr/local/lib/omm")
    {}
    
    ~PluginLoader()
    {
        delete _pPluginLoader;
    }
    
    void loadPlugin(const std::string& name)
    {
        try {
            _pluginPath = Poco::Environment::get("OMM_PLUGIN_PATH") + _pluginPath;
            Log::instance()->upnp().debug(Poco::format("plugin loader OMM_PLUGIN_PATH is: %s", _pluginPath));
        }
        catch (Poco::NotFoundException) {
            Log::instance()->upnp().debug(Poco::format("plugin loader OMM_PLUGIN_PATH not set, standard search path is: %s", _pluginPath));
        }
        Poco::StringTokenizer pathSplitter(_pluginPath, ":");
        Poco::StringTokenizer::Iterator it;
        for (it = pathSplitter.begin(); it != pathSplitter.end(); ++it) {
            if (*it == "") {
                continue;
            }
            try {
                _pPluginLoader->loadLibrary((*it) + "/libomm" + name + ".so");
            }
            catch (Poco::NotFoundException) {
                continue;
            }
            catch (Poco::LibraryLoadException) {
                continue;
            }
            break;
        }
        if (it == pathSplitter.end()) {
            throw Poco::NotFoundException();
        }
    }
    
    C* create(const std::string& className)
    {
        return _pPluginLoader->create(className);
    }
    
private:
    Poco::ClassLoader<C>*    _pPluginLoader;
    std::string              _pluginPath;
};


class Icon {
    friend class IconRequestHandler;
    friend class DeviceRoot;
    friend class Device;
    friend class DeviceDescriptionWriter;
    
public:
    Icon(int width, int height, int depth, const std::string& mime, const std::string& uri = "");
    ~Icon();
    
private:
    void retrieve(const std::string& uri);
    
    int                     _width;
    int                     _height;
    int                     _depth;
    Poco::Net::MediaType    _mime;
    void*                   _pData;
    std::size_t             _size;
    std::string             _requestUri;
    std::string             _iconPath;
};


class NetworkInterfaceManager
{
public:
    static NetworkInterfaceManager* instance();
    // clients like DeviceRoot and Controller can register here
    void registerInterfaceChangeHandler(const Poco::AbstractObserver& observer);
    // some OS dependent hardware abstraction layer can add and remove devices here
    void addInterface(const std::string& name);
    void removeInterface(const std::string& name);
    // this address can be announced for the HTTP servers to be reached at
    const Poco::Net::IPAddress& getValidInterfaceAddress();
    
private:
    NetworkInterfaceManager();
    void findValidIpAddress();
    
    static NetworkInterfaceManager*     _pInstance;
    std::vector<std::string>            _interfaceList;
    Poco::Net::IPAddress                _validIpAddress;
    Poco::NotificationCenter            _notificationCenter;
    bool                                _loopbackProvided;
    Poco::Net::IPAddress                _loopbackAddress;
};


class SsdpNetworkInterface
{
    friend class SsdpSocket;
    
public:
    SsdpNetworkInterface(const std::string& interfaceName, SsdpSocket* pSsdpSocket);
    ~SsdpNetworkInterface();
    
private:
    void onReadable(Poco::Net::ReadableNotification* pNotification);
    
    std::string                     _name;
    SsdpSocket*                     _pSsdpSocket;
    Poco::Net::NetworkInterface*    _pInterface;
    Poco::Net::MulticastSocket*     _pSsdpListenerSocket;
    Poco::Net::MulticastSocket*     _pSsdpSenderSocket;
    char*                           _pBuffer;
    
    enum {
        BUFFER_SIZE = 65536 // Max UDP Packet size is 64 Kbyte.
            // Note that each SSDP message must fit into one UDP Packet.
    };
};


class SsdpSocket
{ 
    friend class SsdpNetworkInterface;
public:
//     SsdpSocket(const Poco::Net::NetworkInterface& interface);
    SsdpSocket();
    ~SsdpSocket();
    
//     const Poco::Net::NetworkInterface& getInterface() { return _interface; }
    void addInterface(const std::string& name);
    void removeInterface(const std::string& name);
    void setObserver(const Poco::AbstractObserver& observer);
    void start();
    
    void sendMessage(SsdpMessage& message, const std::string& interface = "*", const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    
private:
//     void onUnicastReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
    
//     Poco::Net::NetworkInterface     _interface;
//     Poco::Net::MulticastSocket*     _pSsdpSocket;
//     Poco::Net::MulticastSocket*     _pSsdpSenderSocket;
    
    std::map<std::string,SsdpNetworkInterface*>     _interfaces;
    Poco::Net::SocketReactor                        _reactor;
    Poco::Thread                                    _listenerThread;
    Poco::NotificationCenter                        _notificationCenter;
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
    
    Poco::Random                        _randomTimeGenerator;
    Poco::Timer                         _sendTimer;
    SsdpSocket*                         _socket;
    std::vector<SsdpMessage*>           _ssdpMessages;
    int                                 _repeat;
    long                                _delay;
    bool                                _continuous;
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
//             std::cerr << "Container::setValue() could not find key"<< std::endl;
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
    
private:
    std::map<std::string,E*>    _pEntities;
    std::vector<E*>             _keys;
};


class DescriptionReader
{
public:
//     DescriptionReader(const std::string& deviceDescriptionUri);  
    DescriptionReader();
    ~DescriptionReader();
    
    
protected:
//     virtual void fixQuirkyPath(std::string& path);
//     virtual void fixQuirkyPathRemoveFileName(std::string& path);
    virtual std::string& getDescription(const std::string& path) = 0;
    
    void releaseDescriptionDocument();
    
    Poco::XML::Node* parseDescription(const std::string& description);
    DeviceRoot* parseDeviceRoot(Poco::XML::Node* pNode);
    Device* device(Poco::XML::Node* pNode, DeviceRoot* pDeviceRoot);
    Service* service(Poco::XML::Node* pNode);
    Action* action(Poco::XML::Node* pNode);
    Argument* argument(Poco::XML::Node* pNode);
    StateVar* stateVar(Poco::XML::Node* pNode);
    
    
//     Poco::URI                           _deviceDescriptionUri;  // this is the base URI
//     std::string                         _deviceDescriptionPath;
    // TODO: replace _nodeStack by argument Node* in the factory methods
//     std::stack<Poco::XML::Node*>        _nodeStack;
    std::stack<Poco::XML::Document*>    _pDocStack;
//     DeviceRoot*                         _pDeviceRoot;
};


class UriDescriptionReader : public DescriptionReader
{
public:
    // deviceDescriptionUri is the base URI
//     UriDescriptionReader(const std::string& deviceDescriptionUri);
//     UriDescriptionReader(Poco::URI uri, const std::string& deviceDescriptionPath);
    DeviceRoot* deviceRoot(const std::string& deviceDescriptionUri);
    
private:
    virtual std::string& getDescription(const std::string& relativeUri);
    
    std::string   _deviceDescriptionUri; // this is the base URI
//     Poco::URI _uri;
};


class StringDescriptionReader : public DescriptionReader
{
public:
    StringDescriptionReader(std::map<std::string,std::string*>& stringMap);
    
    DeviceRoot* deviceRoot(const std::string& deviceDescriptionKey);
    
private:
    virtual std::string& getDescription(const std::string& descriptionKey);
    
    std::map<std::string,std::string*>*  _pStringMap;
};


class HttpSocket
{
    friend class DeviceRoot;
    
public:
//     HttpSocket(Poco::Net::NetworkInterface interface);
    HttpSocket(Poco::Net::IPAddress address);
    ~HttpSocket();
    
    std::string getServerUri() { return "http://" + _httpServerAddress.toString() + "/"; }
    void init();
    void startServer();
    void stopServer();
    
private:
//     Poco::Net::NetworkInterface           _interface;
    Poco::Net::IPAddress                  _address;
    Poco::Net::SocketAddress              _httpServerAddress;
    DeviceRequestHandlerFactory*          _pDeviceRequestHandlerFactory;
    Poco::NotificationCenter              _notificationCenter;
    Poco::Net::HTTPServer*                _pHttpServer;
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


class Service {
public:
    Service() {}
    ~Service();
    
    std::string getServiceType() const { return _serviceType; }
    std::string getServiceId() const { return _serviceId; }
    std::string getDescriptionPath() const { return _descriptionPath; }
    std::string* getDescription() const { return _pDescription; }
    std::string getControlPath() const { return _controlPath; }
    std::string getEventPath() const { return _eventPath; }
    DescriptionRequestHandler* getDescriptionRequestHandler() const { return _pDescriptionRequestHandler; }
    ControlRequestHandler* getControlRequestHandler() const { return _controlRequestHandler; }
    Device* getDevice() const { return _pDevice; }
    Action* getAction(const std::string& actionName) { return &_actions.get(actionName); }
    Subscription* getSubscription(const std::string& sid) { return &_eventSubscriptions.get(sid); }
    StateVar* getStateVarReference(const std::string& key) { return &_stateVars.get(key); }
    template<typename T> T getStateVar(const std::string& key);
    
    void setServiceType(std::string serviceType) { _serviceType = serviceType; }
    void setServiceId(std::string serviceId) { _serviceId = serviceId; }
    void setDescriptionPath(std::string descriptionPath) { _descriptionPath = descriptionPath; }
    void setDescription(std::string& description) { _pDescription = &description; }
    void setDescriptionRequestHandler();
    void setControlPath(std::string controlPath) { _controlPath = controlPath; }
    void setEventPath(std::string eventPath) { _eventPath = eventPath; }
    void setDevice(Device* pDevice) { _pDevice = pDevice; }
    template<typename T> void setStateVar(std::string key, const T& val);
    
    void addAction(Action* pAction);
    void addStateVar(StateVar* pStateVar);
    
    void initClient();
    void sendAction(Action* pAction);
    
    typedef Container<StateVar>::Iterator StateVarIterator;
    StateVarIterator beginStateVar() { return _stateVars.begin(); }
    StateVarIterator endStateVar() { return _stateVars.end(); }
    StateVarIterator beginEventedStateVar() { return _eventedStateVars.begin(); }
    StateVarIterator endEventedStateVar() { return _eventedStateVars.end(); }
    
    typedef Container<Action>::Iterator ActionIterator;
    ActionIterator beginAction() { return _actions.begin(); }
    ActionIterator endAction() { return _actions.end(); }
    
    typedef Container<Subscription>::Iterator SubscriptionIterator;
    SubscriptionIterator beginEventSubscription() { return _eventSubscriptions.begin(); }
    SubscriptionIterator endEventSubscription() { return _eventSubscriptions.end(); }
    
    void registerSubscription(Subscription* subscription);
    void unregisterSubscription(Subscription* subscription);
    
    // TODO: moderated event messaging
    // TODO: honor maximumRate and minimumDelta (specs p. 72)
    void sendEventMessage(StateVar& stateVar);
    void sendInitialEventMessage(Subscription* pSubscription);
    
private:
    Device*                                 _pDevice;
    std::string                             _vendorDomain;
    std::string                             _serviceType;
    std::string                             _serviceId;
    std::string                             _serviceVersion;
    std::string*                            _pDescription;
    std::string                             _descriptionPath;
    DescriptionRequestHandler*              _pDescriptionRequestHandler;
    std::string                             _controlPath;
    ControlRequestHandler*                  _controlRequestHandler;
    Poco::Net::HTTPClientSession*                _pControlRequestSession;
    std::string                             _eventPath;
    Container<Action>                       _actions;
    Container<StateVar>                     _stateVars;
    Container<StateVar>                     _eventedStateVars;
    Container<Subscription>                 _eventSubscriptions;
    
    Poco::FastMutex                         _serviceLock;
};


// TODO: finish Device Property stuff ...
// class Property
// {
//     std::string getValue() { return _value; }
//     void setValue(const std::string& val) { _value = val; }
//     
// private:
//     std::string     _value;
// };


class Device
{
    friend class DeviceDescriptionWriter;
    
public:
    Device();
    ~Device();
    
    typedef Container<Service>::Iterator ServiceIterator;
    ServiceIterator beginService() { return _services.begin(); }
    ServiceIterator endService() { return _services.end(); }
    
    typedef Container<std::string>::Iterator PropertyIterator;
    PropertyIterator beginProperty() { return _properties.begin(); }
    PropertyIterator endProperty() { return _properties.end(); }
    
    typedef std::vector<Icon*>::iterator IconIterator;
    IconIterator beginIcon() { return _iconList.begin(); }
    IconIterator endIcon() { return _iconList.end(); }
    
    DeviceRoot* getDeviceRoot() const { return _pDeviceRoot; }
    std::string getUuid() const { return _uuid; }
    std::string getDeviceType() const { return _deviceType; }
    const std::string& getFriendlyName() { return getProperty("friendlyName"); }
    Service* getService(std::string serviceType) { return &_services.get(serviceType); }
    const std::string& getProperty(const std::string& name) { return _properties.get(name); }
    
    void setDeviceRoot(DeviceRoot* pDeviceRoot) { _pDeviceRoot = pDeviceRoot; }
    void setUuid(std::string uuid) { _uuid = uuid; }
    void setRandomUuid();
    void setDeviceType(std::string deviceType) { _deviceType = deviceType; }
    void setProperty(const std::string& name, const std::string& val) { _properties.get(name) = val; }
    
    void addProperty(const std::string& name, const std::string& val) { _properties.append(name, new std::string(val)); }
    void addService(Service* pService);
    void addIcon(Icon* pIcon);
    
private:
    DeviceRoot*                         _pDeviceRoot;
    std::string                         _uuid;
    std::string                         _deviceType;
    Container<Service>                  _services;
    Container<std::string>              _properties;
    std::vector<Icon*>                  _iconList;
};


class DeviceRootImplAdapter;

class DeviceRoot
{
    friend class DeviceRootImplAdapter;
    
public:
    DeviceRoot();
    ~DeviceRoot();
    
    typedef Container<Device>::Iterator DeviceIterator;
    DeviceIterator beginDevice() { return _devices.begin(); }
    DeviceIterator endDevice() { return _devices.end(); }
    
    typedef std::map<std::string,Service*>::iterator ServiceTypeIterator;
    ServiceTypeIterator beginServiceType() { return _serviceTypes.begin(); }
    ServiceTypeIterator endServiceType() { return _serviceTypes.end(); }
    
    
    /*const*/ Device* getDevice(std::string uuid) /*const*/ { return &_devices.get(uuid); }
    Device* getRootDevice() const { return _pRootDevice; }
    std::string* getDeviceDescription() const { return _pDeviceDescription; }
    const std::string& getDescriptionUri() const { return _descriptionUri; }
    Service* getServiceType(const std::string& serviceType);
    
    void setRootDevice(Device* pDevice) { _pRootDevice = pDevice; }
    void setDeviceDescription(std::string& description) { _pDeviceDescription = &description; }
    void setDescriptionUri(const std::string uri) { _descriptionUri = uri; }
    void addDevice(Device* pDevice) { _devices.append(pDevice); }
    void addServiceType(std::string serviceType, Service* pService) { _serviceTypes[serviceType] = pService; }
    
    void print();
    
    void initDevice();
    void initController();
    void startSsdp();
    void startHttp();
    void stopSsdp();
    void stopHttp();
    
    void registerActionHandler(const Poco::AbstractObserver& observer)
    {
        _httpSocket._notificationCenter.addObserver(observer);
    }
    
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler);
    
    void sendMessage(SsdpMessage& message, const std::string& interface, const Poco::Net::SocketAddress& receiver = Poco::Net::SocketAddress(SSDP_FULL_ADDRESS));
    void handleSsdpMessage(SsdpMessage* pNf);
    void handleNetworkInterfaceChangedNotification(NetworkInterfaceNotification* pNotification);
        
    void postAction(Action* pAction) { _httpSocket._notificationCenter.postNotification(pAction); }
    
    void setImplAdapter(DeviceRootImplAdapter* implAdapter) { _pDeviceRootImplAdapter = implAdapter; }
    void initStateVars(const std::string& serviceType, Service* pThis);

    
private:
//     Poco::URI                       _baseUri;              // base URI for control URI and event URI
//     Poco::URI                       _descriptionUri;       // for controller to download description
    std::string                     _descriptionUri;
    std::string*                    _pDeviceDescription;
    Container<Device>               _devices;
    Device*                         _pRootDevice;
    std::map<std::string,Service*>  _serviceTypes;
    SsdpSocket                      _ssdpSocket;
    SsdpMessageSet                  _ssdpNotifyAliveMessages;
    SsdpMessageSet                  _ssdpNotifyByebyeMessages;
    HttpSocket                      _httpSocket;
    DescriptionRequestHandler*      _descriptionRequestHandler;
    DeviceRootImplAdapter*          _pDeviceRootImplAdapter;
};


class DeviceRootImplAdapter
{
    friend class DeviceRoot;
    
public:
    DeviceRootImplAdapter();
    ~DeviceRootImplAdapter();
    
    void start();
    void stop();
    
    void setUuid(std::string uuid, int deviceNumber = 0);
    void setRandomUuid(int deviceNumber = 0);
    void setFriendlyName(const std::string& friendlyName, int deviceNumber = 0);
    void addIcon(Icon* pIcon, int deviceNumber = 0);
    
protected:
    virtual void actionHandler(Action* action) = 0;
    virtual void initStateVars(const std::string& serviceType, Service* pThis) = 0;
    
    std::map<std::string,std::string*>  _descriptions;
    // _deviceRoot is the link into the "dynamic-string-world".
    DeviceRoot*                         _pDeviceRoot;
};


class Controller
{
public:
    Controller();
    ~Controller();

    void start();
    virtual void deviceAdded(DeviceRoot* pDeviceRoot) = 0;
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot) = 0;
    
private:
    void sendMSearch();
    void handleSsdpMessage(SsdpMessage* pMessage);
    void handleNetworkInterfaceChangedNotification(NetworkInterfaceNotification* pNotification);
    void discoverDevice(const std::string& location);
    void addDevice(DeviceRoot* pDevice);
    void removeDevice(const std::string& uuid);
    
    SsdpSocket              _ssdpSocket;
    Container<DeviceRoot>   _devices;
//     Poco::FastMutex     _controllerLock;
};


class ControllerImplAdapter
{
public:
    Device* getDevice() const { return _pDevice; }

protected:
    ControllerImplAdapter(Device* pDevice) : _pDevice(pDevice) {}
    
    virtual void eventHandler(StateVar* stateVar) = 0;
    void init();
    
    Device*         _pDevice;
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
//     std::clog << "Service::setStateVar() name: " << key << std::endl;
    // FIXME: segfault here
//     std::clog << "service type: " << getServiceType() << std::endl;
    _stateVars.setValue(key, val);
    if (_stateVars.get(key).getSendEvents()) {
//         std::clog << "Service::setStateVar() " << key << " sends event message" << std::endl;
        sendEventMessage(_stateVars.get(key));
    }
//     std::clog << "Service::setStateVar() finished" << std::endl;
}


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

} // namespace Omm

#endif
