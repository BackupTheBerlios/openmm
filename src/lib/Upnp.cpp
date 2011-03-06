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

#include "Upnp.h"
#include "UpnpPrivate.h"

namespace Omm {

Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::FormattingChannel* pFormatLogger = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
//     Poco::FileChannel* pFileChannel = new Poco::FileChannel("omm.log");
    pSplitterChannel->addChannel(pConsoleChannel);
//     pSplitterChannel->addChannel(pFileChannel);
    pFormatLogger->setChannel(pSplitterChannel);
    pFormatLogger->open();
#ifdef NDEBUG
    _pUpnpLogger = &Poco::Logger::create("UPNP", pFormatLogger, 0);
    _pSsdpLogger = &Poco::Logger::create("UPNP.SSDP", pFormatLogger, 0);
    _pHttpLogger = &Poco::Logger::create("UPNP.HTTP", pFormatLogger, 0);
    _pDescriptionLogger = &Poco::Logger::create("UPNP.DESC", pFormatLogger, 0);
    _pControlLogger = &Poco::Logger::create("UPNP.CONTROL", pFormatLogger, 0);
    _pEventingLogger = &Poco::Logger::create("UPNP.EVENTING", pFormatLogger, 0);
#else
    _pUpnpLogger = &Poco::Logger::create("UPNP", pFormatLogger, Poco::Message::PRIO_ERROR);
//    _pSsdpLogger = &Poco::Logger::create("UPNP.SSDP", pFormatLogger, Poco::Message::PRIO_ERROR);
    _pSsdpLogger = &Poco::Logger::create("UPNP.SSDP", pFormatLogger, Poco::Message::PRIO_ERROR);
    _pHttpLogger = &Poco::Logger::create("UPNP.HTTP", pFormatLogger, Poco::Message::PRIO_ERROR);
    _pDescriptionLogger = &Poco::Logger::create("UPNP.DESC", pFormatLogger, Poco::Message::PRIO_ERROR);
    _pControlLogger = &Poco::Logger::create("UPNP.CONTROL", pFormatLogger, Poco::Message::PRIO_ERROR);
    _pEventingLogger = &Poco::Logger::create("UPNP.EVENTING", pFormatLogger, Poco::Message::PRIO_ERROR);
#endif
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::upnp()
{
    return *_pUpnpLogger;
}


Poco::Logger&
Log::ssdp()
{
    return *_pSsdpLogger;
}


Poco::Logger&
Log::http()
{
    return *_pHttpLogger;
}


Poco::Logger&
Log::desc()
{
    return *_pDescriptionLogger;
}


Poco::Logger&
Log::ctrl()
{
    return *_pControlLogger;
}


Poco::Logger&
Log::event()
{
    return *_pEventingLogger;
}


Icon::Icon(int width, int height, int depth, const std::string& mime, const std::string& uri) :
_width(width),
_height(height),
_depth(depth),
_mime(mime),
_requestUri(""),
_iconPath(":/usr/lib/omm:/usr/local/lib/omm")
{
    try {
        _iconPath = Poco::Environment::get("OMM_ICON_PATH") + _iconPath;
    }
    catch (Poco::NotFoundException) {
    }
    
    if (uri != "") {
        try {
            retrieve(uri);
        }
        catch (Poco::NotFoundException) {
            Log::instance()->upnp().error("failed to retrieve icon from: " + uri);
        }
    }
}


Icon::~Icon()
{
    _buffer.clear();
}


const std::string&
Icon::getBuffer()
{
    return _buffer;
}


void
Icon::retrieve(const std::string& uri)
{
    Poco::URI iconUri(uri);
    
    if (iconUri.getScheme() == "file") {
        Poco::File f(iconUri.getPath());
        if (!f.exists()) {
            throw Poco::NotFoundException();
        }
        Log::instance()->upnp().debug("reading icon from file: " + iconUri.getPath());
        std::ifstream ifs(iconUri.getPath().c_str());
        char* pData = new char[f.getSize()];
        ifs.read(pData, f.getSize());
        _buffer = std::string(pData, f.getSize());

    }
    else if (iconUri.getScheme() == "http") {
        Log::instance()->upnp().debug("download icon: " + uri);
        Poco::Net::HTTPStreamFactory streamOpener;

        _buffer.clear();
        std::size_t size;
        try {
            std::istream* pInStream = streamOpener.open(Poco::URI(uri));
            if (pInStream) {
                size = Poco::StreamCopier::copyToString(*pInStream, _buffer);
            }
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnp().error("download icon failed: " + e.displayText());
        }
        if (size == 0) {
            Log::instance()->upnp().error("download icon failed, no bytes received.");
            return;
        }
        else {
            Log::instance()->upnp().debug("download icon success, bytes: " + Poco::NumberFormatter::format(size));
        }
    }
    else if (iconUri.isRelative()) {
        Poco::StringTokenizer pathSplitter(_iconPath, ":");
        Poco::StringTokenizer::Iterator it;
        for (it = pathSplitter.begin(); it != pathSplitter.end(); ++it) {
            if (*it == "") {
                continue;
            }
            Poco::URI baseUri(*it + "/");
            baseUri.resolve(iconUri);
            Log::instance()->upnp().debug("try to read icon from file: " + baseUri.getPath());
            Poco::File f(baseUri.getPath());
            if (!f.exists()) {
                continue;
            }
            Log::instance()->upnp().debug("reading icon from file: " + baseUri.getPath());
            std::ifstream ifs(baseUri.getPath().c_str());
            char* pData = new char[f.getSize()];
            ifs.read(pData, f.getSize());
            _buffer = std::string(pData, f.getSize());
            break;
        }
        if (it == pathSplitter.end()) {
            throw Poco::NotFoundException();
        }
    }
}


SsdpSocket::SsdpSocket()
{
}


SsdpSocket::~SsdpSocket()
{
    stop();
}


void
SsdpSocket::init()
{
    _mode = NotConfigured;
    _pBuffer = new char[BUFFER_SIZE];
    // listen to UDP unicast and send out to multicast
    Log::instance()->ssdp().debug("create broadcast socket");
    _pSsdpSenderSocket = new Poco::Net::MulticastSocket;

    // listen to UDP multicast
    Log::instance()->ssdp().debug("create listener socket ...");
    _pSsdpListenerSocket = new Poco::Net::MulticastSocket(Poco::Net::SocketAddress("0.0.0.0", SSDP_PORT), true);

    _reactor.addEventHandler(*_pSsdpSenderSocket, Poco::Observer<SsdpSocket, Poco::Net::ReadableNotification>(*this, &SsdpSocket::onReadable));
    _reactor.addEventHandler(*_pSsdpListenerSocket, Poco::Observer<SsdpSocket, Poco::Net::ReadableNotification>(*this, &SsdpSocket::onReadable));
}


void
SsdpSocket::deinit()
{
    _reactor.removeEventHandler(*_pSsdpSenderSocket, Poco::Observer<SsdpSocket, Poco::Net::ReadableNotification>(*this, &SsdpSocket::onReadable));
    _reactor.removeEventHandler(*_pSsdpListenerSocket, Poco::Observer<SsdpSocket, Poco::Net::ReadableNotification>(*this, &SsdpSocket::onReadable));
    delete _pSsdpSenderSocket;
    delete _pSsdpListenerSocket;
    delete _pBuffer;
    _mode = NotConfigured;
}


void
SsdpSocket::addInterface(const std::string& name)
{
    Log::instance()->ssdp().information("add interface: " + name);
    setupSockets();
}


void
SsdpSocket::removeInterface(const std::string& name)
{
    Log::instance()->ssdp().information("remove interface: " + name);
//    resetSockets();
//    deinit();
//    init();
    setupSockets();
}


void
SsdpSocket::setObserver(const Poco::AbstractObserver& observer)
{
    _notificationCenter.addObserver(observer);
}


void
SsdpSocket::start()
{
    Log::instance()->ssdp().information("starting SSDP listener ...");
    _listenerThread.start(_reactor);
    Log::instance()->ssdp().information("SSDP listener started.");
}


void
SsdpSocket::stop()
{
    Log::instance()->ssdp().information("stopping SSDP listener ...");
    _reactor.stop();
    _listenerThread.join();
    resetSockets();
    Log::instance()->ssdp().information("SSDP listener stopped.");
}


void
SsdpSocket::sendMessage(SsdpMessage& message, const Poco::Net::SocketAddress& receiver)
{
    std::string m = message.toString();

    int bytesSent = 0;
    Poco::Net::SocketAddress loopReceiver;
    if (_mode == Broadcast && receiver.toString() == SSDP_FULL_ADDRESS) {
        loopReceiver = Poco::Net::SocketAddress(SSDP_LOOP_ADDRESS, SSDP_PORT);
    }
    else {
        loopReceiver = receiver;
    }
    Log::instance()->ssdp().debug("sending SSDP message to address: " + loopReceiver.toString() + " ...");
    try {
        bytesSent = _pSsdpSenderSocket->sendTo(m.c_str(), m.length(), loopReceiver);
    }
    catch(Poco::Net::NetException& e) {
        Log::instance()->ssdp().error("sending SSDP message failed: " + e.message());
    }
    Log::instance()->ssdp().debug("SSDP message sent.");
}


void
SsdpSocket::setupSockets()
{
#ifndef __DARWIN__
    if (Sys::NetworkInterfaceManager::instance()->loopbackOnly()) {
        Log::instance()->ssdp().warning("loopback is the only network interface, forcing broadcast mode.");
        setBroadcast();
    }
    else
#endif
    {
        if (_mode != Multicast) {
            try {
                Log::instance()->ssdp().debug("set ttl to 4");
                // FIXME: setting TTL on windows segfaults
    //            _pSsdpSenderSocket->setTimeToLive(4);  // TODO: let TTL be configurable
                setMulticast();
            }
            catch (Poco::IOException) {
                Log::instance()->ssdp().error("failed to join multicast group");
                Log::instance()->ssdp().warning("MULTICAST socket option and route to multicast address on loopback interface probably need to be set.");
                Log::instance()->ssdp().warning("as superuser do something like \"ifconfig lo multicast; route add 239.255.255.250 lo\".");
                Log::instance()->ssdp().warning("switching to non-standard compliant broadcast mode for loopback interface.");
                setBroadcast();
            }
        }
    }
}


void
SsdpSocket::resetSockets()
{
    Log::instance()->ssdp().debug("reset SSDP sockets ...");
    if (_mode == Multicast) {
        Log::instance()->ssdp().debug("leave multicast group ...");
        _pSsdpListenerSocket->leaveGroup(Poco::Net::IPAddress(SSDP_ADDRESS));
    }
    else if (_mode == Broadcast) {
        Log::instance()->ssdp().debug("disable broadcast mode ...");
        _pSsdpSenderSocket->setBroadcast(false);
    }
    _mode = NotConfigured;
}


void
SsdpSocket::setMulticast()
{
    Log::instance()->ssdp().debug("enable multicast ...");
    if (_mode == Broadcast) {
        Log::instance()->ssdp().debug("disable broadcast mode ...");
        _pSsdpSenderSocket->setBroadcast(false);
    }
    if (_mode != Multicast) {
        Log::instance()->ssdp().debug("join multicast group ...");
        _pSsdpListenerSocket->joinGroup(Poco::Net::IPAddress(SSDP_ADDRESS));
    }
    _mode = Multicast;
}


void
SsdpSocket::setBroadcast()
{
    Log::instance()->ssdp().debug("enable broadcast ...");
    if (_mode == Multicast) {
        Log::instance()->ssdp().debug("leave multicast group ...");
        _pSsdpListenerSocket->leaveGroup(Poco::Net::IPAddress(SSDP_ADDRESS));
    }
    if (_mode != Broadcast) {
        Log::instance()->ssdp().debug("set broadcast mode ...");
        _pSsdpSenderSocket->setBroadcast(true);
    }
    _mode = Broadcast;
}


void
SsdpSocket::onReadable(Poco::Net::ReadableNotification* pNotification)
{
    Poco::Net::SocketAddress sender;
    Poco::Net::Socket* pSocket = &(pNotification->socket());
    Poco::Net::DatagramSocket* pDatagramSocket = static_cast<Poco::Net::DatagramSocket*>(pSocket);
    int n = pDatagramSocket->receiveFrom(_pBuffer, BUFFER_SIZE, sender);
    if (n > 0) {
        Log::instance()->ssdp().debug("received message from: " + sender.toString() + "\n" + std::string(_pBuffer, n));

        _notificationCenter.postNotification(new SsdpMessage(std::string(_pBuffer, n), sender));
    }
    // FIXME: this results in a segfault
//     delete pNotification;
}


DeviceRoot*
UriDescriptionReader::deviceRoot(const std::string& deviceDescriptionUri)
{
    _deviceDescriptionUri = deviceDescriptionUri;
    return parseDeviceRoot(parseDescription(getDescription(deviceDescriptionUri)));
}


std::string&
UriDescriptionReader::getDescription(const std::string& relativeUri)
{
    Poco::URI targetUri(_deviceDescriptionUri);
    targetUri.resolve(relativeUri);
    Log::instance()->desc().information("retrieving device description from: " + targetUri.toString());
    
    std::string* res;
    
    if (targetUri.getScheme() == "file") {
        Log::instance()->desc().information("reading description from file: " + targetUri.getPath());
        std::ifstream ifs(targetUri.getPath().c_str());
        std::stringstream ss;
        Poco::StreamCopier::copyStream(ifs, ss);
        res = new std::string(ss.str());
    }
    else if (targetUri.getScheme() == "http") {
        Log::instance()->desc().information("downloading description from host: " + targetUri.getHost()
             + ", port: " + Poco::NumberFormatter::format(targetUri.getPort()) + ", path: " + targetUri.getPath());
        Poco::Net::HTTPClientSession session(targetUri.getHost(), targetUri.getPort());
        Poco::Net::HTTPRequest request("GET", targetUri.getPath());
        session.sendRequest(request);
        
        Poco::Net::HTTPResponse response;
        std::istream& rs = session.receiveResponse(response);
        Log::instance()->desc().information("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream header;
        response.write(header);
        Log::instance()->desc().debug("response header:\n" + header.str());
        res = new std::string;
        Poco::StreamCopier::copyToString(rs, *res);
    }
    else {
        Log::instance()->desc().error("Error in UriDescriptionReader: unknown scheme in description uri");
        res = new std::string;
        return *res;
    }
    Log::instance()->desc().debug("retrieved description:\n*BEGIN*" + *res + "*END*");
    return *res;
}


StringDescriptionReader::StringDescriptionReader(std::map<std::string,std::string*>& stringMap) :
_pStringMap(&stringMap)
{
}


DeviceRoot*
StringDescriptionReader::deviceRoot(const std::string& deviceDescriptionKey)
{
    return parseDeviceRoot(parseDescription(getDescription(deviceDescriptionKey)));
}


std::string&
StringDescriptionReader::getDescription(const std::string& path)
{
    std::string* res = (*_pStringMap)[path];
    parseDescription(*res);
    return *res;
}


DescriptionReader::DescriptionReader()
{
}


DescriptionReader::~DescriptionReader()
{
    releaseDescriptionDocument();
}


Poco::XML::Node*
DescriptionReader::parseDescription(const std::string& description)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    _pDocStack.push(parser.parseString(description));
    return _pDocStack.top()->documentElement()->firstChild();
}


void
DescriptionReader::releaseDescriptionDocument()
{
//     if (!_pDocStack.empty()) {
//         _pDocStack.top()->release();
//         _pDocStack.pop();
//     }
}


DeviceRoot*
DescriptionReader::parseDeviceRoot(Poco::XML::Node* pNode)
{
    DeviceRoot* pRes = new DeviceRoot();
    // NOTE: a running HttpSocket is needed here, to set host and port of BaseUri and DescriptionUri
    //       that's why ommgen crashes without setting up a socket in HttpSocket::init()
    while (pNode)
    {
        if (pNode->nodeName() == "device" && pNode->hasChildNodes()) {
            Device* pDevice = device(pNode->firstChild(), pRes);
            pRes->addDevice(pDevice);
            pRes->setRootDevice(pDevice);
        }
        pNode = pNode->nextSibling();
    }
    return pRes;
}


Device*
DescriptionReader::device(Poco::XML::Node* pNode, DeviceRoot* pDeviceRoot)
{
    Device* pRes = new Device();
    pRes->setDeviceRoot(pDeviceRoot);

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
                            pRes->addService(service(pChild->firstChild()));
                        }
                        else {
                            Log::instance()->desc().error("empty service");
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                Log::instance()->desc().error("service list without services");
            }
        }
        else if (pNode->nodeName() == "deviceList") {
            if (pNode->hasChildNodes()) {
                Poco::XML::Node* pChild = pNode->firstChild();
                while (pChild) {
                    if (pChild->nodeName() == "device") {
                        if (pChild->hasChildNodes()) {
                            pDeviceRoot->addDevice(device(pChild->firstChild(), pDeviceRoot));
                        }
                        else {
                            Log::instance()->desc().error("empty embedded device");
                        }
                    }
                    pChild = pChild->nextSibling();
                }
            }
            else {
                Log::instance()->desc().error("device list without embedded devices");
            }
        }
        else if (pNode->nodeName() == "iconList") {
        }
        else {
            pRes->addProperty(pNode->nodeName(), pNode->innerText());
        }
        pNode = pNode->nextSibling();
    }
    return pRes;
}


Service*
DescriptionReader::service(Poco::XML::Node* pNode)
{
    Service* pRes = new Service();
    
    while (pNode)
    {
        if (pNode->nodeName() == "serviceType" && pNode->hasChildNodes()) {
            pRes->setServiceType(pNode->innerText());
        }
        if (pNode->nodeName() == "serviceId" && pNode->hasChildNodes()) {
            pRes->setServiceId(pNode->innerText());
        }
        else if (pNode->nodeName() == "SCPDURL" && pNode->hasChildNodes()) {
            std::string descriptionPath = pNode->innerText();
            pRes->setDescriptionPath(descriptionPath);
            // load the service description into the Service object.
            pRes->setDescription(getDescription(descriptionPath));
            Poco::XML::Node* pScpd = parseDescription(*pRes->getDescription());
            while (pScpd) {
                if (pScpd->nodeName() == "actionList" && pScpd->hasChildNodes()) {
                    Poco::XML::Node* pChild = pScpd->firstChild();
                    while (pChild) {
                        if (pChild->nodeName() == "action") {
                            pRes->addAction(action(pChild->firstChild()));
                        }
                        pChild = pChild->nextSibling();
                    }
                }
                else if (pScpd->nodeName() == "serviceStateTable" && pScpd->hasChildNodes()) {
                    Poco::XML::Node* pChild = pScpd->firstChild();
                    while (pChild) {
                        if (pChild->nodeName() == "stateVariable") {
                            StateVar* pStateVar = stateVar(pChild->firstChild());
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
            releaseDescriptionDocument();
        }
        else if (pNode->nodeName() == "controlURL" && pNode->hasChildNodes()) {
            std::string controlPath = pNode->innerText();
            pRes->setControlPath(controlPath);
        }
        else if (pNode->nodeName() == "eventSubURL" && pNode->hasChildNodes()) {
            std::string eventPath = pNode->innerText();
            pRes->setEventPath(eventPath);
        }
        
        pNode = pNode->nextSibling();
    }
    return pRes;
}


Action*
DescriptionReader::action(Poco::XML::Node* pNode)
{
    Action* pRes = new Action();
    
    while (pNode)
    {
        if (pNode->nodeName() == "name" && pNode->hasChildNodes()) {
            pRes->setName(pNode->innerText());
        }
        else if (pNode->nodeName() == "argumentList" && pNode->hasChildNodes()) {
            Poco::XML::Node* pChild = pNode->firstChild();
            while (pChild) {
                if (pChild->nodeName() == "argument") {
                    pRes->appendArgument(argument(pChild->firstChild()));
                }
                pChild = pChild->nextSibling();
            }
        }
        pNode = pNode->nextSibling();
    }
    return pRes;
}


Argument*
DescriptionReader::argument(Poco::XML::Node* pNode)
{
    Argument* pRes = new Argument();
    
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
    
    return pRes;
}


StateVar*
DescriptionReader::stateVar(Poco::XML::Node* pNode)
{
    StateVar* pRes = new StateVar();
    
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
            Log::instance()->desc().debug("set default value for state variable \"" + pRes->getName() + "\" to: " + val);
            
            pRes->setValue(val);
        }
        pNode = pNode->nextSibling();
    }
    return pRes;
}


ActionRequestReader::ActionRequestReader(const std::string& requestBody, Action* pActionTemplate) : _pActionTemplate(pActionTemplate)
{
    Log::instance()->ctrl().debug("action request:\n" + requestBody);
    
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    _pDoc = parser.parseString(requestBody);
    Poco::XML::NodeIterator it(_pDoc, Poco::XML::NodeFilter::SHOW_ALL);
    _nodeStack.push(it.nextNode());
}


Action*
ActionRequestReader::action()
{
    Action* pRes = _pActionTemplate;
    Poco::XML::Node* pNode = _nodeStack.top();
    Poco::XML::NodeIterator it(pNode, Poco::XML::NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Poco::XML::Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Poco::XML::Node* pAction = pBody->firstChild();
        
        if (pAction->hasChildNodes()) {
            Poco::XML::Node* pArgument = pAction->firstChild();
    
            while (pArgument) {
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            Log::instance()->ctrl().debug("action without arguments");
        }
    }
    else {
        Log::instance()->ctrl().error("action without body");
    }
    _nodeStack.pop();
    return pRes;
}


ActionResponseReader::ActionResponseReader(const std::string& responseBody, Action* pActionTemplate) :
_pActionTemplate(pActionTemplate),
_pDoc(0)
{
//     Log::instance()->ctrl().debug(Omm::Util::format("action response:\n%s", responseBody));
    Poco::XML::DOMParser parser;
    // TODO: set encoding with parser.setEncoding();
    // there's coming a lot of rubbish thru the wire, decorated with white-spaces all over the place ...
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    // FIXME: shouldn't throw exception in ctor, put it in a method (e.g. read())
    try {
        _pDoc = parser.parseString(responseBody);
    }
    catch (Poco::XML::SAXParseException) {
        Log::instance()->ctrl().error("could not parse action response");
        return;
    }
    catch (Poco::XML::DOMException) {
        Log::instance()->ctrl().error("could not parse action response");
        return;
    }
    // TODO: new reader design: don't go further if parser has failed
    Poco::XML::NodeIterator it(_pDoc, Poco::XML::NodeFilter::SHOW_ALL);
    _nodeStack.push(it.nextNode());
}


Action*
ActionResponseReader::action()
{
    // FIXME: action shouldn't be called, when _pDoc is 0 (see ctor of ActionResponseReader)
    if (!_pDoc) {
        return 0;
    }
    // TODO: same code as in ActionRequestReader
    Action* pRes = _pActionTemplate;
    Poco::XML::Node* pNode = _nodeStack.top();
    Poco::XML::NodeIterator it(pNode, Poco::XML::NodeFilter::SHOW_ELEMENT);
    
    while(pNode && (pNode->nodeName() != pNode->prefix() + ":Body")) {
        pNode = it.nextNode();
    }
    Poco::XML::Node* pBody = pNode;
    if (pBody && pBody->hasChildNodes()) {
        Poco::XML::Node* pAction = pBody->firstChild();
        if (pAction->hasChildNodes()) {
            Poco::XML::Node* pArgument = pAction->firstChild();
            
            while (pArgument) {
                Log::instance()->ctrl().debug("action response arg: " + pArgument->nodeName() + " = " + pArgument->innerText());
                pRes->setArgument(pArgument->nodeName(), pArgument->innerText());
                pArgument = pArgument->nextSibling();
            }
        }
        else {
            Log::instance()->ctrl().debug("action without arguments");
        }
    }
    else {
        Log::instance()->ctrl().error("action without body");
    }
    _nodeStack.pop();
    return pRes;
}



DeviceDescriptionWriter::DeviceDescriptionWriter()
{
    _pDoc = new Poco::XML::Document;
}


void
DeviceDescriptionWriter::deviceRoot(DeviceRoot& deviceRoot)
{
    Poco::AutoPtr<Poco::XML::Element> pRoot = _pDoc->createElement("root");
    pRoot->setAttribute("xmlns", "urn:schemas-upnp-org:device-1-0");
    Poco::AutoPtr<Poco::XML::Element> pSpecVersion = _pDoc->createElement("specVersion");
    Poco::AutoPtr<Poco::XML::Element> pMajor = _pDoc->createElement("major");
    Poco::AutoPtr<Poco::XML::Element> pMinor = _pDoc->createElement("minor");
    Poco::AutoPtr<Poco::XML::Text> pMajorVersion = _pDoc->createTextNode("1");
    Poco::AutoPtr<Poco::XML::Text> pMinorVersion = _pDoc->createTextNode("0");
    pMajor->appendChild(pMajorVersion);
    pMinor->appendChild(pMinorVersion);
    pSpecVersion->appendChild(pMajor);
    pSpecVersion->appendChild(pMinor);
    pRoot->appendChild(pSpecVersion);
    // write root device
    Poco::AutoPtr<Poco::XML::Element> pRootDevice = device(*deviceRoot.getRootDevice());
    pRoot->appendChild(pRootDevice);
    _pDoc->appendChild(pRoot);
    // end root device
    
    // TODO: if there are embedded devices open a deviceList
    // write embedded devices
    // end embedded devices
    
    // end DeviceRoot
}


Poco::XML::Element*
DeviceDescriptionWriter::device(Device& device)
{
    Poco::XML::Element* pDevice = _pDoc->createElement("device");
    
    // device type
    Poco::AutoPtr<Poco::XML::Element> pDeviceType = _pDoc->createElement("deviceType");
    Poco::AutoPtr<Poco::XML::Text> pDeviceTypeVal = _pDoc->createTextNode(device.getDeviceType());
    pDeviceType->appendChild(pDeviceTypeVal);
    pDevice->appendChild(pDeviceType);
    // UUID
    Poco::AutoPtr<Poco::XML::Element> pUuid = _pDoc->createElement("UDN");
    Poco::AutoPtr<Poco::XML::Text> pUuidVal = _pDoc->createTextNode("uuid:" + device.getUuid());
    pUuid->appendChild(pUuidVal);
    pDevice->appendChild(pUuid);
    
    // Properties
    for (Container<std::string>::KeyIterator it = device._properties.beginKey(); it != device._properties.endKey(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pProp = _pDoc->createElement((*it).first);
        Poco::AutoPtr<Poco::XML::Text> pVal = _pDoc->createTextNode(*((*it).second));
        pProp->appendChild(pVal);
        pDevice->appendChild(pProp);
        Log::instance()->desc().debug("writer added property: " + (*it).first + " = " + *(*it).second);
    }
    
    Poco::AutoPtr<Poco::XML::Element> pIconList = _pDoc->createElement("iconList");
    pDevice->appendChild(pIconList);
    // Icons
    for (Device::IconIterator it = device.beginIcon(); it != device.endIcon(); ++it) {
//        if ((*it)->_pData) {
        if ((*it)->_buffer.size()) {
            pIconList->appendChild(icon(*it));
            Log::instance()->desc().debug("writer added icon: " + (*it)->_requestUri);
        }
    }
    
    Poco::AutoPtr<Poco::XML::Element> pServiceList = _pDoc->createElement("serviceList");
    pDevice->appendChild(pServiceList);
    // Services
    for (Container<Service>::Iterator it = device._services.begin(); it != device._services.end(); ++it) {
        pServiceList->appendChild(service(*it));
        Log::instance()->desc().debug("writer added service: " + (*it)->getServiceType());
    }
    
    return pDevice;
}


Poco::XML::Element*
DeviceDescriptionWriter::service(Service* pService)
{
    Poco::XML::Element* pServiceElement = _pDoc->createElement("service");
    
    // serviceType
    Poco::AutoPtr<Poco::XML::Element> pServiceType = _pDoc->createElement("serviceType");
    Poco::AutoPtr<Poco::XML::Text> pServiceTypeVal = _pDoc->createTextNode(pService->getServiceType());
    pServiceType->appendChild(pServiceTypeVal);
    pServiceElement->appendChild(pServiceType);
    // serviceId
    Poco::AutoPtr<Poco::XML::Element> pServiceId = _pDoc->createElement("serviceId");
    Poco::AutoPtr<Poco::XML::Text> pServiceIdVal = _pDoc->createTextNode(pService->getServiceId());
    pServiceId->appendChild(pServiceIdVal);
    pServiceElement->appendChild(pServiceId);
    // SCPDURL
    Poco::AutoPtr<Poco::XML::Element> pDescription = _pDoc->createElement("SCPDURL");
    Poco::AutoPtr<Poco::XML::Text> pDescriptionVal = _pDoc->createTextNode(pService->getDescriptionPath());
    pDescription->appendChild(pDescriptionVal);
    pServiceElement->appendChild(pDescription);
    // controlURL
    Poco::AutoPtr<Poco::XML::Element> pControl = _pDoc->createElement("controlURL");
    Poco::AutoPtr<Poco::XML::Text> pControlVal = _pDoc->createTextNode(pService->getControlPath());
    pControl->appendChild(pControlVal);
    pServiceElement->appendChild(pControl);
    // eventSubURL
    Poco::AutoPtr<Poco::XML::Element> pEvent = _pDoc->createElement("eventSubURL");
    Poco::AutoPtr<Poco::XML::Text> pEventVal = _pDoc->createTextNode(pService->getEventPath());
    pEvent->appendChild(pEventVal);
    pServiceElement->appendChild(pEvent);
    
    return pServiceElement;
}


Poco::XML::Element*
DeviceDescriptionWriter::icon(Icon* pIcon)
{
    Poco::XML::Element* pIconElement = _pDoc->createElement("icon");
    
    // mimetype
    Poco::AutoPtr<Poco::XML::Element> pMimetype = _pDoc->createElement("mimetype");
    Poco::AutoPtr<Poco::XML::Text> pMimetypeVal = _pDoc->createTextNode(pIcon->_mime.toString());
    pMimetype->appendChild(pMimetypeVal);
    pIconElement->appendChild(pMimetype);
    // width
    Poco::AutoPtr<Poco::XML::Element> pWidth = _pDoc->createElement("width");
    Poco::AutoPtr<Poco::XML::Text> pWidthVal = _pDoc->createTextNode(Poco::NumberFormatter::format(pIcon->_width));
    pWidth->appendChild(pWidthVal);
    pIconElement->appendChild(pWidth);
    // height
    Poco::AutoPtr<Poco::XML::Element> pHeight = _pDoc->createElement("height");
    Poco::AutoPtr<Poco::XML::Text> pHeightVal = _pDoc->createTextNode(Poco::NumberFormatter::format(pIcon->_height));
    pHeight->appendChild(pHeightVal);
    pIconElement->appendChild(pHeight);
    // depth
    Poco::AutoPtr<Poco::XML::Element> pDepth = _pDoc->createElement("depth");
    Poco::AutoPtr<Poco::XML::Text> pDepthVal = _pDoc->createTextNode(Poco::NumberFormatter::format(pIcon->_depth));
    pDepth->appendChild(pDepthVal);
    pIconElement->appendChild(pDepth);
    // url
    Poco::AutoPtr<Poco::XML::Element> pUrl = _pDoc->createElement("url");
    Poco::AutoPtr<Poco::XML::Text> pUrlVal = _pDoc->createTextNode(pIcon->_requestUri);
    pUrl->appendChild(pUrlVal);
    pIconElement->appendChild(pUrl);
    
    return pIconElement;
}


std::string*
DeviceDescriptionWriter::write()
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    Log::instance()->desc().debug("rewrote description:\n*BEGIN*" + ss.str() + "*END*");
    return new std::string(ss.str());
}


void
ActionRequestWriter::action(Action* action)
{
    // TODO: nearly same code as in ActionResponseWriter
    _pDoc = new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pEnvelope = _pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    Poco::AutoPtr<Poco::XML::Element> pBody = _pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    Poco::AutoPtr<Poco::XML::Element> pActionRequest = _pDoc->createElementNS(action->getService()->getServiceType(), action->getName());
    
    for(Action::ArgumentIterator i = action->beginInArgument(); i != action->endInArgument(); ++i) {
        Poco::AutoPtr<Poco::XML::Element> pArgument = _pDoc->createElement((*i)->getName());
        Poco::AutoPtr<Poco::XML::Text> pArgumentValue = _pDoc->createTextNode(action->getArgument<std::string>((*i)->getName()));
        pArgument->appendChild(pArgumentValue);
        pActionRequest->appendChild(pArgument);
    }
    
    pBody->appendChild(pActionRequest);
    pEnvelope->appendChild(pBody);
    _pDoc->appendChild(pEnvelope);
}


void
ActionRequestWriter::write(std::string& actionMessage)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    actionMessage = ss.str();
}


ActionResponseWriter::ActionResponseWriter(std::string& responseBody) :
_responseBody(&responseBody)
{
}


void
ActionResponseWriter::action(Action& action)
{
    Poco::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pEnvelope = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Envelope");
    pEnvelope->setAttributeNS("http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    Poco::AutoPtr<Poco::XML::Element> pBody = pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "Body");
    Poco::AutoPtr<Poco::XML::Element> pActionResponse = pDoc->createElementNS(action.getService()->getServiceType(), action.getName() + "Response");
    
    for(Action::ArgumentIterator i = action.beginOutArgument(); i != action.endOutArgument(); ++i) {
        Poco::AutoPtr<Poco::XML::Element> pArgument = pDoc->createElement((*i)->getName());
        Poco::AutoPtr<Poco::XML::Text> pArgumentValue = pDoc->createTextNode(action.getArgument<std::string>((*i)->getName()));
        pArgument->appendChild(pArgumentValue);
        pActionResponse->appendChild(pArgument);
    }
    
    pBody->appendChild(pActionResponse);
    pEnvelope->appendChild(pBody);
    pDoc->appendChild(pEnvelope);
    
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, pDoc);
    *_responseBody = ss.str();
//     Log::instance()->ctrl().debug(Omm::Util::format("response body:\n%s", *_responseBody));
}


StateVar*
Argument::getRelatedStateVarReference() const
{
    return _pAction->getService()->getStateVarReference(_relatedStateVar);
}


EventMessageWriter::EventMessageWriter()
{
    _pDoc = new Poco::XML::Document;
    _pPropertySet = _pDoc->createElementNS("urn:schemas-upnp-org:event-1-0", "propertyset");
    _pDoc->appendChild(_pPropertySet);
}


void
EventMessageWriter::write(std::string& eventMessage)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    eventMessage = ss.str();
    Log::instance()->event().debug("event message:\n" + ss.str());
}


void
EventMessageWriter::stateVar(const StateVar& stateVar)
{
    Poco::AutoPtr<Poco::XML::Element> pProperty = _pDoc->createElementNS("http://schemas.xmlsoap.org/soap/envelope/", "property");
    Poco::AutoPtr<Poco::XML::Element> pStateVar = _pDoc->createElement(stateVar.getName());
    Poco::AutoPtr<Poco::XML::Text> pStateVarValue = _pDoc->createTextNode(stateVar.getValue());
    pStateVar->appendChild(pStateVarValue);
    pProperty->appendChild(pStateVar);
    _pPropertySet->appendChild(pProperty);
}


Subscription::Subscription(std::string callbackUri) :
_deliveryAddress(callbackUri)
{
    Log::instance()->event().debug("subscription uri: " + callbackUri);
    
    // TODO: implement timer stuff
    _uuid = Poco::UUIDGenerator().createRandom();
    Log::instance()->event().debug("SID: " + _uuid.toString());
    _eventKey = 0;
    _pSession = new Poco::Net::HTTPClientSession(_deliveryAddress.getHost(), _deliveryAddress.getPort());
}


std::string
Subscription::getEventKey()
{
    // TODO: should lock this
    _eventKey = (++_eventKey == 0) ? 1 : _eventKey;
    return Poco::NumberFormatter::format(_eventKey);
}


void
Subscription::sendEventMessage(const std::string& eventMessage)
{
    // TODO: queue the eventMessages for sending ...?
//     std::string path = _deliveryAddress.getPath();
//     if (path.substr(0, 1) == "/") {
//         path = path.substr(1);
//     }
    Poco::Net::HTTPRequest request("NOTIFY", _deliveryAddress.getPath(), "HTTP/1.1");
    request.set("HOST", _deliveryAddress.getAuthority());
    request.setContentType("text/xml");
    request.set("NT", "upnp:event");
    request.set("NTS", "upnp:propchange");
    request.set("SID", "uuid:" + _uuid.toString());
    
//     HTTPRequest* request = newRequest();
    request.set("SEQ", getEventKey());
    request.setContentLength(eventMessage.size());
    // set request body and send request
    Log::instance()->event().debug("sending event message to: " + getSession()->getHost() + ":" + Poco::NumberFormatter::format(getSession()->getPort()) + "/" + request.getURI() + " ...");
//     std::clog << "Header:" << std::endl;
//     request.write(std::clog);
    
    std::ostream& ostr = getSession()->sendRequest(request);
    ostr << eventMessage;
    
    // receive answer ...
    Poco::Net::HTTPResponse response;
    getSession()->receiveResponse(response);
    Log::instance()->event().debug("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
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
    _actions.append(pAction->getName(), pAction);
    pAction->setService(this);
}


void
Service::addStateVar(StateVar* pStateVar)
{
//     std::clog << "Service::addStateVar() name: " << pStateVar->getName() << " is evented: " << pStateVar->getSendEvents() << std::endl;
    
    _stateVars.append(pStateVar->getName(), pStateVar);
    if(pStateVar->getSendEvents()) {
        _eventedStateVars.append(pStateVar->getName(), pStateVar);
    }
}


void
Service::initClient()
{
    Poco::URI baseUri(_pDevice->getDeviceRoot()->getDescriptionUri());
    _pControlRequestSession = new Poco::Net::HTTPClientSession(Poco::Net::SocketAddress(baseUri.getAuthority()));
    // TODO: subscribe to services
    // -> generate eventHandlerURIs: device uuid + service type
    // TODO: setup event message handler
    // -> this should go in DeviceRoot::initClient()?
    // -> or in Controller::init()?
    // _pDevice->getDeviceRoot()->registerEventHandler(...);
}

// FIXME: when starting 15 servers and killing them with killall at once
//        ommc still shows a couple of them though they are dead


void
Service::actionNetworkActivity(bool begin)
{
    Controller* pController = _pDevice->getDeviceRoot()->getController();
    if (pController) {
        UserInterface* pUserInterface = pController->getUserInterface();
        if (pUserInterface) {
            if (begin) {
                pUserInterface->beginNetworkActivity();
            }
            else {
                pUserInterface->endNetworkActivity();
            }
        }
    }
}


void
Service::sendAction(Action* pAction)
{
    std::string actionMessage;
    ActionRequestWriter requestWriter;
    requestWriter.action(pAction);
    requestWriter.write(actionMessage);
    
    Poco::URI baseUri(_pDevice->getDeviceRoot()->getDescriptionUri());
    Poco::URI controlUri(baseUri);
    controlUri.resolve(_controlPath);
    Poco::Net::HTTPRequest* request = new Poco::Net::HTTPRequest("POST", controlUri.getPath(), "HTTP/1.1");
    request->set("HOST", baseUri.getAuthority());
    request->setContentType("text/xml; charset=\"utf-8\"");
    request->set("SOAPACTION", "\"" + _serviceType + "#" + pAction->getName() + "\"");
    request->setContentLength(actionMessage.size());
    // set request body and send request
    Log::instance()->ctrl().debug("*** sending action \"" + pAction->getName() + "\" to " + baseUri.getAuthority() + request->getURI() + " ***");
//     std::clog << "Header:" << std::endl;
//     request->write(std::clog);
    actionNetworkActivity(true);
    try {
        std::ostream& ostr = _pControlRequestSession->sendRequest(*request);
        ostr << actionMessage;
        Log::instance()->ctrl().debug("action request sent:\n" + actionMessage);
    }
    catch(Poco::Net::ConnectionRefusedException) {
        actionNetworkActivity(false);
        Log::instance()->ctrl().error("sending of action request failed, connection refused");
        throw Poco::Exception("");
    }
    catch (...) {
        actionNetworkActivity(false);
        Log::instance()->ctrl().error("sending of action request failed for some reason");
        throw Poco::Exception("");
    }
    // receive answer ...
    Poco::Net::HTTPResponse response;
    try {
        std::istream& rs = _pControlRequestSession->receiveResponse(response);
        Log::instance()->ctrl().debug("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::string responseBody;
        Poco::StreamCopier::copyToString(rs, responseBody);
        Log::instance()->ctrl().debug("action response received:\n" + responseBody);
        ActionResponseReader responseReader(responseBody, pAction);
        responseReader.action();
        actionNetworkActivity(false);
    }
    catch (Poco::Net::NoMessageException) {
        actionNetworkActivity(false);
        Log::instance()->ctrl().error("no response to action request");
        throw Poco::Exception("");
    }
    catch (...) {
        actionNetworkActivity(false);
        Log::instance()->ctrl().error("no response to action request received for some reason");
        throw Poco::Exception("");
    }
    Log::instance()->ctrl().debug("*** action \"" + pAction->getName() + "\" completed ***");
}


void
Service::registerSubscription(Subscription* subscription)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serviceLock);
    // TODO: only register a Subscription once from one distinct Controller
    //       note that Subscription has a new SID
    std::string sid = subscription->getUuid();
    Log::instance()->event().debug("register subscription with SID: " + sid);
    _eventSubscriptions.append(sid, subscription);
}


void
Service::unregisterSubscription(Subscription* subscription)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_serviceLock);
    std::string sid = subscription->getUuid();
    _eventSubscriptions.remove(sid);
    Log::instance()->event().debug("unregister subscription with SID: " + sid);
    delete subscription;
}


void
Service::sendEventMessage(StateVar& stateVar)
{
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
    std::string eventMessage;
    EventMessageWriter messageWriter;
    for (StateVarIterator i = beginEventedStateVar(); i != endEventedStateVar(); ++i) {
        messageWriter.stateVar(**i);
    }
    messageWriter.write(eventMessage);
    pSubscription->sendEventMessage(eventMessage);
    Log::instance()->event().debug("sending initial event message:\n" + eventMessage);
}


void
Service::setDescriptionRequestHandler()
{
    _pDescriptionRequestHandler = new DescriptionRequestHandler(_pDescription);
}


Action*
Action::clone()
{
    Action* res = new Action();
    res->_actionName = _actionName;
    res->_pService = _pService;
    // make a deep copy of the Arguments
    for (Container<Argument>::Iterator i = _arguments.begin(); i != _arguments.end(); ++i) {
        res->appendArgument(new Argument(**i));
    }
    return res;
}


void
Action::appendArgument(Argument* pArgument)
{
    pArgument->setAction(this);
    _arguments.append(pArgument->getName(), pArgument);
    if (pArgument->getDirection() == "in") {
        _inArguments.append(pArgument->getName(), pArgument);
    }
    else {
        _outArguments.append(pArgument->getName(), pArgument);
    }
}


DeviceRequestHandlerFactory::DeviceRequestHandlerFactory(HttpSocket* pHttpSocket):
_pHttpSocket(pHttpSocket)
{
    registerRequestHandler(std::string(""), new RequestNotFoundRequestHandler());
}


Poco::Net::HTTPRequestHandler*
DeviceRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    Log::instance()->http().debug("dispatching request: " + request.getURI());
    Poco::Net::HTTPRequestHandler* res;
    std::map<std::string,UpnpRequestHandler*>::iterator i = _requestHandlerMap.find(request.getURI());
    if (i != _requestHandlerMap.end()) {
        return i->second->create();
    }
    else {
        return _requestHandlerMap[""]->create();
    }
}


void
DeviceRequestHandlerFactory::registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler)
{
    _requestHandlerMap[Uri] = requestHandler;
}


RequestNotFoundRequestHandler*
RequestNotFoundRequestHandler::create()
{
    return new RequestNotFoundRequestHandler();
}


void
RequestNotFoundRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->http().debug("unknown request " + request.getURI() + " HTTP 404 - not found error");
    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
}


// DescriptionRequestHandler::DescriptionRequestHandler(std::string& description):
DescriptionRequestHandler::DescriptionRequestHandler(std::string* pDescription):
_pDescription(pDescription)
{
}


DescriptionRequestHandler*
DescriptionRequestHandler::create()
{
    // TODO: can we somehow avoid to make a copy of the RequestHandler on each request?
//     return new DescriptionRequestHandler(_description);
    return new DescriptionRequestHandler(_pDescription);
}


void
DescriptionRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::ostringstream requestHeader;
    request.write(requestHeader);
    Log::instance()->desc().debug("description request: " + request.getURI());
    Log::instance()->desc().debug("description request header: \n" + requestHeader.str());
    if (request.has("Accept-Language")) {
        std::string lang = request.get("Accept-Language");
        // TODO: set proper lang in description response
        response.set("Content-Language", lang);
//         response.set("CONTENT-LANGUAGE", lang);
    }
    response.setContentLength(_pDescription->size());
    response.setContentType("text/xml");
    response.setDate(Poco::Timestamp());
//     response.setVersion("HTTP/1.1");
//     response.set("CONTENT-LENGTH", Poco::NumberFormatter::format(_pDescription->size()));
//     response.set("CONTENT-TYPE", "text/xml; charset=UTF-8");
//     response.set("DATE", Poco::DateTimeFormatter::format(Poco::Timestamp(), Poco::DateTimeFormat::HTTP_FORMAT));
//     response.set("CONNECTION", "close");
//     response.set("SERVER", "Linux/2.6.27-17-generic, UPnP/1.0, Omm/0.1.0");
//     response.set("LAST-MODIFIED", "Sun, 14 Mar 2010 11:30:22 GMT");
    std::ostringstream responseHeader;
    response.write(responseHeader);
    Log::instance()->desc().debug("description response header: \n" + responseHeader.str());
    response.sendBuffer(_pDescription->c_str(), _pDescription->size());
    if (response.sent()) {
        Log::instance()->desc().debug("description (header) successfully sent");
    }
    
    /*
    mediatomb needs this for SAMSUNGs:
    <custom-http-headers> 
    <!-- Samsung needs it --> 
    <add header="transferMode.dlna.org: Streaming"/> 
    <add header="contentFeatures.dlna.org: DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=01500000000000000000000000000000"/> 
    </custom-http-headers> 
    */
}


ControlRequestHandler::ControlRequestHandler(Service* service):
_pService(service)
{
}


ControlRequestHandler*
ControlRequestHandler::create()
{
    return new ControlRequestHandler(_pService);
}


void
ControlRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->ctrl().debug("*** action request: " + request.getURI() + " ***");
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
    Log::instance()->ctrl().debug("action received: \"" + actionName + "\" (service type " + serviceType + ")");
    
//     std::clog << "ControlRequestHandler for ServiceType: " << _pService->getServiceType() << std::endl;
    // TODO: make getAction() robust against wrong action names
    Action* pAction = _pService->getAction(actionName);
//     std::clog << "getAction(): " << pAction->getName() << std::endl;
    pAction = pAction->clone();
//     std::clog << "cloned Action(): " << pAction->getName() << std::endl;
    // TODO: introduce ActionRequestReader::write(Action*) to get rid of confusing pAction stuff
    ActionRequestReader requestReader(requestBody, pAction);
    pAction = requestReader.action();
    // the corresponding Service should register as a Notification Handler
    _pService->getDevice()->getDeviceRoot()->postAction(pAction);
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
    Log::instance()->ctrl().debug("action response sent:\n" + responseBody);
    Log::instance()->ctrl().debug("*** action request completed: " + request.getURI() + " ***");
}


EventRequestHandler*
EventRequestHandler::create()
{
    return new EventRequestHandler(_pService);
}


void
EventRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->event().debug("event request from: " + request.getHost() + " (" + request.get("CALLBACK") + ")");
//     std::clog << "handle event request: " << request.getMethod() << std::endl;
//     std::clog << "HOST: " << request.getHost() << std::endl;
//     std::clog << "CALLBACK: " << request.get("CALLBACK") << std::endl;
//     std::clog << "NT: " << request.get("NT") << std::endl;
//     std::clog << "TIMEOUT: " << request.get("TIMEOUT") << std::endl;
    
    std::string sid;
    
    if (request.getMethod() == "SUBSCRIBE") {
//         Poco::Timestamp t;
        if (request.has("SID")) {
            sid = request.get("SID");
            // renew subscription
            _pService->getSubscription(sid)->renew(1800);
        }
        else {
            std::string callback = request.get("CALLBACK");
            callback = callback.substr(1, callback.size() - 2);
            Subscription* pSubscription = new Subscription(callback);
            _pService->sendInitialEventMessage(pSubscription);
            _pService->registerSubscription(pSubscription);
        }
//         response.set("DATE", Poco::DateTimeFormatter::format(t, Poco::DateTimeFormat::HTTP_FORMAT));
        response.setDate(Poco::Timestamp());
        response.set("SERVER", 
                     Poco::Environment::osName() + "/"
                     + Poco::Environment::osVersion() + ", "
                    + "UPnP/" + UPNP_VERSION + ", "
                    + "Omm/" + OMM_VERSION);
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
        _pService->unregisterSubscription(_pService->getSubscription(sid));
    }
}


IconRequestHandler*
IconRequestHandler::create()
{
    return new IconRequestHandler(_pIcon);
}


void
IconRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->upnp().debug("icon request from: " + request.getHost());
    
    response.sendBuffer(_pIcon->_buffer.data(), _pIcon->_buffer.size());
}


HttpSocket::HttpSocket()
{
}


HttpSocket::~HttpSocket()
{
    delete _pHttpServer;
    // FIXME: segfault on deleting _pDeviceRequestHandlerFactory
    //delete _pDeviceRequestHandlerFactory;
}


void
HttpSocket::init()
{
    _pDeviceRequestHandlerFactory = new DeviceRequestHandlerFactory(this);
    Poco::Net::ServerSocket socket(0);
    _httpServerPort = socket.address().port();
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    _pHttpServer = new Poco::Net::HTTPServer(_pDeviceRequestHandlerFactory, socket, pParams);
}


void
HttpSocket::startServer()
{
    _pHttpServer->start();
    Log::instance()->http().information("HTTP server started on port: " + Poco::NumberFormatter::format(_httpServerPort));
}


void
HttpSocket::stopServer()
{
    _pHttpServer->stop();
    Log::instance()->http().information("HTTP server stopped on port: " + Poco::NumberFormatter::format(_httpServerPort));
}


std::string
HttpSocket::getServerUri()
{
    Poco::Net::IPAddress validIpAddress = Sys::NetworkInterfaceManager::instance()->getValidIpAddress();
    std::string validIpAddressString = validIpAddress.toString();
    if (validIpAddress.family() == Poco::Net::IPAddress::IPv6) {
        validIpAddressString = "[" + validIpAddressString + "]";
    }
    return "http://" + validIpAddressString + ":" + Poco::NumberFormatter::format(_httpServerPort) + "/";
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
//     std::clog << "Device::addService(): " << pService->getServiceType() << std::endl;
    _services.append(pService->getServiceType(), pService);
    _pDeviceRoot->addServiceType(pService->getServiceType(), pService);
    pService->setDevice(this);
}


void
Device::setRandomUuid()
{
    Poco::UUIDGenerator uuidGenerator;
    _uuid = uuidGenerator.createRandom().toString();
}


void
Device::addIcon(Icon* pIcon)
{
    _iconList.push_back(pIcon);
    pIcon->_requestUri = "/DeviceIcon" + Poco::NumberFormatter::format(_iconList.size());
}


DeviceRoot::DeviceRoot() :
_pController(0)
{
}


DeviceRoot::~DeviceRoot()
{
    stopSsdp();
    stopHttp();
    // TODO: free all Devices, Services, Actions, ...
//     delete _descriptionRequestHandler;
}


Service*
DeviceRoot::getServiceType(const std::string& serviceType)
{
    std::map<std::string,Service*>::iterator i = _serviceTypes.find(serviceType);
    if (i == _serviceTypes.end()) {
        Log::instance()->upnp().error("unknown service type: " + serviceType);
    }
    return _serviceTypes[serviceType];
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
    Log::instance()->upnp().debug("init state vars of service: " + serviceType);
    _pDeviceRootImplAdapter->initStateVars(serviceType, pThis);
}


void
DeviceRoot::initController()
{
}


void
DeviceRoot::initDevice()
{
    _ssdpSocket.init();
    _httpSocket.init();
    setDescriptionUri();
    
    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        Log::instance()->upnp().debug("init device root: setting random uuid for device");
        (*d)->setRandomUuid();
        // FIXME: this should override a base uri, if already present in the device description
//         (*d)->addProperty("URLBase", _httpSocket.getServerUri());
        
        for(Device::ServiceIterator s = (*d)->beginService(); s != (*d)->endService(); ++s) {
            Service* ps = *s;
            initStateVars(ps->getServiceType(), ps);
            ps->setDescriptionPath("/" + ps->getServiceId() + "/Description.xml");
            ps->setControlPath("/" + ps->getServiceId() + "/Control");
            ps->setEventPath("/" + ps->getServiceId() + "/EventSubscription");
        }
    }
    
    DeviceDescriptionWriter descriptionWriter;
    descriptionWriter.deviceRoot(*this);
    _pDeviceDescription = descriptionWriter.write();
}


void
DeviceRoot::registerActionHandler(const Poco::AbstractObserver& observer)
{
    _httpSocket._notificationCenter.addObserver(observer);
}


void
DeviceRoot::registerHttpRequestHandler(std::string path, UpnpRequestHandler* requestHandler) 
{
    _httpSocket._pDeviceRequestHandlerFactory->registerRequestHandler(path, requestHandler);
}


void
DeviceRoot::writeSsdpMessages()
{
    _ssdpNotifyAliveMessages.clear();
    _ssdpNotifyByebyeMessages.clear();
    setDescriptionUri();
    
    SsdpNotifyAliveWriter aliveWriter(_ssdpNotifyAliveMessages);
    SsdpNotifyByebyeWriter byebyeWriter(_ssdpNotifyByebyeMessages);
    aliveWriter.deviceRoot(*this);
    byebyeWriter.deviceRoot(*this);

    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        Device& device = **d;
        aliveWriter.device(device);
        byebyeWriter.device(device);
        for(Device::ServiceIterator s = device.beginService(); s != device.endService(); ++s) {
            Service* ps = *s;

            aliveWriter.service(*ps);
            byebyeWriter.service(*ps);
        }
    }
}


void
DeviceRoot::sendSsdpAliveMessages()
{
    // 1. wait random intervall of less than 100msec when sending message set first time
    // 2. send out all message sets two times (max three times according to specs, should be configurable).
    _ssdpNotifyAliveMessages.send(_ssdpSocket, 2, 100, false);
}


void
DeviceRoot::sendSsdpByebyeMessages()
{
    _ssdpNotifyByebyeMessages.send(_ssdpSocket, 2, 0, false);
}


void
DeviceRoot::startSsdp()
{
    Log::instance()->ssdp().information("starting SSDP ...");
    writeSsdpMessages();
    
    _ssdpSocket.setObserver(Poco::Observer<DeviceRoot, SsdpMessage>(*this, &DeviceRoot::handleSsdpMessage));
    _ssdpSocket.setupSockets();
    _ssdpSocket.start();
    writeSsdpMessages();
    sendSsdpAliveMessages();

    Sys::NetworkInterfaceManager::instance()->registerInterfaceChangeHandler
        (Poco::Observer<DeviceRoot,Sys::NetworkInterfaceNotification>(*this, &DeviceRoot::handleNetworkInterfaceChangedNotification));
    Log::instance()->upnp().debug("device root network interface manager installed");
    
    // 4. resend advertisements in random intervals of max half the expiraton time (CACHE-CONTROL header)
    _ssdpNotifyAliveMessages.startSendContinuous(_ssdpSocket);
    Log::instance()->ssdp().information("SSDP started.");
}


void
DeviceRoot::stopSsdp()
{
    Log::instance()->ssdp().information("stopping SSDP ...");
    _ssdpNotifyAliveMessages.stopSendContinuous();
    sendSsdpByebyeMessages();
    Log::instance()->ssdp().information("SSDP stopped.");
}


void
DeviceRoot::startHttp()
{
    Log::instance()->http().information("starting HTTP ...");
//    Log::instance()->http().debug("device description: " + *_pDeviceDescription);
    _descriptionRequestHandler = new DescriptionRequestHandler(_pDeviceDescription);
//    Log::instance()->http().debug("description URI: " + _descriptionUri);
    Poco::URI descriptionUri;
    try {
        descriptionUri = _descriptionUri;
    }
    catch(Poco::Exception& e) {
        Log::instance()->http().error("could not parse description URI: " + e.displayText());
    }
    registerHttpRequestHandler(descriptionUri.getPath(), _descriptionRequestHandler);

    for(DeviceIterator d = beginDevice(); d != endDevice(); ++d) {
        for(Device::IconIterator i = (*d)->beginIcon(); i != (*d)->endIcon(); ++i) {
            registerHttpRequestHandler((*i)->_requestUri, new IconRequestHandler(*i));
        }
        for(Device::ServiceIterator s = (*d)->beginService(); s != (*d)->endService(); ++s) {
            Service* ps = *s;
            // TODO: to be totally correct, all relative URIs should be resolved to base URI (=description uri)
            registerHttpRequestHandler(ps->getDescriptionPath(), new DescriptionRequestHandler(ps->getDescription()));
            registerHttpRequestHandler(ps->getControlPath(), new ControlRequestHandler(ps));
            registerHttpRequestHandler(ps->getEventPath(), new EventRequestHandler(ps));
        }
    }
    Log::instance()->http().information("initialized message sets, service request handlers, and state variables.");
    
    _httpSocket.startServer();
    Log::instance()->http().information("HTTP started.");
}


void
DeviceRoot::stopHttp()
{
    Log::instance()->http().information("stopping HTTP ...");
    _httpSocket.stopServer();
    Log::instance()->http().information("HTTP stopped.");
}


void 
DeviceRoot::sendMessage(SsdpMessage& message, const Poco::Net::SocketAddress& receiver)
{
    _ssdpSocket.sendMessage(message, receiver);
}


void
DeviceRoot::handleSsdpMessage(SsdpMessage* pMessage)
{
    if (pMessage->getRequestMethod() == SsdpMessage::REQUEST_SEARCH) {
        SsdpMessage m;
        // TODO: use a skeleton to create response message
        m.setRequestMethod(SsdpMessage::REQUEST_RESPONSE);
        m.setCacheControl();
        m.setDate();
        m.setHttpExtensionConfirmed();
        m.setLocation(_descriptionUri);
        m.setServer("Omm/" + OMM_VERSION);
        // ST field in response depends on ST field in M-SEARCH
        m.setSearchTarget("upnp:rootdevice");
        // same as USN in NOTIFY message
        m.setUniqueServiceName("uuid:" + _pRootDevice->getUuid() + "::upnp:rootdevice");
        
        // TODO: react on ST field (search target)
        // TODO: react on MX field (seconds to delay response)
        //       -> create an SsdpMessageSet and send it out delayed
        // TODO: fill in the correct value for CacheControl
        //       -> _ssdpNotifyAliveMessages._sendTimer
        //       -> need to know the elapsed time ... (though initial timer val isn't so wrong)
        
        _ssdpSocket.sendMessage(m, pMessage->getSender());
    }
}


void
DeviceRoot::handleNetworkInterfaceChange(const std::string& interfaceName, bool added)
{
    Log::instance()->upnp().debug("device root adds network interface: " + interfaceName);
    writeSsdpMessages();
    if (added) {
        _ssdpSocket.addInterface(interfaceName);
        // TODO: send alive messages only once at startup of server
        // (now triggered by loopback device and real network device)
        sendSsdpAliveMessages();
    }
    else {
        _ssdpSocket.removeInterface(interfaceName);
    }
}


void
DeviceRoot::handleNetworkInterfaceChangedNotification(Sys::NetworkInterfaceNotification* pNotification)
{
    Log::instance()->upnp().debug("device root receives network interface change notification");
    handleNetworkInterfaceChange(pNotification->_interfaceName, pNotification->_added);
}


void
DeviceRoot::setDescriptionUri()
{
    _descriptionUri = _httpSocket.getServerUri() + "Description.xml";

}


DeviceRootImplAdapter::DeviceRootImplAdapter()
{
    // register the great action dispatcher
};


DeviceRootImplAdapter::~DeviceRootImplAdapter()
{
    delete _pDeviceRoot;
}


void
DeviceRootImplAdapter::start()
{
    _pDeviceRoot->registerActionHandler(Poco::Observer<DeviceRootImplAdapter, Action>(*this, &DeviceRootImplAdapter::actionHandler));
    
//     _pDeviceRoot->print();
    _pDeviceRoot->initDevice();
//     _pDeviceRoot->print();
    _pDeviceRoot->startHttp();
    _pDeviceRoot->startSsdp();
}


void
DeviceRootImplAdapter::stop()
{
    _pDeviceRoot->stopSsdp();
    _pDeviceRoot->stopHttp();
}


void
DeviceRootImplAdapter::setUuid(std::string uuid, int deviceNumber)
{
    _pDeviceRoot->_devices.get(deviceNumber).setUuid(uuid);
}


void
DeviceRootImplAdapter::setRandomUuid(int deviceNumber)
{
    _pDeviceRoot->_devices.get(deviceNumber).setRandomUuid();
}


void
DeviceRootImplAdapter::setFriendlyName(const std::string& friendlyName, int deviceNumber)
{
    _pDeviceRoot->_devices.get(deviceNumber).setProperty("friendlyName", friendlyName);
}


void
DeviceRootImplAdapter::addIcon(Icon* pIcon, int deviceNumber)
{
    _pDeviceRoot->_devices.get(deviceNumber).addIcon(pIcon);
}


SsdpMessageSet::SsdpMessageSet()
{
    _randomTimeGenerator.seed();
}


SsdpMessageSet::~SsdpMessageSet()
{
    if (_continuous) {
        _sendTimer.stop();
    }
/* // This causes a segfault:
   for (std::vector<SsdpMessage*>::iterator i = _ssdpMessages.begin(); i != _ssdpMessages.end(); ++i) {
        delete *i;
    }*/
}


void
SsdpMessageSet::clear()
{
    _ssdpMessages.clear();
}


void
SsdpMessageSet::addMessage(SsdpMessage& message)
{
    _ssdpMessages.push_back(&message);
}


void
SsdpMessageSet::send(SsdpSocket& socket, int repeat, long delay, bool continuous)
{
//     Poco::ScopedLock<Poco::FastMutex> lock(_sendLock);
    // TODO: check if continuous Timer is already running and return
//     if (_sendTimer) {
//         return;
//     }
    _socket = &socket;
    _repeat = repeat;
    _delay = delay;
    _continuous = continuous;
    if (_delay > 0) {
        _sendTimer.setStartInterval(_randomTimeGenerator.next(_delay));
    }
    if (_continuous) {
        // start asynchronously
        _sendTimer.start(Poco::TimerCallback<SsdpMessageSet>(*this, &SsdpMessageSet::onTimer));
    }
    else {
        // start synchronously
        onTimer(_sendTimer);
    }
}


void
SsdpMessageSet::startSendContinuous(SsdpSocket& socket)
{
    send(socket, 2, SSDP_CACHE_DURATION * 1000 / 2, true);
}


void
SsdpMessageSet::stopSendContinuous()
{
    _sendTimer.stop();
}


void
SsdpMessageSet::onTimer(Poco::Timer& timer)
{
    int r = _repeat;
    while (r--) {
        Log::instance()->ssdp().debug("#message sets left to send: " + Poco::NumberFormatter::format(r+1));
        
        for (std::vector<SsdpMessage*>::const_iterator i = _ssdpMessages.begin(); i != _ssdpMessages.end(); ++i) {
            _socket->sendMessage(**i);
        }
    }
    if (_continuous) {
        Log::instance()->ssdp().debug("restarting timer");
        timer.restart(_randomTimeGenerator.next(_delay));
    }
}


Controller::Controller()
{
}


Controller::~Controller()
{
}


void
Controller::handleNetworkInterfaceChangedNotification(Sys::NetworkInterfaceNotification* pNotification)
{
    Log::instance()->upnp().debug("controller receives network interface change notification");
    
    if (pNotification->_added) {
        _ssdpSocket.addInterface(pNotification->_interfaceName);
    }
    else {
        _ssdpSocket.removeInterface(pNotification->_interfaceName);
        // TODO: unregister subscriptions for devices on this interface
    }
    update();
}


void
Controller::start()
{
    Log::instance()->upnp().debug("starting controller ...");

    _ssdpSocket.init();
    _ssdpSocket.setObserver(Poco::Observer<Controller, SsdpMessage>(*this, &Controller::handleSsdpMessage));
    _ssdpSocket.setupSockets();
    _ssdpSocket.start();
    sendMSearch();

    Sys::NetworkInterfaceManager::instance()->registerInterfaceChangeHandler
        (Poco::Observer<Controller,Sys::NetworkInterfaceNotification>(*this, &Controller::handleNetworkInterfaceChangedNotification));    
    
    Log::instance()->upnp().debug("controller started");
}


void
Controller::setUserInterface(UserInterface* pUserInterface)
{
    _pUserInterface = pUserInterface;
}


UserInterface*
Controller::getUserInterface()
{
    return _pUserInterface;
}


void
Controller::sendMSearch()
{
    Log::instance()->upnp().debug("controller sends MSearch");
    SsdpMessage m;
    m.setRequestMethod(SsdpMessage::REQUEST_SEARCH);
    m.setHost();
    m.setHttpExtensionNamespace();
    m.setMaximumWaitTime();
//     m.setSearchTarget("ssdp:all");
    m.setSearchTarget("upnp:rootdevice");

    // FIXME: network exception in controller when sending MSearch after network device removal
    _ssdpSocket.sendMessage(m);
}


void
Controller::discoverDevice(const std::string& location)
{
    Log::instance()->upnp().debug("controller discovers device location: " + location);
    
    UriDescriptionReader descriptionReader;
    DeviceRoot* deviceRoot = descriptionReader.deviceRoot(location);
    deviceRoot->setDescriptionUri(location);
    addDevice(deviceRoot);
}


void
Controller::handleSsdpMessage(SsdpMessage* pMessage)
{
    // we load all device descriptions, regardless of service types contained in the device
    
    // get UUID from USN
    std::string usn = pMessage->getUniqueServiceName();
    std::string::size_type left = usn.find(":") + 1;
    std::string uuid = usn.substr(left, usn.find("::") - left);
    Log::instance()->ssdp().debug("controller received message:\n" + pMessage->toString());
    
    switch(pMessage->getRequestMethod()) {
    case SsdpMessage::REQUEST_NOTIFY:
        switch(pMessage->getNotificationSubtype()) {
        case SsdpMessage::SUBTYPE_ALIVE:
//             Log::instance()->ssdp().debug("identified alive message");
            if (pMessage->getNotificationType() == "upnp:rootdevice" && !_devices.contains(uuid)) {
                discoverDevice(pMessage->getLocation());
            }
            break;
        case SsdpMessage::SUBTYPE_BYEBYE:
//             Log::instance()->ssdp().debug("identified byebye message");
            if (pMessage->getNotificationType() == "upnp:rootdevice") {
                removeDevice(uuid);
            }
            break;
        }
    break;
    case SsdpMessage::REQUEST_RESPONSE:
//         Log::instance()->ssdp().debug("identified m-search response");
        if (!_devices.contains(uuid)) {
            discoverDevice(pMessage->getLocation());
        }
        break;
    }
}


void
Controller::addDevice(DeviceRoot* pDeviceRoot)
{
    // TODO: handle "alive refreshments"
    std::string uuid = pDeviceRoot->getRootDevice()->getUuid();
    if (!_devices.contains(uuid)) {
        Log::instance()->upnp().debug("controller adds device: " + uuid);
        pDeviceRoot->_pController = this;
        _pUserInterface->beginAddDevice(_devices.position(uuid));
        _devices.append(uuid, pDeviceRoot);
        _pUserInterface->endAddDevice(_devices.position(uuid));
        deviceAdded(pDeviceRoot);
    }
}


void
Controller::removeDevice(const std::string& uuid)
{
    if (_devices.contains(uuid)) {
        Log::instance()->upnp().debug("controller removes device: " + uuid);
        DeviceRoot* pDeviceRoot = &_devices.get(uuid);
        deviceRemoved(pDeviceRoot);
        _pUserInterface->beginRemoveDevice(_devices.position(uuid));
        _devices.remove(uuid);
        _pUserInterface->endRemoveDevice(_devices.position(uuid));
        pDeviceRoot->_pController = 0;
    }
}


void
Controller::update()
{
    // TODO: do a more carefull controller update and don't remove servers that are still active.
    for (Container<DeviceRoot>::KeyIterator it = _devices.beginKey(); it != _devices.endKey(); ++it) {
        removeDevice((*it).first);
    }
    sendMSearch();
}


void
ControllerImplAdapter::init()
{
    for (Device::ServiceIterator i = _pDevice->beginService(); i != _pDevice->endService(); ++i) {
        (*i)->initClient();
    }
}


Urn::Urn(const std::string& urn) :
_urn(urn)
{
    std::vector<std::string> vec;
    std::string::size_type left = 0;
    std::string::size_type right = 0;
    do 
    {
        left = _urn.find(':', right);
        right = _urn.find(':', ++left);
        vec.push_back(_urn.substr(left, right - left));
    } while (right != std::string::npos);
    _domainName = vec[0];
    _type = vec[1];
    _typeName = vec[2];
    _version = vec[3];
}


void
SsdpNotifyAliveWriter::deviceRoot(const DeviceRoot& pDeviceRoot)
{
    // root device first message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setLocation(pDeviceRoot.getDescriptionUri());    // location of UPnP description of the root device
    m->setNotificationType("upnp:rootdevice");  // once for root device
    m->setUniqueServiceName("uuid:" + pDeviceRoot.getRootDevice()->getUuid() + "::upnp:rootdevice");
    _res->addMessage(*m);
}


void
SsdpNotifyAliveWriter::device(const Device& pDevice)
{
    // device first message (root device second message)
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setLocation(pDevice.getDeviceRoot()->getDescriptionUri());    // location of UPnP description of the root device
    m->setNotificationType("uuid:" + pDevice.getUuid());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid());
    _res->addMessage(*m);
    // device second message (root device third message)
    m->setNotificationType(pDevice.getDeviceType());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid() + "::" + pDevice.getDeviceType());
    _res->addMessage(*m);
}


void
SsdpNotifyAliveWriter::service(const Service& pService)
{
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_ALIVE);
    m->setLocation(pService.getDevice()->getDeviceRoot()->getDescriptionUri());    // location of UPnP description of the root device
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    _res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::deviceRoot(const DeviceRoot& pDeviceRoot)
{
    // root device first message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType("upnp:rootdevice");  // once for root device
    m->setUniqueServiceName("uuid:" + pDeviceRoot.getRootDevice()->getUuid() + "::upnp:rootdevice");
    _res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::device(const Device& pDevice)
{
    // device first message (root device second message)
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType("uuid:" + pDevice.getUuid());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid());
    _res->addMessage(*m);
    // device second message (root device third message)
    m->setNotificationType(pDevice.getDeviceType());
    m->setUniqueServiceName("uuid:" + pDevice.getUuid() + "::" + pDevice.getDeviceType());
    _res->addMessage(*m);
}


void
SsdpNotifyByebyeWriter::service(const Service& pService)
{
    // service first (and only) message
    SsdpMessage* m = new SsdpMessage(SsdpMessage::REQUEST_NOTIFY_BYEBYE);
    m->setNotificationType(pService.getServiceType());
    m->setUniqueServiceName("uuid:" + pService.getDevice()->getUuid() + "::" +  pService.getServiceType());
    _res->addMessage(*m);
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
            setServer("Omm/" + OMM_VERSION);
            setCacheControl();
        }
    
    switch (requestMethod) {
    case REQUEST_NOTIFY_ALIVE:
        _requestMethod = REQUEST_NOTIFY;
        setNotificationSubtype(SsdpMessage::SUBTYPE_ALIVE);          // alive message
        break;
    case REQUEST_NOTIFY_BYEBYE:
        _requestMethod = REQUEST_NOTIFY;
        setNotificationSubtype(SsdpMessage::SUBTYPE_BYEBYE);         // byebye message
        break;
    }
}


SsdpMessage::SsdpMessage(const std::string& buf, const Poco::Net::SocketAddress& sender)
{
    // FIXME: this shouldn't be executed on every SsdpMessage ctor
    initMessageMap();
    
    for (std::map<TRequestMethod,std::string>::iterator i = _messageMap.begin(); i != _messageMap.end(); i++) {
        _messageConstMap[(*i).second] = (*i).first;
    }
    
    // FIXME: what about Poco::Message header for retrieving the request method?
    std::istringstream is(buf);
    // get first line and check request method type
    std::string line;
    getline(is, line);
    Poco::trimInPlace(line);
    
    // TODO: must be case-insensitive map?!
    _requestMethod = _messageConstMap[line];
    _sender = sender;
    
    while(getline(is, line)) {
        std::string::size_type col = line.find(":");
        if (col != std::string::npos) {
            _messageHeader[line.substr(0, col)] = Poco::trim(line.substr(col + 1));
        }
    }
    
//     if (getNotificationSubtype() == SsdpMessage::SUBTYPE_ALIVE) {
//             _requestMethod = REQUEST_NOTIFY_ALIVE;
//     }
//     else if (getNotificationSubtype() == SsdpMessage::SUBTYPE_BYEBYE) {
//         _requestMethod = REQUEST_NOTIFY_BYEBYE;
//     }
//     try {
//         _messageHeader.read(is);
//     } catch (Poco::Net::MessageException) {
//         std::clog << "Error in sdpMessage::SsdpMessage(): malformed header" << std::endl;
//     }
    // FIXME: workaround for bug in Poco: empty values are mistreated
//     if (_messageHeader.has("EXT")) {
//         _messageHeader.set("EXT", "fix bug in poco");
//         _messageHeader.erase("EXT");
//     }
}


void
SsdpMessage::initMessageMap()
{
    _messageMap[REQUEST_NOTIFY]            = "NOTIFY * HTTP/1.1";
    _messageMap[REQUEST_NOTIFY_ALIVE]      = "dummy1";
    _messageMap[REQUEST_NOTIFY_BYEBYE]     = "dummy2";
    _messageMap[REQUEST_SEARCH]            = "M-SEARCH * HTTP/1.1";
    _messageMap[REQUEST_RESPONSE]          = "HTTP/1.1 200 OK";
    _messageMap[SUBTYPE_ALIVE]             = "ssdp:alive";
    _messageMap[SUBTYPE_BYEBYE]            = "ssdp:byebye";
    _messageMap[SSDP_ALL]                  = "ssdp:all";
    _messageMap[UPNP_ROOT_DEVICES]         = "upnp:rootdevice";
}


SsdpMessage::~SsdpMessage()
{
}


std::string 
SsdpMessage::toString()
{
    std::ostringstream os;
    
    os << _messageMap[_requestMethod] << "\r\n";
    for (std::map<std::string,std::string>::iterator i = _messageHeader.begin(); i != _messageHeader.end(); ++i) {
        os << (*i).first << ": " << (*i).second << "\r\n";
    }
//     _messageHeader.write(os);
    os << "\r\n";
    
    return os.str();
}


void
SsdpMessage::setRequestMethod(TRequestMethod requestMethod)
{
    _requestMethod = requestMethod;
//     if (_requestMethod == REQUEST_NOTIFY_ALIVE || _requestMethod == REQUEST_NOTIFY_BYEBYE) {
//         requestMethod = REQUEST_NOTIFY;
//     }
}


SsdpMessage::TRequestMethod
SsdpMessage::getRequestMethod()
{
    return _requestMethod;
}


void
SsdpMessage::setCacheControl(int duration)
{
//     _messageHeader.set("CACHE-CONTROL", "max-age = " + NumberFormatter::format(duration));
    _messageHeader["CACHE-CONTROL"] = "max-age = " + Poco::NumberFormatter::format(duration);
}


int
SsdpMessage::getCacheControl()
{
    try {
        std::string value = _messageHeader["CACHE-CONTROL"];
        return Poco::NumberParser::parse(value.substr(value.find('=')+1));
    }
    catch (Poco::NotFoundException) {
        Log::instance()->ssdp().error("missing CACHE-CONTROL in ssdp header");
    }
    catch (Poco::SyntaxException) {
        Log::instance()->ssdp().error("wrong number format of CACHE-CONTROL in ssdp header");
    }
    return SSDP_CACHE_DURATION;  // if no valid number is given, return minimum required value
}


void
SsdpMessage::setNotificationType(const std::string& searchTarget)
{
//     _messageHeader.set("NT", searchTarget);
    _messageHeader["NT"] =  searchTarget;
}


std::string
SsdpMessage::getNotificationType()
{
    return _messageHeader["NT"];
}


void
SsdpMessage::setNotificationSubtype(TRequestMethod notificationSubtype)
{
//     _messageHeader.set("NTS", _messageMap[notificationSubtype]);
    _messageHeader["NTS"] =  _messageMap[notificationSubtype];
    _notificationSubtype = notificationSubtype;
}


SsdpMessage::TRequestMethod
SsdpMessage::getNotificationSubtype()
{
    _notificationSubtype = _messageConstMap[_messageHeader["NTS"]];
    return _notificationSubtype;
}


void
SsdpMessage::setSearchTarget(const std::string& searchTarget)
{
//     _messageHeader.set("ST", searchTarget);
    _messageHeader["ST"] = searchTarget;
}


std::string
SsdpMessage::getSearchTarget()
{
    return _messageHeader["ST"];
}


void
SsdpMessage::setUniqueServiceName(const std::string& serviceName)
{
//     _messageHeader.set("USN", serviceName);
    _messageHeader["USN"] = serviceName;
}


std::string
SsdpMessage::getUniqueServiceName()
{
    return _messageHeader["USN"];
}


void
SsdpMessage::setLocation(const std::string& location)
{
//     _messageHeader.set("LOCATION", location.toString());
    _messageHeader["LOCATION"] = location;
}


std::string
SsdpMessage::getLocation()
{
    try {
        return _messageHeader["LOCATION"];
    }
    catch (Poco::NotFoundException) {
        Log::instance()->ssdp().error("LOCATION field not found");
        return "";
    }
}


void
SsdpMessage::setHost()
{
//     _messageHeader.set("HOST", SSDP_FULL_ADDRESS);
    _messageHeader["HOST"] = SSDP_FULL_ADDRESS;
}


void
SsdpMessage::setHttpExtensionNamespace()
{
//     _messageHeader.set("MAN", "\"ssdp:discover\"");
    _messageHeader["MAN"] = "\"ssdp:discover\"";
}


void
SsdpMessage::setHttpExtensionConfirmed()
{
//     _messageHeader.set("EXT", "");
    _messageHeader["EXT"] = "";
}


bool
SsdpMessage::getHttpExtensionConfirmed()
{
//     return _messageHeader.has("EXT");
    return true;
}


void
SsdpMessage::setServer(const std::string& productNameVersion)
{   
//     _messageHeader.set("SERVER", 
//                         Environment::osName() + "/"
//                         + Environment::osVersion() + ", "
//                         + "UPnP/" + UPNP_VERSION + ", "
//                         + productNameVersion);
    _messageHeader["SERVER"] = 
        Poco::Environment::osName() + "/"
        + Poco::Environment::osVersion() + ", "
        + "UPnP/" + UPNP_VERSION + ", "
        + productNameVersion;
}


std::string
SsdpMessage::getServerOperatingSystem()
{
    std::string value = _messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    return elements[0];
}


std::string
SsdpMessage::getServerUpnpVersion()
{
    std::string value = _messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    std::string upnpVersion = elements[1];
    return upnpVersion.substr(upnpVersion.find('/')+1);
}


std::string
SsdpMessage::getServerProductNameVersion()
{
    std::string value = _messageHeader["SERVER"];
    std::vector<std::string> elements;
    Poco::Net::MessageHeader::splitElements(value, elements);
    return elements[2];
}


void
SsdpMessage::setMaximumWaitTime(int waitTime)
{
//     _messageHeader.set("MX", NumberFormatter::format(waitTime));
    _messageHeader["MX"] = Poco::NumberFormatter::format(waitTime);
}


int
SsdpMessage::getMaximumWaitTime()
{
    try {
        return Poco::NumberParser::parse(_messageHeader["MX"]);
    }
    catch (Poco::NotFoundException) {
//         std::clog << "missing MX in SSDP header" << std::endl;
        Log::instance()->ssdp().error("missing MX in SSDP header");
        
    }
    catch (Poco::SyntaxException) {
//         std::clog << "wrong number format of MX in SSDP header" << std::endl;
        Log::instance()->ssdp().error("wrong number format of MX in SSDP header");
    }
    return SSDP_MIN_WAIT_TIME;  // if no valid number is given, return minimum required value
}


void
SsdpMessage::setDate()
{
    Poco::Timestamp t;
//     _messageHeader.set("DATE", DateTimeFormatter::format(t, DateTimeFormat::HTTP_FORMAT));
    _messageHeader["DATE"] = Poco::DateTimeFormatter::format(t, Poco::DateTimeFormat::HTTP_FORMAT);
}


Poco::DateTime
SsdpMessage::getDate()
{
    std::string value = _messageHeader["DATE"];
    int timeZoneDifferentail;
    try {
        return Poco::DateTimeParser::parse(Poco::DateTimeFormat::HTTP_FORMAT, value, timeZoneDifferentail);
    }
    catch (Poco::SyntaxException) {
//         std::clog << "wrong date format of DATE in SSDP header" << std::endl;
        Log::instance()->ssdp().error("wrong date format of DATE in SSDP header");
    }
}


Poco::Net::SocketAddress
SsdpMessage::getSender()
{
    return _sender;
}


} // namespace Omm
