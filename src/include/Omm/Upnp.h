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

#ifndef Upnp_INCLUDED
#define Upnp_INCLUDED

#include <string>
#include <vector>
#include <map>


#ifdef POCO_VERSION_HEADER_FOUND
#include <Poco/Version.h>
#endif
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/URI.h>
#include <Poco/Timestamp.h>
#include <Poco/Timer.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Net/MediaType.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/NotificationCenter.h>

#include "Sys/Visual.h"
#include "Log.h"
#include "Net.h"
#include "UpnpTypes.h"

namespace Omm {

static const std::string    OMM_VERSION         = __OMM_VERSION_STRING__;
static const std::string    UPNP_VERSION        = "1.0";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";

class DescriptionProvider;
class DeviceManager;
class Controller;
class ControllerUserInterface;
class DeviceContainer;
class Device;
class DeviceGroup;
class DeviceGroupDelegate;
class DeviceData;
class DevDeviceCode;
class CtlDeviceCode;
class Service;
class Action;
class Socket;
class UpnpRequestHandler;
class DescriptionRequestHandler;
class SsdpMessage;
class SsdpMessageSet;


#ifndef NDEBUG
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
    Poco::Logger*   _pEventLogger;
};
#endif // NDEBUG


class Icon
{
    friend class IconRequestHandler;
    friend class DeviceServer;
    friend class DeviceContainer;
    friend class Device;
    friend class DeviceData;
    friend class DescriptionWriter;

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

    virtual const std::string& getValue() {
        return _val;
    }
    void getValue(std::string& val) {
        val = _val;
    }
    void getValue(bool& val) { val = (_val == "1" || _val == "true" || _val == "yes") ? true : false; }
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
    typedef typename std::map<std::string, E*>::iterator KeyIterator;
    typedef typename std::vector<E*>::iterator Iterator;
    typedef typename std::vector<std::string>::iterator PosIterator;

    E& get(std::string key)
    {
        KeyIterator it = _elementMap.find(key);
        if (it != _elementMap.end()) {
            return *(*it).second;
        }
        else {
            LOG(upnp, error, "container has no element with key: " + key);
            throw Poco::Exception("container has no element with key: " + key);
        }
    }

    E& get(int index) const
    {
        return *_elementVector[index];
    }

    void append(std::string key, E* pElement)
    {
        KeyIterator it = _elementMap.find(key);
        if (it == _elementMap.end()) {
            _elementMap[key] = pElement;
            _elementVector.push_back(pElement);
            _keyVector.push_back(key);
        }
        else {
            LOG(upnp, error, "could not append element to container, key already present , ignoring): " + key);
        }
    }

    int size() const
    {
        return _elementVector.size();
    }

    int position(const std::string& key)
    {
        PosIterator it = find(_keyVector.begin(), _keyVector.end(), key);
        if (it != _keyVector.end()) {
            return it - _keyVector.begin();
        }
        else {
            LOG(upnp, error, "could not find position of element in container, key not found: " + key);
            throw Poco::Exception("");
        }
    }

    bool contains(const std::string& key)
    {
        return _elementMap.find(key) != _elementMap.end();
    }

    void remove(std::string key)
    {
        KeyIterator it = _elementMap.find(key);
        if (it != _elementMap.end()) {
            _elementMap.erase(key);
            _elementVector.erase(_elementVector.begin() + position(key));
            _keyVector.erase(_keyVector.begin() + position(key));
        }
        else {
            LOG(upnp, error, "could not remove element from container, key not found: " + key);
            throw Poco::Exception("");
        }
    }

    void replace(const std::string& key, E* pElement)
    {
        KeyIterator it = _elementMap.find(key);
        if (it == _elementMap.end()) {
            LOG(upnp, error, "could not replace element, key not found: " + key);
            return;
        }
        int i = position(key);
        _elementVector[i] = pElement;
        LOG(upnp, debug, "replace position: " + Poco::NumberFormatter::format(i) + ", old: " + Poco::NumberFormatter::format((*it).second) + ", new: " + Poco::NumberFormatter::format(pElement));
        (*it).second = pElement;
    }

    template<typename T> T getValue(const std::string& key)
    {
        Variant* e = (*_elementMap.find(key)).second;
        if (e) {
            T res;
            e->getValue(res);
            return res;
        } else {
             LOG(upnp, error, "get container value, could not find key: " + key);
//            throw Poco::Exception("");
            return T();
        }
    }

    template<typename T> void setValue(const std::string& key, const T& val)
    {
        if (_elementMap.find(key) == _elementMap.end()) {
            LOG(upnp, error, "could not set container value, key not found: " + key);
            return;
        }
        Variant* e = (*_elementMap.find(key)).second;
        if (e) {
            e->setValue(val);
        }
        else {
             LOG(upnp, error, "set container value, pointer to Variant is invalid , (ignoring)");
        }
    }

    Iterator begin()
    {
        return _elementVector.begin();
    }

    Iterator end()
    {
        return _elementVector.end();
    }

    KeyIterator beginKey()
    {
        return _elementMap.begin();
    }

    KeyIterator endKey()
    {
        return _elementMap.end();
    }

    void clear()
    {
        _elementMap.clear();
        _elementVector.clear();
        _keyVector.clear();
    }

    void deepClear()
    {
        for (Iterator it = begin(); it != end(); ++it) {
            delete *it;
        }
        clear();
    }

private:
    std::map<std::string, E*>   _elementMap;
    std::vector<E*>             _elementVector;
    std::vector<std::string>    _keyVector;
};


class DeviceManager //: public Util::Startable
{
    friend class ControlRequestHandler;
    friend class DeviceContainer;
    friend class Device;

public:
    typedef std::string State;
    static const std::string Transitioning;
    static const std::string Stopped;
    static const std::string Local;
    static const std::string Public;
    static const std::string PublicLocal;

    DeviceManager(Socket* pNetworkListener);
    virtual ~DeviceManager();

    typedef Container<DeviceContainer>::Iterator DeviceContainerIterator;
    DeviceContainerIterator beginDeviceContainer();
    DeviceContainerIterator endDeviceContainer();

    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer);
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer);
    int getDeviceContainerCount();

    void init();
    void setState(State newState);
    State getState();

    std::string getHttpServerUri();

    void postDeviceNotification(Poco::Notification* pNotification);
    void registerDeviceNotificationHandler(const Poco::AbstractObserver& observer);

protected:
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler);
    void handleNetworkInterfaceChangedNotification(Net::NetworkInterfaceNotification* pNotification);

    virtual void handleSsdpMessage(SsdpMessage* pMessage) {}

    virtual void startSsdp();
    virtual void stopSsdp();

    void startHttp();
    void stopHttp();

//    void clear();

    State                                      _state;
    Container<DeviceContainer>                 _deviceContainers;
    Poco::NotificationCenter                   _deviceNotificationCenter;
    Socket*                                    _pSocket;
    Poco::FastMutex                            _stateLock;
};


class Controller : public DeviceManager
{
    friend class DeviceContainer;

public:
    Controller();
    virtual ~Controller();

    void setState(State newState);
    void registerDeviceGroup(DeviceGroup* pDeviceGroup);
    DeviceGroup* getDeviceGroup(const std::string& deviceType);

    virtual void signalNetworkActivity(bool on) {}

protected:
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}

    virtual void addDeviceGroup(DeviceGroup* pDeviceGroup, bool begin) {}
    /// called when a new device type is discovered.
    virtual void removeDeviceGroup(DeviceGroup* pDeviceGroup, bool begin) {}
    /// called when a last device of a certain type disappears.

    // next two methods should go into private api
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer);
    /// adds device container if not already added before (checks for uuid of root device).
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer);

private:
    virtual void startSsdp();
    void sendMSearch();
    void handleSsdpMessage(SsdpMessage* pMessage);
//    void handleNetworkInterfaceChangedNotification(Net::NetworkInterfaceNotification* pNotification);
    void discoverDeviceContainer(const std::string& location);
//    void update();
    void subscribeAllDevicesInContainer(DeviceContainer* pDeviceContainer);
    void unsubscribeAllDevices();
    void stopDeviceContainerTimers();
    void deleteDeviceContainers();

    std::map<std::string, DeviceGroup*>         _deviceGroups;
    bool                                        _featureSubscribeToEvents;
};


class Device
{
    friend class DescriptionReader;
    friend class DescriptionWriter;
    friend class DeviceContainer;
    friend class DeviceManager;
    friend class DeviceServer;
    friend class DeviceGroup;
    friend class EventMessageReader;
    friend class Controller;
    friend class ControlRequestHandler;

public:
    Device();
    virtual ~Device();

    void deepDelete();

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
    std::string getUuid() const;
    std::string getDeviceType() const;
    Service* getService(std::string serviceType);
    /// Get the service data of a device. If device has no service of type
    /// serviceType, 0 is returned.
    const std::string& getFriendlyName();
    const std::string& getProperty(const std::string& name);

    void setDeviceContainer(DeviceContainer* pDeviceContainer);
    void setDeviceData(DeviceData* pDeviceData);
    void setDevDeviceCode(DevDeviceCode* pDevDevice);
    void setCtlDeviceCode(CtlDeviceCode* pCtlDevice);
    void setUuid(std::string uuid);
    void setRandomUuid();
    void setProperty(const std::string& name, const std::string& val);
    void setFriendlyName(const std::string& name);

    void addIcon(Icon* pIcon);

    void initStateVars();
    void controllerSubscribeEventing();
    void controllerUnsubscribeEventing();

    void stopSubscriptionTimer();

    // some devices (e.g. media servers) need some action to be started and stopped
    virtual void start() {}
    virtual void stop() {}

    virtual void addCtlDeviceCode() {}

protected:
    virtual void initController() {}
    virtual void selected() {}
    void postAction(Action* pAction);

private:
    void addProperty(const std::string& name, const std::string& val);
    void addService(Service* pService);
    DeviceData* getDeviceData() const;
    DevDeviceCode* getDevDeviceCode() const;
    CtlDeviceCode* getCtlDeviceCode() const;
    void registerActionHandler(const Poco::AbstractObserver& observer);

    DeviceContainer*                    _pDeviceContainer;
    DeviceData*                         _pDeviceData;
    DevDeviceCode*                      _pDevDeviceCode;
    CtlDeviceCode*                      _pCtlDeviceCode;
    Poco::NotificationCenter            _notificationCenter;
};


class DeviceServer : public DeviceManager
{
public:
    DeviceServer();
    virtual ~DeviceServer();

    void init();
    void setState(State newState);

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
    friend class DescriptionReader;
    friend class Service;
    friend class DeviceData;
    friend class SsdpNotifyAliveWriter;

public:
    DeviceContainer();
    ~DeviceContainer();

    typedef Container<Device>::Iterator DeviceIterator;
    DeviceIterator beginDevice();
    DeviceIterator endDevice();

    typedef std::map<std::string,Service*>::iterator ServiceTypeIterator;
    ServiceTypeIterator beginServiceType();
    ServiceTypeIterator endServiceType();

    void addDevice(Device* pDevice);
    void clear();
    void replaceDevice(Device* pOldDevice, Device* pNewDevice);
    /// replaces pOldDevice with pNewDevice in device tree (data only, code is not touched).

    DeviceManager* getDeviceManager();
    /*const*/ Device* getDevice(std::string uuid) /*const*/;
    int getDeviceCount();
    Device* getRootDevice() const;
    Controller* getController() const;

    void setRootDevice(Device* pDevice);

    // ommgen needs this
    void rewriteDescriptions();
    std::string* getDeviceDescription() const;

private:
    const std::string& getDescriptionUri() const;
    std::string getDescriptionPath() const;
    Service* getServiceType(const std::string& serviceType);

    void setDeviceManager(DeviceManager* pDeviceManager);
    void setDeviceDescription(std::string& description);
    void setDescriptionUri(const std::string uri);
    void addServiceType(Service* pService);

    void print();

    void initUuid();
//    void initStateVars();
    void initDeviceDescriptionHandler();
    void initDevice();

    void writeSsdpMessages();
    void restartSsdpTimer(int maxAge);
    void stopSsdpTimer();
    void expireSsdp(Poco::Timer& timer);

//     Poco::URI                       _baseUri;              // base URI for control URI and event URI
    std::string                             _stringDescriptionUri;            // for controller to download description
    Poco::URI                               _descriptionUri;
    std::string*                            _pDeviceDescription;
    DeviceManager*                          _pDeviceManager;
    Container<Device>                       _devices;
    Device*                                 _pRootDevice;
    std::map<std::string,Service*>          _serviceTypes;
    SsdpMessageSet*                         _pSsdpNotifyAliveMessages;
    SsdpMessageSet*                         _pSsdpNotifyByebyeMessages;
    Poco::Timer*                            _pSsdpTimer;
    Poco::TimerCallback<DeviceContainer>    _expireSsdpCallback;
    static Poco::AtomicCounter              _ssdpTimerReleaseCounter;
    DescriptionRequestHandler*              _descriptionRequestHandler;
    // TODO: remove _pController, it should be a specialized _pDeviceManager
    Controller*                             _pController;
};


class DeviceGroup
/// Contains only devices of the same type (which can be in different containers).
/// May corresponds to one tab in the user interface.
/// May be loaded as a plugin when a new device type is discovered.
{
    friend class Controller;
    friend class DeviceGroupDelegate;

public:
    DeviceGroup(const std::string& deviceType, const std::string& shortName);
    DeviceGroup(DeviceGroupDelegate* pDeviceGroupDelegate);

    int getDeviceCount() const;
    Device* getDevice(int index) const;
    Device* getDevice(const std::string& uuid);
    Controller* getController() const;
    void selectDevice(Device* pDevice);
    Device* getSelectedDevice() const;
    void clearDevices();

    virtual std::string getDeviceType();
    virtual std::string shortName();
    virtual Icon* groupIcon();

    virtual Device* createDevice();
    /// factory method to create a device of a certain type.
    virtual void showDeviceGroup() {}

    virtual void addDevice(Device* pDevice, int index, bool begin) {}
    virtual void removeDevice(Device* pDevice, int index, bool begin) {}
    virtual void selectDevice(Device* pDevice, int index) {}
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}

protected:
    virtual void init() {}

    void addDevice(Device* pDevice);
    void removeDevice(Device* pDevice);

private:
    void initDelegate();

    DeviceGroupDelegate*            _pDeviceGroupDelegate;
    Controller*                     _pController;
    std::vector<DeviceContainer>    _deviceContainers;
    Container<Device>               _devices;
    Device*                         _pSelectedDevice;
    std::string                     _preferredDeviceUuid;
    std::string                     _deviceType;
    std::string                     _shortName;
};


} // namespace Omm

#endif
