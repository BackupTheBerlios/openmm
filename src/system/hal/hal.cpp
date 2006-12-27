/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "hal.h"
#include "debug.h"


HalDevice::HalDevice(DBus::Connection& connection, DBus::Path& udi)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Device"),
   DBus::ObjectProxy(connection, udi, "org.freedesktop.Hal")
{
    connect_signal(HalDevice, PropertyModified, propertyModifiedCb);
    connect_signal(HalDevice, Condition, conditionCb);
}


map<string, DBus::Variant>
HalDevice::getAllProperties()
{
    map<string, DBus::Variant> props;
    DBus::CallMessage call;

    call.member("GetAllProperties");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();

    it >> props;
    TRACE("HalDevice::getAllProperties() found properties:");
    printProperties(props);
    return props;
}


void
HalDevice::printProperties(map<string, DBus::Variant> props)
{
    map<string, DBus::Variant>::iterator iter;
    for( iter = props.begin(); iter != props.end(); iter++ ) {
        TRACE("    %s: %s", iter->first.c_str(), iter->second.signature() == "s" ? 
                iter->second.reader().get_string() : iter->second.signature().c_str());
    }
}


void
HalDevice::propertyModifiedCb(const DBus::SignalMessage& sig)
{
    typedef DBus::Struct<string, DBus::Bool, DBus::Bool> HalProperty;

    DBus::MessageIter it = sig.reader();
    DBus::Int32 number;
    it >> number;
    DBus::MessageIter arr = it.recurse();

    for(int i = 0; i < number; ++i, ++arr) {
        HalProperty hp;
        arr >> hp;
        TRACE("HalDevice::PropertyModifiedCb() modified property %s", hp._1.c_str());
    }
}


void
HalDevice::conditionCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    string condition;
    it >> condition;
    TRACE("HalDevice::ConditionCb() encountered condition %s", condition.c_str());
}


HalManager::HalManager(DBus::Connection& connection)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Manager"),
   DBus::ObjectProxy(connection, "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal")
{
    connect_signal(HalManager, DeviceAdded, deviceAddedCb);
    connect_signal(HalManager, DeviceRemoved, deviceRemovedCb);

    vector<string> devices = getAllDevices();

    vector<string>::iterator it;
    for(it = devices.begin(); it != devices.end(); ++it) {
        DBus::Path udi = *it;
        TRACE("HalManager::HalManager() found device %s", udi.c_str());
        m_devices[udi] = new HalDevice(connection, udi);
        m_properties[udi] = m_devices[udi]->getAllProperties();
    }
}


vector<string>
HalManager::getAllDevices()
{
    vector<string> udis;
    DBus::CallMessage call;

    call.member("GetAllDevices");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();

    it >> udis;
    return udis;
}


void
HalManager::deviceAddedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    string devname;

    it >> devname;
    DBus::Path udi(devname);
    m_devices[devname] = new HalDevice(conn(), udi);
    TRACE("HalManager::DeviceAddedCb() added device %s", udi.c_str());
    m_properties[devname] = m_devices[devname]->getAllProperties();
}


void
HalManager::deviceRemovedCb(const DBus::SignalMessage& sig)
{
    DBus::MessageIter it = sig.reader();
    string devname;

    it >> devname;
    TRACE("HalManager::DeviceRemovedCb() removed device %s", devname.c_str());
    m_devices.erase(devname);
    m_properties.erase(devname);
}
