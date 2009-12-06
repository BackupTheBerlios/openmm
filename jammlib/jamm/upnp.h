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

#ifndef JAMMUPNP_H
#define JAMMUPNP_H

#include <string>
#include <map>
#include <iostream>
#include <istream>
#include <ostream>


#include "Poco/SingletonHolder.h"
#include "Poco/NObserver.h"
#include "Poco/Observer.h"
#include "Poco/Thread.h"
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/NotificationCenter.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/MessageHeader.h"


using Poco::SingletonHolder;
using Poco::NObserver;
using Poco::Observer;
using Poco::Thread;
using Poco::AutoPtr;
using Poco::UInt16;
using Poco::AbstractObserver;
using Poco::Notification;
using Poco::NotificationQueue;
using Poco::NotificationCenter;
using Poco::Net::Socket;
using Poco::Net::DatagramSocket;
using Poco::Net::MulticastSocket;
using Poco::Net::NetworkInterface;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::Net::SocketInputStream;
using Poco::Net::SocketOutputStream;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::IPAddress;
using Poco::Net::MessageHeader;

namespace Jamm {


/// SSDP - Simple Service Discovery Protocol
/// HTTP dialect transmitted over UDP Multicast
/// See "UPnP Device Architecture" for details


class SsdpMessage : public Notification // public MessageHeader
{
public:
    SsdpMessage();
    SsdpMessage(const std::string& buf);
    ~SsdpMessage();
    
//     const std::string& toString() const;
    const std::string& toString();
//     std::string toString();
    
    void setRequestType();
    void getRequestType();
    
    void setCacheControl(int leaseTime);
    int getCacheControl();
    
    enum {
        REQUEST_NOTIFY      = 1,
        REQUEST_SEARCH      = 2,
        REQUEST_RESPONSE    = 3
    };
    
    typedef int TRequestMethod;
    
private:
/*    static const std::string REQUEST_NOTIFY_STR;
    static const std::string REQUEST_SEARCH_STR;
    static const std::string REQUEST_RESPONSE_STR;*/
    
    TRequestMethod m_requestMethod;
    
    std::map<TRequestMethod,std::string> m_messageMap;
    std::map<std::string,TRequestMethod> m_messageConstMap;
};


/// This class provides an interface to a
/// UDP Multicast socket.
class SsdpSocket
{ 
public:
    SsdpSocket(const AbstractObserver& observer);
    ~SsdpSocket();
    
    void sendMessage(SsdpMessage& message);
    
private:
//     SocketAddress       m_ssdpAddress;
    IPAddress           m_ssdpAddress;
    UInt16              m_ssdpPort;
    MulticastSocket     m_ssdpSocket;
    NetworkInterface    m_interface;
    SocketReactor       m_reactor;
    Thread              m_listenerThread;
    NotificationCenter  m_notificationCenter;
    char*               m_pBuffer;
    
    static const std::string SSDP_ADDRESS;
    enum {
        SSDP_PORT   = 1900,
        BUFFER_SIZE = 65536 // Max UDP Packet size is 64 Kbyte.
            // Note that each SSDP message must fit into one UDP Packet.
    };
    
    void onReadable(const AutoPtr<ReadableNotification>& pNf);
};


class Device {
public:
    Device();
    ~Device();
    
private:
};


class ControlPoint {
public:
    ControlPoint();
    ~ControlPoint();

private:
};

} // namespace Jamm

#endif
