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
#ifndef NetworkDevice_INCLUDED
#define NetworkDevice_INCLUDED

#include <dbus-1.0/dbus/dbus-shared.h>
#include <dbus-c++-1/dbus-c++/dbus.h>
#include <NetworkManager/NetworkManager.h>

#include <vector>
#include <map>

namespace Omm {
namespace Sys {


class NetworkDeviceProperties : public DBus::PropertiesProxy, public DBus::ObjectProxy
{
public:
    NetworkDeviceProperties(DBus::Connection& connection, DBus::Path& udi);

    std::string getDeviceName();

private:
    DBus::Variant getProperty(const std::string property);
};


class NetworkDevice : public DBus::InterfaceProxy, public DBus::ObjectProxy
{
public:
    NetworkDevice(DBus::Connection& connection, DBus::Path& udi);
    
private:
    void stateChangedCb(const DBus::SignalMessage& sig);
    std::string stateName(NMDeviceState state);
    std::string reasonName(NMDeviceStateReason reason);

    NetworkDeviceProperties     _deviceProperties;
    std::string                 _deviceName;
};


class NetworkManager : public DBus::InterfaceProxy, public DBus::ObjectProxy
{
public:
    NetworkManager(DBus::Connection& connection);

private:
    std::vector<DBus::Path> getDevices();
    void deviceAddedCb(const DBus::SignalMessage& sig);
    void deviceRemovedCb(const DBus::SignalMessage& sig);
    void stateChangedCb(const DBus::SignalMessage& sig);
    std::string stateName(NMState state);

    std::map<std::string, DBus::RefPtr<NetworkDevice> > m_devices;
};


}  // namespace Sys
}  // namespace Omm

#endif
