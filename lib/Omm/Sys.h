/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef Sys_INCLUDED
#define Sys_INCLUDED

#include <Poco/Logger.h>
#include <Poco/NotificationCenter.h>
#include <Poco/Net/IPAddress.h>
#include <vector>
#include <string>

namespace Omm {
namespace Sys {

class Log
{
public:
    static Log* instance();
    
    Poco::Logger& sys();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pSysLogger;
};

class NetworkInterfaceManagerImpl;

class NetworkInterfaceManager
{
    friend class NetworkInterfaceManagerImpl;

public:
    static NetworkInterfaceManager* instance();
    // clients like DeviceRoot and Controller can register here
    void registerInterfaceChangeHandler(const Poco::AbstractObserver& observer);
    void scanInterfaces();
    // some OS dependent hardware abstraction layer can add and remove devices here
    void addInterface(const std::string& name);
    void removeInterface(const std::string& name);
    // this address can be announced for the HTTP servers to be reached at
    const Poco::Net::IPAddress& getValidInterfaceAddress();
    const std::string loopbackInterfaceName();

private:
    NetworkInterfaceManager();
    void findValidIpAddress();
    static bool isLoopback(const std::string& interfaceName);
    
    static NetworkInterfaceManager*     _pInstance;
    NetworkInterfaceManagerImpl*        _pImpl;
    std::string                         _loopbackInterfaceName;
    std::vector<std::string>            _interfaceList;
    Poco::Net::IPAddress                _validIpAddress;
    Poco::NotificationCenter            _notificationCenter;
};


class NetworkInterfaceNotification : public Poco::Notification
{
public:
    NetworkInterfaceNotification(const std::string& interfaceName, bool added);
    
    std::string     _interfaceName;
    bool            _added;
};


}  // namespace Sys
}  // namespace Omm

#endif
