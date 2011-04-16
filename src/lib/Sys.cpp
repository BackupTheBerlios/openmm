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
#include "SysImpl.h"
#ifdef __LINUX__
#include "Sys/Linux/SysImplLinux.h"
#elif __DARWIN__
#include "Sys/Darwin/SysImplDarwin.h"
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
#ifdef NDEBUG
    _pSysLogger = &Poco::Logger::create("SYS", pFormatLogger, 0);
#else
    _pSysLogger = &Poco::Logger::create("SYS", pFormatLogger, Poco::Message::PRIO_DEBUG);
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
Log::sys()
{
    return *_pSysLogger;
}


NetworkInterfaceManager* NetworkInterfaceManager::_pInstance = 0;
Poco::FastMutex NetworkInterfaceManager::_instanceLock;

NetworkInterfaceManager::NetworkInterfaceManager()
{
    Log::instance()->sys().debug("installing network interface manager");
    
    scanInterfaces();
    findValidIpAddress();

    // start monitoring of changes on network interfaces (if available on particular platform)
    _pImpl = new NetworkInterfaceManagerImpl;
    _pImpl->start();
}


NetworkInterfaceManager*
NetworkInterfaceManager::instance()
{
    Log::instance()->sys().debug("getting instance of network interface manager");
    
    Poco::FastMutex::ScopedLock lock(_instanceLock);
    if (!_pInstance) {
        _pInstance = new NetworkInterfaceManager;
    }
    return _pInstance;
}


const std::string
NetworkInterfaceManager::loopbackInterfaceName()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    return _loopbackInterfaceName;
}


void
NetworkInterfaceManager::findValidIpAddress()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    bool validAddressFound = false;
    bool loopBackProvided = false;
    Log::instance()->sys().debug("number of scanned IPv4 interfaces: " + Poco::NumberFormatter::format(_interfaceList.size()));
    for (std::vector<Poco::Net::NetworkInterface>::iterator it = _interfaceList.begin(); it != _interfaceList.end(); ++it) {
        if (isLoopback(*it)) {
            loopBackProvided = true;
        }
        else {
            // FIXME:
            // iphone simulator returns an IPv6 address on an IPv4 interface, we have to double-check.
            // funny, that it works anyway on iphone-simulator with a validIpAddress = 0.0.0.0
            // and for example device descriptions are downloaded from http://0.0.0.0:port/Description.xml
            //
            // using forName can give the IPv6 address of the interface with the same name ...
            Poco::Net::IPAddress validIpAddress = (*it).address();
            if (validIpAddress.family() == Poco::Net::IPAddress::IPv4) {
                validAddressFound = true;
                _validIpAddress = validIpAddress;
            }
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
NetworkInterfaceManager::isLoopback(const Poco::Net::NetworkInterface& interface)
{
    Poco::Net::IPAddress address = interface.address();
    
    return address.isLoopback();
}


void
NetworkInterfaceManager::registerInterfaceChangeHandler(const Poco::AbstractObserver& observer)
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    _notificationCenter.addObserver(observer);
}


void
NetworkInterfaceManager::scanInterfaces()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    std::vector<Poco::Net::NetworkInterface> ifList = Poco::Net::NetworkInterface::list();
    Log::instance()->sys().debug("number of network interfaces: " + Poco::NumberFormatter::format(ifList.size()));
    for (std::vector<Poco::Net::NetworkInterface>::iterator it = ifList.begin(); it != ifList.end(); ++it) {
        std::string interfaceName = (*it).name();
        Poco::Net::IPAddress address = (*it).address();
        if (address.family() == Poco::Net::IPAddress::IPv4) {
            if (isLoopback(*it)) {
                Log::instance()->sys().debug("found loopback interface: " + interfaceName + ", (index " + Poco::NumberFormatter::format((*it).index()) + ")");
                _loopbackInterfaceName = interfaceName;
            }
            else {
                Log::instance()->sys().information("found network interface: " + interfaceName + ", (index: " + Poco::NumberFormatter::format((*it).index()) + ")");
            }
            _interfaceList.push_back(*it);
        }
        else {
            Log::instance()->sys().debug("interface "  + interfaceName + " (index " + Poco::NumberFormatter::format((*it).index()) + ") is not IP4, ignoring.");
        }
    }
}


void
NetworkInterfaceManager::addInterface(const std::string& name)
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    Poco::Net::NetworkInterface interface = Poco::Net::NetworkInterface::forName(name, false);
    Poco::Net::IPAddress address = interface.address();
    // FIXME: check if network interface is already registered
    //if (std::find(_interfaceList.begin(), _interfaceList.end(), name) == _interfaceList.end() && address.family() == Poco::Net::IPAddress::IPv4) {
        Log::instance()->sys().information("adding network interface: " + name);
        // FIXME: add the right interface ...?
        _interfaceList.push_back(Poco::Net::NetworkInterface::forName(name, false));
        findValidIpAddress();
        Log::instance()->sys().information("notify observer of new network interface: " + name);
        _notificationCenter.postNotification(new NetworkInterfaceNotification(name, true));
        //findValidIpAddress();
//    }
//    else {
//        Log::instance()->sys().information("added network interface already known or not IP4: " + name);
//    }
}


void
NetworkInterfaceManager::removeInterface(const std::string& name)
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);
    
    Log::instance()->sys().information("removing network interface: " + name);

    // FIXME: erase interface from interface list.
    //_interfaceList.erase(std::find(_interfaceList.begin(), _interfaceList.end(), name));
    findValidIpAddress();
    Log::instance()->sys().information("notify observer of removed network interface: " + name);
    _notificationCenter.postNotification(new NetworkInterfaceNotification(name, false));
}


const Poco::Net::IPAddress&
NetworkInterfaceManager::getValidIpAddress()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    return _validIpAddress;
}


bool
NetworkInterfaceManager::loopbackOnly()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    return _interfaceList.size() == 1 && isLoopback(_interfaceList[0]);
}


int
NetworkInterfaceManager::interfaceCount()
{
    Poco::ScopedLock<Poco::Mutex> lock(_lock);

    return _interfaceList.size();
}


NetworkInterfaceNotification::NetworkInterfaceNotification(const std::string& interfaceName, bool added) :
_interfaceName(interfaceName),
_added(added)
{
}


Visual::Visual()
{
    _pImpl = new VisualImpl;
}


Visual::~Visual()
{
    delete _pImpl;
}


void*
Visual::getWindow()
{
    if (_pImpl) {
        return _pImpl->getWindow();
    }
    else {
        return 0;
    }
}


void
Visual::show()
{
    if (_pImpl) {
        _pImpl->show();
    }
}


void
Visual::hide()
{
    if (_pImpl) {
        _pImpl->hide();
    }
}


int
Visual::getWidth()
{
    if (_pImpl) {
        return _pImpl->getWidth();
    }
    else {
        return 0;
    }
}


int
Visual::getHeight()
{
    if (_pImpl) {
        return _pImpl->getHeight();
    }
    else {
        return 0;
    }
}


Visual::VisualType
Visual::getType()
{
    if (_pImpl) {
        return _pImpl->getType();
    }
    else {
        return VTNone;
    }
}



}  // namespace Sys
}  // namespace Omm
