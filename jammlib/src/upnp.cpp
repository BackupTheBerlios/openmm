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


UriDescriptionReader::UriDescriptionReader(URI uri, const std::string& deviceDescriptionPath) :
DescriptionReader(deviceDescriptionPath),
m_uri(uri)
{
}


std::string&
UriDescriptionReader::getDescription(const std::string& path)
{
    // TODO: get description according to m_uri.service() ("file:/" or "http:/")
    std::string p = m_uri.getPath() + path;
    std::cerr << "DescriptionReader::getDescription() from: " << p << std::endl;
    std::stringstream ss;
    std::ifstream ifs(p.c_str());
    StreamCopier::copyStream(ifs, ss);
    
    std::string* res = new std::string(ss.str());
    // TODO: put this into deviceRoot() as it is common with StringDescriptionReader
    DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Node* n = m_pDocStack.top()->documentElement()->firstChild();
    std::cerr << "first node: " << n->nodeName() << ", " << n << std::endl;
    m_nodeStack.push(n);
    return *res;
}


StringDescriptionReader::StringDescriptionReader(std::map<std::string,std::string*>& stringMap, const std::string& deviceDescriptionPath) :
DescriptionReader(deviceDescriptionPath),
m_pStringMap(&stringMap)
{
}


std::string&
StringDescriptionReader::getDescription(const std::string& path)
{
    std::cerr << "StringDescriptionReader::getDescription()" << std::endl;
    std::string* res = (*m_pStringMap)[path];
    DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Node* n = m_pDocStack.top()->documentElement()->firstChild();
    std::cerr << "first node: " << n->nodeName() << ", " << n << std::endl;
    m_nodeStack.push(n);
    return *res;
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
//                     Service* pService = service();
//                     pService->setDevice(pRes);
                    pRes->addService(service());
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
            std::string val = pNode->firstChild()->nodeValue();
            pRes->setDefaultValue(val);
            // FIXME: seems StateVar's value isn't set to the default value
            pRes->setValue(val);
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}



ActionRequestReader::ActionRequestReader(const std::string& requestBody, Action* pActionTemplate) : m_pActionTemplate(pActionTemplate)
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
//     Action* pRes = new Action();
    Action* pRes = m_pActionTemplate;
    Node* pNode = m_nodeStack.top();
    NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
    
    while (pNode)
    {
        if (pNode->nodeName() == pNode->prefix() + ":Body" && pNode->hasChildNodes()) {
            Node* pAction = pNode->firstChild();
//             std::string s = pAction->nodeName();
//             pRes->setName(s.substr(s.find(":") + 1));
            std::cerr << "action: " << pRes->getName() << std::endl;
//             s = pAction->namespaceURI();
//             pRes->setServiceType(s.substr(s.find("service:") + 8));
            std::cerr << "serviceType: " << pRes->getService()->getServiceType() << std::endl;
            
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


DeviceDescriptionWriter::DeviceDescriptionWriter()
{
    m_pDoc = new Document;
}


void
DeviceDescriptionWriter::deviceRoot(DeviceRoot& deviceRoot)
{
    AutoPtr<Element> pRoot = m_pDoc->createElementNS("urn:schemas-upnp-org:device-1-0", "root");
    AutoPtr<Element> pSpecVersion = m_pDoc->createElement("specVersion");
    AutoPtr<Element> pMajor = m_pDoc->createElement("major");
    AutoPtr<Element> pMinor = m_pDoc->createElement("major");
    AutoPtr<Text> pMajorVersion = m_pDoc->createTextNode("1");
    AutoPtr<Text> pMinorVersion = m_pDoc->createTextNode("0");
    pMajor->appendChild(pMajorVersion);
    pMinor->appendChild(pMinorVersion);
    pSpecVersion->appendChild(pMajor);
    pSpecVersion->appendChild(pMinor);
    pRoot->appendChild(pSpecVersion);
    // write root device
    Element* pRootDevice = device(*deviceRoot.getRootDevice());
    pRoot->appendChild(pRootDevice);
    // end root device
    
    // if there are embedded devices open a deviceList
    // write embedded devices
    // end embedded devices
    
    // end DeviceRoot
}


Element*
DeviceDescriptionWriter::device(Device& device)
{
    AutoPtr<Element> pDevice = m_pDoc->createElement("device");
    // write Properties
    
    // write Services
    
    // write PresentationURL
//     AutoPtr<Element> pPresentationUrl = m_pDoc->createElement(device->getPresentationUri());
    return pDevice;
}


void
DeviceDescriptionWriter::write(std::string& description)
{
    std::cerr << "DeviceDescriptionWriter::write()" << std::endl;
    
    DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.setOptions(XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    description = ss.str();
    std::cerr << "description:" << std::endl << ss.str() << std::endl;
}


// void
// ActionWriter::argument(const Argument& argument)
// {
// }


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
    AutoPtr<Element> pActionResponse = pDoc->createElementNS("urn:schemas-upnp-org:service:" + action.getService()->getServiceType(), action.getName() + "Response");
    
    for(Action::ArgumentIterator i = action.beginOutArgument(); i != action.endOutArgument(); ++i) {
        AutoPtr<Element> pArgument = pDoc->createElement((*i)->getName());
        AutoPtr<Text> pArgumentValue = pDoc->createTextNode(action.getArgument<std::string>((*i)->getName()));
        std::cerr << "ActionResponseWriter returns arg: " << (*i)->getName() << ", val: " << action.getArgument<std::string>((*i)->getName()) << std::endl;
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


StateVar*
Argument::getRelatedStateVarReference() const
{
    return m_pAction->getService()->getStateVarReference(m_relatedStateVar);
}


EventMessageWriter::EventMessageWriter()
{
    std::cerr << "EventMessageWriter::EventMessageWriter()" << std::endl;
    
    m_pDoc = new Document;
    m_pPropertySet = m_pDoc->createElementNS("urn:schemas-upnp-org:event-1-0", "propertyset");
    m_pDoc->appendChild(m_pPropertySet);
}


void
EventMessageWriter::write(std::string& eventMessage)
{
    std::cerr << "EventMessageWriter::write()" << std::endl;
    
    DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.setOptions(XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    eventMessage = ss.str();
    std::cerr << "event message():" << std::endl << ss.str() << std::endl;
}


void
EventMessageWriter::stateVar(const StateVar& stateVar)
{
    std::cerr << "EventMessageWriter::stateVar()" << std::endl;
    
    AutoPtr<Element> pProperty = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "property");
    AutoPtr<Element> pStateVar = m_pDoc->createElement(stateVar.getName());
    AutoPtr<Text> pStateVarValue = m_pDoc->createTextNode(stateVar.getValue());
    pStateVar->appendChild(pStateVarValue);
    pProperty->appendChild(pStateVar);
    m_pPropertySet->appendChild(pProperty);
}


Subscription::Subscription(std::string callbackUri) :
m_deliveryAddress(callbackUri)
{
    std::cerr << "Subscription::Subscription() uri: " << callbackUri << std::endl;
    // TODO: implement timer stuff
    m_uuid = UUIDGenerator().createRandom();
    std::cerr << "SID: " << m_uuid.toString() << std::endl;
    m_eventKey = 0;
    m_pSession = new HTTPClientSession(m_deliveryAddress.getHost(), m_deliveryAddress.getPort());
}


std::string
Subscription::getEventKey()
{
    // TODO: should lock this
    m_eventKey = (++m_eventKey == 0) ? 1 : m_eventKey;
    return NumberFormatter::format(m_eventKey);
}


HTTPRequest*
Subscription::getRequest()
{
    HTTPRequest* res = new HTTPRequest("NOTIFY", m_deliveryAddress.getPath(), "HTTP/1.1");
    res->set("HOST", m_deliveryAddress.getAuthority());
    res->setContentType("text/xml");
    res->set("NT", "upnp:event");
    res->set("NTS", "upnp:propchange");
    res->set("SID", "uuid:" + m_uuid.toString());
    return res;
}


void
Subscription::sendEventMessage(const std::string& eventMessage)
{
    // set remaining request header fields
    HTTPRequest* request = getRequest();
    request->set("SEQ", getEventKey());
    request->setContentLength(eventMessage.size());
    // set request body and send request
    std::ostream& ostr = getSession()->sendRequest(*request);
    ostr << eventMessage;
    // TODO: receive answer ...?
}


void
Subscription::renew(int seconds)
{
}


void
Subscription::expire(Timer& timer)
{
}


Service::~Service()
{
}


void
Service::addAction(Action* pAction)
{
    m_actions.append(pAction->getName(), pAction);
    pAction->setService(this);
}


void
Service::addStateVar(StateVar* pStateVar)
{
    std::cerr << "Service::addStateVar() name: " << pStateVar->getName() << " is evented: " << pStateVar->getSendEvents() << std::endl;
    
    m_stateVars.append(pStateVar->getName(), pStateVar);
    if(pStateVar->getSendEvents()) {
        m_eventedStateVars.append(pStateVar->getName(), pStateVar);
    }
}

void
Service::registerSubscription(Subscription* subscription)
{
    std::cerr << "Service::registerSubscription()" << std::endl;
    
    Poco::ScopedLock<Poco::FastMutex> lock(m_serviceLock);
    // TODO: only register a Subscription once from one distinct Controller
    //       note that Subscription has a new SID
    std::string sid = subscription->getUuid();
    std::cerr << "SID: " << sid << std::endl;
    m_eventSubscriptions.append(sid, subscription);
}


void
Service::unregisterSubscription(Subscription* subscription)
{
    std::cerr << "Service::unregisterSubscription()" << std::endl;
    
    Poco::ScopedLock<Poco::FastMutex> lock(m_serviceLock);
    m_eventSubscriptions.remove(subscription->getUuid());
    delete subscription;
}


void
Service::sendEventMessage(StateVar& stateVar)
{
    std::cerr << "Service::sendEventMessage()" << std::endl;
    
    // TODO: send the messages asynchronous and don't block the Device main thread
    std::string eventMessage;
    EventMessageWriter messageWriter;
    messageWriter.stateVar(stateVar);
    messageWriter.write(eventMessage);
    
    for (SubscriptionIterator i = beginEventSubscription(); i != endEventSubscription(); ++i) {
        (*i)->sendEventMessage(eventMessage);
    }
}


void
Service::sendInitialEventMessage(Subscription* pSubscription)
{
    std::cerr << "Service::sendInitialEventMessage()" << std::endl;
    
    std::string eventMessage;
    EventMessageWriter messageWriter;
    for (StateVarIterator i = beginEventedStateVar(); i != endEventedStateVar(); ++i) {
        messageWriter.stateVar(**i);
    }
    messageWriter.write(eventMessage);
    pSubscription->sendEventMessage(eventMessage);
}


Action*
Action::clone()
{
    Action* res = new Action();
    res->m_actionName = m_actionName;
    res->m_pService = m_pService;
    // make a deep copy of the Arguments
    for (Container<Argument>::Iterator i = m_arguments.begin(); i != m_arguments.end(); ++i) {
        res->appendArgument(new Argument(**i));
    }
    return res;
}


void
Action::appendArgument(Argument* pArgument)
{
    pArgument->setAction(this);
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
// m_description(description)
m_pDescription(&description)
{
}


DescriptionRequestHandler*
DescriptionRequestHandler::create()
{
    // TODO: can we somehow avoid to make a copy of the RequestHandler on each request?
//     return new DescriptionRequestHandler(m_description);
    return new DescriptionRequestHandler(*m_pDescription);
}


void
DescriptionRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle description request: " << request.getURI() << std::endl;
//         std::cerr << "sending contents of length: " << m_description.size() << std::endl;
//     response.setContentLength(m_description.size());
    response.setContentLength(m_pDescription->size());
    response.setContentType("text/xml");
    std::ostream& ostr = response.send();
//     ostr << m_description;
    ostr << *m_pDescription;
}


// ControlRequestHandler::ControlRequestHandler(DeviceRoot& deviceRoot):
// m_deviceRoot(&deviceRoot)
// {
// }
// 
// 
// ControlRequestHandler*
// ControlRequestHandler::create()
// {
//     return new ControlRequestHandler(*m_deviceRoot);
// }


ControlRequestHandler::ControlRequestHandler(Service* service):
m_pService(service)
{
}


ControlRequestHandler*
ControlRequestHandler::create()
{
    return new ControlRequestHandler(m_pService);
}


void
ControlRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "***********************" << std::endl
              << "handle control request: " << request.getURI() << std::endl;
    // synchronous action handling: wait until handleAction() has finished. This must be done in under 30 sec,
    // otherwise it should return and an event should be sent on finishing the action request.
    int length = request.getContentLength();
    char buf[length];
    request.stream().read(buf, length);
    std::string requestBody(buf, length);
    
    std::string soap = request.get("SOAPACTION");
    std::string soapAction = soap.substr(1, soap.size()-2);
    std::string::size_type hash = soapAction.find('#');
    std::string serviceType = soapAction.substr(0, hash);
    std::string actionName = soapAction.substr(hash+1);
    std::cerr << "SOAP request, service: " << serviceType << ", action: " << actionName << std::endl;
    
    std::cerr << "getServiceType(): " << m_pService->getServiceType() << std::endl;
    Action* pAction = m_pService->getAction(actionName);
    std::cerr << "getAction(): " << pAction->getName() << std::endl;
    pAction = pAction->clone();
    std::cerr << "cloned Action(): " << pAction->getName() << std::endl;
    // TODO: introduce ActionRequestReader::write(Action*) to get rid of confusing pAction stuff
    ActionRequestReader requestReader(requestBody, pAction);
    pAction = requestReader.action();
    // the corresponding Service should register as a Notification Handler
    m_pService->getDevice()->getDeviceRoot()->postAction(pAction);
    // return Action response with out arguments filled in by Notification Handler
    std::string responseBody;
    ActionResponseWriter responseWriter(responseBody);
    responseWriter.action(*pAction);
    
    response.setContentType("text/xml");
        // TODO: set EXT header
        // TODO: set SERVER header
    response.setContentLength(responseBody.size());
    std::ostream& ostr = response.send();
    ostr << responseBody;
    std::cerr << "finished control request: " << request.getURI() << std::endl
              << "*************************" << std::endl;
}


EventRequestHandler*
EventRequestHandler::create()
{
    return new EventRequestHandler(m_pService);
}


void
EventRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    std::cerr << "handle event request: " << request.getMethod() << std::endl;
    std::cerr << "HOST: " << request.getHost() << std::endl;
    std::cerr << "CALLBACK: " << request.get("CALLBACK") << std::endl;
    std::cerr << "NT: " << request.get("NT") << std::endl;
    std::cerr << "TIMEOUT: " << request.get("TIMEOUT") << std::endl;
    
    std::string sid;
    
    if (request.getMethod() == "SUBSCRIBE") {
        Timestamp t;
        if (request.has("SID")) {
            sid = request.get("SID");
            // renew subscription
            m_pService->getSubscription(sid)->renew(1800);
        }
        else {
            Subscription* pSubscription = new Subscription(request.get("CALLBACK"));
            m_pService->sendInitialEventMessage(pSubscription);
            m_pService->registerSubscription(pSubscription);
        }
        response.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
        response.set("SERVER", 
                    Environment::osName() + "/"
                    + Environment::osVersion() + ", "
                    + "UPnP/" + UPNP_VERSION + ", "
                    + JAMM_VERSION);
        response.set("SID", "uuid:" + UUIDGenerator().create().toString());
        response.set("TIMEOUT", "Second-1800");
        // TODO: make shure the SUBSCRIBE message is received by the controller before
        //       sending out the initial event message.
        // TODO: choose timeout according to controller activity
        // TODO: provide TCP FIN flag or Content-Length=0 before initial event message (see specs p. 65)
        // TODO: may make subscription uuid's persistance
    }
    else if (request.getMethod() == "UNSUBSCRIBE") {
        m_pService->unregisterSubscription(m_pService->getSubscription(sid));
    }
}


HttpSocket::HttpSocket(NetworkInterface interface)
{
    std::cerr << "HttpSocket()" << std::endl;
    
    m_pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory(this);
    
    HTTPServerParams* pParams = new HTTPServerParams;
    //     pParams->setMaxQueued(maxQueued);
    //     pParams->setMaxThreads(maxThreads);
        // set-up a server socket on an available port
    ServerSocket socket(0);
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out. Or: bind to 0.0.0.0 and broadcast SSDP to all available network interfaces by default.
    //     socket.bind(m_ssdpSocket.m_interface.address());
    m_httpServerAddress = SocketAddress(interface.address(), socket.address().port());
    m_pHttpServer = new HTTPServer(m_pDeviceRequestHandlerFactory, socket, pParams);
}


void
HttpSocket::startServer()
{
    m_pHttpServer->start();
    std::cerr << "started HTTP server on: " << m_httpServerAddress.toString() << std::endl;
}


void
HttpSocket::stopServer()
{
    m_pHttpServer->stop();
    std::cerr << "stopped HTTP server on: " << m_httpServerAddress.toString() << std::endl;
}


HttpSocket::~HttpSocket()
{
    delete m_pHttpServer;
}


Device::Device()
{
}


Device::~Device()
{
}


void
Device::addService(Service* pService)
{
    std::cerr << "Device::addService(): " << pService->getServiceType() << std::endl;
    m_services.append(pService->getServiceType(), pService);
    m_pDeviceRoot->addServiceType(pService->getServiceType(), pService);
    pService->setDevice(this);
}


DeviceRoot::DeviceRoot() :
// TODO: allocate sockets later, not in ctor (e.g. jammgen)
m_ssdpSocket(Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage)),
m_httpSocket(m_ssdpSocket.m_interface)
{
}


DeviceRoot::~DeviceRoot()
{
    std::cerr << "DeviceRoot::~DeviceRoot()" << std::endl;
    stopSsdp();
    stopHttp();
//     delete m_descriptionRequestHandler;
}


Service*
DeviceRoot::getServiceType(const std::string& serviceType)
{
    std::cerr << "DeviceRoot::getServiceType(): " << serviceType << std::endl;
    std::map<std::string,Service*>::iterator i = m_serviceTypes.find(serviceType);
    if (i == m_serviceTypes.end()) {
        std::cerr << "could not find serviceType" << std::endl;
    }
    return m_serviceTypes[serviceType];
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
        Device& device = **d;
        aliveWriter.device(device);
        byebyeWriter.device(device);
        for(Device::ServiceIterator s = device.beginService(); s != device.endService(); ++s) {
            Service& service = **s;
            aliveWriter.service(service);
            byebyeWriter.service(service);
            
//             service.setDescriptionRequestHandler(); // TODO: probably not needed ...
//             registerHttpRequestHandler(service.getDescriptionPath(), service.getDescriptionRequestHandler());
            
            registerHttpRequestHandler(service.getDescriptionPath(), new DescriptionRequestHandler((**s).getDescription()));
            registerHttpRequestHandler(service.getControlPath(), new ControlRequestHandler(&(**s)));
            registerHttpRequestHandler(service.getEventPath(), new EventRequestHandler(&(**s)));
        }
    }
    std::cerr << "DeviceRoot::init() finished" << std::endl;
}


DeviceRootImplAdapter::DeviceRootImplAdapter()
{
    std::cerr << "DeviceRootImplAdapter::DeviceRootImplAdapter()" << std::endl;
    // register the great action dispatcher
};


DeviceRootImplAdapter::~DeviceRootImplAdapter()
{
    std::cerr << "DeviceRootImplAdapter::~DeviceRootImplAdapter()" << std::endl;
    delete m_pDeviceRoot;
}


void
DeviceRootImplAdapter::start()
{
    // TODO: write modified device description to m_description
    // TODO: maybe, write newly assigned uuids as keys to DeviceRoot::m_devices
    //       -> Container needs an append(const Entity&)
    std::cerr << "DeviceRootImplAdapter::start()" << std::endl;
    m_pDeviceRoot->registerActionHandler(Observer<DeviceRootImplAdapter, Action>(*this, &DeviceRootImplAdapter::actionHandler));
    
    m_pDeviceRoot->init();
    m_pDeviceRoot->startHttp();
    m_pDeviceRoot->startSsdp();
}


void
DeviceRootImplAdapter::stop()
{
    std::cerr << "DeviceRootImplAdapter::stop()" << std::endl;
    m_pDeviceRoot->stopSsdp();
    m_pDeviceRoot->stopHttp();
}


void
DeviceRootImplAdapter::setFriendlyName(const std::string& friendlyName)
{
    // set Property friendlyName of **Device**
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
    m_httpSocket.startServer();
}


void
DeviceRoot::stopHttp()
{
    m_httpSocket.stopServer();
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
    
    if (pNf->getRequestMethod() == SsdpMessage::REQUEST_SEARCH) {
        SsdpMessage m;
        // TODO: use a skeleton to create response message
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
        
        // TODO: react on ST field (search target)
        // TODO: react on MX field (seconds to delay response)
        //       -> create an SsdpMessageSet and send it out delayed
        // TODO: fill in the correct value for CacheControl
        //       -> m_ssdpNotifyAliveMessages.m_sendTimer
        //       -> need to know the elapsed time ... (though initial timer val isn't so wrong)
        
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


Urn::Urn(const std::string& urn) :
m_urn(urn)
{
    std::vector<std::string> vec;
    std::string::size_type left = 0;
    std::string::size_type right = 0;
    do 
    {
        left = m_urn.find(':', right);
        right = m_urn.find(':', ++left);
        vec.push_back(m_urn.substr(left, right - left));
    } while (right != std::string::npos);
    m_domainName = vec[0];
    m_type = vec[1];
    m_typeName = vec[2];
    m_version = vec[3];
}


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
//         std::cerr << "SsdpSocket::onReadable() receives: " << std::endl << std::string(m_pBuffer, n) << std::endl;
        m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n), sender));
    }
}


void
SsdpSocket::sendMessage(SsdpMessage& message, const SocketAddress& receiver)
{
    std::string m = message.toString();
    int bytesSent = m_ssdpSocket.sendTo(m.c_str(), m.length(), receiver);
//     std::cerr << "SsdpSocket::sendMessage() message sent: " << std::endl << m;
}
