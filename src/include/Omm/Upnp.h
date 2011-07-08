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
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Net/MediaType.h>

#include "UpnpTypes.h"
#include "Net.h"
#include "Util.h"
#include "Sys.h"

namespace Omm {

static const std::string    UPNP_VERSION        = "1.0";
static const std::string    OMM_VERSION         = "0.1.0";
static const std::string    SSDP_FULL_ADDRESS   = "239.255.255.250:1900";

class DescriptionProvider;
class DeviceManager;
class Controller;
class ControllerUserInterface;
class DeviceContainer;
class Device;
class DeviceGroup;
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
        KeyIterator it = _pElements.find(key);
        if (it != _pElements.end()) {
            return *(*it).second;
        }
        else {
            Log::instance()->upnp().error("container has no element with key: " + key);
            throw Poco::Exception("");
        }
    }
    
    E& get(int index)
    {
        return *_keys[index];
    }
    
    // TODO: make shure that only one entry exists in _pEntities and _keys for key
    void append(std::string key, E* pElement)
    {
        _pElements[key] = pElement;
        _keys.push_back(pElement);
    }
    
    void append(E* pElement)
    {
        _keys.push_back(pElement);
    }
    
    void remove(std::string key)
    {
        KeyIterator it = _pElements.find(key);
        if (it != _pElements.end()) {
            E* pEntity = (*it).second;
            _pElements.erase(key);
            _keys.erase(find(_keys.begin(), _keys.end(), pEntity));
        }
        else {
            Log::instance()->upnp().error("could not remove element from container, key not found: " + key);
            throw Poco::Exception("");
        }
    }
    
    void remove(E* pElement)
    {   
        _keys.erase(find(_keys.begin(), _keys.end(), pElement));
    }
    
    bool contains(const std::string& key)
    {
        return _pElements.find(key) != _pElements.end();
    }
    
    bool contains(E* pEntity)
    {
        return find(_keys.begin(), _keys.end(), pEntity) != _keys.end();
    }
    
    int position(const std::string& key)
    {
        KeyIterator it = _pElements.find(key);
        if (it != _pElements.end()) {
            E* pEntity = (*it).second;
            return find(_keys.begin(), _keys.end(), pEntity) - _keys.begin();
        }
        else {
            Log::instance()->upnp().error("could not find position of element in container, key not found: " + key);
            throw Poco::Exception("");
        }
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
        Variant* e = (*_pElements.find(key)).second;
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
        if (_pElements.find(key) == _pElements.end()) {
            Log::instance()->upnp().error("could not set container value, key not found: " + key);
            return;
        }
//         std::clog << "Container::setValue() found key" << std::endl;
        Variant* e = (*_pElements.find(key)).second;
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
        return _pElements.begin();
    }
        
    KeyIterator endKey()
    {
        return _pElements.end();
    }
    
    void clear()
    {
        _pElements.clear();
        _keys.clear();
    }
    
private:
    std::map<std::string,E*>    _pElements;
    std::vector<E*>             _keys;
};


class DeviceManager : public Util::Startable
{
    friend class ControlRequestHandler;
    friend class DeviceContainer;
    friend class Device;

public:
    DeviceManager(Socket* pNetworkListener);
    virtual ~DeviceManager();

    typedef Container<DeviceContainer>::Iterator DeviceContainerIterator;
    DeviceContainerIterator beginDeviceContainer();
    DeviceContainerIterator endDeviceContainer();

    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer);
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer);

    void init();
    void start();
    void stop();

    std::string getHttpServerUri();

protected:
    void registerActionHandler(const Poco::AbstractObserver& observer);
    void registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler);

    virtual void handleSsdpMessage(SsdpMessage* pMessage) {}

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

    void start();
    void stop();

    virtual DeviceGroup* createDeviceGroup(const std::string deviceType) { return 0; }
    void addDeviceGroup(DeviceGroup* pDeviceGroup);
    DeviceGroup* getDeviceGroup(const std::string& deviceType);

    // deprecated
    void setUserInterface(ControllerUserInterface* pUserInterface);
    // deprecated
    ControllerUserInterface* getUserInterface();

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

    // deprecated
    ControllerUserInterface*                      _pUserInterface;

private:
    void sendMSearch();
    void handleSsdpMessage(SsdpMessage* pMessage);
//    void handleNetworkInterfaceChangedNotification(Net::NetworkInterfaceNotification* pNotification);
    void discoverDevice(const std::string& location);
    void update();

    std::map<std::string, DeviceGroup*>        _deviceGroups;
};


class Device
{
    friend class DescriptionReader;
    friend class DescriptionWriter;
    friend class DeviceContainer;
    friend class DeviceManager;
    friend class DeviceServer;
    friend class EventMessageReader;
    friend class Controller;

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
    void initControllerEventing();

private:
    void addProperty(const std::string& name, const std::string& val);
    void addService(Service* pService);
    DeviceData* getDeviceData() const;
    DevDeviceCode* getDevDeviceCode() const;
    CtlDeviceCode* getCtlDeviceCode() const;

    DeviceContainer*                    _pDeviceContainer;
    DeviceData*                         _pDeviceData;
    DevDeviceCode*                      _pDevDeviceCode;
    CtlDeviceCode*                      _pCtlDeviceCode;
};


class DeviceServer : public DeviceManager
{
public:
    DeviceServer();
    virtual ~DeviceServer();

    void init();

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
    void initStateVars();
    void initDeviceDescriptionHandler();
    void initDevice();

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


class DeviceGroup
/// Contains only devices of the same type (which can be in different containers).
/// Corresponds to one tab in the user interface.
/// May be loaded as a plugin when a new device type is discovered.
{
    friend class Controller;

public:
    DeviceGroup();

    int getDeviceCount();

    virtual std::string getDeviceType() { return ""; }
    virtual std::string shortName() { return ""; }
    virtual Icon* groupIcon() { return 0; }

    virtual Device* createDevice(DeviceData* pDeviceData) { return 0; }
    /// factory method to create a device of a certain type associated with the
    /// corresponding device code.
    
    virtual void addDevice(Device* pDevice, int index, bool begin) {}
    virtual void removeDevice(Device* pDevice, int index, bool begin) {}
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}

protected:
    void addDevice(Device* pDevice);
    void removeDevice(Device* pDevice);

    std::vector<DeviceContainer>    _deviceContainers;
    Container<Device>               _devices;
    Device*                         _pSelectedDevice;
    std::string                     _preferredDeviceUuid;
};




//////////////////////// deprecated ///////////////////////////


class ControllerUserInterface
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
    virtual void beginAddDeviceContainer(int position) {}
    /// Before a device container is added, beginAddDevice() is called.
    virtual void beginRemoveDeviceContainer(int position) {}
    /// Before a device container is removed, beginRemoveDevice() is called.
    virtual void endAddDeviceContainer(int position) {}
    /// After adding a device container, endAddDevice() is called.
    virtual void endRemoveDeviceContainer(int position) {}
    /// After removing a device container, endRemoveDevice() is called.

    virtual void error(const std::string& message) {}
    virtual void beginNetworkActivity() {}
    virtual void endNetworkActivity() {}
};

} // namespace Omm

#endif
