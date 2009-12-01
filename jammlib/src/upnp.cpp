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

#include "upnp.h"

using namespace Jamm;


const std::string SsdpSocket::SSDP_ADDRESS = "239.255.255.250";

SsdpSocket::SsdpSocket():
// bind socket to port 1900 and ANY_ADDRESS
// m_ssdpAddress("0.0.0.0", "1900"),
// m_ssdpAddress(IPAddress(), 1900),
m_ssdpAddress(SSDP_ADDRESS),
m_ssdpPort(SSDP_PORT),
// set socket in a non-exclusive state, thus allowing other process to bind to the same port
// i.e. set SO_REUSEADDR flag on socket
// m_ssdpSocket(m_ssdpAddress, true),
m_ssdpSocket(SocketAddress(m_ssdpAddress, m_ssdpPort), true),
// m_interface(NetworkInterface()),
// m_interface(NetworkInterface::forName("wlan0")),
// m_reactor(),
// m_listenerThread(Thread()),
m_pBuffer(new char[BUFFER_SIZE])
{
/*    NetworkInterface::NetworkInterfaceList nl = NetworkInterface::list();
    std::cout << "available network interfaces:" << std::endl;
    for (int i = 0; i < nl.size(); i++) {
        std::cout << " " << nl[i].name() << " (" << nl[i].address().toString() << ")" << std::endl;
    }
    std::cout << std::endl;*/
    // set the default interface by providing an empty NetworkInterface as argument
//     m_interface = NetworkInterface::forName("wlan0");
    // TODO: if not default interface, let interface be configurable
    m_ssdpSocket.setInterface(m_interface);
//     std::cout << "interface set to:" << std::endl << " " << m_interface.name() << " (" << m_interface.address().toString() << ")"<< std::endl;
//     std::cout << "with ip address: " << m_interface.address().toString() << std::endl;
//     std::cout << "broadcast address: " << m_interface.broadcastAddress().toString() << std::endl;
//     m_ssdpSocket.setBroadcast(true);
    m_ssdpSocket.setLoopback(true);
    m_ssdpSocket.setTimeToLive(4);  // TODO: let TTL be configurable
//     m_ssdpSocket.joinGroup(IPAddress("239.255.255.250"));
    m_ssdpSocket.joinGroup(m_ssdpAddress);
    m_reactor.addEventHandler(m_ssdpSocket, NObserver<SsdpSocket, ReadableNotification>(*this, &SsdpSocket::onReadable));
//     m_reactor.addEventHandler(_socket, NObserver<EchoServiceHandler, ShutdownNotification>(*this, &EchoServiceHandler::onShutdown));
    m_listenerThread.start(m_reactor);
    std::string welcomeMessage("HELLO WORLD, THIS IS JAMM ON UDP MULTICAST\r\n");
//     int bytesSent = m_ssdpSocket.sendTo(welcomeMessage.c_str(), welcomeMessage.length(), SocketAddress("239.255.255.250:1900"));
    int bytesSent = m_ssdpSocket.sendTo(welcomeMessage.c_str(), welcomeMessage.length(), SocketAddress(m_ssdpAddress, m_ssdpPort));
    std::cout << "message sent, number bytes: " << bytesSent << std::endl;
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
        std::cout << m_pBuffer << std::endl;
    }
//     else
//         delete this;
}


SsdpMessage::SsdpMessage()
{
}


SsdpMessage::~SsdpMessage()
{
}


// SsdpSession::SsdpSession()
// {
// }
// 
// 
// SsdpSession::~SsdpSession()
// {
// }






/*
SsdpReceiver::SsdpReceiver()
// m_ssdpSocket(SsdpSocket())
// m_reactor(m_ssdpSocket.getReactor())
{
//     m_ssdpSocket = SsdpSocket();
//     m_reactor = m_ssdpSocket.getReactor();
//     m_reactor = SingletonHolder<SsdpSocket>.get()->getReactor();
//     m_reactor.addEventHandler(_socket, NObserver<EchoServiceHandler, ReadableNotification>(*this, &EchoServiceHandler::onReadable));
//     m_reactor.addEventHandler(_socket, NObserver<EchoServiceHandler, ShutdownNotification>(*this, &EchoServiceHandler::onShutdown));
}


SsdpReceiver::~SsdpReceiver()
{
}


SsdpSender::SsdpSender()
{
}


SsdpSender::~SsdpSender()
{
}
*/

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
