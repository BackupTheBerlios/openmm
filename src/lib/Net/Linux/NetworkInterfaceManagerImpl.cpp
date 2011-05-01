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
#include <dbus-c++-1/dbus-c++/dbus.h>

#include "Sys.h"
#include "NetworkDevice.h"
#include "SysImplLinux.h"


namespace Omm {
namespace Net {

class NetworkInterfaceManagerImpl::Private
{
public:
    DBus::BusDispatcher _dispatcher;
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
    Log::instance()->sys().debug("connecting NetworkManager ...");
    _monitorThread.start(*this);
}


void
NetworkInterfaceManagerImpl::run()
{
    try {
        DBus::default_dispatcher = &_p->_dispatcher;
        Log::instance()->sys().debug("connecting system bus");
        DBus::Connection conn = DBus::Connection::SystemBus();
        Log::instance()->sys().debug("initializing NetworkManager");
        NetworkManager network(conn);
        Log::instance()->sys().debug("waiting for network device changes ...");
        _p->_dispatcher.enter();
    }
    catch(DBus::Error err) {
        Log::instance()->sys().error("DBus error occured: " + std::string(err.what()));
    }
}


void
NetworkInterfaceManagerImpl::stop()
{
    _p->_dispatcher.leave();
    Log::instance()->sys().debug("disconnected from NetworkManager.");
}

}  // namespace Net
} // namespace Omm
