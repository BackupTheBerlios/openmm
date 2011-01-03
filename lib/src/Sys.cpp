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

#include <Poco/NumberFormatter.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Net/NetworkInterface.h>

#include "Sys.h"

#ifdef __LINUX__
#include "SysLinux/SysImplLinux.h"
#elif __DARWIN__

#else
#include "SysImpl.h"
#endif

namespace Omm {
namespace Sys {

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
    _pSysLogger = &Poco::Logger::create("SYS", pFormatLogger, Poco::Message::PRIO_DEBUG);
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
Log::sys()
{
    return *_pSysLogger;
}


NetworkInterfaceManager* NetworkInterfaceManager::_pInstance = 0;

NetworkInterfaceManager::NetworkInterfaceManager()
{
    scanInterfaces();
    findValidIpAddress();
    _pImpl = new NetworkInterfaceManagerImpl;
    _pImpl->start();
}


NetworkInterfaceManager*
NetworkInterfaceManager::instance()
{
    if (!_pInstance) {
        _pInstance = new NetworkInterfaceManager;
    }
    return _pInstance;
}


const std::string
NetworkInterfaceManager::loopbackInterfaceName()
{
    return _loopbackInterfaceName;
}


void
NetworkInterfaceManager::findValidIpAddress()
{
    bool validAddressFound = false;
    bool loopBackProvided = false;
    for (std::vector<std::string>::iterator it = _interfaceList.begin(); it != _interfaceList.end(); ++it) {
        if (isLoopback(*it)) {
            loopBackProvided = true;
        }
        else {
            validAddressFound = true;
            _validIpAddress = Poco::Net::NetworkInterface::forName(*it).address();
        }
    }
    if (_interfaceList.size() == 1 && loopBackProvided) {
        Log::instance()->sys().debug("loopback is the only network interface, setting valid IP address to 127.0.0.1");
        validAddressFound = true;
        _validIpAddress = Poco::Net::IPAddress("127.0.0.1");
    }
    if (validAddressFound) {
        Log::instance()->sys().information("found valid IP address: " + _validIpAddress.toString());
    }
    else {
        Log::instance()->sys().fatal("no valid IP address found");
        // TODO: bail out of application
    }
}


bool
NetworkInterfaceManager::isLoopback(const std::string& interfaceName)
{
    Poco::Net::IPAddress address = Poco::Net::NetworkInterface::forName(interfaceName).address();
    
    return (address.isLoopback()
            || interfaceName == "lo0"); // FIXME: workaround for MacOS, isLoopback() doesn't work?
}


void
NetworkInterfaceManager::registerInterfaceChangeHandler(const Poco::AbstractObserver& observer)
{
    _notificationCenter.addObserver(observer);
    for (std::vector<std::string>::iterator it = _interfaceList.begin(); it != _interfaceList.end(); ++it) {
        Log::instance()->sys().information("notify observer of new network interface: " + (*it));
        observer.notify(new NetworkInterfaceNotification((*it), true));
    }
}


void
NetworkInterfaceManager::scanInterfaces()
{
    std::vector<Poco::Net::NetworkInterface> ifList = Poco::Net::NetworkInterface::list();
    for (std::vector<Poco::Net::NetworkInterface>::iterator it = ifList.begin(); it != ifList.end(); ++it) {
        std::string interfaceName = (*it).name();
        Poco::Net::IPAddress address = (*it).address();
        if (isLoopback(interfaceName)) {
            Log::instance()->sys().debug("found loopback interface: " + interfaceName);
            _loopbackInterfaceName = interfaceName;
        }
        else {
            Log::instance()->sys().information("found network interface: " + interfaceName);
        }
        _interfaceList.push_back(interfaceName);
    }
}


void
NetworkInterfaceManager::addInterface(const std::string& name)
{
    if (find(_interfaceList.begin(), _interfaceList.end(), name) == _interfaceList.end()) {
        Log::instance()->sys().information("adding network interface: " + name);
        _interfaceList.push_back(name);
        Log::instance()->sys().information("notify observer of new network interface: " + name);
        _notificationCenter.postNotification(new NetworkInterfaceNotification(name, true));
    }
    else {
        Log::instance()->sys().information("added network interface already known: " + name);
    }
    findValidIpAddress();
}


void
NetworkInterfaceManager::removeInterface(const std::string& name)
{
    Log::instance()->sys().information("removing network interface: " + name);
    
    _interfaceList.erase(find(_interfaceList.begin(), _interfaceList.end(), name));
    Log::instance()->sys().information("notify observer of removed network interface: " + name);
    _notificationCenter.postNotification(new NetworkInterfaceNotification(name, false));
    findValidIpAddress();
}


const Poco::Net::IPAddress&
NetworkInterfaceManager::getValidInterfaceAddress()
{
    // TODO: probably need some locking here
    return _validIpAddress;
}


NetworkInterfaceNotification::NetworkInterfaceNotification(const std::string& interfaceName, bool added) :
_interfaceName(interfaceName),
_added(added)
{
}


}  // namespace Sys
}  // namespace Omm
