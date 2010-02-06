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
    // FIXME: Bug in Poco? getPath should not return leading "/", it's not part of path
    //        URL, url-path: 
    //        http://tools.ietf.org/html/rfc1738, Chapter 3.1  
    //        in contradiction to:
    //        URI, abs_path: (url-path corresponds to path_segments)
    //        http://tools.ietf.org/html/rfc2396#section-3
    
    // TODO: make all paths always relative (remove leading "/")
    // file://bla/foo/file is URI bla must be a host (RFC 1738)
    // file:/bla/foo/file is URI for path bla/foo/file
    // URN is: <scheme>://<authority>/<path>
    // <scheme>:<path>
    // <scheme>://<authority>
    // <scheme>://<authority>/<path>
    //
    // NOTE: relative URIs see: http://tools.ietf.org/html/rfc2396#section-5
    //
    // if m_uri is directory (base directory), it must end with "/"
    // does file:myfile point to ./myfile ?
    // if basepath is "" -> "./"
    // if basepath has no trailing "/", append it
    //
    // NOTE: all URLs refer to a base URL, same host and port for all URLs (-> BaseURL)
    //       details on URL: http://tools.ietf.org/html/rfc1738
    //       details on URI: http://tools.ietf.org/html/rfc3986
    //
    //     URLBase
    //          Optional. Defines the base URL. Used to construct fully-qualified URLs. 
    //          All relative URLs that appear elsewhere in
    //          the description are combined with this base URL according to the rules in RFC 2396. 
    //          If URLBase is empty or not
    //          given, the base URL is the URL from which the device description was retrieved 
    //          (which is the preferred implementation; use of URLBase is no longer recommended).
    //          Specified by UPnP vendor. Single URL.
        
    std::string p = m_uri.getPath() + path;
    std::cerr << "UriDescriptionReader::getDescription() from: " << m_uri.toString() << std::endl;
    std::cerr << "request path is: " << p << std::endl;
    std::string* res;
    
    if (m_uri.getScheme() == "file") {
        std::stringstream ss;
        
        std::ifstream ifs(p.c_str());
        StreamCopier::copyStream(ifs, ss);
        res = new std::string(ss.str());
    }
    else if (m_uri.getScheme() == "http") {
        HTTPClientSession session(m_uri.getHost(), m_uri.getPort());
        HTTPRequest request("GET", path);
        session.sendRequest(request);
        
        // TODO: platinum's URL has no leading / but expects one ...
        HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);
        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
            std::cerr << "Error: " << path << " HTTP_NOT_FOUND 404" << std::endl;
            std::cerr << "Trying: /" << path << std::endl;
            request.setURI("/" + path);
            session.sendRequest(request);
            response.clear();
            std::istream& rs = session.receiveResponse(response);
        }
        char* buf = new char[response.getContentLength()];
        rs.read(buf, response.getContentLength());
        res = new std::string(buf, response.getContentLength());
        
        std::cerr << "downloaded description:" << std::endl << "*BEGIN*" << *res << "*END*" << std::endl;
    }
    else {
        std::cerr << "Error in UriDescriptionReader: unknown scheme in description uri" << std::endl;
    }
    
    // TODO: put this into deviceRoot() as it is common with StringDescriptionReader
    DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Node* n = m_pDocStack.top()->documentElement()->firstChild();
//     std::cerr << "first node: " << n->nodeName() << ", " << n << std::endl;
    m_nodeStack.push(n);
    std::cerr << "UriDescriptionReader::getDescription() finished" << std::endl;
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
//     std::cerr << "StringDescriptionReader::getDescription()" << std::endl;
    std::string* res = (*m_pStringMap)[path];
    DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Node* n = m_pDocStack.top()->documentElement()->firstChild();
//     std::cerr << "first node: " << n->nodeName() << ", " << n << std::endl;
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
    // NOTE: a running HttpSocket is needed here, to set host and port of BaseUri and DescriptionUri
    //       that's why jammgen crashes without setting up a socket in HttpSocket::init()
    pRes->setBaseUri();
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
        if (pNode->nodeName() == "deviceType") {
            pRes->setDeviceType(pNode->innerText());
        }
        else if (pNode->nodeName() == "UDN") {
            pRes->setUuid(pNode->innerText().substr(5));
        }
        else if (pNode->nodeName() == "serviceList") {
            if (pNode->hasChildNodes()) {
                Node* pChild = pNode->firstChild();
                while (pChild) {
                    if (pChild->nodeName() == "service") {
                        if (pChild->hasChildNodes()) {
                            m_nodeStack.push(pChild->firstChild());
                            pRes->addService(service());
                        }
                        else {
                            std::cerr << "Error in DescriptionReader: empty service" << std::endl;
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                std::cerr << "Error in DescriptionReader: service list without services" << std::endl;
            }
        }
        else if (pNode->nodeName() == "deviceList") {
            if (pNode->hasChildNodes()) {
                Node* pChild = pNode->firstChild();
                while (pChild) {
                    if (pChild->nodeName() == "device") {
                        if (pChild->hasChildNodes()) {
                            m_nodeStack.push(pChild->firstChild());
                            m_pDeviceRoot->addDevice(device());
                        }
                        else {
                            std::cerr << "Error in DescriptionReader: empty embedded device" << std::endl;
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                std::cerr << "Error in DescriptionReader: device list without embedded devices" << std::endl;
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
            pRes->setServiceType(pNode->innerText());
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            pRes->setDescriptionPath(pNode->innerText());
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
            pRes->setControlPath(pNode->innerText());
        }
        else if (pNode->nodeName() == "eventSubURL" && pNode->hasChildNodes()) {
            pRes->setEventPath(pNode->innerText());
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
            pRes->setName(pNode->innerText());
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
            pRes->setName(pNode->innerText());
        }
        else if (pNode->nodeName() == "relatedStateVariable" && pNode->hasChildNodes()) {
            pRes->setRelatedStateVar(pNode->innerText());
        }
        else if (pNode->nodeName() == "direction" && pNode->hasChildNodes()) {
            pRes->setDirection(pNode->innerText());
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
            pRes->setName(pNode->innerText());
        }
        else if (pNode->nodeName() == "dataType" && pNode->hasChildNodes()) {
            pRes->setType(pNode->innerText());
        }
        else if (pNode->nodeName() == "defaultValue" && pNode->hasChildNodes()) {
            std::string val = pNode->innerText();
            pRes->setDefaultValue(val);
            // FIXME: seems StateVar's value isn't set to the default value
//             std::cerr << "DescriptionReader::stateVar() set defaultValue: " << val << std::endl;
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
//     std::cerr << "ActionRequestReader::action()" << std::endl;
    Action* pRes = m_pActionTemplate;
    Node* pNode = m_nodeStack.top();
    NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Node* pAction = pBody->firstChild();
//         std::cerr << "action: " << pRes->getName() << std::endl;
//         std::cerr << "serviceType: " << pRes->getService()->getServiceType() << std::endl;
        
        if (pAction->hasChildNodes()) {
            Node* pArgument = pAction->firstChild();
    
            while (pArgument) {
//                 std::cerr << "ActionRequestReader::action() setting Argument: " << pArgument->nodeName() << " val: " << pArgument->innerText() << std::endl;
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            std::cerr << "ActionRequestReader(): action without arguments" << std::endl;
        }
    }
    else {
        std::cerr << "Error in ActionRequestReader(): action without body" << std::endl;
    }
    m_nodeStack.pop();
//     std::cerr << "ActionRequestReader::action() finished" << std::endl;
    return pRes;
}


ActionResponseReader::ActionResponseReader(const std::string& responseBody, Action* pActionTemplate) :
m_pActionTemplate(pActionTemplate)
{
    std::cerr << "ActionResponseReader::ActionResponseReader()" << std::endl;
    std::cerr << "response: " << responseBody << std::endl;
    
    DOMParser parser;
    m_pDoc = parser.parseString(responseBody);
    NodeIterator it(m_pDoc, NodeFilter::SHOW_ALL);
    m_nodeStack.push(it.nextNode());
}


Action*
ActionResponseReader::action()
{
    // TODO: same code as in ActionRequestReader
    std::cerr << "ActionResponseReader::action()" << std::endl;
    Action* pRes = m_pActionTemplate;
    Node* pNode = m_nodeStack.top();
    NodeIterator it(pNode, NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Node* pAction = pBody->firstChild();
//         std::cerr << "action: " << pRes->getName() << std::endl;
//         std::cerr << "serviceType: " << pRes->getService()->getServiceType() << std::endl;
        
        if (pAction->hasChildNodes()) {
            Node* pArgument = pAction->firstChild();
            
            while (pArgument) {
//                 std::cerr << "ActionRequestReader::action() setting Argument: " << pArgument->nodeName() << " val: " << pArgument->innerText() << std::endl;
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            std::cerr << "ActionResponseReader(): action without arguments" << std::endl;
        }
    }
    else {
        std::cerr << "Error in ActionResponseReader(): action without body" << std::endl;
    }
    m_nodeStack.pop();
    std::cerr << "ActionResponseReader::action() finished" << std::endl;
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


void
ActionRequestWriter::action(Action* action)
{
    // TODO: nearly same code as in ActionResponseWriter
    m_pDoc = new Document;
    AutoPtr<Element> pEnvelope = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    AutoPtr<Element> pBody = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    AutoPtr<Element> pActionRequest = m_pDoc->createElementNS(action->getService()->getServiceType(), action->getName());
    
    for(Action::ArgumentIterator i = action->beginInArgument(); i != action->endInArgument(); ++i) {
        AutoPtr<Element> pArgument = m_pDoc->createElement((*i)->getName());
        AutoPtr<Text> pArgumentValue = m_pDoc->createTextNode(action->getArgument<std::string>((*i)->getName()));
        pArgument->appendChild(pArgumentValue);
        pActionRequest->appendChild(pArgument);
        
        std::cerr << "ActionRequestWriter called with arg: " << (*i)->getName() << ", val: " << action->getArgument<std::string>((*i)->getName()) << std::endl;
    }
    
    pBody->appendChild(pActionRequest);
    pEnvelope->appendChild(pBody);
    m_pDoc->appendChild(pEnvelope);
}


void
ActionRequestWriter::write(std::string& actionMessage)
{
    DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.setOptions(XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    actionMessage = ss.str();
    std::cerr << "action request message():" << std::endl << ss.str() << std::endl;
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
    AutoPtr<Element> pActionResponse = pDoc->createElementNS(action.getService()->getServiceType(), action.getName() + "Response");
    
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


void
Subscription::sendEventMessage(const std::string& eventMessage)
{
    // TODO: queue the eventMessages for sending ...?
    std::cerr << "Subscription::sendEventMessage()" << std::endl;
    std::string path = m_deliveryAddress.getPath();
    if (path.substr(0, 1) == "/") {
        path = path.substr(1);
    }
    HTTPRequest request("NOTIFY", path, "HTTP/1.1");
    request.set("HOST", m_deliveryAddress.getAuthority());
    request.setContentType("text/xml");
    request.set("NT", "upnp:event");
    request.set("NTS", "upnp:propchange");
    request.set("SID", "uuid:" + m_uuid.toString());
    
//     HTTPRequest* request = newRequest();
    request.set("SEQ", getEventKey());
    request.setContentLength(eventMessage.size());
    // set request body and send request
    std::cerr << "Subscription::sendEventMessage() send request to: " << getSession()->getHost() << ":" << getSession()->getPort() << "/" << request.getURI() << " ..." << std::endl;
    std::cerr << "Header:" << std::endl;
    request.write(std::cerr);
    
    std::ostream& ostr = getSession()->sendRequest(request);
    ostr << eventMessage;
    
    // receive answer ...
    HTTPResponse response;
    getSession()->receiveResponse(response);
    int stat = response.getStatus();
    if (stat == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
        std::cerr << "Error: " << request.getURI() << " HTTP_NOT_FOUND 404" << std::endl;
//         std::cerr << "Trying: " << m_deliveryAddress.getPath().substr(1) << std::endl;
//         request.setURI(m_deliveryAddress.getPath().substr(1));
//         std::cerr << "Subscription::sendEventMessage() send request to: " << getSession()->getHost() << ":" << getSession()->getPort() << request.getURI() << " ..." << std::endl;
//         std::cerr << "Header:" << std::endl;
//         request.write(std::cerr);
//         
//         std::ostream& ostr = getSession()->sendRequest(request);
//         ostr << eventMessage;
//         
//         response.clear();
//         getSession()->receiveResponse(response);
    }
    else if (stat == Poco::Net::HTTPResponse::HTTP_OK) {
        std::cerr << "Subscription::sendEventMessage() recieved HTTP_OK 200" << std::endl;
    }
    std::cerr << "Subscription::sendEventMessage() finished" << std::endl;
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
Service::initClient()
{
    m_pControlRequestSession = new HTTPClientSession(SocketAddress(m_pDevice->getDeviceRoot()->getBaseUri().getAuthority()));
    // TODO: subscribe to services
    // -> generate eventHandlerURIs: device uuid + service type
    // TODO: setup event message handler
    // -> this should go in DeviceRoot::initClient()?
    // -> or in Controller::init()?
    // m_pDevice->getDeviceRoot()->registerEventHandler(...);
}


void
Service::sendAction(Action* pAction)
{
    std::cerr << "*********************" << std::endl;
    std::cerr << "Service::sendAction()" << std::endl;
    std::string actionMessage;
    ActionRequestWriter requestWriter;
    requestWriter.action(pAction);
    requestWriter.write(actionMessage);
    
    HTTPRequest* request = new HTTPRequest("POST", m_controlPath, "HTTP/1.1");
    request->set("HOST", m_pDevice->getDeviceRoot()->getBaseUri().getAuthority());
    request->setContentType("text/xml; charset=\"utf-8\"");
    request->set("SOAPACTION", "\"" + m_serviceType + "#" + pAction->getName() + "\"");
    request->setContentLength(actionMessage.size());
    // set request body and send request
    std::cerr << "Service::sendAction() send request to: " << m_pDevice->getDeviceRoot()->getBaseUri().getAuthority() << request->getURI() << " ..." << std::endl;
    std::cerr << "Header:" << std::endl;
    request->write(std::cerr);
    
    std::ostream& ostr = m_pControlRequestSession->sendRequest(*request);
    ostr << actionMessage;
    
    // receive answer ...
    HTTPResponse response;
    std::istream& rs = m_pControlRequestSession->receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
        std::cerr << "Error: " << m_controlPath << " HTTP_NOT_FOUND 404" << std::endl;
    }
    char* buf = new char[response.getContentLength()];
    rs.read(buf, response.getContentLength());
    std::string responseBody = std::string(buf, response.getContentLength());
    std::cerr << "Service::sendAction() response received: " << responseBody << std::endl;
    ActionResponseReader responseReader(responseBody, pAction);
    responseReader.action();
    std::cerr << "Service::sendAction() finished" << std::endl;
    std::cerr << "******************************" << std::endl;
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
//     std::cerr << "register request handler: " << Uri << std::endl;
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
    
    std::cerr << "ControlRequestHandler for ServiceType: " << m_pService->getServiceType() << std::endl;
    // TODO: make getAction() robust against wrong action names
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
            std::string callback = request.get("CALLBACK");
            callback = callback.substr(1, callback.size() - 2);
            Subscription* pSubscription = new Subscription(callback);
            m_pService->sendInitialEventMessage(pSubscription);
            m_pService->registerSubscription(pSubscription);
        }
        response.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
        response.set("SERVER", 
                    Environment::osName() + "/"
                    + Environment::osVersion() + ", "
                    + "UPnP/" + UPNP_VERSION + ", "
                    + "Jamm/" + JAMM_VERSION);
        response.set("SID", "uuid:" + UUIDGenerator().create().toString());
        response.set("TIMEOUT", "Second-1800");
        response.setContentLength(0);
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


HttpSocket::HttpSocket(NetworkInterface interface) :
m_interface(interface)
{
    init();
}


void
HttpSocket::init()
{
    m_pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory(this);
    ServerSocket socket(0);
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out. Or: bind to 0.0.0.0 and broadcast SSDP to all available network interfaces by default.
    //     socket.bind(m_ssdpSocket.m_interface.address());
    HTTPServerParams* pParams = new HTTPServerParams;
    m_httpServerAddress = SocketAddress(m_interface.address(), socket.address().port());
    m_pHttpServer = new HTTPServer(m_pDeviceRequestHandlerFactory, socket, pParams);
}


void
HttpSocket::startServer()
{

    //     pParams->setMaxQueued(maxQueued);
    //     pParams->setMaxThreads(maxThreads);
        // set-up a server socket on an available port

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
// TODO: allocate sockets later, not in ctor (e.g. jammgen doesn't need them)
// m_ssdpSocket(/*Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage)*/),
m_ssdpSocket(NetworkInterface::forName("wlan0")),
m_httpSocket(m_ssdpSocket.getInterface())
{
}


DeviceRoot::~DeviceRoot()
{
    std::cerr << "DeviceRoot::~DeviceRoot()" << std::endl;
    stopSsdp();
    stopHttp();
    // TODO: free all Devices, Services, Actions, ...
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
DeviceRoot::print()
{
    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        std::cout << "DeviceType: " << (*d)->getDeviceType() << std::endl;
        for(Device::ServiceIterator s = (*d)->beginService(); s != (*d)->endService(); ++s) {
            std::cout << "    Service pointer: " << *s << std::endl;
            std::cout << "    ServiceType: " << (*s)->getServiceType() << std::endl;
            int stateVarCount = 0;
            for(Service::StateVarIterator v = (*s)->beginStateVar(); v != (*s)->endStateVar(); ++v) {
                stateVarCount++;
                std::string val;
                (*v)->getValue(val);
                std::cout
                    << "        StateVar: " << (*v)->getName() << std::endl
                    << "          number: " << stateVarCount << std::endl
                    << "            type: " << (*v)->getType() << std::endl
                    << "         evented: " << (*v)->getSendEvents() << std::endl
                    << "             val: " << val << std::endl;
            }
        }
    }
}


void
DeviceRoot::initStateVars(const std::string& serviceType, Service* pThis)
{
    std::cerr << "DeviceRoot::initStateVars() serviceType: " << serviceType << " , pThis: " << pThis << std::endl;
    m_pDeviceRootImplAdapter->initStateVars(serviceType, pThis);
}


void
DeviceRoot::initController()
{
    std::cerr << "DeviceRoot::initController()" << std::endl;
    
    std::cerr << "DeviceRoot::initController() finished" << std::endl;
}


void
DeviceRoot::initDevice()
{
    // TODO: break this up into DeviceRoot::initDevice(), Device::initDevice(), Service::initDevice()
    std::cerr << "DeviceRoot::initDevice()" << std::endl;
        
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
            Service* ps = *s;
            
            initStateVars(ps->getServiceType(), ps);
            
            aliveWriter.service(*ps);
            byebyeWriter.service(*ps);
            
            registerHttpRequestHandler(ps->getDescriptionPath(), new DescriptionRequestHandler(ps->getDescription()));
            registerHttpRequestHandler(ps->getControlPath(), new ControlRequestHandler(ps));
            registerHttpRequestHandler(ps->getEventPath(), new EventRequestHandler(ps));
        }
    }
    std::cerr << "DeviceRoot::initDevice() finished" << std::endl;
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
    
//     m_pDeviceRoot->print();
    m_pDeviceRoot->initDevice();
//     m_pDeviceRoot->print();
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
    m_ssdpSocket.setObserver(Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage));
    m_ssdpSocket.init();
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
DeviceRoot::handleSsdpMessage(SsdpMessage* pMessage)
{
//     std::cerr << "root device gets SSDP message:" << std::endl;
//     std::cerr << pNf->toString();
    
    if (pMessage->getRequestMethod() == SsdpMessage::REQUEST_SEARCH) {
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
        
        m_ssdpSocket.sendMessage(m, pMessage->getSender());
    }
}


Controller::Controller() :
// m_ssdpSocket(Observer<Controller, SsdpMessage>(*this, &Controller::handleSsdpMessage))
m_ssdpSocket(NetworkInterface::forName("wlan0"))
{

}


void
Controller::init()
{
    m_ssdpSocket.setObserver(Observer<Controller, SsdpMessage>(*this, &Controller::handleSsdpMessage));
//     m_ssdpSocket.setUnicastObserver(Observer<Controller, SsdpMessage>(*this, &Controller::handleMSearchResponse));
    m_ssdpSocket.init();
    sendMSearch();
}


void
Controller::sendMSearch()
{
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
Controller::handleSsdpMessage(SsdpMessage* pMessage)
{
    // we load all device descriptions, regardless of service types contained in the device
    
    // get UUID from USN
    std::string usn = pMessage->getUniqueServiceName();
    std::string::size_type left = usn.find(":") + 1;
    std::string uuid = usn.substr(left, usn.find("::") - left);
    std::cerr << "Controller::handleSsdpMessage() with UUID: " << uuid << std::endl;
//     std::cerr << pMessage->toString();
    
    switch(pMessage->getRequestMethod()) {
    case SsdpMessage::REQUEST_NOTIFY:
//         std::cerr << "Controller::handleSsdpMessage() REQUEST_NOTIFY" << std::endl;
        switch(pMessage->getNotificationSubtype()) {
        case SsdpMessage::SUBTYPE_ALIVE:
//             std::cerr << "Controller::handleSsdpMessage() REQUEST_NOTIFY_ALIVE" << std::endl;
            if (pMessage->getNotificationType() == "upnp:rootdevice" && !m_devices.contains(uuid)) {
                URI location = pMessage->getLocation();
                std::string baseUri = location.getScheme() + "://" + location.getAuthority();
                UriDescriptionReader descriptionReader(URI(baseUri), location.getPath());
                DeviceRoot* deviceRoot = descriptionReader.deviceRoot();
                deviceRoot->setBaseUri(URI(baseUri));
                addDevice(deviceRoot);
            }
            break;
        case SsdpMessage::SUBTYPE_BYEBYE:
//             std::cerr << "Controller::handleSsdpMessage() REQUEST_NOTIFY_BYEBYE" << std::endl;
            if (pMessage->getNotificationType() == "upnp:rootdevice") {
                removeDevice(uuid);
            }
            break;
        }
    break;
    case SsdpMessage::REQUEST_RESPONSE:
        std::cerr << "Controller::handleSsdpMessage() REQUEST_RESPONSE" << std::endl;
        if (!m_devices.contains(uuid)) {
            URI location = pMessage->getLocation();
            std::cerr << "Controller::handleSsdpMessage() LOCATION: " <<  location.toString() << std::endl;
            
            std::string baseUri = location.getScheme() + "://" + location.getAuthority();
            std::cerr << "Controller::handleSsdpMessage() root device baseUri: " << baseUri << " , path: " << location.getPath() << std::endl;
            
            UriDescriptionReader descriptionReader(URI(baseUri), location.getPath());
            // TODO: better trust the device uuid in the SSDP message then in the device description ...
            //       though platinum-upnp doesn't provide a USN field ...
            DeviceRoot* deviceRoot = descriptionReader.deviceRoot();
            deviceRoot->setBaseUri(URI(baseUri));
            addDevice(deviceRoot);
        }
        break;
    }
    std::cerr << "Controller::handleSsdpMessage() finished" << std::endl;
}


void
Controller::addDevice(DeviceRoot* pDeviceRoot)
{
    // TODO: handle "alive refreshments"
//     std::cerr << "Controller::addDevice()" << std::endl;
    std::string uuid = pDeviceRoot->getRootDevice()->getUuid();
    if (!m_devices.contains(uuid)) {
        m_devices.append(uuid, pDeviceRoot);
        deviceAdded(pDeviceRoot);
    }
}


void
Controller::removeDevice(const std::string& uuid)
{
//     std::cerr << "Controller::removeDevice()" << std::endl;
    if (m_devices.contains(uuid)) {
        deviceRemoved(&m_devices.get(uuid));
        m_devices.remove(uuid);
    }
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
        m_requestMethod = REQUEST_NOTIFY;
        setNotificationSubtype(SsdpMessage::SUBTYPE_ALIVE);          // alive message
        break;
    case REQUEST_NOTIFY_BYEBYE:
        m_requestMethod = REQUEST_NOTIFY;
        setNotificationSubtype(SsdpMessage::SUBTYPE_BYEBYE);         // byebye message
        break;
    }
}


SsdpMessage::SsdpMessage(const std::string& buf, const SocketAddress& sender)
{
    // FIXME: this shouldn't be executed on every SsdpMessage ctor
    initMessageMap();
    
    for (std::map<TRequestMethod,std::string>::iterator i = m_messageMap.begin(); i != m_messageMap.end(); i++) {
        m_messageConstMap[(*i).second] = (*i).first;
    }
    
    // FIXME: what about Poco::Message header for retrieving the request method?
    istringstream is(buf);
    // get first line and check request method type
    std::string line;
    getline(is, line);
    Poco::trimInPlace(line);
    
    // TODO: must be case-insensitive map?!
    m_requestMethod = m_messageConstMap[line];
    m_sender = sender;
    
    while(getline(is, line)) {
        std::string::size_type col = line.find(":");
        if (col != std::string::npos) {
            m_messageHeader[line.substr(0, col)] = Poco::trim(line.substr(col + 1));
        }
    }
    
//     if (getNotificationSubtype() == SsdpMessage::SUBTYPE_ALIVE) {
//             m_requestMethod = REQUEST_NOTIFY_ALIVE;
//     }
//     else if (getNotificationSubtype() == SsdpMessage::SUBTYPE_BYEBYE) {
//         m_requestMethod = REQUEST_NOTIFY_BYEBYE;
//     }
//     try {
//         m_messageHeader.read(is);
//     } catch (Poco::Net::MessageException) {
//         std::cerr << "Error in sdpMessage::SsdpMessage(): malformed header" << std::endl;
//     }
    // FIXME: workaround for bug in Poco: empty values are mistreated
//     if (m_messageHeader.has("EXT")) {
//         m_messageHeader.set("EXT", "fix bug in poco");
//         m_messageHeader.erase("EXT");
//     }
}


void
SsdpMessage::initMessageMap()
{
    m_messageMap[REQUEST_NOTIFY]            = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_NOTIFY_ALIVE]      = "dummy1";
    m_messageMap[REQUEST_NOTIFY_BYEBYE]     = "dummy2";
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
    for (std::map<std::string,std::string>::iterator i = m_messageHeader.begin(); i != m_messageHeader.end(); ++i) {
        os << (*i).first << ": " << (*i).second << "\r\n";
    }
//     m_messageHeader.write(os);
    os << "\r\n";
    
    return os.str();
}


void
SsdpMessage::setRequestMethod(TRequestMethod requestMethod)
{
    m_requestMethod = requestMethod;
//     if (m_requestMethod == REQUEST_NOTIFY_ALIVE || m_requestMethod == REQUEST_NOTIFY_BYEBYE) {
//         requestMethod = REQUEST_NOTIFY;
//     }
}


SsdpMessage::TRequestMethod
SsdpMessage::getRequestMethod()
{
    return m_requestMethod;
}


void
SsdpMessage::setCacheControl(int duration)
{
//     m_messageHeader.set("CACHE-CONTROL", "max-age = " + NumberFormatter::format(duration));
    m_messageHeader["CACHE-CONTROL"] = "max-age = " + NumberFormatter::format(duration);
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
//     m_messageHeader.set("NT", searchTarget);
    m_messageHeader["NT"] =  searchTarget;
}


std::string
SsdpMessage::getNotificationType()
{
    return m_messageHeader["NT"];
}


void
SsdpMessage::setNotificationSubtype(TRequestMethod notificationSubtype)
{
//     m_messageHeader.set("NTS", m_messageMap[notificationSubtype]);
    m_messageHeader["NTS"] =  m_messageMap[notificationSubtype];
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
//     m_messageHeader.set("ST", searchTarget);
    m_messageHeader["ST"] = searchTarget;
}


std::string
SsdpMessage::getSearchTarget()
{
    return m_messageHeader["ST"];
}


void
SsdpMessage::setUniqueServiceName(const std::string& serviceName)
{
//     m_messageHeader.set("USN", serviceName);
    m_messageHeader["USN"] = serviceName;
}


std::string
SsdpMessage::getUniqueServiceName()
{
    return m_messageHeader["USN"];
}


void
SsdpMessage::setLocation(const URI& location)
{
//     m_messageHeader.set("LOCATION", location.toString());
    m_messageHeader["LOCATION"] = location.toString();
}


URI
SsdpMessage::getLocation()
{
    try {
        return URI(m_messageHeader["LOCATION"]);
    }
    catch (Poco::NotFoundException) {
        std::cerr << "Error in SsdpMessage::getLocation(): LOCATION field not found" << std::endl;
        return URI("");
    }
}


void
SsdpMessage::setHost()
{
//     m_messageHeader.set("HOST", SSDP_FULL_ADDRESS);
    m_messageHeader["HOST"] = SSDP_FULL_ADDRESS;
}


void
SsdpMessage::setHttpExtensionNamespace()
{
//     m_messageHeader.set("MAN", "\"ssdp:discover\"");
    m_messageHeader["MAN"] = "\"ssdp:discover\"";
}


void
SsdpMessage::setHttpExtensionConfirmed()
{
//     m_messageHeader.set("EXT", "");
    m_messageHeader["EXT"] = "";
}


bool
SsdpMessage::getHttpExtensionConfirmed()
{
//     return m_messageHeader.has("EXT");
    return true;
}


void
SsdpMessage::setServer(const std::string& productNameVersion)
{   
//     m_messageHeader.set("SERVER", 
//                         Environment::osName() + "/"
//                         + Environment::osVersion() + ", "
//                         + "UPnP/" + UPNP_VERSION + ", "
//                         + productNameVersion);
    m_messageHeader["SERVER"] = 
                        Environment::osName() + "/"
                        + Environment::osVersion() + ", "
                        + "UPnP/" + UPNP_VERSION + ", "
                        + productNameVersion;
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
//     m_messageHeader.set("MX", NumberFormatter::format(waitTime));
    m_messageHeader["MX"] = NumberFormatter::format(waitTime);
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
//     m_messageHeader.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
    m_messageHeader["DATE"] = DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT);
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


SsdpSocket::SsdpSocket(const NetworkInterface& interface):
m_interface(interface),
m_pBuffer(new char[BUFFER_SIZE])
{
}


void
SsdpSocket::setObserver(const AbstractObserver& observer)
{
    m_notificationCenter.addObserver(observer);
}


// void
// SsdpSocket::setUnicastObserver(const AbstractObserver& observer)
// {
//     m_unicastNotificationCenter.addObserver(observer);
// }


void
SsdpSocket::init()
{
    
    // set the default interface by providing an empty NetworkInterface as argument
    // TODO: let interface be configurable
    // TODO: default value: find out an interface name, that routes to the default route entry
    // TODO: dynamic adding and removing of interfaces
//     m_interface = NetworkInterface::forName("wlan0");
    
    // listen to UDP unicast and send out to multicast
    m_pSsdpSenderSocket = new MulticastSocket(SocketAddress("0.0.0.0", 0));
    m_pSsdpSenderSocket->setInterface(m_interface);
    m_pSsdpSenderSocket->setLoopback(true);
    m_pSsdpSenderSocket->setTimeToLive(4);  // TODO: let TTL be configurable
    m_unicastReactor.addEventHandler(*m_pSsdpSenderSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onUnicastReadable));
    m_unicastListenerThread.start(m_unicastReactor);
    
    // listen to UDP multicast
    m_pSsdpSocket = new MulticastSocket(SocketAddress(IPAddress(SSDP_ADDRESS), SSDP_PORT), true);
    m_pSsdpSocket->setInterface(m_interface);
    m_pSsdpSocket->setLoopback(true);
    m_pSsdpSocket->joinGroup(IPAddress(SSDP_ADDRESS));
    m_reactor.addEventHandler(*m_pSsdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
    m_listenerThread.start(m_reactor);
}


SsdpSocket::~SsdpSocket()
{
    std::cerr << std::endl << "closing SSDP socket ..." << std::endl;
    m_reactor.removeEventHandler(*m_pSsdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
    m_unicastReactor.removeEventHandler(*m_pSsdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
//     m_ssdpSocket.leaveGroup(m_ssdpAddress);
    m_pSsdpSocket->leaveGroup(IPAddress(SSDP_ADDRESS));
    m_reactor.stop();
    m_unicastReactor.stop();
    m_listenerThread.join();
    m_unicastListenerThread.join();
    delete m_pSsdpSenderSocket;
    delete m_pSsdpSocket;
    delete [] m_pBuffer;
}


void
SsdpSocket::onReadable(const AutoPtr<ReadableNotification>& pNf)
{
    SocketAddress sender;
    int n = m_pSsdpSocket->receiveFrom(m_pBuffer, BUFFER_SIZE, sender);
    if (n > 0) {
//         std::cerr << "SSDP message received from: " << sender.toString() << std::endl << std::string(m_pBuffer, n) << std::endl;
        m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n), sender));
    }
}


void
SsdpSocket::onUnicastReadable(const AutoPtr<ReadableNotification>& pNf)
{
    SocketAddress sender;
    int n = m_pSsdpSenderSocket->receiveFrom(m_pBuffer, BUFFER_SIZE, sender);
    if (n > 0) {
//         std::cerr << "SSDP Unicast message received from: " << sender.toString() << std::endl << std::string(m_pBuffer, n) << std::endl;
        m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n), sender));
    }
}


void
SsdpSocket::sendMessage(SsdpMessage& message, const SocketAddress& receiver)
{
    std::string m = message.toString();
//     int bytesSent = m_ssdpSocket.sendTo(m.c_str(), m.length(), receiver);
    int bytesSent = m_pSsdpSenderSocket->sendTo(m.c_str(), m.length(), receiver);
//     std::cerr << "SSDP message sent to: " << receiver.toString() << std::endl << m;
}
