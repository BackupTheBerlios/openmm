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


DescriptionReader::DescriptionReader(URI uri, std::string deviceDescriptionPath) :
m_uri(uri),
m_deviceDescriptionPath(deviceDescriptionPath)
{
}


DescriptionReader::~DescriptionReader()
{
    releaseDescriptionDocument();
}


void
DescriptionReader::releaseDescriptionDocument()
{
/*    std::cerr << "DescriptionReader::releaseDescriptionDocument()" << std::endl;
    if (!m_pDocStack.empty()) {
        m_pDocStack.top()->release();
        m_pDocStack.pop();
    }*/
}


std::string
DescriptionReader::getDescription(std::string path)
{
    // TODO: get description according to m_uri.service() ("file:/" or "http:/")
    std::string p = m_uri.getPath() + path;
    std::cerr << "DescriptionReader::getDescription() from: " << p << std::endl;
    std::stringstream ss;
    std::ifstream ifs(p.c_str());
    StreamCopier::copyStream(ifs, ss);
    std::string res = ss.str();
    DOMParser parser;
    m_pDocStack.push(parser.parseString(res));
    Node* n = m_pDocStack.top()->documentElement()->firstChild();
    std::cerr << "first node: " << n->nodeName() << ", " << n << std::endl;
    m_nodeStack.push(n);
    return res;
}


DeviceRoot*
DescriptionReader::deviceRoot()
{
    std::cerr << "DescriptionReader::deviceRoot()" << std::endl;
    DeviceRoot* pRes = new DeviceRoot();
    m_pDeviceRoot = pRes;
    pRes->setDeviceDescription(getDescription(m_deviceDescriptionPath));
    pRes->setDescriptionUri(m_deviceDescriptionPath);
    Node* pNode = m_nodeStack.top();
//     std::cerr << "top of stack: " << pNode << std::endl;
    
    while (pNode)
    {
//         std::cerr << "node: " << pNode->nodeName() << std::endl;
        if (pNode->nodeName() == "device" && pNode->hasChildNodes()) {
            m_nodeStack.push(pNode->firstChild());
            Device* pDevice = device();
            m_pDeviceRoot->addDevice(pDevice);
            pRes->setRootDevice(pDevice);
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Device*
DescriptionReader::device()
{
    std::cerr << "DescriptionReader::device()" << std::endl;
    Device* pRes = new Device();
    Node* pNode = m_nodeStack.top();
    pRes->setDeviceRoot(m_pDeviceRoot);

    while (pNode)
    {
//         std::cerr << "node: " << pNode->nodeName() << std::endl;
        if (pNode->nodeName() == "deviceType" && pNode->hasChildNodes()) {
            pRes->setDeviceType(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "UDN" && pNode->hasChildNodes()) {
            pRes->setUuid(pNode->firstChild()->nodeValue().substr(5));
        }
        else if (pNode->nodeName() == "serviceList" && pNode->hasChildNodes()) {
            Node* pChild = pNode->firstChild();
            while (pChild) {
//                 std::cerr << "child: " << pChild->nodeName() << std::endl;
                if (pChild->nodeName() == "service") {
                    m_nodeStack.push(pChild->firstChild());
                    Service* pService = service();
                    pService->setDevice(pRes);
                    pRes->addService(pService);
                }
                pChild = pChild->nextSibling();
            }
        }
        else if (pNode->nodeName() == "deviceList" && pNode->hasChildNodes()) {
            Node* pChild = pNode->firstChild();
            while (pChild) {
                if (pChild->nodeName() == "device") {
                    m_nodeStack.push(pChild->firstChild());
                    m_pDeviceRoot->addDevice(device());
                }
                pChild = pChild->nextSibling();
            }
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Service*
DescriptionReader::service()
{
    std::cerr << "DescriptionReader::service()" << std::endl;
    Service* pRes = new Service();
    Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
//         std::cerr << "node: " << pNode->nodeName() << std::endl;
        if (pNode->nodeName() == "serviceType" && pNode->hasChildNodes()) {
            pRes->setServiceType(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            pRes->setDescriptionPath(pNode->firstChild()->nodeValue());
            // load the service description into the Service object.
            pRes->setDescription(getDescription(pRes->getDescriptionPath()));
            Node* pScpd = m_nodeStack.top();
            while (pScpd) {
//                 std::cerr << "node: " << pScpd->nodeName() << std::endl;
                if (pScpd->nodeName() == "actionList" && pScpd->hasChildNodes()) {
                    Node* pChild = pScpd->firstChild();
                    while (pChild) {
//                         std::cerr << "node: " << pChild->nodeName() << std::endl;
                        if (pChild->nodeName() == "action") {
                            m_nodeStack.push(pChild->firstChild());
                            pRes->addAction(action());
                        }
                        pChild = pChild->nextSibling();
                    }
                }
                else if (pScpd->nodeName() == "serviceStateTable" && pScpd->hasChildNodes()) {
                    Node* pChild = pScpd->firstChild();
                    while (pChild) {
//                         std::cerr << "node: " << pChild->nodeName() << std::endl;
                        if (pChild->nodeName() == "stateVariable") {
                            m_nodeStack.push(pChild->firstChild());
                            StateVar* pStateVar = stateVar();
                            if (pChild->hasAttributes()) {
                                NamedNodeMap* attr = pChild->attributes();
                                pStateVar->setSendEvents(attr->getNamedItem("sendEvents")->nodeValue());
                                attr->release();
                        }
                        pRes->addStateVar(pStateVar);
                        }
                        pChild = pChild->nextSibling();
                    }
                }
                pScpd = pScpd->nextSibling();
            }
            // finished with this service description, getDescription() did a m_nodeStack.push()
            m_nodeStack.pop();
            releaseDescriptionDocument();
        }
        else if (pNode->nodeName() == "controlURL" && pNode->hasChildNodes()) {
            pRes->setControlPath(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "eventSubURL" && pNode->hasChildNodes()) {
            pRes->setEventPath(pNode->firstChild()->nodeValue());
        }
        
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Action*
DescriptionReader::action()
{
    std::cerr << "DescriptionReader::action()" << std::endl;
    Action* pRes = new Action();
    Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
        if (pNode->nodeName() == "name" && pNode->hasChildNodes()) {
            pRes->setName(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "argumentList" && pNode->hasChildNodes()) {
            // TODO: this if branch should be written as only one line of code ...
            Node* pChild = pNode->firstChild();
            while (pChild) {
                if (pChild->nodeName() == "argument") {
                    m_nodeStack.push(pChild->firstChild());
                    pRes->appendArgument(argument());
                }
                pChild = pChild->nextSibling();
            }
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Argument*
DescriptionReader::argument()
{
    std::cerr << "DescriptionReader::argument()" << std::endl;
    Argument* pRes = new Argument();
    Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
        if (pNode->nodeName() == "name" && pNode->hasChildNodes()) {
            pRes->setName(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "relatedStateVariable" && pNode->hasChildNodes()) {
            pRes->setRelatedStateVar(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "direction" && pNode->hasChildNodes()) {
            pRes->setDirection(pNode->firstChild()->nodeValue());
        }
        
        pNode = pNode->nextSibling();
    }
    
    m_nodeStack.pop();
    return pRes;
}


StateVar*
DescriptionReader::stateVar()
{
    std::cerr << "DescriptionReader::stateVar()" << std::endl;
    StateVar* pRes = new StateVar();
    Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
        if (pNode->nodeName() == "name" && pNode->hasChildNodes()) {
            pRes->setName(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "dataType" && pNode->hasChildNodes()) {
            pRes->setType(pNode->firstChild()->nodeValue());
        }
        else if (pNode->nodeName() == "defaultValue" && pNode->hasChildNodes()) {
            pRes->setDefaultValue(pNode->firstChild()->nodeValue());
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}



ActionRequestReader::ActionRequestReader(const std::string& requestBody)
{
    std::cerr << "ActionRequestReader::ActionRequestReader()" << std::endl;
    std::cerr << "request: " << requestBody << std::endl;
    
    DOMParser parser;
    m_pDoc = parser.parseString(requestBody);
    NodeIterator it(m_pDoc, NodeFilter::SHOW_ALL);
    m_nodeStack.push(it.nextNode());
}


Action*
ActionRequestReader::action()
{
    std::cerr << "ActionRequestReader::action()" << std::endl;
    // TODO: clone an Action with inArgs and outArgs retained
    Action* pRes = new Action();
    Node* pNode = m_nodeStack.top();
    NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
    
    while (pNode)
    {
        if (pNode->nodeName() == pNode->prefix() + ":Body" && pNode->hasChildNodes()) {
            Node* pAction = pNode->firstChild();
            std::string s = pAction->nodeName();
            pRes->setName(s.substr(s.find(":") + 1));
            std::cerr << "action: " << pRes->getName() << std::endl;
            s = pAction->namespaceURI();
            pRes->setServiceType(s.substr(s.find("service:") + 8));
            std::cerr << "serviceType: " << pRes->getServiceType() << std::endl;
            
            Node* pChild = pAction->firstChild();
            while (pChild) {
                pRes->setArgument(pChild->nodeName(), pChild->firstChild()->nodeValue());
                pChild = pChild->nextSibling();
            }
        }
        pNode = it.nextNode();
    }
    m_nodeStack.pop();
    std::cerr << "ActionRequestReader::action() finished" << std::endl;
    return pRes;
}


void
ActionWriter::argument(const Argument& argument)
{
}


ActionResponseWriter::ActionResponseWriter(std::string& responseBody) :
m_responseBody(&responseBody)
{
    std::cerr << "ActionResponseWriter::ActionResponseWriter()" << std::endl;
}


void
ActionResponseWriter::action(Action& action)
{
    std::cerr << "ActionResponseWriter::action()" << std::endl;
    
    AutoPtr<Document> pDoc = new Document;
    AutoPtr<Element> pEnvelope = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    AutoPtr<Element> pBody = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    AutoPtr<Element> pActionResponse = pDoc->createElementNS("urn:schemas-upnp-org:service:" + action.getServiceType(), action.getName() + "Response");
    
    for(Action::OutArgumentIterator i = action.beginOutArgument(); i != action.endOutArgument(); ++i) {
        AutoPtr<Element> pArgument = pDoc->createElement((*i).getName());
        AutoPtr<Text> pArgumentValue = pDoc->createTextNode(action.getArgument<std::string>((*i).getName()));
        pArgument->appendChild(pArgumentValue);
        pActionResponse->appendChild(pArgument);
    }
    
    pBody->appendChild(pActionResponse);
    pEnvelope->appendChild(pBody);
    pDoc->appendChild(pEnvelope);
    
    DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.setOptions(XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, pDoc);
    *m_responseBody = ss.str();
    std::cerr << "ResponseBody():" << std::endl << *m_responseBody << std::endl;
}


Service::~Service()
{
}


// void
// Service::setStateVar(std::string name, std::string val)
// {
    // TODO: lock the m_stateVariables map because different threads could access it
//     m_stateVars.setValue(name, val);
// }


// std::string
// Service::getStateVar(const std::string& name) /*const*/
// {
//     return m_stateVars.get(name).convert<std::string>();
//     return m_stateVars.getValue<std::string>(name);
// }


// Argument handling methods
/*std::string
Action::getArgument(const std::string& name)
{*/
//     std::cerr << "Action::getArgument() name: " << name << ", val: " << m_arguments.get(name) << std::endl;
//     return m_argumentValues[name];
//     return m_arguments.get(name);
// }


// template<typename T>
// const T&
// Action::getArgument(std::string key)
// {
//     return m_arguments.get(key).convert<T>();
// }


void
Action::setArgument(std::string name, std::string value)
{
    std::cerr << "Action::setArgument() name: " << name << ", val: " << value << std::endl;
    m_arguments.setValue(name, value);
}


void
Action::appendArgument(Argument* pArgument)
{
    m_arguments.append(pArgument->getName(), pArgument);
    if (pArgument->getDirection() == "in") {
        m_inArguments.append(pArgument->getName(), pArgument);
    }
    else {
        m_outArguments.append(pArgument->getName(), pArgument);
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
    std::cerr << "handle description request: " << request.getURI() << std::endl;
//         std::cerr << "sending contents of length: " << m_description.size() << std::endl;
    response.setContentLength(m_description.size());
    response.setContentType("text/xml");
    std::ostream& ostr = response.send();
    ostr << m_description;
}


ControlRequestHandler::ControlRequestHandler(HttpSocket* pHttpSocket):
m_pHttpSocket(pHttpSocket)
{
}


ControlRequestHandler*
ControlRequestHandler::create()
{
    return new ControlRequestHandler(m_pHttpSocket);
}


void
ControlRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle control request: " << request.getURI() << std::endl;
        // synchronous action handling: wait until handleAction() has finished. This must be done in under 30 sec,
        // otherwise it should return and an event should be sent on finishing the action request.
//         handleAction(new Action());
    int length = request.getContentLength();
    char buf[length];
    request.stream().read(buf, length);
    std::string requestBody(buf, length);
    
    // TODO: Action needs some info about the Service description (e.g. order of out args)
    ActionRequestReader requestReader(requestBody);
    Action* pAction = requestReader.action();
        // the corresponding Service should register as a Notification Handler
    m_pHttpSocket->m_notificationCenter.postNotification(pAction);
        // return Action response with out arguments filled in by Notification Handler
    std::string responseBody;
    ActionResponseWriter responseWriter(responseBody);
    responseWriter.action(*pAction);
    
    response.setContentType("text/xml");
        // TODO: set EXT header
        // TODO: set SERVER header
    std::ostream& ostr = response.send();
    ostr << responseBody;
    response.setContentLength(responseBody.size());
}


EventRequestHandler::EventRequestHandler(HttpSocket* pHttpSocket):
m_pHttpSocket(pHttpSocket)
{
}


EventRequestHandler*
EventRequestHandler::create()
{
    return new EventRequestHandler(m_pHttpSocket);
}


void
EventRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle event request: " << request.getMethod() << std::endl;
    std::cerr << "HOST: " << request.getHost() << std::endl;
    std::cerr << "CALLBACK: " << request.get("CALLBACK") << std::endl;
    std::cerr << "NT: " << request.get("NT") << std::endl;
    std::cerr << "TIMEOUT: " << request.get("TIMEOUT") << std::endl;
    
    if (request.getMethod() == "SUBSCRIBE") {
        Timestamp t;
        response.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
        response.set("SERVER", 
                    Environment::osName() + "/"
                    + Environment::osVersion() + ", "
                    + "UPnP/" + UPNP_VERSION + ", "
                    + JAMM_VERSION);
        response.set("SID", "uuid:" + UUIDGenerator().create().toString());
        response.set("TIMEOUT", "Second-1800");
    }
    else if (request.getMethod() == "UNSUBSCRIBE") {
    }
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
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out. Or: bind to 0.0.0.0 and broadcast SSDP to all available network interfaces by default.
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


Device::~Device()
{
}


DeviceRoot::DeviceRoot() :
m_ssdpSocket(Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage)),
m_httpSocket(m_ssdpSocket.m_interface)
{
}


DeviceRoot::~DeviceRoot()
{
    std::cerr << "DeviceRoot::~DeviceRoot()" << std::endl;
    stopSsdp();
//     delete m_descriptionRequestHandler;
}


void
DeviceRoot::init()
{
    std::cerr << "DeviceRoot::init()" << std::endl;
    SsdpNotifyAliveWriter aliveWriter(m_ssdpNotifyAliveMessages);
    SsdpNotifyByebyeWriter byebyeWriter(m_ssdpNotifyByebyeMessages);
    aliveWriter.deviceRoot(*this);
    byebyeWriter.deviceRoot(*this);
    m_descriptionRequestHandler = new DescriptionRequestHandler(getDeviceDescription());
    registerHttpRequestHandler(getDescriptionUri().getPath(), m_descriptionRequestHandler);
    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        Device& device = *d;
        aliveWriter.device(device);
        byebyeWriter.device(device);
        for(Device::ServiceIterator s = device.beginService(); s != device.endService(); ++s) {
            Service& service = *s;
            aliveWriter.service(service);
            byebyeWriter.service(service);
            
            service.setDescriptionRequestHandler(); // TODO: probably not needed ...
            registerHttpRequestHandler(service.getDescriptionPath(), service.getDescriptionRequestHandler());
            registerHttpRequestHandler(service.getControlPath(), new ControlRequestHandler(&m_httpSocket));
            registerHttpRequestHandler(service.getEventPath(), new EventRequestHandler(&m_httpSocket));
        }
    }
    std::cerr << "DeviceRoot::init() finished" << std::endl;
}


SsdpMessageSet::SsdpMessageSet()
{
    m_randomTimeGenerator.seed();
}


SsdpMessageSet::~SsdpMessageSet()
{
    if (m_continuous) {
        m_sendTimer.stop();
    }
/* // This causes a segfault:
   for (std::vector<SsdpMessage*>::iterator i = m_ssdpMessages.begin(); i != m_ssdpMessages.end(); ++i) {
        delete *i;
    }*/
}


void
SsdpMessageSet::addMessage(SsdpMessage& message)
{
    m_ssdpMessages.push_back(&message);
}


void
SsdpMessageSet::send(SsdpSocket& socket, int repeat, long delay, bool continuous)
{
    std::cerr << "SsdpMessageSet::send()" << std::endl;
//     Poco::ScopedLock<Poco::FastMutex> lock(m_sendLock);
    // TODO: check if continuous Timer is already running and return
//     if (m_sendTimer) {
//         return;
//     }
    m_socket = &socket;
    m_repeat = repeat;
    m_delay = delay;
    m_continuous = continuous;
    if (m_delay > 0) {
        m_sendTimer.setStartInterval(m_randomTimeGenerator.next(m_delay));
    }
    if (m_continuous) {
        // start asynchronously
        m_sendTimer.start(TimerCallback<SsdpMessageSet>(*this, &SsdpMessageSet::onTimer));
    }
    else {
        // start synchronously
        onTimer(m_sendTimer);
    }
    std::cerr << "SsdpMessageSet::send() finished" << std::endl;
}


void
SsdpMessageSet::stop()
{
    m_sendTimer.stop();
}


void
SsdpMessageSet::onTimer(Timer& timer)
{
    std::cerr << "SsdpMessageSet::onTimer()" << std::endl;
    int r = m_repeat;
    while (r--) {
        std::cerr << "#message sets left to send: " << r+1 << std::endl;
        for (std::vector<SsdpMessage*>::const_iterator i = m_ssdpMessages.begin(); i != m_ssdpMessages.end(); ++i) {
//             std::cerr << "sending message: " << (*i)->toString() << std::endl;
            m_socket->sendMessage(**i);
        }
    }
    if (m_continuous) {
        std::cerr << "SsdpMessageSet::onTimer() restarting timer" << std::endl;
        timer.restart(m_randomTimeGenerator.next(m_delay));
    }
    std::cerr << "SsdpMessageSet::onTimer() finished" << std::endl;
}


void
DeviceRoot::startSsdp()
{
    std::cerr << "DeviceRoot::startSsdp()" << std::endl;
    // TODO: 3. send out initial set also on the occasion of new IP address or network interface.
    
    // 1. wait random intervall of less than 100msec when sending message set first time
    // 2. send out all message sets two times (max three times according to specs, should be configurable).
    m_ssdpNotifyAliveMessages.send(m_ssdpSocket, 2, 100, false);
    // 4. resend advertisements in random intervals of max half the expiraton time (CACHE-CONTROL header)
    m_ssdpNotifyAliveMessages.send(m_ssdpSocket, 2, SSDP_CACHE_DURATION * 1000 / 2, true);
    std::cerr << "DeviceRoot::startSsdp() finished" << std::endl;
}


void
DeviceRoot::stopSsdp()
{
    std::cerr << "DeviceRoot::stopSsdp()" << std::endl;
    m_ssdpNotifyAliveMessages.stop();
    m_ssdpNotifyByebyeMessages.send(m_ssdpSocket, 2, 0, false);
}


void
DeviceRoot::startHttp()
{
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
        m.setUniqueServiceName("uuid:" + m_pRootDevice->getUuid() + "::upnp:rootdevice");
        
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


// SsdpNotifyAliveWriter::~SsdpNotifyAliveWriter()
// {
// }


void
SsdpNotifyAliveWriter::deviceRoot(const DeviceRoot& pDeviceRoot)
{
    std::cerr << "SsdpNotifyAliveWriter::deviceRoot()" << std::endl;
    // root device first message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setLocation(pDeviceRoot.getDescriptionUri());    // location of UPnP description of the root device
    m->setNotificationType("upnp:rootdevice");  // once for root device
    m->setUniqueServiceName("uuid:" + pDeviceRoot.getRootDevice()->getUuid() + "::upnp:rootdevice");
    m_res->addMessage(*m);
}


void
SsdpNotifyAliveWriter::device(const Device& pDevice)
{
    std::cerr << "SsdpNotifyAliveWriter::device(): " << pDevice.getUuid() << std::endl;
    // device first message (root device second message)
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setLocation(pDevice.getDeviceRoot()->getDescriptionUri());    // location of UPnP description of the root device
    m->setNotificationType("uuid:" + pDevice.getUuid());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid());
    m_res->addMessage(*m);
    // device second message (root device third message)
    m->setNotificationType(pDevice.getDeviceType());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid() + "::" + pDevice.getDeviceType());
    m_res->addMessage(*m);
}


void
SsdpNotifyAliveWriter::service(const Service& pService)
{
    std::cerr << "SsdpNotifyAliveWriter::service()" << std::endl;
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    m_res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::deviceRoot(const DeviceRoot& pDeviceRoot)
{
    std::cerr << "SsdpNotifyByebyeWriter::deviceRoot()" << std::endl;
    // root device first message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType("upnp:rootdevice");  // once for root device
    m->setUniqueServiceName("uuid:" + pDeviceRoot.getRootDevice()->getUuid() + "::upnp:rootdevice");
    m_res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::device(const Device& pDevice)
{
    std::cerr << "SsdpNotifyByebyeWriter::device(): " << pDevice.getUuid() << std::endl;
    // device first message (root device second message)
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType("uuid:" + pDevice.getUuid());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid());
    m_res->addMessage(*m);
    // device second message (root device third message)
    m->setNotificationType(pDevice.getDeviceType());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid() + "::" + pDevice.getDeviceType());
    m_res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::service(const Service& pService)
{
    std::cerr << "SsdpNotifyByebyeWriter::service()" << std::endl;
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    m_res->addMessage(*m);
}


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
