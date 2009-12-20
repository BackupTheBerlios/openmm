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


#include <Poco/String.h>
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/Environment.h>

#include "upnp.h"

using Poco::NumberFormatter;
using Poco::NumberParser;
using Poco::Environment;

using namespace Jamm;


SsdpMessage::SsdpMessage()
{
    initMessageMap();
}


SsdpMessage::SsdpMessage(TRequestMethod requestMethod)
{
    initMessageMap();
    setRequestMethod(requestMethod);
    
    if (requestMethod == REQUEST_NOTIFY ||
        requestMethod == REQUEST_NOTIFY_ALIVE ||
        requestMethod == REQUEST_NOTIFY_BYEBYE) {
            setHost();
            setServer("Jamm/0.0.3");
            setCacheControl();
    }
    
    switch (requestMethod) {
    case REQUEST_NOTIFY_ALIVE:
        setNotificationSubtype(SsdpMessage::SUBTYPE_ALIVE);          // alive message
        break;
    case REQUEST_NOTIFY_BYEBYE:
        setNotificationSubtype(SsdpMessage::SUBTYPE_BYEBYE);         // byebye message
        break;
    }
}


SsdpMessage::SsdpMessage(const std::string& buf, const SocketAddress& sender)
{
    initMessageMap();
    
    for (std::map<TRequestMethod,std::string>::iterator i = m_messageMap.begin(); i != m_messageMap.end(); i++) {
        m_messageConstMap[(*i).second] = (*i).first;
    }
    
    istringstream is(buf);
    // get first line and check request method type
    std::string line;
    getline(is, line);
    Poco::trimInPlace(line);
    
    // TODO: must be case-insensitive map?!
    m_requestMethod = m_messageConstMap[line];
    m_sender = sender;
    
    m_messageHeader.read(is);
}


void
SsdpMessage::initMessageMap()
{
    m_messageMap[REQUEST_NOTIFY]            = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_NOTIFY_ALIVE]      = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_NOTIFY_BYEBYE]     = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_SEARCH]            = "M-SEARCH * HTTP/1.1";
    m_messageMap[REQUEST_RESPONSE]          = "HTTP/1.1 200 OK";
    m_messageMap[SUBTYPE_ALIVE]             = "ssdp:alive";
    m_messageMap[SUBTYPE_BYEBYE]            = "ssdp:byebye";
    m_messageMap[SSDP_ALL]                  = "ssdp:all";
    m_messageMap[UPNP_ROOT_DEVICES]         = "upnp:rootdevice";
}


SsdpMessage::~SsdpMessage()
{
}


std::string 
SsdpMessage::toString()
{
    std::ostringstream os;
    
    os << m_messageMap[m_requestMethod] << "\r\n";
    m_messageHeader.write(os);
    os << "\r\n";
    
    return os.str();
}


void
SsdpMessage::setRequestMethod(TRequestMethod requestMethod)
{
    m_requestMethod = requestMethod;
}


SsdpMessage::TRequestMethod
SsdpMessage::getRequestMethod()
{
    return m_requestMethod;
}


void
SsdpMessage::setCacheControl(int duration)
{
    m_messageHeader.set("CACHE-CONTROL", "max-age = " + NumberFormatter::format(duration));
}


int
SsdpMessage::getCacheControl()
{
    try {
        std::string value = m_messageHeader["CACHE-CONTROL"];
        return NumberParser::parse(value.substr(value.find('=')+1));
    }
    catch (Poco::NotFoundException) {
        std::cerr << "missing CACHE-CONTROL in SSDP header" << std::endl;
    }
    catch (Poco::SyntaxException) {
        std::cerr << "wrong number format of CACHE-CONTROL in SSDP header" << std::endl;
    }
    return SSDP_CACHE_DURATION;  // if no valid number is given, return minimum required value
}


void
SsdpMessage::setNotificationType(const std::string& searchTarget)
{
    m_messageHeader.set("NT", searchTarget);
}


std::string
SsdpMessage::getNotificationType()
{
    return m_messageHeader["NT"];
}


void
SsdpMessage::setNotificationSubtype(TRequestMethod notificationSubtype)
{
    m_messageHeader.set("NTS", m_messageMap[notificationSubtype]);
    m_notificationSubtype = notificationSubtype;
}


SsdpMessage::TRequestMethod
SsdpMessage::getNotificationSubtype()
{
    m_notificationSubtype = m_messageConstMap[m_messageHeader["NTS"]];
    return m_notificationSubtype;
}


void
SsdpMessage::setSearchTarget(const std::string& searchTarget)
{
    m_messageHeader.set("ST", searchTarget);
}


std::string
SsdpMessage::getSearchTarget()
{
    return m_messageHeader["ST"];
}


void
SsdpMessage::setUniqueServiceName(const std::string& serviceName)
{
    m_messageHeader.set("USN", serviceName);
}


std::string
SsdpMessage::getUniqueServiceName()
{
    return m_messageHeader["USN"];
}


void
SsdpMessage::setLocation(const URI& location)
{
    m_messageHeader.set("LOCATION", location.toString());
}


URI
SsdpMessage::getLocation()
{
    return URI(m_messageHeader["LOCATION"]);
}


void
SsdpMessage::setHost()
{
    m_messageHeader.set("HOST", SSDP_FULL_ADDRESS);
}


void
SsdpMessage::setHttpExtensionNamespace()
{
    m_messageHeader.set("MAN", "\"ssdp:discover\"");
}


void
SsdpMessage::setHttpExtensionConfirmed()
{
    m_messageHeader.set("EXT", "");
}


bool
SsdpMessage::getHttpExtensionConfirmed()
{
    return m_messageHeader.has("EXT");
}


void
SsdpMessage::setServer(const std::string& productNameVersion)
{   m_messageHeader.set("SERVER", 
                        Environment::osName() + "/"
                        + Environment::osVersion() + ", "
                        + "UPnP/" + UPNP_VERSION + ", "
                        + productNameVersion);
}


std::string
SsdpMessage::getServerOperatingSystem()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    MessageHeader::splitElements(value, elements);
    return elements[0];
}


std::string
SsdpMessage::getServerUpnpVersion()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    MessageHeader::splitElements(value, elements);
    std::string upnpVersion = elements[1];
    return upnpVersion.substr(upnpVersion.find('/')+1);
}


std::string
SsdpMessage::getServerProductNameVersion()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    MessageHeader::splitElements(value, elements);
    return elements[2];
}


void
SsdpMessage::setMaximumWaitTime(int waitTime)
{
    m_messageHeader.set("MX", NumberFormatter::format(waitTime));
}


int
SsdpMessage::getMaximumWaitTime()
{
    try {
        return NumberParser::parse(m_messageHeader["MX"]);
    }
    catch (Poco::NotFoundException) {
        std::cerr << "missing MX in SSDP header" << std::endl;
    }
    catch (Poco::SyntaxException) {
        std::cerr << "wrong number format of MX in SSDP header" << std::endl;
    }
    return SSDP_MIN_WAIT_TIME;  // if no valid number is given, return minimum required value
}


void
SsdpMessage::setDate()
{
    Timestamp t;
    m_messageHeader.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
}


DateTime
SsdpMessage::getDate()
{
    std::string value = m_messageHeader["DATE"];
    int timeZoneDifferentail;
    try {
        return DateTimeParser::parse(DateTimeFormat::HTTP_FORMAT, value, timeZoneDifferentail);
    }
    catch (Poco::SyntaxException) {
        std::cerr << "wrong date format of DATE in SSDP header" << std::endl;
    }
}


SocketAddress
SsdpMessage::getSender()
{
    return m_sender;
}


SsdpSocket::SsdpSocket(const AbstractObserver& observer):
m_ssdpAddress(SSDP_ADDRESS),
m_ssdpPort(SSDP_PORT),
// set socket in a non-exclusive state, thus allowing other process to bind to the same port
// i.e. set SO_REUSEADDR flag on socket
m_ssdpSocket(SocketAddress(m_ssdpAddress, m_ssdpPort), true),
m_pBuffer(new char[BUFFER_SIZE])
{
    std::cerr << "SsdpSocket()" << std::endl;
    
    // set the default interface by providing an empty NetworkInterface as argument
    // TODO: let interface be configurable
    // TODO: default value: find out an interface name, that routes to the default route entry
    m_interface = NetworkInterface::forName("wlan0");
    m_ssdpSocket.setInterface(m_interface);
    m_ssdpSocket.setLoopback(true);
    m_ssdpSocket.setTimeToLive(4);  // TODO: let TTL be configurable
    m_ssdpSocket.joinGroup(m_ssdpAddress);
    m_reactor.addEventHandler(m_ssdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
//     m_reactor.addEventHandler(_socket, NObserver<EchoServiceHandler, ShutdownNotification>(*this, &EchoServiceHandler::onShutdown));
    m_notificationCenter.addObserver(observer);
    m_listenerThread.start(m_reactor);
}


SsdpSocket::~SsdpSocket()
{
    std::cerr << std::endl << "closing SSDP socket ..." << std::endl;
    m_reactor.removeEventHandler(m_ssdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
    m_ssdpSocket.leaveGroup(m_ssdpAddress);
    m_reactor.stop();
    m_listenerThread.join();
    delete [] m_pBuffer;
}


void
SsdpSocket::onReadable(const AutoPtr<ReadableNotification>& pNf)
{
    SocketAddress sender;
    int n = m_ssdpSocket.receiveFrom(m_pBuffer, BUFFER_SIZE, sender);
    if (n > 0) {
//         std::cerr << "SsdpSocket::onReadable() receives: " << std::endl << m_pBuffer << std::endl;
//         std::cerr << "SsdpSocket::onReadable() receives: " << n << " bytes from: " << s.toString() << std::endl;
        m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n), sender));
    }
}


void
SsdpSocket::sendMessage(SsdpMessage& message, const SocketAddress& receiver)
{
    std::string m = message.toString();
    int bytesSent = m_ssdpSocket.sendTo(m.c_str(), m.length(), receiver);
//     std::cerr << "SsdpSocket::sendMessage() message sent, number bytes: " << bytesSent << std::endl;
}


Service::Service(Device* device, NodeIterator rootNode) :
// m_description(std::string("")),
m_vendorDomain("schemas-upnp-org:device"),  // if vendor is UPnP forum
m_serviceType("fooservice"),
m_serviceVersion("fooserviceversion"),
m_device(device)
{
    // TODO: offer service description for control-points to download
    Node* pNode = rootNode.nextNode();
    while (pNode)
    {
        if (pNode->nodeName() == "serviceType" && pNode->hasChildNodes()) {
            m_serviceType = pNode->firstChild()->nodeValue();
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            m_descriptionUri = URI(pNode->firstChild()->nodeValue());
            std::ifstream ifs(("/home/jb/devel/cc/jamm/tests/" + m_descriptionUri.getPath()).c_str());
            std::stringstream ss;
            StreamCopier::copyStream(ifs, ss);
            m_description = std::string(ss.str());
            std::cerr << m_description << std::endl;
//             std::cerr << m_device << std::endl;
//             std::cerr << m_deviceRoot << std::endl;
//             m_descriptionLength = m_description.size();
            m_device->m_deviceRoot->m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler(m_descriptionUri.getPath(), new DescriptionRequestHandler(m_description));
        }
        else if (pNode->nodeName() == "controlURL" && pNode->hasChildNodes()) {
            m_controlUri = URI(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "eventSubURL" && pNode->hasChildNodes()) {
            m_eventUri = URI(pNode->firstChild()->nodeValue());
        }
        pNode = rootNode.nextNode();
    }
    // send SSDP messages
    SsdpMessage m(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m.setNotificationType(m_serviceType);  // one message for every service
    m.setUniqueServiceName(m_device->m_uuidDescription + "::" +  m_serviceType);
    m_device->m_deviceRoot->sendMessage(m);
}


Service::~Service()
{
}


Device::Device()
{
}


Device::Device(DeviceRoot* deviceRoot, NodeIterator rootNode) :
m_vendorDomain("schemas-upnp-org:device"),  // if vendor is UPnP forum
m_deviceType("foodevice"),
m_deviceVersion("foodeviceversion"),
m_deviceRoot(deviceRoot)
{
    // TODO: if uuid is not given in the description, generate one and try to save it on
    //       persistance storage
//     UUIDGenerator uuidGen;
//     m_uuid = uuidGen.createOne();
    
    Node* pNode = rootNode.nextNode();
    while (pNode)  // TODO: stop when closing tag </device> is reached, or is root node set by copy-ctor?
    {
        if (pNode->nodeName() == "deviceType" && pNode->hasChildNodes()) {
            m_deviceType = pNode->firstChild()->nodeValue();
        }
        else if (pNode->nodeName() == "UDN" && pNode->hasChildNodes()) {
            m_uuidDescription = pNode->firstChild()->nodeValue();
        }
        else if (pNode->nodeName() == "serviceList" && pNode->hasChildNodes()) {
            // for each childnode append a service
            NodeIterator childIterator(pNode, NodeFilter::SHOW_ELEMENT);
            for (Node* c = childIterator.nextNode(); c; c = childIterator.nextNode()) {
                if (c->nodeName() == "service") {
                    m_services.push_back(Service(this, NodeIterator(c, NodeFilter::SHOW_ALL)));
                }
            }
        }
        pNode = rootNode.nextNode();
    }
    SsdpMessage m(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m.setLocation(m_deviceRoot->m_descriptionUri);    // location of UPnP description of the root device
    
    m.setNotificationType(m_uuidDescription);
    m.setUniqueServiceName(m_uuidDescription);
    m_deviceRoot->sendMessage(m);  // root device second message
    
    m.setNotificationType(m_deviceType);
    m.setUniqueServiceName(m_uuidDescription + "::" + m_deviceType);
    m_deviceRoot->sendMessage(m);  // root device third message
}


Device::~Device()
{
}


// SsdpSocket DeviceRoot::m_ssdpSocket(Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage));


DeviceRoot::DeviceRoot(std::string& description) :
m_ssdpSocket(Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage)),
m_httpSocket(m_ssdpSocket.m_interface),
m_descriptionStream(description)
{
    
//---------------------------------------------------------------------------------
    //  1. read device description and extract device and service information
    DOMParser parser;
    AutoPtr<Document> pDoc = parser.parseString(description);
    NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
    Node* pNode = it.nextNode();
    
    while (pNode)
    {
        if (pNode->nodeName() == "device" && pNode->hasChildNodes()) {
            m_rootDevice = Device(this, it);
        }
        else if (pNode->nodeName() == "deviceList" && pNode->hasChildNodes()) {  // root-device only!!!
            // for each childnode (embedded device) create a Device object
            // TODO: this doesn't iterate through the childnodes only, but through the whole subtree, which is
            //       unnecessary
            NodeIterator childIterator(pNode, NodeFilter::SHOW_ELEMENT);
            for (Node* c = childIterator.nextNode(); c; c = childIterator.nextNode()) {
                if (c->nodeName() == "device") {
                   m_embeddedDevices.push_back(Device(this, NodeIterator(c, NodeFilter::SHOW_ALL)));
                }
            }
        }
        pNode = it.nextNode();
    }
    
    //---------------------------------------------------------------------------------
    // 2. setup HTTP server for description download and action/eventing
//     m_descriptionRequestHandler = new DescriptionRequestHandler(m_descriptionStream, description.size());
    m_descriptionRequestHandler = new DescriptionRequestHandler(description);
    m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler("/device_description", m_descriptionRequestHandler);
    
    m_descriptionUri = URI("http://" + m_httpSocket.m_httpServerAddress.toString() + "/device_description");
    std::cerr << "DeviceRoot offering device description on: " << m_descriptionUri.toString() << std::endl;
    
    //---------------------------------------------------------------------------------
    // 3. send out SSDP messages to announce what we got
    // TODO: handle services that appear in the same subdevices (no double messages should be send).
    // send NOTIFY alive messages
    // -> but then a message with (embedded-device uuid, service type) is missing? I don't get the specs here ...
    SsdpMessage m(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m.setLocation(m_descriptionUri);    // location of UPnP description of the root device
    
    m.setNotificationType("upnp:rootdevice");  // once for root device
    m.setUniqueServiceName(m_rootDevice.m_uuidDescription + "::upnp:rootdevice");
    m_ssdpSocket.sendMessage(m);  // root device first message
}


DeviceRoot::~DeviceRoot()
{
    // TODO: send NOTIFY byebye messages
    SsdpMessage m(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m.setRequestMethod(SsdpMessage::REQUEST_NOTIFY);
    m.setHost();
    m.setServer("Jamm/0.0.3");
    m.setNotificationSubtype(SsdpMessage::SUBTYPE_BYEBYE);         // byebye message
    
    m.setUniqueServiceName(m_rootDevice.m_uuidDescription + "::upnp:rootdevice");
    
    m_ssdpSocket.sendMessage(m);
    delete m_descriptionRequestHandler;
}


void 
DeviceRoot::sendMessage(SsdpMessage& message, const SocketAddress& receiver)
{
    m_ssdpSocket.sendMessage(message, receiver);
}


void
DeviceRoot::handleSsdpMessage(SsdpMessage* pNf)
{
    std::cout << "root device gets SSDP message:" << std::endl;
    std::cout << pNf->toString();
    
    // TODO: reply to M-SEARCH messages
    if (pNf->getRequestMethod() == SsdpMessage::REQUEST_SEARCH) {
        SsdpMessage m;
        m.setRequestMethod(SsdpMessage::REQUEST_RESPONSE);
        m.setCacheControl();
        m.setDate();
        m.setHttpExtensionConfirmed();
        m.setLocation(m_descriptionUri);
        m.setServer("Jamm/0.0.3");
        // ST field in response depends on ST field in M-SEARCH
        m.setSearchTarget("upnp:rootdevice");
        // same as USN in NOTIFY message
        m.setUniqueServiceName("uuid:" + m_rootDevice.m_uuid.toString() + "::upnp:rootdevice");
        
        m_ssdpSocket.sendMessage(m, pNf->getSender());
    }
}


Controller::Controller() :
m_ssdpSocket(Observer<Controller, SsdpMessage>(*this, &Controller::handleSsdpMessage))
{
    // TODO: send M-SEARCH messages
    SsdpMessage m;
    m.setRequestMethod(SsdpMessage::REQUEST_SEARCH);
    m.setHost();
    m.setHttpExtensionNamespace();
    m.setMaximumWaitTime();
//     m.setSearchTarget("ssdp:all");
    m.setSearchTarget("upnp:rootdevice");
    
    m_ssdpSocket.sendMessage(m);
}


Controller::~Controller()
{
}


void
Controller::handleSsdpMessage(SsdpMessage* pNf)
{
    std::cout << "controller gets SSDP message:" << std::endl;
    std::cout << pNf->toString();
    // TODO: handle NOTIFY messages and M-SEARCH reply messages
    // do we need to open a seperate socket to receive the M-SEARCH reply on a different port than 1900?
    // device advertisement (chapter 1.1) is HTTPMU (multicast) only,
    // while device search (chapter 1.2) is HTTPU (unicast) and HTTPMU (multicast)
    // need to open a unicast socket ...?? On both sides?? for sending and listening?
    // send to multicast address with sender port different than 1900?
    // use a normal DatagramSocket for HTTPU, do a connect(), this socket is not bound to a specific
    // interface then ...
}
