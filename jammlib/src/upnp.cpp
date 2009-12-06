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

#include "upnp.h"

using namespace Jamm;

// const std::string SsdpMessage::REQUEST_NOTIFY_STR = "NOTIFY * HTTP/1.1";
// const std::string SsdpMessage::REQUEST_SEARCH_STR = "M-SEARCH * HTTP/1.1";
// const std::string SsdpMessage::REQUEST_RESPONSE_STR = "HTTP/1.1 200 OK";

const std::string SsdpSocket::SSDP_ADDRESS = "239.255.255.250";


SsdpMessage::SsdpMessage()
{
}


SsdpMessage::SsdpMessage(const std::string& buf)
{
    std::cout << "SsdpMessage() creating message: " << buf << std::endl;
    istringstream is(buf);
    // get first line and check request method type
    std::string line;
    getline(is, line);
    Poco::trimInPlace(line);
    std::cout << "SsdpMessage first line: " << line << std::endl;
/*    if (line == REQUEST_NOTIFY_STR) {
        m_requestMethod = REQUEST_NOTIFY;
    }
    else if (line == REQUEST_SEARCH_STR) {
        m_requestMethod = REQUEST_SEARCH;
    }
    else if (line == REQUEST_RESPONSE_STR) {
        m_requestMethod = REQUEST_SEARCH;
    }*/
    
    // loop until blank line
/*    do {
        getline(is, line);
        // for each line: get name and value of HTTP message
        
    } while(line != "");*/
    
    m_messageMap[REQUEST_NOTIFY]   = "NOTIFY * HTTP/1.1";
    m_messageMap[REQUEST_SEARCH]   = "M-SEARCH * HTTP/1.1";
    m_messageMap[REQUEST_RESPONSE] = "HTTP/1.1 200 OK";
    
    for (std::map<TRequestMethod,std::string>::iterator i = m_messageMap.begin(); i != m_messageMap.end(); i++) {
        m_messageConstMap[(*i).second] = (*i).first;
    }
    
    m_requestMethod = m_messageConstMap[line];
}


SsdpMessage::~SsdpMessage()
{
}


const std::string& 
// std::string
SsdpMessage::toString()
{
    std::cout << "SsdpMessage::toString()" << std::endl;
//     std::string res;
//     std::map<TRequestMethod,std::string>::iterator i = m_constMessageMap[m_requestMethod];
//     std::map<TRequestMethod,std::string>::iterator i;
//     i = m_messageMap.find(m_requestMethod);
/*    switch(m_requestMethod) {
    case REQUEST_NOTIFY: ;
        res = REQUEST_NOTIFY_STR;
        std::cout << "SsdpMessage::toString() REQUEST_NOTIFY" << std::endl;
        break;
    case REQUEST_SEARCH: ;
        res = REQUEST_SEARCH_STR;
        std::cout << "SsdpMessage::toString() REQUEST_SEARCH" << std::endl;
        break;
    case REQUEST_RESPONSE: ;
        res = REQUEST_RESPONSE_STR;
        std::cout << "SsdpMessage::toString() REQUEST_RESPONSE" << std::endl;
        break;
    }*/
    std::string res = m_messageMap[m_requestMethod];
    
    res += "\n";
    
    res += "\n";
    return res;
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
    std::cout << std::endl << "~SsdpSocket() cleaning up ..." << std::endl;
    m_reactor.removeEventHandler(m_ssdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
    m_ssdpSocket.leaveGroup(m_ssdpAddress);
    m_reactor.stop();
    m_listenerThread.join();
    delete [] m_pBuffer;
}


void
SsdpSocket::onReadable(const AutoPtr<ReadableNotification>& pNf)
{
    int n = m_ssdpSocket.receiveBytes(m_pBuffer, BUFFER_SIZE);
    if (n > 0) {
        std::cout << "SsdpSocket::onReadable() receives: " << m_pBuffer << std::endl;
//         SsdpMessage m;
        m_notificationCenter.postNotification(new SsdpMessage(std::string(m_pBuffer, n)));
    }
    
    
//     else
//         delete this;
//     std::cout << m_inputStream;
}


void
SsdpSocket::sendMessage(SsdpMessage& message)
{
    std::string welcomeMessage("HELLO WORLD, THIS IS JAMM ON UDP MULTICAST\r\n");
    int bytesSent = m_ssdpSocket.sendTo(welcomeMessage.c_str(), welcomeMessage.length(), SocketAddress(m_ssdpAddress, m_ssdpPort));
    std::cout << "message sent, number bytes: " << bytesSent << std::endl;
}


Device::Device()
{
}


Device::~Device()
{
}


ControlPoint::ControlPoint()
{
}


ControlPoint::~ControlPoint()
{
}
