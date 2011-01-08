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

#include "Sys.h"
#include "SysImplDarwin.h"

#include <SystemConfiguration.h>
#include <CoreFoundation.h>

/*
 * find System Configuration Framework and use kernel event monitor
 * relevant keys in the dynamic store are:
 * State:/Network/Interface, property Interfaces
 * State:/Network/Interface/<InterfaceName>/Link, property Active
 *
 * command line utility: scutil
 * header (for example): /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.0.sdk/System/Library/Frameworks/SystemConfiguration.framework/Headers
 */


namespace Omm {
namespace Sys {


class NetworkInterfaceManagerImpl::Private
{
public:
    Private()
    {
        SCDynamicStoreContext dynamicStoreContext;
        dynamicStoreContext.version = 0;
        dynamicStoreContext.info = 0;
        dynamicStoreContext.retain = 0;
        dynamicStoreContext.release = 0;
        dynamicStoreContext.copyDescription = 0;
        _dynamicStoreRef = SCDynamicStoreCreate(NULL, CFSTR("Omm::Sys::SysImplDarwin"), callback, &dynamicStoreContext);
    }


    ~Private()
    {
        CFRelease(_dynamicStoreRef);
    }

    
    static void callback(const SCDynamicStoreRef storeRef, const CFArrayRef changedKeysRef, void* pInfo)
    {
        Log::instance()->sys().debug("dynamic store detects network device change.");
        for(int i = 0; i < CFArrayGetCount(changedKeysRef); i++) {
            CFStringRef key = static_cast<CFStringRef>(CFArrayGetValueAtIndex(changedKeysRef, i));
            Log::instance()->sys().debug("interface: " + std::string(CFStringGetCStringPtr(key, CFStringGetSystemEncoding())));
            CFPropertyListRef val = SCDynamicStoreCopyValue(storeRef, key);
            CFRange left = CFStringFind(key, CFSTR("Interface/"), 0);
            CFRange right = CFStringFind(key, CFSTR("/IPv4"), 0);
            CFStringRef cfInterfaceName = CFStringCreateWithSubstring(0, key, CFRangeMake(left.location + left.length, right.location - left.location - left.length));
            std::string interfaceName(CFStringGetCStringPtr(cfInterfaceName, CFStringGetSystemEncoding()));
            if (val) {
                NetworkInterfaceManager::instance()->addInterface(interfaceName);
                CFRelease(val);
            }
            else {
                NetworkInterfaceManager::instance()->removeInterface(interfaceName);
            }
        }
    }
    
    SCDynamicStoreRef       _dynamicStoreRef;
};


NetworkInterfaceManagerImpl::NetworkInterfaceManagerImpl()
{
    _d = new Private;
}


NetworkInterfaceManagerImpl::~NetworkInterfaceManagerImpl()
{
    stop();
    delete _d;
}


void
NetworkInterfaceManagerImpl::start()
{
    // list network devices
    Log::instance()->sys().debug("initializing dynamic store ...");
    CFPropertyListRef interfacePropRef = SCDynamicStoreCopyValue(_d->_dynamicStoreRef, CFSTR("State:/Network/Interface"));
    CFDictionaryRef interfaceDictRef = static_cast<CFDictionaryRef>(interfacePropRef);
    CFArrayRef interfaceListRef = static_cast<CFArrayRef>(CFDictionaryGetValue(interfaceDictRef, CFSTR("Interfaces")));
    for(int i = 0; i < CFArrayGetCount(interfaceListRef); i++) {
        CFStringRef interface = static_cast<CFStringRef>(CFArrayGetValueAtIndex(interfaceListRef, i));
        Log::instance()->sys().debug("dynamic store recognizes interface: " + std::string(CFStringGetCStringPtr(interface, CFStringGetSystemEncoding())));
    }
    CFRelease(interfacePropRef);

    // register for changes on network interfaces
    CFStringRef notifyKeyRegex[1];
    notifyKeyRegex[0] = CFSTR("State:/Network/Interface/.*/IPv4");
    if (SCDynamicStoreSetNotificationKeys(_d->_dynamicStoreRef, 0, CFArrayCreate(0, (const void**)notifyKeyRegex, 1, &kCFTypeArrayCallBacks))) {
        Log::instance()->sys().debug("registered network interface monitor.");
    }
    else {
        Log::instance()->sys().warning("registration of network interface monitor failed.");
    }
    if (SCDynamicStoreSetDispatchQueue(_d->_dynamicStoreRef, dispatch_get_current_queue())) {
        Log::instance()->sys().debug("waiting for network device changes ...");
    }
    else {
        Log::instance()->sys().warning("start of network device monitor failed.");
    }
}


void
NetworkInterfaceManagerImpl::stop()
{
    if (SCDynamicStoreSetDispatchQueue(_d->_dynamicStoreRef, 0)) {
        Log::instance()->sys().debug("stop waiting for network device changes.");
    }
    else {
        Log::instance()->sys().warning("stop of network device monitor failed.");
    }
    if (SCDynamicStoreSetNotificationKeys(_d->_dynamicStoreRef, 0, 0)) {
        Log::instance()->sys().debug("unregistered network interface monitor.");
    }
    else {
        Log::instance()->sys().warning("unregister network interface monitor failed.");
    }
}


}  // namespace Sys
} // namespace Omm
