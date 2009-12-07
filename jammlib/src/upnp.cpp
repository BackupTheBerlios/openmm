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

#include <sstream>
using std::istringstream;

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
    m_messageMap[REQUEST_NOTIFY]    = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_SEARCH]    = "M-SEARCH * HTTP/1.1";
    m_messageMap[REQUEST_RESPONSE]  = "HTTP/1.1 200 OK";
    m_messageMap[SUBTYPE_ALIVE]     = "ssdp:alive";
    m_messageMap[SUBTYPE_BYEBYE]    = "ssdp:byebye";
    m_messageMap[SSDP_ALL]          = "ssdp:all";
    m_messageMap[UPNP_ROOT_DEVICES] = "upnp:rootdevice";
}


SsdpMessage::SsdpMessage(const std::string& buf, const SocketAddress& sender)
{
//     SsdpMessage();
    // TODO: put this into a singleton or similar ...
    m_messageMap[REQUEST_NOTIFY]    = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_SEARCH]    = "M-SEARCH * HTTP/1.1";
    m_messageMap[REQUEST_RESPONSE]  = "HTTP/1.1 200 OK";
    m_messageMap[SUBTYPE_ALIVE]     = "ssdp:alive";
    m_messageMap[SUBTYPE_BYEBYE]    = "ssdp:byebye";
    m_messageMap[SSDP_ALL]          = "ssdp:all";
    m_messageMap[UPNP_ROOT_DEVICES] = "upnp:rootdevice";
    
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
    m_messageHeader.set("CACHE-CONTROL", "maxe-age = " + NumberFormatter::format(duration));
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
    // set the default interface by providing an empty NetworkInterface as argument
    // TODO: if not default interface, let interface be configurable
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
//     int n = m_ssdpSocket.receiveBytes(m_pBuffer, BUFFER_SIZE);
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
//     int bytesSent = m_ssdpSocket.sendTo(m.c_str(), m.length(), SocketAddress(m_ssdpAddress, m_ssdpPort));
//     std::cerr << "SsdpSocket::sendMessage() message sent, number bytes: " << bytesSent << std::endl;
}


Service::Service() :
m_vendorDomain("schemas-upnp-org:device"),  // if vendor is UPnP forum
m_serviceType("fooservice"),
m_serviceVersion("fooserviceversion")
{
}


Service::~Service()
{
}


Device::Device() :
m_descriptionUri("http://192.168.178.20:9191/description_uri"),
m_vendorDomain("schemas-upnp-org:device"),  // if vendor is UPnP forum
m_deviceType("foodevice"),
m_deviceVersion("foodeviceversion")
{
    UUIDGenerator uuidGen;
    m_uuid = uuidGen.createOne();
}


Device::~Device()
{
}


RootDevice::RootDevice() :
Device(),
m_ssdpSocket(Observer<RootDevice, SsdpMessage>(*this, &RootDevice::handleSsdpMessage))
{
    // TODO: send NOTIFY alive messages
    SsdpMessage m;
    m.setRequestMethod(SsdpMessage::REQUEST_NOTIFY);
    m.setHost();
    m.setServer("Jamm/0.0.3");
    m.setCacheControl();
    m.setLocation(m_descriptionUri);    // location of UPnP description of the root device
    m.setNotificationSubtype(SsdpMessage::SUBTYPE_ALIVE);         // alive message
    
    m.setNotificationType("upnp:rootdevice");  // once for root device
    m.setNotificationType("uuid:" + m_uuid.toString());  // once for every device
    m.setNotificationType("urn:" + m_vendorDomain + ":device:" + m_deviceType + ":" + m_deviceVersion);  // once for every device
    std::string serviceType = "fooservice";
    std::string serviceVersion = "fooserviceversion";
    m.setNotificationType("urn:" + m_vendorDomain + ":service:" + serviceType + ":" + serviceVersion);  // once for every service
    
    m.setUniqueServiceName("uuid:" + m_uuid.toString() + "::upnp:rootdevice");
    
    m_ssdpSocket.sendMessage(m);
}


RootDevice::~RootDevice()
{
    // TODO: send NOTIFY byebye messages
    SsdpMessage m;
    m.setRequestMethod(SsdpMessage::REQUEST_NOTIFY);
    m.setHost();
    m.setServer("Jamm/0.0.3");
    m.setNotificationSubtype(SsdpMessage::SUBTYPE_BYEBYE);         // byebye message
    
    m.setUniqueServiceName("uuid:" + m_uuid.toString() + "::upnp:rootdevice");
    
    m_ssdpSocket.sendMessage(m);
}


void
RootDevice::handleSsdpMessage(SsdpMessage* pNf)
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
        m.setUniqueServiceName("uuid:" + m_uuid.toString() + "::upnp:rootdevice");
        
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
