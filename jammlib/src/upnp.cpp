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
//     std::cerr << "SsdpSocket()" << std::endl;
    
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
    Node* pNode = rootNode.nextNode();
    while (pNode)
    {
        if (pNode->nodeName() == "serviceType" && pNode->hasChildNodes()) {
            m_serviceType = pNode->firstChild()->nodeValue();
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            // TODO: analyze service description and create a map of Actions with
            //       all argument names and default values filled in
            
            // offer service description for control-points to download
            m_descriptionUri = URI(pNode->firstChild()->nodeValue());
            // TODO: handle base path for xml files
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
            m_controlRequestHandler = new ControlRequestHandler(&m_device->m_deviceRoot->m_httpSocket);
            m_device->m_deviceRoot->m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler(m_controlUri.toString(), m_controlRequestHandler);
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


void
Service::setStateVar(std::string name, std::string val)
{
    // TODO: lock the m_stateVariables map because different threads could access it
    m_stateVariables[name] = val;
}


void
Service::setStateVarBool(std::string name, bool val)
{
    if (val) {
        setStateVar(name, std::string("1"));
    } else {
        setStateVar(name, std::string("0"));
    }
}


std::string
Service::getStateVar(std::string name)
{
    return m_stateVariables[name];
}


bool
Service::getStateVarBool(std::string name)
{
    std::string out = getStateVar(name);
    if (out == "1") {
        return true;
    } else {
        return false;
    }
}


Action::Action(std::string requestBody)
{
//         std::cerr << "Action request:" << std::endl << requestBody << std::endl;
    
    DOMParser parser;
    AutoPtr<Document> pDoc = parser.parseString(requestBody);
    NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
    Node* pNode = it.nextNode();
    
    while (pNode)
    {
//             std::cerr << "XML namespace prefix of node: " << pNode->nodeName() << " is: " << pNode->prefix() << std::endl;
        if (pNode->nodeName() == pNode->prefix() + ":Body" && pNode->hasChildNodes()) {
//                 std::cerr << "Action element:" << std::endl;
//                     << "nodeName(): " << pNode->firstChild()->nodeName() << std::endl
//                     << "namespaceURI(): " << pNode->firstChild()->namespaceURI() << std::endl
//                     << "prefix(): " << pNode->firstChild()->prefix() << std::endl;
            Node* pAction = pNode->firstChild();
            std::string s = pAction->nodeName();
            m_actionName = s.substr(s.find(":") + 1);
            s = pAction->namespaceURI();
            m_serviceType = s.substr(s.find("service:") + 8);
            std::cerr << "Service: " << m_serviceType << std::endl;
            std::cerr << "Action: " << m_actionName << std::endl;
                // read in the list of arguments
            NodeIterator childIterator(pAction->firstChild(), NodeFilter::SHOW_ELEMENT);
            for (Node* c = childIterator.nextNode(); c; c = childIterator.nextNode()) {
                std::cerr << "Argument: " << c->nodeName() << " = " << c->firstChild()->nodeValue() << std::endl;
                m_inArgumentNames.push_back(c->nodeName());
                m_argumentValues[c->nodeName()] = c->firstChild()->nodeValue();
            }
        }
        pNode = it.nextNode();
    }
}


std::string
Action::responseBody()
{
//         std::cerr << "Action::responseBody()" << std::endl;
    AutoPtr<Document> pDoc = new Document;
    
    AutoPtr<Element> pEnvelope = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
//         pEnvelope->setAttribute("encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    AutoPtr<Element> pBody = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    AutoPtr<Element> pActionResponse = pDoc->createElementNS("urn:schemas-upnp-org:service:" + m_serviceType, m_actionName + "Response");
    
    // TODO: fill vector of out arguments with the help of the service description
        // append all out arguments
    for (std::vector<std::string>::iterator i = m_outArgumentNames.begin(); i != m_outArgumentNames.end(); ++i) {
        AutoPtr<Element> pArgument = pDoc->createElement(*i);
        AutoPtr<Text> pArgumentValue = pDoc->createTextNode(m_argumentValues[*i]);
        pArgument->appendChild(pArgumentValue);
        pActionResponse->appendChild(pArgument);
    }
    
    pBody->appendChild(pActionResponse);
    pEnvelope->appendChild(pBody);
    pDoc->appendChild(pEnvelope);
    
    DOMWriter writer;
    writer.setNewLine("\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.setOptions(XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, pDoc);
    m_responseBody = ss.str();
    std::cerr << "ResponseBody():" << std::endl << m_responseBody << std::endl;
    return m_responseBody;
}


int
Action::responseSize()
{
    return m_responseBody.size();
}


// Argument handling methods
void
Action::setArgument(std::string name, std::string value)
{
    m_argumentValues[name] = value;
}


void
Action::setArgumentBool(std::string name, bool value)
{
    if (value) {
        setArgument(name, std::string("1"));
    } else {
        setArgument(name, std::string("0"));
    }
}


std::string
Action::getArgument(std::string name)
{
    return m_argumentValues[name];
}


bool
Action::getArgumentBool(std::string name)
{
    std::string out = getArgument(name);
    if (out == "1") {
        return true;
    } else {
        return false;
    }
}


DeviceRequestHandlerFactory::DeviceRequestHandlerFactory(HttpSocket* pHttpSocket):
m_pHttpSocket(pHttpSocket)
{
    registerRequestHandler(std::string(""), new RequestNotFoundRequestHandler());
}


HTTPRequestHandler*
DeviceRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
    std::cerr << "dispatch HTTP request: " << request.getURI() << std::endl;
        // TODO: if no request is registered for the query, return a default request
        // handler with error 404 or similar ...
    HTTPRequestHandler* res;
    std::map<std::string,UpnpRequestHandler*>::iterator i = m_requestHandlerMap.find(request.getURI());
    if (i != m_requestHandlerMap.end()) {
        return i->second->create();
    }
    else {
        return m_requestHandlerMap[""]->create();
    }
}


void
DeviceRequestHandlerFactory::registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler)
{
    std::cerr << "register request handler: " << Uri << std::endl;
    m_requestHandlerMap[Uri] = requestHandler;
}


RequestNotFoundRequestHandler*
RequestNotFoundRequestHandler::create()
{
    return new RequestNotFoundRequestHandler();
}


void
RequestNotFoundRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle unknown request with HTTP 404 - not found error on request: " << request.getURI() << std::endl;
    response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
}


ControlRequestHandler::ControlRequestHandler(HttpSocket* pHttpSocket):
m_pHttpSocket(pHttpSocket)
{
}


DescriptionRequestHandler::DescriptionRequestHandler(std::string& description):
m_description(description)
{
}


DescriptionRequestHandler*
DescriptionRequestHandler::create()
{
        // TODO: can we somehow avoid to make a copy of the RequestHandler on each request?
    return new DescriptionRequestHandler(m_description);
}


void
DescriptionRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle request: " << request.getURI() << std::endl;
//         std::cerr << "sending contents of length: " << m_description.size() << std::endl;
    response.setContentLength(m_description.size());
    response.setContentType("text/xml");
    std::ostream& ostr = response.send();
    ostr << m_description;
}


ControlRequestHandler*
ControlRequestHandler::create()
{
    return new ControlRequestHandler(m_pHttpSocket);
}


void
ControlRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle request: " << request.getURI() << std::endl;
        // synchronous action handling: wait until handleAction() has finished. This must be done in under 30 sec,
        // otherwise it should return and an event should be sent on finishing the action request.
//         handleAction(new Action());
    int length = request.getContentLength();
    char buf[length];
    request.stream().read(buf, length);
    std::string s(buf, length);
    
//         std::stringstream ss;
//         StreamCopier::copyStream(request.stream(), ss);
//         std::string s = ss.str();
    
    // TODO: Action needs some info about the Service description (e.g. order of out args)
    Action* pAction = new Action(s);
        // the corresponding Service should register as a Notification Handler
    m_pHttpSocket->m_notificationCenter.postNotification(pAction);
        // return Action response with out arguments filled in by Notification Handler
    response.setContentType("text/xml");
        // TODO: set EXT header
        // TODO: set SERVER header
    std::ostream& ostr = response.send();
    ostr << pAction->responseBody();
    response.setContentLength(pAction->responseSize());
}


HttpSocket::HttpSocket(NetworkInterface interface)
{
    std::cerr << "HttpSocket()" << std::endl;
    
    m_pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory(this);
    
        // TODO: offer device description via HTTP, TCP/IP for download to controllers
    HTTPServerParams* pParams = new HTTPServerParams;
    //     pParams->setMaxQueued(maxQueued);
    //     pParams->setMaxThreads(maxThreads);
        // set-up a server socket on an available port
    ServerSocket socket(0);
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out
    //     socket.bind(m_ssdpSocket.m_interface.address());
    m_httpServerAddress = SocketAddress(interface.address(), socket.address().port());
        // set-up a HTTPServer instance
        // TODO: pass a map of all URI (devices and services) -> xml stream
        // for URI use the device/service name
    m_pHttpServer = new HTTPServer(m_pDeviceRequestHandlerFactory, socket, pParams);
        // start the HTTPServer
    m_pHttpServer->start();
    std::cerr << "started HTTP server on: " << m_httpServerAddress.toString() << std::endl;
}

HttpSocket::~HttpSocket()
{
    m_pHttpServer->stop();
    delete m_pHttpServer;
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
//                     m_services.push_back(Service(this, NodeIterator(c, NodeFilter::SHOW_ALL)));
                    Service* pService = new Service(this, NodeIterator(c, NodeFilter::SHOW_ALL));
                    m_services[pService->m_serviceType] = pService;
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
//     std::cerr << "DeviceRoot offering device description on: " << m_descriptionUri.toString() << std::endl;
    
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
//     std::cerr << "root device gets SSDP message:" << std::endl;
//     std::cerr << pNf->toString();
    
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
//     std::cerr << "controller gets SSDP message:" << std::endl;
//     std::cerr << pNf->toString();
    // TODO: handle NOTIFY messages and M-SEARCH reply messages
    // do we need to open a seperate socket to receive the M-SEARCH reply on a different port than 1900?
    // device advertisement (chapter 1.1) is HTTPMU (multicast) only,
    // while device search (chapter 1.2) is HTTPU (unicast) and HTTPMU (multicast)
    // need to open a unicast socket ...?? On both sides?? for sending and listening?
    // send to multicast address with sender port different than 1900?
    // use a normal DatagramSocket for HTTPU, do a connect(), this socket is not bound to a specific
    // interface then ...
}
