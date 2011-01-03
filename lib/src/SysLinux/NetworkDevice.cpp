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
#include <stdlib.h>
#include <sstream>

#include "NetworkDevice.h"


namespace Omm {
namespace Sys {

LinuxNetworkDeviceMonitor::LinuxNetworkDeviceMonitor()
{
}


LinuxNetworkDeviceMonitor::~LinuxNetworkDeviceMonitor()
{
    stop();
}


void
LinuxNetworkDeviceMonitor::start()
{
    Log::instance()->sys().debug("connecting NetworkManager ...");
    _monitorThread.start(*this);
}


void
LinuxNetworkDeviceMonitor::run()
{
    try {
        DBus::default_dispatcher = &_dispatcher;
        Log::instance()->sys().debug("connecting system bus");
        DBus::Connection conn = DBus::Connection::SystemBus();
        Log::instance()->sys().debug("initializing NetworkManager");
        NetworkManager network(conn);
        Log::instance()->sys().debug("waiting for network device changes ...");
        _dispatcher.enter();
    }
    catch(DBus::Error err) {
        Log::instance()->sys().error("DBus error occured: " + std::string(err.what()));
    }
}


bool
LinuxNetworkDeviceMonitor::stop()
{
    _dispatcher.leave();
    Log::instance()->sys().debug("disconnected from NetworkManager.");
    return true;
}


NetworkDeviceProperties::NetworkDeviceProperties(DBus::Connection& connection, DBus::Path& udi) :
   DBus::ObjectProxy(connection, udi, NM_DBUS_SERVICE)
{
}


std::string
NetworkDeviceProperties::getDeviceName()
{
    DBus::Variant deviceNameVariant = getProperty("Interface");
    std::string res;
    DBus::MessageIter itProp = deviceNameVariant.reader();
    itProp >> res;
    return res;
}


DBus::Variant
NetworkDeviceProperties::getProperty(const std::string property)
{
// TODO: this should work, but gives a type mismatch. The variant returned has type ''.
//     return Get(NM_DBUS_INTERFACE_DEVICE, property);

    DBus::CallMessage call;
    DBus::MessageIter itw = call.writer();
    itw.append_string(NM_DBUS_INTERFACE_DEVICE);
    itw.append_string(property.c_str());

    call.member("Get");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();
    DBus::Variant res;
    it >> res;
    return res;
    
}


NetworkDevice::NetworkDevice(DBus::Connection& connection, DBus::Path& udi) :
   DBus::InterfaceProxy(NM_DBUS_INTERFACE_DEVICE),
   DBus::ObjectProxy(connection, udi, NM_DBUS_SERVICE),
   _deviceProperties(connection, udi)
{
    connect_signal(NetworkDevice, StateChanged, stateChangedCb);
    _deviceName = _deviceProperties.getDeviceName();
    Log::instance()->sys().debug("found network device: " + _deviceName);
}


void
NetworkDevice::stateChangedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    
    uint32_t newState;
    uint32_t oldState;
    uint32_t reason;
    
    it >> newState;
    it >> oldState;
    it >> reason;
    
    Log::instance()->sys().debug("network device " + _deviceName + " changes state from: " 
        + stateName(static_cast<NMDeviceState>(oldState))
        + " to: " + stateName(static_cast<NMDeviceState>(newState)) 
        + ", reason: " + reasonName(static_cast<NMDeviceStateReason>(reason)));
    
    if (newState == NM_DEVICE_STATE_ACTIVATED) {
        NetworkInterfaceManager::instance()->addInterface(_deviceName);
    }
    else if (newState == NM_DEVICE_STATE_UNAVAILABLE) {
        NetworkInterfaceManager::instance()->removeInterface(_deviceName);
    }
}


std::string
NetworkDevice::stateName(NMDeviceState state)
{
    switch (state) {
        case NM_DEVICE_STATE_UNKNOWN:
            return "NM_DEVICE_STATE_UNKNOWN";
        case NM_DEVICE_STATE_UNMANAGED:
            return "NM_DEVICE_STATE_UNMANAGED";
            break;
        case NM_DEVICE_STATE_UNAVAILABLE:
            return "NM_DEVICE_STATE_UNAVAILABLE";
            break;
        case NM_DEVICE_STATE_DISCONNECTED:
            return "NM_DEVICE_STATE_DISCONNECTED";
            break;
        case NM_DEVICE_STATE_PREPARE:
            return "NM_DEVICE_STATE_PREPARE";
            break;
        case NM_DEVICE_STATE_CONFIG:
            return "NM_DEVICE_STATE_CONFIG";
            break;
        case NM_DEVICE_STATE_NEED_AUTH:
            return "NM_DEVICE_STATE_NEED_AUTH";
            break;
        case NM_DEVICE_STATE_IP_CONFIG:
            return "NM_DEVICE_STATE_IP_CONFIG";
            break;
        case NM_DEVICE_STATE_ACTIVATED:
            return "NM_DEVICE_STATE_ACTIVATED";
            break;
        case NM_DEVICE_STATE_FAILED:
            return "NM_DEVICE_STATE_FAILED";
            break;
    }
}


std::string
NetworkDevice::reasonName(NMDeviceStateReason reason)
{
    switch (reason) {
        case NM_DEVICE_STATE_REASON_NONE:
            return "NM_DEVICE_STATE_REASON_NONE";
        case NM_DEVICE_STATE_REASON_UNKNOWN:
            return "NM_DEVICE_STATE_REASON_UNKNOWN";
        case NM_DEVICE_STATE_REASON_NOW_MANAGED:
            return "NM_DEVICE_STATE_REASON_NOW_MANAGED";
        case NM_DEVICE_STATE_REASON_NOW_UNMANAGED:
            return "NM_DEVICE_STATE_REASON_NOW_UNMANAGED";
        case NM_DEVICE_STATE_REASON_CONFIG_FAILED:
            return "NM_DEVICE_STATE_REASON_CONFIG_FAILED";
        case NM_DEVICE_STATE_REASON_IP_CONFIG_UNAVAILABLE:
            return "NM_DEVICE_STATE_REASON_IP_CONFIG_UNAVAILABLE";
        case NM_DEVICE_STATE_REASON_IP_CONFIG_EXPIRED:
            return "NM_DEVICE_STATE_REASON_IP_CONFIG_EXPIRED";
        case NM_DEVICE_STATE_REASON_NO_SECRETS:
            return "NM_DEVICE_STATE_REASON_NO_SECRETS";
        case NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT:
            return "NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT";
        case NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED:
            return "NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED";
        case NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED:
            return "NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED";
        case NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT:
            return "NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT";
        case NM_DEVICE_STATE_REASON_PPP_START_FAILED:
            return "NM_DEVICE_STATE_REASON_PPP_START_FAILED";
        case NM_DEVICE_STATE_REASON_PPP_DISCONNECT:
            return "NM_DEVICE_STATE_REASON_PPP_DISCONNECT";
        case NM_DEVICE_STATE_REASON_PPP_FAILED:
            return "NM_DEVICE_STATE_REASON_PPP_FAILED";
        case NM_DEVICE_STATE_REASON_DHCP_START_FAILED:
            return "NM_DEVICE_STATE_REASON_DHCP_START_FAILED";
        case NM_DEVICE_STATE_REASON_DHCP_ERROR:
            return "NM_DEVICE_STATE_REASON_DHCP_ERROR";
        case NM_DEVICE_STATE_REASON_DHCP_FAILED:
            return "NM_DEVICE_STATE_REASON_DHCP_FAILED";
        case NM_DEVICE_STATE_REASON_SHARED_START_FAILED:
            return "NM_DEVICE_STATE_REASON_SHARED_START_FAILED";
        case NM_DEVICE_STATE_REASON_SHARED_FAILED:
            return "NM_DEVICE_STATE_REASON_SHARED_FAILED";
        case NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED:
            return "NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED";
        case NM_DEVICE_STATE_REASON_AUTOIP_ERROR:
            return "NM_DEVICE_STATE_REASON_AUTOIP_ERROR";
        case NM_DEVICE_STATE_REASON_AUTOIP_FAILED:
            return "NM_DEVICE_STATE_REASON_AUTOIP_FAILED";
        case NM_DEVICE_STATE_REASON_MODEM_BUSY:
            return "NM_DEVICE_STATE_REASON_MODEM_BUSY";
        case NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE:
            return "NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE";
        case NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER:
            return "NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER";
        case NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT:
            return "NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT";
        case NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED:
            return "NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED";
        case NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED:
            return "NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED";
        case NM_DEVICE_STATE_REASON_GSM_APN_FAILED:
            return "NM_DEVICE_STATE_REASON_GSM_APN_FAILED";
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING:
            return "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING";
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED:
            return "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED";
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT:
            return "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT";
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED:
            return "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED";
        case NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED:
            return "NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED";
        case NM_DEVICE_STATE_REASON_FIRMWARE_MISSING:
            return "NM_DEVICE_STATE_REASON_FIRMWARE_MISSING";
        case NM_DEVICE_STATE_REASON_REMOVED:
            return "NM_DEVICE_STATE_REASON_REMOVED";
        case NM_DEVICE_STATE_REASON_SLEEPING:
            return "NM_DEVICE_STATE_REASON_SLEEPING";
        case NM_DEVICE_STATE_REASON_CONNECTION_REMOVED:
            return "NM_DEVICE_STATE_REASON_CONNECTION_REMOVED";
        case NM_DEVICE_STATE_REASON_USER_REQUESTED:
            return "NM_DEVICE_STATE_REASON_USER_REQUESTED";
        case NM_DEVICE_STATE_REASON_CARRIER:
            return "NM_DEVICE_STATE_REASON_CARRIER";
        case NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED:
            return "NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED";
        case NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE:
            return "NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE";
    }
}


NetworkManager::NetworkManager(DBus::Connection& connection)
 : DBus::InterfaceProxy(NM_DBUS_INTERFACE),
   DBus::ObjectProxy(connection, NM_DBUS_PATH, NM_DBUS_SERVICE)
{
    connect_signal(NetworkManager, DeviceAdded, deviceAddedCb);
    connect_signal(NetworkManager, DeviceRemoved, deviceRemovedCb);
    connect_signal(NetworkManager, StateChanged, stateChangedCb);

    std::vector<DBus::Path> devices = getDevices();

    std::vector<DBus::Path>::iterator it;
    for(it = devices.begin(); it != devices.end(); ++it) {
        DBus::Path udi = *it;
        m_devices[udi] = new NetworkDevice(connection, udi);
    }
}


std::vector<DBus::Path>
NetworkManager::getDevices()
{
    Log::instance()->sys().debug("getting network device list ...");
    std::vector<DBus::Path> paths;
    DBus::CallMessage call;

    call.member("GetDevices");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();

    it >> paths;
    return paths;
}


void
NetworkManager::deviceAddedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    DBus::Path udi;
    it >> udi;
    Log::instance()->sys().debug("added device: " + udi);
    m_devices[udi] = new NetworkDevice(conn(), udi);
}


void
NetworkManager::deviceRemovedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    DBus::Path udi;
    it >> udi;
    Log::instance()->sys().debug("removed device: " + udi);
    m_devices.erase(udi);
}


void
NetworkManager::stateChangedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    
    uint32_t newState;
    it >> newState;
    Log::instance()->sys().debug("network state changed to: " + stateName(static_cast<NMState>(newState)));
}


std::string
NetworkManager::stateName(NMState state)
{
    switch (state) {
        case NM_STATE_UNKNOWN:
            return "NM_STATE_UNKNOWN";
        case NM_STATE_ASLEEP:
            return "NM_STATE_ASLEEP";
        case NM_STATE_CONNECTING:
            return "NM_STATE_CONNECTING";
        case NM_STATE_CONNECTED:
            return "NM_STATE_CONNECTED";
        case NM_STATE_DISCONNECTED:
            return "NM_STATE_DISCONNECTED";
    }
}


}  // namespace Sys
} // namespace Omm
