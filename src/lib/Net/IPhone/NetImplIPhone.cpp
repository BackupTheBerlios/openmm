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

#include "Net.h"
#include "NetImplIPhone.h"
#include "Log.h"

#include <SystemConfiguration.h>
#include <CoreFoundation.h>
#include <Poco/Thread.h>


namespace Omm {
namespace Net {


class NetworkInterfaceManagerImpl::Private
{
public:
    Private()
    {
        struct sockaddr_in localWifiAddress;
        bzero(&localWifiAddress, sizeof(localWifiAddress));
        localWifiAddress.sin_len = sizeof(localWifiAddress);
        localWifiAddress.sin_family = AF_INET;
        // IN_LINKLOCALNETNUM is defined in <netinet/in.h> as 169.254.0.0
        localWifiAddress.sin_addr.s_addr = htonl(IN_LINKLOCALNETNUM);
        _reachabilityRef = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr*)&localWifiAddress);
        SCNetworkReachabilityContext context = {0, this, NULL, NULL, NULL};
        SCNetworkReachabilitySetCallback(_reachabilityRef, callback, &context);
    }


    ~Private()
    {
        if(_reachabilityRef)
        {
            CFRelease(_reachabilityRef);
        }
    }


    static void callback(SCNetworkReachabilityRef target, SCNetworkReachabilityFlags flags, void* info)
    {
        if ((flags & kSCNetworkReachabilityFlagsReachable) && (flags & kSCNetworkReachabilityFlagsIsDirect)) {
//          FIXME: MSearch packet is lost when iphone wifi is reachable but network not. As a workaround
//          introduced a delay of 1000ms after iphone tells that wifi is reachable. Without that
//          the msearch udp packet is lost. This should be replaced by a safer method, as it may
//          take longer until the connection to the router is established. Reachability framework
//          only tells availability of connection ... the Darwin backend uses another framework.
            Poco::Thread::sleep(1000);
            NetworkInterfaceManager::instance()->addInterface("en0");
        }
        else {
            NetworkInterfaceManager::instance()->removeInterface("en0");
        }
    }


    SCNetworkReachabilityRef _reachabilityRef;
};


NetworkInterfaceManagerImpl::NetworkInterfaceManagerImpl()
{
    _p = new Private;
}


NetworkInterfaceManagerImpl::~NetworkInterfaceManagerImpl()
{
    stop();
    delete _p;
}


void
NetworkInterfaceManagerImpl::start()
{
    if(SCNetworkReachabilityScheduleWithRunLoop(_p->_reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode))
    {
        LOG(net, debug, "waiting for network device changes ...");
    }
    else {
        LOG(net, warning, "start of network device monitor failed.");
    }
}


void
NetworkInterfaceManagerImpl::stop()
{
    if(_p->_reachabilityRef)
    {
        SCNetworkReachabilityUnscheduleFromRunLoop(_p->_reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    }
}


}  // namespace Net
} // namespace Omm
