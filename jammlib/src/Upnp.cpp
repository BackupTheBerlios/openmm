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

#include "Upnp.h"
#include "UpnpPrivate.h"


using namespace Jamm;


HttpFileServer::HttpFileServer() :
m_socket(Poco::Net::ServerSocket(0))
{
    
}


HttpFileServer::~HttpFileServer()
{
    std::clog << "stopping http file server" << std::endl;
    m_pHttpServer->stop();
    delete m_pHttpServer;
}


void
HttpFileServer::start()
{
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    m_pHttpServer = new Poco::Net::HTTPServer(new FileRequestHandlerFactory(this), m_socket, pParams);
    m_pHttpServer->start();
    std::clog << "http file server started" << std::endl;
}


void
HttpFileServer::stop()
{
    m_pHttpServer->stop();
}


Poco::UInt16
HttpFileServer::getPort() const
{
    return m_socket.address().port();
}


void
HttpFileServer::registerFile(const std::string& uri, const std::string& path)
{
    m_uriPathMap["/" + uri] = path;
}


FileRequestHandlerFactory::FileRequestHandlerFactory(HttpFileServer* pFileServer) :
m_pFileServer(pFileServer)
{
}


Poco::Net::HTTPRequestHandler*
FileRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new FileRequestHandler(m_pFileServer);
}


FileRequestHandler::FileRequestHandler(HttpFileServer* pFileServer) :
m_pFileServer(pFileServer)
{
}


void
FileRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::clog << "handle description request: " << request.getURI() << std::endl;
    
    std::string fileName = m_pFileServer->m_uriPathMap[request.getURI()];
    std::clog << "open file: " << fileName << std::endl;
//     Poco::File inFile(fileName);
    
    std::ifstream ifstr(fileName.c_str());
    
    response.setChunkedTransferEncoding(true);
//     response.setContentLength(inFile.getSize());
    response.setContentLength(Poco::Net::HTTPMessage::UNKNOWN_CONTENT_LENGTH);
//     response.setContentType(Poco::Net::MediaType::NO_CONTENT_TYPE);
    response.setContentType("multipart/mixed");
//     response.setContentType("text/plain");
    std::ostream& ostr = response.send();
    Poco::StreamCopier::copyStream(ifstr, ostr);
    std::clog << "handle description request finished" << std::endl;
}


NetworkInterfaceNotification::NetworkInterfaceNotification(const std::string& interfaceName, bool added) :
m_interfaceName(interfaceName),
m_added(added)
{
}


NetworkInterfaceManager* NetworkInterfaceManager::m_pInstance = 0;

// TODO: may add an option to ignore loopback device

NetworkInterfaceManager::NetworkInterfaceManager()
{
    std::clog << "NetworkInterfaceManager::NetworkInterfaceManager()" << std::endl;
    
    m_loopbackProvided = false;
    std::vector<Poco::Net::NetworkInterface> ifList = Poco::Net::NetworkInterface::list();
    for (std::vector<Poco::Net::NetworkInterface>::iterator it = ifList.begin(); it != ifList.end(); ++it) {
        std::clog << "found network interface: " << (*it).name() << std::endl;
        m_interfaceList.push_back((*it).name());
    }
    findValidIpAddress();
}


NetworkInterfaceManager*
NetworkInterfaceManager::instance()
{
    if (!m_pInstance) {
        return new NetworkInterfaceManager;
    }
    else {
        return m_pInstance;
    }
}


void
NetworkInterfaceManager::findValidIpAddress()
{
    std::clog << "NetworkInterfaceManager::findValidIpAddress()" << std::endl;
    
    bool validAddressFound = false;
    for (std::vector<std::string>::iterator it = m_interfaceList.begin(); it != m_interfaceList.end(); ++it) {
        Poco::Net::IPAddress address = Poco::Net::NetworkInterface::forName(*it).address();
        if (address.isLoopback()) {
            m_loopbackProvided = true;
            m_loopbackAddress = address;
        }
        if (address.isUnicast() && !address.isLoopback()/* && !address.isLinkLocal()*/) {
            validAddressFound = true;
            m_validIpAddress = address;
            std::clog << "NetworkInterfaceManager::NetworkInterfaceManager() setting m_validIpAddress to: " << m_validIpAddress.toString() << std::endl;
        }
    }
    if (m_interfaceList.size() == 1 && m_loopbackProvided) {
        validAddressFound = true;
        m_validIpAddress = m_loopbackAddress;
        std::clog << "NetworkInterfaceManager::NetworkInterfaceManager() setting m_validIpAddress to: " << m_validIpAddress.toString() << std::endl;
    }
    if (!validAddressFound) {
        std::cerr << "Error in NetworkInterfaceManager: no valid IP address found" << std::endl;
    }
}


void
NetworkInterfaceManager::registerInterfaceChangeHandler(const Poco::AbstractObserver& observer)
{
    std::clog << "NetworkInterfaceManager::registerInterfaceChangeHandler()" << std::endl;
    
    m_notificationCenter.addObserver(observer);
    for (std::vector<std::string>::iterator it = m_interfaceList.begin(); it != m_interfaceList.end(); ++it) {
        std::clog << "notify observer of new interface: " << (*it) << std::endl;
        observer.notify(new NetworkInterfaceNotification((*it), true));
    }
}


void
NetworkInterfaceManager::addInterface(const std::string& name)
{
//     std::clog << "NetworkInterfaceManager::addInterface() name: " << name << std::endl;
    
    if (find(m_interfaceList.begin(), m_interfaceList.end(), name) == m_interfaceList.end()) {
        std::clog << "NetworkInterfaceManager::addInterface() name: " << name << std::endl;
        m_interfaceList.push_back(name);
        m_notificationCenter.postNotification(new NetworkInterfaceNotification(name, true));
    }
    else {
        std::clog << "NetworkInterfaceManager::addInterface() added interface already known: " << name << std::endl;
    }
}


void
NetworkInterfaceManager::removeInterface(const std::string& name)
{
    std::clog << "NetworkInterfaceManager::removeInterface() name: " << name << std::endl;
    
    m_interfaceList.erase(find(m_interfaceList.begin(), m_interfaceList.end(), name));
    m_notificationCenter.postNotification(new NetworkInterfaceNotification(name, false));
    findValidIpAddress();
}


const Poco::Net::IPAddress&
NetworkInterfaceManager::getValidInterfaceAddress()
{
    // TODO: probably need some locking here
    return m_validIpAddress;
}


UriDescriptionReader::UriDescriptionReader(Poco::URI uri, const std::string& deviceDescriptionPath) :
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
        
//     std::string p = m_uri.getPath() + path;
    std::clog << "UriDescriptionReader::getDescription() from: " << m_uri.toString() + path << std::endl;
//     std::clog << "uri path is: " << m_uri.getPath() << std::endl;
//     std::clog << "path is: " << path << std::endl;
    std::string* res;
    
    if (m_uri.getScheme() == "file") {
        std::stringstream ss;
        
        std::ifstream ifs(path.c_str());
        Poco::StreamCopier::copyStream(ifs, ss);
        res = new std::string(ss.str());
    }
    else if (m_uri.getScheme() == "http") {
        Poco::Net::HTTPClientSession session(m_uri.getHost(), m_uri.getPort());
        Poco::Net::HTTPRequest request("GET", m_uri.getPath() + path);
        session.sendRequest(request);
        
        // TODO: platinum's URL has no leading / but expects one ...
        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);
        if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
            std::clog << "Error: " << path << " HTTP_NOT_FOUND 404" << std::endl;
            std::clog << "Trying: /" << path << std::endl;
            request.setURI("/" + path);
            session.sendRequest(request);
            response.clear();
            std::istream& rs = session.receiveResponse(response);
        }
//         char* buf = new char[response.getContentLength()];
//         rs.read(buf, response.getContentLength());
//         res = new std::string(buf, response.getContentLength());
        
        res = new std::string;
        Poco::StreamCopier::copyToString(rs, *res);
        
        std::clog << "downloaded description:" << std::endl << "*BEGIN*" << *res << "*END*" << std::endl;
    }
    else {
        std::clog << "Error in UriDescriptionReader: unknown scheme in description uri" << std::endl;
    }
    
    // TODO: put this into deviceRoot() as it is common with StringDescriptionReader
    Poco::XML::DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Poco::XML::Node* n = m_pDocStack.top()->documentElement()->firstChild();
//     std::clog << "first node: " << n->nodeName() << ", " << n << std::endl;
    m_nodeStack.push(n);
    std::clog << "UriDescriptionReader::getDescription() finished" << std::endl;
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
//     std::clog << "StringDescriptionReader::getDescription()" << std::endl;
    std::string* res = (*m_pStringMap)[path];
    Poco::XML::DOMParser parser;
    m_pDocStack.push(parser.parseString(*res));
    Poco::XML::Node* n = m_pDocStack.top()->documentElement()->firstChild();
//     std::clog << "first node: " << n->nodeName() << ", " << n << std::endl;
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
/*    std::clog << "DescriptionReader::releaseDescriptionDocument()" << std::endl;
    if (!m_pDocStack.empty()) {
        m_pDocStack.top()->release();
        m_pDocStack.pop();
    }*/
}


DeviceRoot*
DescriptionReader::deviceRoot()
{
//     std::clog << "DescriptionReader::deviceRoot()" << std::endl;
    DeviceRoot* pRes = new DeviceRoot();
    m_pDeviceRoot = pRes;
    pRes->setDeviceDescription(getDescription(m_deviceDescriptionPath));
    // NOTE: a running HttpSocket is needed here, to set host and port of BaseUri and DescriptionUri
    //       that's why jammgen crashes without setting up a socket in HttpSocket::init()
    pRes->setBaseUri();
    pRes->setDescriptionUri(m_deviceDescriptionPath);
    Poco::XML::Node* pNode = m_nodeStack.top();
//     std::clog << "top of stack: " << pNode << std::endl;
    
    while (pNode)
    {
//         std::clog << "node: " << pNode->nodeName() << std::endl;
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
//     std::clog << "DescriptionReader::device()" << std::endl;
    Device* pRes = new Device();
    Poco::XML::Node* pNode = m_nodeStack.top();
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
                Poco::XML::Node* pChild = pNode->firstChild();
                while (pChild) {
                    if (pChild->nodeName() == "service") {
                        if (pChild->hasChildNodes()) {
                            m_nodeStack.push(pChild->firstChild());
                            pRes->addService(service());
                        }
                        else {
                            std::clog << "Error in DescriptionReader: empty service" << std::endl;
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                std::clog << "Error in DescriptionReader: service list without services" << std::endl;
            }
        }
        else if (pNode->nodeName() == "deviceList") {
            if (pNode->hasChildNodes()) {
                Poco::XML::Node* pChild = pNode->firstChild();
                while (pChild) {
                    if (pChild->nodeName() == "device") {
                        if (pChild->hasChildNodes()) {
                            m_nodeStack.push(pChild->firstChild());
                            m_pDeviceRoot->addDevice(device());
                        }
                        else {
                            std::clog << "Error in DescriptionReader: empty embedded device" << std::endl;
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                std::clog << "Error in DescriptionReader: device list without embedded devices" << std::endl;
            }
        }
        else if (pNode->nodeName() == "iconList") {
        }
        else {
            pRes->addProperty(pNode->nodeName(), pNode->innerText());
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Service*
DescriptionReader::service()
{
//     std::clog << "DescriptionReader::service()" << std::endl;
    Service* pRes = new Service();
    Poco::XML::Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
//         std::clog << "node: " << pNode->nodeName() << std::endl;
        if (pNode->nodeName() == "serviceType" && pNode->hasChildNodes()) {
            pRes->setServiceType(pNode->innerText());
        }
        if (pNode->nodeName() == "serviceId" && pNode->hasChildNodes()) {
            pRes->setServiceId(pNode->innerText());
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            // FIXME: this is somewhat ugly
            std::string descriptionPath = pNode->innerText();
//             fixQuirkyPath(descriptionPath);
            pRes->setDescriptionPath(descriptionPath);
            // load the service description into the Service object.
            pRes->setDescription(getDescription(pRes->getDescriptionPath()));
            Poco::XML::Node* pScpd = m_nodeStack.top();
            while (pScpd) {
//                 std::clog << "node: " << pScpd->nodeName() << std::endl;
                if (pScpd->nodeName() == "actionList" && pScpd->hasChildNodes()) {
                    Poco::XML::Node* pChild = pScpd->firstChild();
                    while (pChild) {
//                         std::clog << "node: " << pChild->nodeName() << std::endl;
                        if (pChild->nodeName() == "action") {
                            m_nodeStack.push(pChild->firstChild());
                            pRes->addAction(action());
                        }
                        pChild = pChild->nextSibling();
                    }
                }
                else if (pScpd->nodeName() == "serviceStateTable" && pScpd->hasChildNodes()) {
                    Poco::XML::Node* pChild = pScpd->firstChild();
                    while (pChild) {
//                         std::clog << "node: " << pChild->nodeName() << std::endl;
                        if (pChild->nodeName() == "stateVariable") {
                            m_nodeStack.push(pChild->firstChild());
                            StateVar* pStateVar = stateVar();
                            if (pChild->hasAttributes()) {
                                Poco::XML::NamedNodeMap* attr = pChild->attributes();
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
            std::string controlPath = pNode->innerText();
//             fixQuirkyPath(controlPath);
//             fixQuirkyPathRemoveFileName(controlPath);
            pRes->setControlPath(controlPath);
        }
        else if (pNode->nodeName() == "eventSubURL" && pNode->hasChildNodes()) {
            std::string eventPath = pNode->innerText();
//             fixQuirkyPath(eventPath);
//             fixQuirkyPathRemoveFileName(eventPath);
            pRes->setEventPath(eventPath);
        }
        
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


Action*
DescriptionReader::action()
{
//     std::clog << "DescriptionReader::action()" << std::endl;
    Action* pRes = new Action();
    Poco::XML::Node* pNode = m_nodeStack.top();
    
    while (pNode)
    {
        if (pNode->nodeName() == "name" && pNode->hasChildNodes()) {
            pRes->setName(pNode->innerText());
        }
        else if (pNode->nodeName() == "argumentList" && pNode->hasChildNodes()) {
            // TODO: this if branch should be written as only one line of code ...
            Poco::XML::Node* pChild = pNode->firstChild();
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
//     std::clog << "DescriptionReader::argument()" << std::endl;
    Argument* pRes = new Argument();
    Poco::XML::Node* pNode = m_nodeStack.top();
    
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
//     std::clog << "DescriptionReader::stateVar()" << std::endl;
    StateVar* pRes = new StateVar();
    Poco::XML::Node* pNode = m_nodeStack.top();
    
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
//             std::clog << "DescriptionReader::stateVar() set defaultValue: " << val << std::endl;
            pRes->setValue(val);
        }
        pNode = pNode->nextSibling();
    }
    m_nodeStack.pop();
    return pRes;
}


void
DescriptionReader::fixQuirkyPath(std::string& path)
{
    int len = path.size();
    if (path.substr(0, 1) != "/") {
        path = "/" + path;
        len++;
        std::clog << "DescriptionReader::fixQuirkyPath() adding leading /: " << path << std::endl;
    }

}


void
DescriptionReader::fixQuirkyPathRemoveFileName(std::string& path)
{
    int len = path.size();
    if (path.substr(len - 4) == ".xml") {
        path = path.substr(0, path.find_last_of('/') + 1);
        std::clog << "DescriptionReader::fixQuirkyPath() removing trailing filename: " << path << std::endl;
    }
}

ActionRequestReader::ActionRequestReader(const std::string& requestBody, Action* pActionTemplate) : m_pActionTemplate(pActionTemplate)
{
    std::clog << "ActionRequestReader::ActionRequestReader()" << std::endl;
    std::clog << "request: " << requestBody << std::endl;
    
    Poco::XML::DOMParser parser;
    m_pDoc = parser.parseString(requestBody);
    Poco::XML::NodeIterator it(m_pDoc, Poco::XML::NodeFilter::SHOW_ALL);
    m_nodeStack.push(it.nextNode());
}


Action*
ActionRequestReader::action()
{
//     std::clog << "ActionRequestReader::action()" << std::endl;
    Action* pRes = m_pActionTemplate;
    Poco::XML::Node* pNode = m_nodeStack.top();
    Poco::XML::NodeIterator it(pNode, Poco::XML::NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Poco::XML::Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Poco::XML::Node* pAction = pBody->firstChild();
//         std::clog << "action: " << pRes->getName() << std::endl;
//         std::clog << "serviceType: " << pRes->getService()->getServiceType() << std::endl;
        
        if (pAction->hasChildNodes()) {
            Poco::XML::Node* pArgument = pAction->firstChild();
    
            while (pArgument) {
//                 std::clog << "ActionRequestReader::action() setting Argument: " << pArgument->nodeName() << " val: " << pArgument->innerText() << std::endl;
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            std::clog << "ActionRequestReader(): action without arguments" << std::endl;
        }
    }
    else {
        std::clog << "Error in ActionRequestReader(): action without body" << std::endl;
    }
    m_nodeStack.pop();
//     std::clog << "ActionRequestReader::action() finished" << std::endl;
    return pRes;
}


ActionResponseReader::ActionResponseReader(const std::string& responseBody, Action* pActionTemplate) :
m_pActionTemplate(pActionTemplate)
{
    std::clog << "ActionResponseReader::ActionResponseReader()" << std::endl;
//     std::clog << "response: " << responseBody << std::endl;
    
    Poco::XML::DOMParser parser;
    // TODO: set encoding with parser.setEncoding();
    // there's coming a lot of rubbish thru the wire, decorated with white-spaces all over the place ...
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
    try {
        m_pDoc = parser.parseString(responseBody);
    }
    catch (Poco::XML::SAXParseException) {
        std::cerr << "Error in ActionResponseReader: could not parse action response." << std::endl;
        return;
    }
    catch (Poco::XML::DOMException) {
        std::cerr << "Error in ActionResponseReader: could not parse action response." << std::endl;
        return;
    }
    // TODO: new reader design: don't go further if parser has failed
    Poco::XML::NodeIterator it(m_pDoc, Poco::XML::NodeFilter::SHOW_ALL);
    m_nodeStack.push(it.nextNode());
}


Action*
ActionResponseReader::action()
{
    // TODO: same code as in ActionRequestReader
    std::clog << "ActionResponseReader::action()" << std::endl;
    Action* pRes = m_pActionTemplate;
    Poco::XML::Node* pNode = m_nodeStack.top();
    Poco::XML::NodeIterator it(pNode, Poco::XML::NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Poco::XML::Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Poco::XML::Node* pAction = pBody->firstChild();
//         std::clog << "action: " << pRes->getName() << std::endl;
//         std::clog << "serviceType: " << pRes->getService()->getServiceType() << std::endl;
        
        if (pAction->hasChildNodes()) {
            Poco::XML::Node* pArgument = pAction->firstChild();
            
            while (pArgument) {
                std::clog << "ActionRequestReader::action() reading Argument: " << pArgument->nodeName() << ", val: " << pArgument->innerText() << std::endl;
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            std::clog << "ActionResponseReader(): action without arguments" << std::endl;
        }
    }
    else {
        std::clog << "Error in ActionResponseReader(): action without body" << std::endl;
    }
    m_nodeStack.pop();
    std::clog << "ActionResponseReader::action() finished" << std::endl;
    return pRes;
}



DeviceDescriptionWriter::DeviceDescriptionWriter()
{
    std::clog << "DeviceDescriptionWriter::DeviceDescriptionWriter()" << std::endl;
    
    m_pDoc = new Poco::XML::Document;
}


void
DeviceDescriptionWriter::deviceRoot(DeviceRoot& deviceRoot)
{
    std::clog << "DeviceDescriptionWriter::deviceRoot()" << std::endl;
    
    Poco::AutoPtr<Poco::XML::Element> pRoot = m_pDoc->createElement("root");
    pRoot->setAttribute("xmlns", "urn:schemas-upnp-org:device-1-0");
    Poco::AutoPtr<Poco::XML::Element> pSpecVersion = m_pDoc->createElement("specVersion");
    Poco::AutoPtr<Poco::XML::Element> pMajor = m_pDoc->createElement("major");
    Poco::AutoPtr<Poco::XML::Element> pMinor = m_pDoc->createElement("minor");
    Poco::AutoPtr<Poco::XML::Text> pMajorVersion = m_pDoc->createTextNode("1");
    Poco::AutoPtr<Poco::XML::Text> pMinorVersion = m_pDoc->createTextNode("0");
    pMajor->appendChild(pMajorVersion);
    pMinor->appendChild(pMinorVersion);
    pSpecVersion->appendChild(pMajor);
    pSpecVersion->appendChild(pMinor);
    pRoot->appendChild(pSpecVersion);
    // write root device
    Poco::AutoPtr<Poco::XML::Element> pRootDevice = device(*deviceRoot.getRootDevice());
    pRoot->appendChild(pRootDevice);
    std::clog << "DeviceDescriptionWriter::deviceRoot() pRootDevice inserted" << std::endl;
    m_pDoc->appendChild(pRoot);
    std::clog << "DeviceDescriptionWriter::deviceRoot() pRoot inserted" << std::endl;
// end root device
    
    // TODO: if there are embedded devices open a deviceList
    // write embedded devices
    // end embedded devices
    
    // end DeviceRoot
    
    std::clog << "DeviceDescriptionWriter::deviceRoot() finished" << std::endl;
}


Poco::XML::Element*
DeviceDescriptionWriter::device(Device& device)
{
    std::clog << "DeviceDescriptionWriter::device()" << std::endl;
    
    Poco::XML::Element* pDevice = m_pDoc->createElement("device");
    
    // device type
    Poco::AutoPtr<Poco::XML::Element> pDeviceType = m_pDoc->createElement("deviceType");
    Poco::AutoPtr<Poco::XML::Text> pDeviceTypeVal = m_pDoc->createTextNode(device.getDeviceType());
    pDeviceType->appendChild(pDeviceTypeVal);
    pDevice->appendChild(pDeviceType);
    // UUID
    Poco::AutoPtr<Poco::XML::Element> pUuid = m_pDoc->createElement("UDN");
    Poco::AutoPtr<Poco::XML::Text> pUuidVal = m_pDoc->createTextNode("uuid:" + device.getUuid());
    pUuid->appendChild(pUuidVal);
    pDevice->appendChild(pUuid);
    
    // Properties
    for (Container<std::string>::KeyIterator it = device.m_properties.beginKey(); it != device.m_properties.endKey(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pProp = m_pDoc->createElement((*it).first);
        Poco::AutoPtr<Poco::XML::Text> pVal = m_pDoc->createTextNode(*((*it).second));
        pProp->appendChild(pVal);
        pDevice->appendChild(pProp);
    }
    
    Poco::AutoPtr<Poco::XML::Element> pServiceList = m_pDoc->createElement("serviceList");
    pDevice->appendChild(pServiceList);
    // write Services
    for (Container<Service>::Iterator it = device.m_services.begin(); it != device.m_services.end(); ++it) {
        pServiceList->appendChild(service(*it));
        std::clog << "added service: " << (*it)->getServiceType() << std::endl;
    }
    
    std::clog << "DeviceDescriptionWriter::device()finished" << std::endl;
    
    return pDevice;
}


Poco::XML::Element*
DeviceDescriptionWriter::service(Service* pService)
{
    std::clog << "DeviceDescriptionWriter::service()" << std::endl;
    
    Poco::XML::Element* pServiceElement = m_pDoc->createElement("service");
    
    // serviceType
    Poco::AutoPtr<Poco::XML::Element> pServiceType = m_pDoc->createElement("serviceType");
    Poco::AutoPtr<Poco::XML::Text> pServiceTypeVal = m_pDoc->createTextNode(pService->getServiceType());
    pServiceType->appendChild(pServiceTypeVal);
    pServiceElement->appendChild(pServiceType);
    // serviceId
    Poco::AutoPtr<Poco::XML::Element> pServiceId = m_pDoc->createElement("serviceId");
    Poco::AutoPtr<Poco::XML::Text> pServiceIdVal = m_pDoc->createTextNode(pService->getServiceId());
    pServiceId->appendChild(pServiceIdVal);
    pServiceElement->appendChild(pServiceId);
    // SCPDURL
    Poco::AutoPtr<Poco::XML::Element> pDescription = m_pDoc->createElement("SCPDURL");
    Poco::AutoPtr<Poco::XML::Text> pDescriptionVal = m_pDoc->createTextNode(pService->getDescriptionPath());
    pDescription->appendChild(pDescriptionVal);
    pServiceElement->appendChild(pDescription);
    // controlURL
    Poco::AutoPtr<Poco::XML::Element> pControl = m_pDoc->createElement("controlURL");
    Poco::AutoPtr<Poco::XML::Text> pControlVal = m_pDoc->createTextNode(pService->getControlPath());
    pControl->appendChild(pControlVal);
    pServiceElement->appendChild(pControl);
    // eventSubURL
    Poco::AutoPtr<Poco::XML::Element> pEvent = m_pDoc->createElement("eventSubURL");
    Poco::AutoPtr<Poco::XML::Text> pEventVal = m_pDoc->createTextNode(pService->getEventPath());
    pEvent->appendChild(pEventVal);
    pServiceElement->appendChild(pEvent);
    
    std::clog << "DeviceDescriptionWriter::service()finished" << std::endl;
    
    return pServiceElement;
}


std::string*
DeviceDescriptionWriter::write()
{
    std::clog << "DeviceDescriptionWriter::write()" << std::endl;
    
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
//     description = ss.str();
    return new std::string(ss.str());
    std::clog << "description:" << std::endl << ss.str() << std::endl;
}


void
ActionRequestWriter::action(Action* action)
{
    // TODO: nearly same code as in ActionResponseWriter
    m_pDoc = new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pEnvelope = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    Poco::AutoPtr<Poco::XML::Element> pBody = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    Poco::AutoPtr<Poco::XML::Element> pActionRequest = m_pDoc->createElementNS(action->getService()->getServiceType(), action->getName());
    
    for(Action::ArgumentIterator i = action->beginInArgument(); i != action->endInArgument(); ++i) {
        Poco::AutoPtr<Poco::XML::Element> pArgument = m_pDoc->createElement((*i)->getName());
        Poco::AutoPtr<Poco::XML::Text> pArgumentValue = m_pDoc->createTextNode(action->getArgument<std::string>((*i)->getName()));
        pArgument->appendChild(pArgumentValue);
        pActionRequest->appendChild(pArgument);
        
        std::clog << "ActionRequestWriter called with arg: " << (*i)->getName() << ", val: " << action->getArgument<std::string>((*i)->getName()) << std::endl;
    }
    
    pBody->appendChild(pActionRequest);
    pEnvelope->appendChild(pBody);
    m_pDoc->appendChild(pEnvelope);
}


void
ActionRequestWriter::write(std::string& actionMessage)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    actionMessage = ss.str();
    std::clog << "action request message():" << std::endl << ss.str() << std::endl << std::endl;
}


ActionResponseWriter::ActionResponseWriter(std::string& responseBody) :
m_responseBody(&responseBody)
{
    std::clog << "ActionResponseWriter::ActionResponseWriter()" << std::endl;
}


void
ActionResponseWriter::action(Action& action)
{
    std::clog << "ActionResponseWriter::action()" << std::endl;
    
    Poco::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pEnvelope = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    Poco::AutoPtr<Poco::XML::Element> pBody = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    Poco::AutoPtr<Poco::XML::Element> pActionResponse = pDoc->createElementNS(action.getService()->getServiceType(), action.getName() + "Response");
    
    for(Action::ArgumentIterator i = action.beginOutArgument(); i != action.endOutArgument(); ++i) {
        Poco::AutoPtr<Poco::XML::Element> pArgument = pDoc->createElement((*i)->getName());
        Poco::AutoPtr<Poco::XML::Text> pArgumentValue = pDoc->createTextNode(action.getArgument<std::string>((*i)->getName()));
        std::clog << "ActionResponseWriter returns arg: " << (*i)->getName() << ", val: " << action.getArgument<std::string>((*i)->getName()) << std::endl;
        pArgument->appendChild(pArgumentValue);
        pActionResponse->appendChild(pArgument);
    }
    
    pBody->appendChild(pActionResponse);
    pEnvelope->appendChild(pBody);
    pDoc->appendChild(pEnvelope);
    
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, pDoc);
    *m_responseBody = ss.str();
    std::clog << "ResponseBody():" << std::endl << *m_responseBody << std::endl;
}


StateVar*
Argument::getRelatedStateVarReference() const
{
    return m_pAction->getService()->getStateVarReference(m_relatedStateVar);
}


EventMessageWriter::EventMessageWriter()
{
    std::clog << "EventMessageWriter::EventMessageWriter()" << std::endl;
    
    m_pDoc = new Poco::XML::Document;
    m_pPropertySet = m_pDoc->createElementNS("urn:schemas-upnp-org:event-1-0", "propertyset");
    m_pDoc->appendChild(m_pPropertySet);
}


void
EventMessageWriter::write(std::string& eventMessage)
{
    std::clog << "EventMessageWriter::write()" << std::endl;
    
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    eventMessage = ss.str();
    std::clog << "event message():" << std::endl << ss.str() << std::endl;
}


void
EventMessageWriter::stateVar(const StateVar& stateVar)
{
    std::clog << "EventMessageWriter::stateVar()" << std::endl;
    
    Poco::AutoPtr<Poco::XML::Element> pProperty = m_pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "property");
    Poco::AutoPtr<Poco::XML::Element> pStateVar = m_pDoc->createElement(stateVar.getName());
    Poco::AutoPtr<Poco::XML::Text> pStateVarValue = m_pDoc->createTextNode(stateVar.getValue());
    pStateVar->appendChild(pStateVarValue);
    pProperty->appendChild(pStateVar);
    m_pPropertySet->appendChild(pProperty);
}


Subscription::Subscription(std::string callbackUri) :
m_deliveryAddress(callbackUri)
{
    std::clog << "Subscription::Subscription() uri: " << callbackUri << std::endl;
    // TODO: implement timer stuff
    m_uuid = Poco::UUIDGenerator().createRandom();
    std::clog << "SID: " << m_uuid.toString() << std::endl;
    m_eventKey = 0;
    m_pSession = new Poco::Net::HTTPClientSession(m_deliveryAddress.getHost(), m_deliveryAddress.getPort());
}


std::string
Subscription::getEventKey()
{
    // TODO: should lock this
    m_eventKey = (++m_eventKey == 0) ? 1 : m_eventKey;
    return Poco::NumberFormatter::format(m_eventKey);
}


void
Subscription::sendEventMessage(const std::string& eventMessage)
{
    // TODO: queue the eventMessages for sending ...?
    std::clog << "Subscription::sendEventMessage()" << std::endl;
    std::string path = m_deliveryAddress.getPath();
    if (path.substr(0, 1) == "/") {
        path = path.substr(1);
    }
    Poco::Net::HTTPRequest request("NOTIFY", path, "HTTP/1.1");
    request.set("HOST", m_deliveryAddress.getAuthority());
    request.setContentType("text/xml");
    request.set("NT", "upnp:event");
    request.set("NTS", "upnp:propchange");
    request.set("SID", "uuid:" + m_uuid.toString());
    
//     HTTPRequest* request = newRequest();
    request.set("SEQ", getEventKey());
    request.setContentLength(eventMessage.size());
    // set request body and send request
    std::clog << "Subscription::sendEventMessage() send request to: " << getSession()->getHost() << ":" << getSession()->getPort() << "/" << request.getURI() << " ..." << std::endl;
    std::clog << "Header:" << std::endl;
    request.write(std::clog);
    
    std::ostream& ostr = getSession()->sendRequest(request);
    ostr << eventMessage;
    
    // receive answer ...
    Poco::Net::HTTPResponse response;
    getSession()->receiveResponse(response);
    int stat = response.getStatus();
    if (stat == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
        std::clog << "Error: " << request.getURI() << " HTTP_NOT_FOUND 404" << std::endl;
//         std::clog << "Trying: " << m_deliveryAddress.getPath().substr(1) << std::endl;
//         request.setURI(m_deliveryAddress.getPath().substr(1));
//         std::clog << "Subscription::sendEventMessage() send request to: " << getSession()->getHost() << ":" << getSession()->getPort() << request.getURI() << " ..." << std::endl;
//         std::clog << "Header:" << std::endl;
//         request.write(std::clog);
//         
//         std::ostream& ostr = getSession()->sendRequest(request);
//         ostr << eventMessage;
//         
//         response.clear();
//         getSession()->receiveResponse(response);
    }
    else if (stat == Poco::Net::HTTPResponse::HTTP_OK) {
        std::clog << "Subscription::sendEventMessage() recieved HTTP_OK 200" << std::endl;
    }
    std::clog << "Subscription::sendEventMessage() finished" << std::endl;
}


void
Subscription::renew(int seconds)
{
}


void
Subscription::expire(Poco::Timer& timer)
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
//     std::clog << "Service::addStateVar() name: " << pStateVar->getName() << " is evented: " << pStateVar->getSendEvents() << std::endl;
    
    m_stateVars.append(pStateVar->getName(), pStateVar);
    if(pStateVar->getSendEvents()) {
        m_eventedStateVars.append(pStateVar->getName(), pStateVar);
    }
}


void
Service::initClient()
{
    m_pControlRequestSession = new Poco::Net::HTTPClientSession(Poco::Net::SocketAddress(m_pDevice->getDeviceRoot()->getBaseUri().getAuthority()));
    // TODO: subscribe to services
    // -> generate eventHandlerURIs: device uuid + service type
    // TODO: setup event message handler
    // -> this should go in DeviceRoot::initClient()?
    // -> or in Controller::init()?
    // m_pDevice->getDeviceRoot()->registerEventHandler(...);
}

// FIXME: when starting 15 servers and killing them with killall at once
//        jammc still shows a couple of them though they are dead

void
Service::sendAction(Action* pAction)
{
    std::clog << "*********************" << std::endl;
    std::clog << "Service::sendAction()" << std::endl;
    std::string actionMessage;
    ActionRequestWriter requestWriter;
    requestWriter.action(pAction);
    requestWriter.write(actionMessage);
    
    Poco::Net::HTTPRequest* request = new Poco::Net::HTTPRequest("POST", m_controlPath, "HTTP/1.1");
    request->set("HOST", m_pDevice->getDeviceRoot()->getBaseUri().getAuthority());
    request->setContentType("text/xml; charset=\"utf-8\"");
    request->set("SOAPACTION", "\"" + m_serviceType + "#" + pAction->getName() + "\"");
    request->setContentLength(actionMessage.size());
    // set request body and send request
    std::clog << "Service::sendAction() send request to: " << m_pDevice->getDeviceRoot()->getBaseUri().getAuthority() << request->getURI() << " ..." << std::endl;
    std::clog << "Header:" << std::endl;
    request->write(std::clog);
    
    // FIXME: catch Poco::Net::ConnectionRefusedException
    std::ostream& ostr = m_pControlRequestSession->sendRequest(*request);
    ostr << actionMessage;
    std::clog << "Action request sent" << std::endl;
    
    // receive answer ...
    Poco::Net::HTTPResponse response;
    std::istream& rs = m_pControlRequestSession->receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
        std::cerr << "Error: " << m_controlPath << " HTTP_NOT_FOUND 404" << std::endl;
    }
    std::clog << response.getStatus() << " " << response.getReason() << std::endl;
    // FIXME: fix similar places in the code
//     char* buf = 0;
//     int bufsize = response.getContentLength();
//     if (bufsize > 0) {
//         buf = new char[bufsize];
//         rs.read(buf, response.getContentLength());
//         std::string responseBody = std::string(buf, bufsize);
//     }
//     else {
//         std::cerr << "Error in Service: empty action response." << std::endl;
//     }
    std::string responseBody;
    Poco::StreamCopier::copyToString(rs, responseBody);
    std::clog << "Service::sendAction() response received: " /*<< responseBody*/ << std::endl;
    ActionResponseReader responseReader(responseBody, pAction);
    responseReader.action();
    std::clog << "Service::sendAction() finished" << std::endl;
    std::clog << "******************************" << std::endl;
}


void
Service::registerSubscription(Subscription* subscription)
{
    std::clog << "Service::registerSubscription()" << std::endl;
    
    Poco::ScopedLock<Poco::FastMutex> lock(m_serviceLock);
    // TODO: only register a Subscription once from one distinct Controller
    //       note that Subscription has a new SID
    std::string sid = subscription->getUuid();
    std::clog << "SID: " << sid << std::endl;
    m_eventSubscriptions.append(sid, subscription);
}


void
Service::unregisterSubscription(Subscription* subscription)
{
    std::clog << "Service::unregisterSubscription()" << std::endl;
    
    Poco::ScopedLock<Poco::FastMutex> lock(m_serviceLock);
    m_eventSubscriptions.remove(subscription->getUuid());
    delete subscription;
}


void
Service::sendEventMessage(StateVar& stateVar)
{
    std::clog << "Service::sendEventMessage()" << std::endl;
    
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
    std::clog << "Service::sendInitialEventMessage()" << std::endl;
    
    std::string eventMessage;
    EventMessageWriter messageWriter;
    for (StateVarIterator i = beginEventedStateVar(); i != endEventedStateVar(); ++i) {
        messageWriter.stateVar(**i);
    }
    messageWriter.write(eventMessage);
    pSubscription->sendEventMessage(eventMessage);
}


void
Service::setDescriptionRequestHandler()
{
    m_pDescriptionRequestHandler = new DescriptionRequestHandler(m_pDescription);
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


Poco::Net::HTTPRequestHandler*
DeviceRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    std::clog << "dispatch HTTP request: " << request.getURI() << std::endl;
        // TODO: if no request is registered for the query, return a default request
        // handler with error 404 or similar ...
    Poco::Net::HTTPRequestHandler* res;
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
//     std::clog << "register request handler: " << Uri << std::endl;
    m_requestHandlerMap[Uri] = requestHandler;
}


RequestNotFoundRequestHandler*
RequestNotFoundRequestHandler::create()
{
    return new RequestNotFoundRequestHandler();
}


void
RequestNotFoundRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::clog << "handle unknown request with HTTP 404 - not found error on request: " << request.getURI() << std::endl;
    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
}


// DescriptionRequestHandler::DescriptionRequestHandler(std::string& description):
DescriptionRequestHandler::DescriptionRequestHandler(std::string* pDescription):
m_pDescription(pDescription)
{
}


DescriptionRequestHandler*
DescriptionRequestHandler::create()
{
    // TODO: can we somehow avoid to make a copy of the RequestHandler on each request?
//     return new DescriptionRequestHandler(m_description);
    return new DescriptionRequestHandler(m_pDescription);
}


void
DescriptionRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::clog << "handle description request: " << request.getURI() << std::endl;
//         std::clog << "sending contents of length: " << m_description.size() << std::endl;
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
ControlRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::clog << "***********************" << std::endl
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
    std::clog << "SOAP request, service: " << serviceType << ", action: " << actionName << std::endl;
    
    std::clog << "ControlRequestHandler for ServiceType: " << m_pService->getServiceType() << std::endl;
    // TODO: make getAction() robust against wrong action names
    Action* pAction = m_pService->getAction(actionName);
    std::clog << "getAction(): " << pAction->getName() << std::endl;
    pAction = pAction->clone();
    std::clog << "cloned Action(): " << pAction->getName() << std::endl;
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
    std::clog << "finished control request: " << request.getURI() << std::endl
              << "*************************" << std::endl;
}


EventRequestHandler*
EventRequestHandler::create()
{
    return new EventRequestHandler(m_pService);
}


void
EventRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::clog << "handle event request: " << request.getMethod() << std::endl;
    std::clog << "HOST: " << request.getHost() << std::endl;
    std::clog << "CALLBACK: " << request.get("CALLBACK") << std::endl;
    std::clog << "NT: " << request.get("NT") << std::endl;
    std::clog << "TIMEOUT: " << request.get("TIMEOUT") << std::endl;
    
    std::string sid;
    
    if (request.getMethod() == "SUBSCRIBE") {
        Poco::Timestamp t;
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
        response.set("DATE", Poco::DateTimeFormatter::format(t, Poco::DateTimeFormat::HTTP_FORMAT));
        response.set("SERVER", 
                     Poco::Environment::osName() + "/"
                     + Poco::Environment::osVersion() + ", "
                    + "UPnP/" + UPNP_VERSION + ", "
                    + "Jamm/" + JAMM_VERSION);
        response.set("SID", "uuid:" + Poco::UUIDGenerator().create().toString());
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


// HttpSocket::HttpSocket(Poco::Net::NetworkInterface interface) :
HttpSocket::HttpSocket(Poco::Net::IPAddress address) :
// m_interface(interface)
m_address(address)
{
    init();
}


void
HttpSocket::init()
{
    m_pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory(this);
    Poco::Net::ServerSocket socket(0);
        // TODO: bind only to the local subnetwork of the interface's IP-Address, where we sent the SSDP broadcasts out. Or: bind to 0.0.0.0 and broadcast SSDP to all available network interfaces by default.
    //     socket.bind(m_ssdpSocket.m_interface.address());
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
//     m_httpServerAddress = Poco::Net::SocketAddress(m_interface.address(), socket.address().port());
    m_httpServerAddress = Poco::Net::SocketAddress(m_address, socket.address().port());
    m_pHttpServer = new Poco::Net::HTTPServer(m_pDeviceRequestHandlerFactory, socket, pParams);
}


void
HttpSocket::startServer()
{

    //     pParams->setMaxQueued(maxQueued);
    //     pParams->setMaxThreads(maxThreads);
        // set-up a server socket on an available port

    m_pHttpServer->start();
    std::clog << "started HTTP server on: " << m_httpServerAddress.toString() << std::endl;
}


void
HttpSocket::stopServer()
{
    m_pHttpServer->stop();
    std::clog << "stopped HTTP server on: " << m_httpServerAddress.toString() << std::endl;
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
    std::clog << "Device::addService(): " << pService->getServiceType() << std::endl;
    m_services.append(pService->getServiceType(), pService);
    m_pDeviceRoot->addServiceType(pService->getServiceType(), pService);
    pService->setDevice(this);
}


void
Device::setRandomUuid()
{
    Poco::UUIDGenerator uuidGenerator;
    m_uuid = uuidGenerator.createRandom().toString();
}


DeviceRoot::DeviceRoot() :
// TODO: allocate sockets later, not in ctor (e.g. jammgen doesn't need them)
// m_ssdpSocket(/*Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage)*/),
m_ssdpSocket(),
// m_httpSocket(m_ssdpSocket.getInterface())
m_httpSocket(NetworkInterfaceManager::instance()->getValidInterfaceAddress())
{
}


DeviceRoot::~DeviceRoot()
{
    std::clog << "DeviceRoot::~DeviceRoot()" << std::endl;
    stopSsdp();
    stopHttp();
    // TODO: free all Devices, Services, Actions, ...
//     delete m_descriptionRequestHandler;
}


Service*
DeviceRoot::getServiceType(const std::string& serviceType)
{
    std::clog << "DeviceRoot::getServiceType(): " << serviceType << std::endl;
    std::map<std::string,Service*>::iterator i = m_serviceTypes.find(serviceType);
    if (i == m_serviceTypes.end()) {
        std::clog << "could not find serviceType" << std::endl;
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
    std::clog << "DeviceRoot::initStateVars() serviceType: " << serviceType << " , pThis: " << pThis << std::endl;
    m_pDeviceRootImplAdapter->initStateVars(serviceType, pThis);
}


void
DeviceRoot::initController()
{
    std::clog << "DeviceRoot::initController()" << std::endl;
    
    std::clog << "DeviceRoot::initController() finished" << std::endl;
}


void
DeviceRoot::initDevice()
{
    // TODO: break this up into DeviceRoot::initDevice(), Device::initDevice(), Service::initDevice()
    std::clog << "DeviceRoot::initDevice()" << std::endl;
    
    NetworkInterfaceManager::instance()->registerInterfaceChangeHandler
        (Poco::Observer<DeviceRoot,NetworkInterfaceNotification>(*this, &DeviceRoot::handleNetworkInterfaceChangedNotification));
    
    std::clog << "DeviceRoot::initDevice() network interface manager installed" << std::endl;
    
    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        std::clog << "setting random uuid for device" << std::endl;
        (*d)->setRandomUuid();
    }
    
    DeviceDescriptionWriter descriptionWriter;
    descriptionWriter.deviceRoot(*this);
    std::clog << "original device description: " << *m_pDeviceDescription << std::endl;
    m_pDeviceDescription = descriptionWriter.write();
    std::clog << "new device description: " << *m_pDeviceDescription << std::endl;
    
    std::clog << "DeviceRoot::initDevice() device description rewritten" << std::endl;
    
    m_descriptionRequestHandler = new DescriptionRequestHandler(m_pDeviceDescription);
    registerHttpRequestHandler(getDescriptionUri().getPath(), m_descriptionRequestHandler);
    
    SsdpNotifyAliveWriter aliveWriter(m_ssdpNotifyAliveMessages);
    SsdpNotifyByebyeWriter byebyeWriter(m_ssdpNotifyByebyeMessages);
    aliveWriter.deviceRoot(*this);
    byebyeWriter.deviceRoot(*this);
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
    std::clog << "DeviceRoot::initDevice() initialized message sets, service request handlers and state variables" << std::endl;
    
    std::clog << "DeviceRoot::initDevice() finished" << std::endl;
}


void
DeviceRoot::registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler) 
{
    m_httpSocket.m_pDeviceRequestHandlerFactory->registerRequestHandler(path, requestHandler);
}


DeviceRootImplAdapter::DeviceRootImplAdapter()
{
    std::clog << "DeviceRootImplAdapter::DeviceRootImplAdapter()" << std::endl;
    // register the great action dispatcher
};


DeviceRootImplAdapter::~DeviceRootImplAdapter()
{
    std::clog << "DeviceRootImplAdapter::~DeviceRootImplAdapter()" << std::endl;
    delete m_pDeviceRoot;
}


void
DeviceRootImplAdapter::start()
{
    std::clog << "DeviceRootImplAdapter::start()" << std::endl;
    m_pDeviceRoot->registerActionHandler(Poco::Observer<DeviceRootImplAdapter, Action>(*this, &DeviceRootImplAdapter::actionHandler));
    
//     m_pDeviceRoot->print();
    m_pDeviceRoot->initDevice();
//     m_pDeviceRoot->print();
    m_pDeviceRoot->startHttp();
    m_pDeviceRoot->startSsdp();
}


void
DeviceRootImplAdapter::stop()
{
    std::clog << "DeviceRootImplAdapter::stop()" << std::endl;
    m_pDeviceRoot->stopSsdp();
    m_pDeviceRoot->stopHttp();
}


void
DeviceRootImplAdapter::setUuid(std::string uuid, int deviceNumber)
{
    m_pDeviceRoot->m_devices.get(deviceNumber).setUuid(uuid);
}


void
DeviceRootImplAdapter::setRandomUuid(int deviceNumber)
{
    m_pDeviceRoot->m_devices.get(deviceNumber).setRandomUuid();
}


void
DeviceRootImplAdapter::setFriendlyName(const std::string& friendlyName, int deviceNumber)
{
    m_pDeviceRoot->m_devices.get(deviceNumber).setProperty("friendlyName", friendlyName);
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
    std::clog << "SsdpMessageSet::send()" << std::endl;
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
        m_sendTimer.start(Poco::TimerCallback<SsdpMessageSet>(*this, &SsdpMessageSet::onTimer));
    }
    else {
        // start synchronously
        onTimer(m_sendTimer);
    }
    std::clog << "SsdpMessageSet::send() finished" << std::endl;
}


void
SsdpMessageSet::stop()
{
    m_sendTimer.stop();
}


void
SsdpMessageSet::onTimer(Poco::Timer& timer)
{
    std::clog << "SsdpMessageSet::onTimer()" << std::endl;
    int r = m_repeat;
    while (r--) {
        std::clog << "#message sets left to send: " << r+1 << std::endl;
        for (std::vector<SsdpMessage*>::const_iterator i = m_ssdpMessages.begin(); i != m_ssdpMessages.end(); ++i) {
//             std::clog << "sending message: " << (*i)->toString() << std::endl;
            m_socket->sendMessage(**i);
        }
    }
    if (m_continuous) {
        std::clog << "SsdpMessageSet::onTimer() restarting timer" << std::endl;
        timer.restart(m_randomTimeGenerator.next(m_delay));
    }
    std::clog << "SsdpMessageSet::onTimer() finished" << std::endl;
}


void
DeviceRoot::startSsdp()
{
    std::clog << "DeviceRoot::startSsdp()" << std::endl;
    m_ssdpSocket.setObserver(Poco::Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage));
    m_ssdpSocket.start();
    // TODO: 3. send out initial set also on the occasion of new IP address or network interface.
    
    // 1. wait random intervall of less than 100msec when sending message set first time
    // 2. send out all message sets two times (max three times according to specs, should be configurable).
    m_ssdpNotifyAliveMessages.send(m_ssdpSocket, 2, 100, false);
    // 4. resend advertisements in random intervals of max half the expiraton time (CACHE-CONTROL header)
    m_ssdpNotifyAliveMessages.send(m_ssdpSocket, 2, SSDP_CACHE_DURATION * 1000 / 2, true);
    std::clog << "DeviceRoot::startSsdp() finished" << std::endl;
}


void
DeviceRoot::stopSsdp()
{
    std::clog << "DeviceRoot::stopSsdp()" << std::endl;
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
DeviceRoot::sendMessage(SsdpMessage& message, const std::string& interface, const Poco::Net::SocketAddress& receiver)
{
    m_ssdpSocket.sendMessage(message, interface, receiver);
}


void
DeviceRoot::handleSsdpMessage(SsdpMessage* pMessage)
{
//     std::clog << "root device gets SSDP message:" << std::endl;
//     std::clog << pNf->toString();
    
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
        
        m_ssdpSocket.sendMessage(m, pMessage->getInterface(), pMessage->getSender());
    }
}


void
DeviceRoot::handleNetworkInterfaceChangedNotification(NetworkInterfaceNotification* pNotification)
{
    std::clog << "DeviceRoot::handleNetworkInterfaceChangedNotification()" << std::endl;
    
    if (pNotification->m_added) {
        m_ssdpSocket.addInterface(pNotification->m_interfaceName);
        // TODO: send alive message set on this interface
    }
    else {
        m_ssdpSocket.removeInterface(pNotification->m_interfaceName);
        // TODO: send bye-bye message set on this interface
    }
    std::clog << "DeviceRoot::handleNetworkInterfaceChangedNotification() finished" << std::endl;
}



Controller::Controller() :
m_ssdpSocket()
{
    m_ssdpSocket.setObserver(Poco::Observer<Controller, SsdpMessage>(*this, &Controller::handleSsdpMessage));
}


// void
// Controller::init()
// {
// 
// //     m_ssdpSocket.setUnicastObserver(Observer<Controller, SsdpMessage>(*this, &Controller::handleMSearchResponse));
// 
// }

void
Controller::handleNetworkInterfaceChangedNotification(NetworkInterfaceNotification* pNotification)
{
    std::clog << "Controller::handleNetworkInterfaceChangedNotification()" << std::endl;
    
    if (pNotification->m_added) {
        m_ssdpSocket.addInterface(pNotification->m_interfaceName);
        // TODO: send M-SEARCH only on this interface
//         sendMSearch();
    }
    else {
        m_ssdpSocket.removeInterface(pNotification->m_interfaceName);
        // TODO: unregister subscriptions for devices on this interface
    }
    std::clog << "Controller::handleNetworkInterfaceChangedNotification() finished" << std::endl;
}


void
Controller::start()
{
    std::clog << "Controller::start()" << std::endl;
    
    NetworkInterfaceManager::instance()->registerInterfaceChangeHandler
        (Poco::Observer<Controller,NetworkInterfaceNotification>(*this, &Controller::handleNetworkInterfaceChangedNotification));
    
    m_ssdpSocket.start();
    sendMSearch();
    
    std::clog << "Controller::start() finished" << std::endl;
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
Controller::discoverDevice(const Poco::URI& location)
{
    std::clog << "Controller::discoverDevice() LOCATION: " <<  location.toString() << std::endl;
    
    Poco::Path path(location.getPath());
    std::clog << "Controller::discoverDevice() path: " << path.toString() << std::endl;
    std::clog << "Controller::discoverDevice() path depth: " << path.depth() << std::endl;
    
    std::string relPath = "";
    std::string relUri = path.toString();
    if (path.depth() > 0) {
        relPath = path.directory(path.depth() - 1) + "/";
        relUri = path.getFileName();
    }
    std::clog << "Controller::discoverDevice() relPath: " << relPath << std::endl;
    
    Poco::URI baseUri(location.getScheme() + "://" + location.getAuthority() + "/" + relPath);
    std::clog << "Controller::discoverDevice() baseUri: " << baseUri.toString() << std::endl;
    std::clog << "Controller::discoverDevice() relUri: " << relUri << std::endl;
    
    UriDescriptionReader descriptionReader(baseUri, relUri);

    DeviceRoot* deviceRoot = descriptionReader.deviceRoot();
    deviceRoot->setBaseUri(baseUri);
    addDevice(deviceRoot);
    std::clog << "Controller::discoverDevice() finished with LOCATION: " <<  location.toString() << std::endl;
}


void
Controller::handleSsdpMessage(SsdpMessage* pMessage)
{
    // we load all device descriptions, regardless of service types contained in the device
    
    // get UUID from USN
    std::string usn = pMessage->getUniqueServiceName();
    std::string::size_type left = usn.find(":") + 1;
    std::string uuid = usn.substr(left, usn.find("::") - left);
//     std::clog << "Controller::handleSsdpMessage() with UUID: " << uuid << std::endl;
//     std::clog << pMessage->toString();
    
    switch(pMessage->getRequestMethod()) {
    case SsdpMessage::REQUEST_NOTIFY:
//         std::clog << "Controller::handleSsdpMessage() REQUEST_NOTIFY" << std::endl;
        switch(pMessage->getNotificationSubtype()) {
        case SsdpMessage::SUBTYPE_ALIVE:
            std::clog << "Controller::handleSsdpMessage() REQUEST_NOTIFY_ALIVE" << std::endl;
            if (pMessage->getNotificationType() == "upnp:rootdevice" && !m_devices.contains(uuid)) {
                discoverDevice(pMessage->getLocation());
            }
            break;
        case SsdpMessage::SUBTYPE_BYEBYE:
            std::clog << "Controller::handleSsdpMessage() REQUEST_NOTIFY_BYEBYE" << std::endl;
            if (pMessage->getNotificationType() == "upnp:rootdevice") {
                removeDevice(uuid);
            }
            break;
        }
    break;
    case SsdpMessage::REQUEST_RESPONSE:
        std::clog << "Controller::handleSsdpMessage() REQUEST_RESPONSE" << std::endl;
        if (!m_devices.contains(uuid)) {
            discoverDevice(pMessage->getLocation());
        }
        break;
    }
//     std::clog << "Controller::handleSsdpMessage() finished" << std::endl;
}


void
Controller::addDevice(DeviceRoot* pDeviceRoot)
{
    // TODO: handle "alive refreshments"
//     std::clog << "Controller::addDevice()" << std::endl;
    std::string uuid = pDeviceRoot->getRootDevice()->getUuid();
    if (!m_devices.contains(uuid)) {
        m_devices.append(uuid, pDeviceRoot);
        deviceAdded(pDeviceRoot);
    }
}


void
Controller::removeDevice(const std::string& uuid)
{
//     std::clog << "Controller::removeDevice()" << std::endl;
    if (m_devices.contains(uuid)) {
        deviceRemoved(&m_devices.get(uuid));
        m_devices.remove(uuid);
    }
}


void
ControllerImplAdapter::init()
{
    for (Device::ServiceIterator i = m_pDevice->beginService(); i != m_pDevice->endService(); ++i) {
        (*i)->initClient();
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
    std::clog << "SsdpNotifyAliveWriter::deviceRoot()" << std::endl;
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
    std::clog << "SsdpNotifyAliveWriter::device(): " << pDevice.getUuid() << std::endl;
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
    std::clog << "SsdpNotifyAliveWriter::service()" << std::endl;
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    m_res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::deviceRoot(const DeviceRoot& pDeviceRoot)
{
    std::clog << "SsdpNotifyByebyeWriter::deviceRoot()" << std::endl;
    // root device first message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType("upnp:rootdevice");  // once for root device
    m->setUniqueServiceName("uuid:" + pDeviceRoot.getRootDevice()->getUuid() + "::upnp:rootdevice");
    m_res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::device(const Device& pDevice)
{
    std::clog << "SsdpNotifyByebyeWriter::device(): " << pDevice.getUuid() << std::endl;
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
    std::clog << "SsdpNotifyByebyeWriter::service()" << std::endl;
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    m_res->addMessage(*m);
}


SsdpMessage::SsdpMessage() :
m_interface("*")
{
    initMessageMap();
}


SsdpMessage::SsdpMessage(TRequestMethod requestMethod) :
m_interface("*")
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


SsdpMessage::SsdpMessage(const std::string& buf, const std::string& interface, const Poco::Net::SocketAddress& sender)
{
    // FIXME: this shouldn't be executed on every SsdpMessage ctor
    initMessageMap();
    
    for (std::map<TRequestMethod,std::string>::iterator i = m_messageMap.begin(); i != m_messageMap.end(); i++) {
        m_messageConstMap[(*i).second] = (*i).first;
    }
    
    // FIXME: what about Poco::Message header for retrieving the request method?
    std::istringstream is(buf);
    // get first line and check request method type
    std::string line;
    getline(is, line);
    Poco::trimInPlace(line);
    
    // TODO: must be case-insensitive map?!
    m_requestMethod = m_messageConstMap[line];
    m_sender = sender;
    m_interface = interface;
    
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
//         std::clog << "Error in sdpMessage::SsdpMessage(): malformed header" << std::endl;
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
    m_messageHeader["CACHE-CONTROL"] = "max-age = " + Poco::NumberFormatter::format(duration);
}


int
SsdpMessage::getCacheControl()
{
    try {
        std::string value = m_messageHeader["CACHE-CONTROL"];
        return Poco::NumberParser::parse(value.substr(value.find('=')+1));
    }
    catch (Poco::NotFoundException) {
        std::clog << "missing CACHE-CONTROL in SSDP header" << std::endl;
    }
    catch (Poco::SyntaxException) {
        std::clog << "wrong number format of CACHE-CONTROL in SSDP header" << std::endl;
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
SsdpMessage::setLocation(const Poco::URI& location)
{
//     m_messageHeader.set("LOCATION", location.toString());
    m_messageHeader["LOCATION"] = location.toString();
}


Poco::URI
SsdpMessage::getLocation()
{
    try {
        return Poco::URI(m_messageHeader["LOCATION"]);
    }
    catch (Poco::NotFoundException) {
        std::clog << "Error in SsdpMessage::getLocation(): LOCATION field not found" << std::endl;
        return Poco::URI("");
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
        Poco::Environment::osName() + "/"
        + Poco::Environment::osVersion() + ", "
        + "UPnP/" + UPNP_VERSION + ", "
        + productNameVersion;
}


std::string
SsdpMessage::getServerOperatingSystem()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    return elements[0];
}


std::string
SsdpMessage::getServerUpnpVersion()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    std::string upnpVersion = elements[1];
    return upnpVersion.substr(upnpVersion.find('/')+1);
}


std::string
SsdpMessage::getServerProductNameVersion()
{
    std::string value = m_messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    return elements[2];
}


void
SsdpMessage::setMaximumWaitTime(int waitTime)
{
//     m_messageHeader.set("MX", NumberFormatter::format(waitTime));
    m_messageHeader["MX"] = Poco::NumberFormatter::format(waitTime);
}


int
SsdpMessage::getMaximumWaitTime()
{
    try {
        return Poco::NumberParser::parse(m_messageHeader["MX"]);
    }
    catch (Poco::NotFoundException) {
        std::clog << "missing MX in SSDP header" << std::endl;
    }
    catch (Poco::SyntaxException) {
        std::clog << "wrong number format of MX in SSDP header" << std::endl;
    }
    return SSDP_MIN_WAIT_TIME;  // if no valid number is given, return minimum required value
}


void
SsdpMessage::setDate()
{
    Poco::Timestamp t;
//     m_messageHeader.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
    m_messageHeader["DATE"] = Poco::DateTimeFormatter::format(t, Poco::DateTimeFormat::HTTP_FORMAT);
}


Poco::DateTime
SsdpMessage::getDate()
{
    std::string value = m_messageHeader["DATE"];
    int timeZoneDifferentail;
    try {
        return Poco::DateTimeParser::parse(Poco::DateTimeFormat::HTTP_FORMAT, value, timeZoneDifferentail);
    }
    catch (Poco::SyntaxException) {
        std::clog << "wrong date format of DATE in SSDP header" << std::endl;
    }
}


const std::string&
SsdpMessage::getInterface()
{
    return m_interface;
}


Poco::Net::SocketAddress
SsdpMessage::getSender()
{
    return m_sender;
}


SsdpSocket::SsdpSocket()
{
}


void
SsdpSocket::addInterface(const std::string& name)
{
    std::clog << "SsdpSocket::addInterface() name: " << name << std::endl;
    
    SsdpNetworkInterface* pInterface = new SsdpNetworkInterface(name, this);
    m_interfaces[name] = pInterface;
    
    std::clog << "SsdpSocket::addInterface() finished" << std::endl;
}

void
SsdpSocket::removeInterface(const std::string& name)
{
    std::clog << "SsdpSocket::removeInterface() name: " << name << std::endl;
    
    SsdpNetworkInterface* pInterface = m_interfaces[name];
    delete pInterface;
    m_interfaces.erase(name);
}


void
SsdpSocket::setObserver(const Poco::AbstractObserver& observer)
{
    m_notificationCenter.addObserver(observer);
}


void
SsdpSocket::start()
{
    std::clog << "SsdpSocket::start()" << std::endl;
    
    m_listenerThread.start(m_reactor);
    std::clog << "ssdp listener thread started" << std::endl;
}


SsdpSocket::~SsdpSocket()
{
    std::clog << std::endl << "closing SSDP socket ..." << std::endl;
    m_reactor.stop();
    m_listenerThread.join();
}


void
SsdpSocket::sendMessage(SsdpMessage& message, const std::string& interface, const Poco::Net::SocketAddress& receiver)
{
    std::string m = message.toString();
    
    for (std::map<std::string,SsdpNetworkInterface*>::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it) {
        if (interface == "*" || interface == (*it).first) {
            int bytesSent = (*it).second->m_pSsdpSenderSocket->sendTo(m.c_str(), m.length(), receiver);
//             std::clog << "SSDP message sent to interface: " << (*it).first << " , address: " << receiver.toString() << std::endl << m;
        }
    }
}


SsdpNetworkInterface::SsdpNetworkInterface(const std::string& interfaceName, SsdpSocket* pSsdpSocket) :
m_name(interfaceName),
m_pSsdpSocket(pSsdpSocket),
m_pBuffer(new char[BUFFER_SIZE])
{
    std::clog << "SsdpNetworkInterface::SsdpNetworkInterface() name: " << interfaceName << std::endl;
    m_pInterface = new Poco::Net::NetworkInterface(Poco::Net::NetworkInterface::forName(interfaceName));
    
    std::clog << "setting up sender socket" << std::endl;
    // listen to UDP unicast and send out to multicast
    m_pSsdpSenderSocket = new Poco::Net::MulticastSocket(Poco::Net::SocketAddress("0.0.0.0", 0));
    m_pSsdpSenderSocket->setInterface(*m_pInterface);
    m_pSsdpSenderSocket->setLoopback(true);
    m_pSsdpSenderSocket->setTimeToLive(4);  // TODO: let TTL be configurable
    
    std::clog << "setting up listener socket" << std::endl;
    // listen to UDP multicast
    m_pSsdpListenerSocket = new Poco::Net::MulticastSocket(Poco::Net::SocketAddress(Poco::Net::IPAddress(SSDP_ADDRESS), SSDP_PORT), true);
    m_pSsdpListenerSocket->setInterface(*m_pInterface);
    m_pSsdpListenerSocket->setLoopback(true);
    m_pSsdpListenerSocket->joinGroup(Poco::Net::IPAddress(SSDP_ADDRESS));
    
    std::clog << "adding event handlers" << std::endl;
    m_pSsdpSocket->m_reactor.addEventHandler(*m_pSsdpSenderSocket, Poco::Observer<SsdpNetworkInterface, Poco::Net::ReadableNotification>(*this, &SsdpNetworkInterface::onReadable));
    m_pSsdpSocket->m_reactor.addEventHandler(*m_pSsdpListenerSocket, Poco::Observer<SsdpNetworkInterface, Poco::Net::ReadableNotification>(*this, &SsdpNetworkInterface::onReadable));
    std::clog << "SsdpNetworkInterface::SsdpNetworkInterface() finished" << std::endl;
}


SsdpNetworkInterface::~SsdpNetworkInterface()
{
    std::clog << "SsdpNetworkInterface::~SsdpNetworkInterface()" << std::endl;
    
    m_pSsdpListenerSocket->leaveGroup(Poco::Net::IPAddress(SSDP_ADDRESS));
    
    m_pSsdpSocket->m_reactor.removeEventHandler(*m_pSsdpSenderSocket, Poco::Observer<SsdpNetworkInterface, Poco::Net::ReadableNotification>(*this, &SsdpNetworkInterface::onReadable));
    m_pSsdpSocket->m_reactor.removeEventHandler(*m_pSsdpListenerSocket, Poco::Observer<SsdpNetworkInterface, Poco::Net::ReadableNotification>(*this, &SsdpNetworkInterface::onReadable));
    
    delete m_pInterface;
    delete m_pSsdpListenerSocket;
    delete m_pSsdpSenderSocket;
    delete [] m_pBuffer;
}


void
SsdpNetworkInterface::onReadable(Poco::Net::ReadableNotification* pNotification)
{
//     std::clog << "SsdpNetworkInterface::onReadable()" << std::endl;
    
    Poco::Net::SocketAddress sender;
    Poco::Net::Socket* pSocket = &(pNotification->socket());
    Poco::Net::DatagramSocket* pDatagramSocket = static_cast<Poco::Net::DatagramSocket*>(pSocket);
    int n = pDatagramSocket->receiveFrom(m_pBuffer, BUFFER_SIZE, sender);
    if (n > 0) {
//         std::clog << "SSDP message received on interface: " << m_name << ", from: " << sender.toString() << std::endl << std::string(m_pBuffer, n) << std::endl;
        m_pSsdpSocket->m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n), m_name, sender));
    }
    // FIXME: this results in a segfault
//     delete pNotification;
}
