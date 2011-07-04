/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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


#ifndef UpnpInternal_INCLUDED
#define	UpnpInternal_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <stack>


#include <Poco/Thread.h>
#include <Poco/Notification.h>
#include <Poco/Timer.h>
#include <Poco/Mutex.h>
#include <Poco/String.h>
#include <Poco/Exception.h>
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


#include "Upnp.h"

namespace Omm {

class DescriptionProvider;
class DeviceData;
class DevDeviceCode;
class CtlDeviceCode;
class Service;
class Action;
class Argument;
class StateVar;
class ControlRequestHandler;
class Socket;
class HttpSocket;
class UpnpRequestHandler;
class DescriptionRequestHandler;
class UpnpRequestHandlerFactory;
class ControlRequestHandler;
class SsdpSocket;
class SsdpMessage;
class SsdpMessageSet;
class Subscription;
class EventMessageQueue;


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
    DeviceData* rootDeviceData();
    /// Parses the device description, creates and returns the device data of
    /// the root device. Subdevices, if present, are ignored. This is used by
    /// the device side, when building the internal device tree from a memory description.

protected:
    virtual std::string& retrieveDescription(const std::string& descriptionUri) = 0;
    /// Retrieves the description from descriptionUri, which can be relative to _deviceDescriptionUri.
    std::string   _deviceDescriptionUri;
    /// Device description URI and base URI for service descriptions.

private:
    DeviceData* deviceData(Poco::XML::Node* pNode, DeviceContainer* pDeviceContainer);
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


class MemoryDescriptionReader : public DescriptionReader
{
public:
    MemoryDescriptionReader(DescriptionProvider& deviceDescriptionProvider);

private:
    virtual std::string& retrieveDescription(const std::string& descriptionKey);

    DescriptionProvider& _deviceDescriptionProvider;
};


class UriDescriptionReader : public DescriptionReader
{
private:
    virtual std::string& retrieveDescription(const std::string& relativeUri);
};


class StateVar : public Variant
{
public:
    StateVar() {};
    template<typename T> StateVar(const T& val) : Variant(val) {}

    std::string getName() const;
    Service* getService();
    const std::string& getType() const;

    void setService(Service* pService);
    void setName(std::string name);
    void setType(std::string type);
    void setDefaultValue(std::string defaultValue);
    void setSendEvents(std::string sendEvents);
    bool getSendEvents() const;

private:
    Service*        _pService;
    std::string     _name;
    std::string     _type;
    std::string     _defaultValue;
    bool            _sendEvents;
};


class Service {
public:
    Service();
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

    void initController();
    void sendAction(Action* pAction);

    void sendSubscriptionRequest(unsigned int duration = 0, bool renew = false);
    void sendCancelSubscriptionRequest();
    void registerSubscription(Subscription* subscription);
    void unregisterSubscription(Subscription* subscription);

    // TODO: honor maximumRate and minimumDelta (specs p. 72)
    void enableEventing(bool enable = true);
    void queueEventMessage(StateVar& stateVar);
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
    Container<Action>                       _actions;
    Container<StateVar>                     _stateVars;
    Container<StateVar>                     _eventedStateVars;
    Container<Subscription>                 _eventSubscriptions;
    Subscription*                           _pControllerSubscriptionData;
    bool                                    _eventingEnabled;
    EventMessageQueue*                      _pEventMessageQueue;

    Poco::FastMutex                         _serviceLock;
};


template<typename T>
T
Service::getStateVar(const std::string& key)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serviceLock);

    return _stateVars.getValue<T>(key);
}


template<typename T>
void
Service::setStateVar(std::string key, const T& val)
{
   Poco::ScopedLock<Poco::FastMutex> lock(_serviceLock);

    _stateVars.setValue(key, val);
    StateVar& stateVar = _stateVars.get(key);

    if (_eventingEnabled && stateVar.getSendEvents()) {
        queueEventMessage(stateVar);
    }
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
    Service*                            _pService;
    std::string                         _actionName;

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


class CtlDeviceCode
{
    friend class EventMessageReader;

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

