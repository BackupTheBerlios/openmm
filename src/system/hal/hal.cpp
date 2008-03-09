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
#include "haldeviceevent.h"
#include "controler.h"
#include "debug.h"

#include <stdlib.h>
#include <sstream>


// TODO: use interface org.freedesktop.Hal.Device.Volume
//       and its methods Mount() and Unmount()
HalVolume::HalVolume(DBus::Connection& connection, DBus::Path& udi)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Device.Volume"),
   DBus::ObjectProxy(connection, udi, "org.freedesktop.Hal")
{
}


void
HalVolume::mount(string mountpoint, string fstype, vector<string> options)
// HalVolume::mount(string mountpoint, string fstype)
{
    DBus::CallMessage call;

    DBus::MessageIter itw = call.writer();
    itw.append_string(mountpoint.c_str());
    itw.append_string(fstype.c_str());
//     itw.append_array('s', &"", 0);
    DBus::MessageIter itsub = itw.new_array("s");
    itw.close_container(itsub);
    call.member("Mount");

/*
gives error:
3287: arguments to dbus_message_iter_append_fixed_array() were incorrect, assertion "dbus_type_is_fixed (element_type)" failed in file dbus-message.c line 2186.
This is normally a bug in some application using the D-BUS library.
*/

//     DBus::Message reply = invoke_method(call);
//     DBus::MessageIter it = reply.reader();
}


HalStorage::HalStorage(DBus::Connection& connection, DBus::Path& udi)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Device.Storage"),
   DBus::ObjectProxy(connection, udi, "org.freedesktop.Hal")
{
}


HalDevice::HalDevice(DBus::Connection& connection, DBus::Path& udi)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Device"),
   DBus::ObjectProxy(connection, udi, "org.freedesktop.Hal")
{
//     connect_signal(HalDevice, PropertyModified, propertyModifiedCb);
//     connect_signal(HalDevice, Condition, conditionCb);
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
/*    TRACE("HalDevice::getAllProperties() found properties:");
    printProperties(props);*/
    return props;
}


DBus::Variant
HalDevice::getProperty(string property)
{
    DBus::Variant prop;
    DBus::CallMessage call;
    DBus::MessageIter itw = call.writer();
    itw.append_string(property.c_str());

    call.member("GetProperty");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();
//     prop = reply.reader();
    TRACE("HalDevice::getProperty() returned");

    it >> prop;
    TRACE("HalDevice::getProperty() found property:");
    TRACE("    %s: %s", property.c_str(), prop.signature() == "s" ? 
                prop.reader().get_string() : prop.signature().c_str());
    return prop;
}


DBus::String
HalDevice::getPropertyString(string property)
{
    DBus::String prop;
    DBus::CallMessage call;
    DBus::MessageIter itw = call.writer();
    itw.append_string(property.c_str());

    call.member("GetPropertyString");

    DBus::Message reply = invoke_method(call);
    DBus::MessageIter it = reply.reader();
//     prop = reply.reader();
    TRACE("HalDevice::getProperty() returned");

    it >> prop;
    TRACE("HalDevice::getProperty() found property %s: %s", property.c_str(), prop.c_str());
    return prop;
}


void
HalDevice::printProperties(map<string, DBus::Variant> props)
{
    map<string, DBus::Variant>::iterator iter;
    for( iter = props.begin(); iter != props.end(); iter++ ) {
        string value;
        string signature = iter->second.signature();
        if (signature == "s") {
            value = iter->second.reader().get_string();
        }
        else if (signature == "b") {
            value = iter->second.reader().get_bool() ? "yes" : "no";
        }
        else if (signature == "i") {
            stringstream sstr;
            sstr << iter->second.reader().get_int32();
            value = sstr.str();
        }
        else {
            value = signature;
        }
        TRACE("    %s: %s", iter->first.c_str(), value.c_str());
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

// TODO: also react on button press on disc-drive -> umount and eject
// HalDevice::ConditionCb() encountered condition EjectPressed
// Problem is, which device is in the drive where the button is pressed?

}


HalManager::HalManager(DBus::Connection& connection)
 : DBus::InterfaceProxy("org.freedesktop.Hal.Manager"),
   DBus::ObjectProxy(connection, "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal"),
   maxAudioCard(-1)
{
    TRACE("HalManager::HalManager() connecting callbacks");
    connect_signal(HalManager, DeviceAdded, deviceAddedCb);
    connect_signal(HalManager, DeviceRemoved, deviceRemovedCb);

    TRACE("HalManager::HalManager() getting device information");
    vector<string> devices = getAllDevices();

    vector<string>::iterator it;
    for(it = devices.begin(); it != devices.end(); ++it) {
        DBus::Path udi = *it;
        TRACE("HalManager::HalManager() found device %s", udi.c_str());
        m_devices[udi] = new HalDevice(connection, udi);
        m_properties[udi] = m_devices[udi]->getAllProperties();
        handleDevice(udi, false);
    }
}


vector<string>
HalManager::getAllDevices()
{
    TRACE("HalManager::getAllDevices()");
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
    TRACE("HalManager::DeviceAddedCb() added device %s", udi.c_str());
    m_devices[devname] = new HalDevice(conn(), udi);
    m_properties[devname] = m_devices[devname]->getAllProperties();
    m_devices[devname]->printProperties(m_properties[devname]);
    handleDevice(devname, true);
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
    // send a device removed event (Controler should remove menu entries ...)
    Controler::instance()->queueEvent(new HalDeviceEvent(devname, HalDeviceEvent::Remove, HalDeviceEvent::AnyT));
}


void
HalManager::handleDevice(string devName, bool hotplug)
{
    // generate an event matching the type of the device
    if (m_properties[devName].find("info.category") != m_properties[devName].end()) {
        TRACE("HalManager::handleDevice() found device of category: %s",
            m_properties[devName]["info.category"].reader().get_string());
        // handle optical discs
        if (string(m_properties[devName]["info.category"].reader().get_string()) == "volume" &&
            m_properties[devName]["volume.is_disc"].reader().get_bool()) {
            string label = string(m_properties[devName]["volume.label"].reader().get_string());
            // VIDEO DVD
            if (m_properties[devName]["volume.disc.is_videodvd"].reader().get_bool()) {
                TRACE("HalManager::handleDevice() found DVD, throttling read speed to 8x");
                string device = m_properties[devName]["block.device"].reader().get_string();
                system(("eject -x 8 " + device).c_str());
                Controler::instance()->queueEvent(new HalDeviceEvent(devName, HalDeviceEvent::Add, HalDeviceEvent::DvdT,
                                    hotplug, device, "DVD: " + label));
            }
            // CD or DVD with data
            else {
                string fstype = m_properties[devName]["volume.fstype"].reader().get_string();
                string device = m_properties[devName]["block.device"].reader().get_string();
                TRACE("HalManager::handleDevice() found volume, trying to mount it.");
                // try to mount the volume
                system(("pmount -t" + fstype + " " + device).c_str());
                // if volume is disc, limit the read speed to 8
                if (m_properties[devName].find("volume.is_disc") != m_properties[devName].end() &&
                    m_properties[devName]["volume.is_disc"].reader().get_bool()) {
                    TRACE("HalManager::handleDevice() found disc, throttling read speed to 8x");
                    system(("eject -x 8 " + device).c_str());
                }

                // send an event of type volume with the path of the mount point
                string mountPoint = m_devices[devName]->getPropertyString("volume.mount_point");
                TRACE("HalManager::handleDevice() mount point is: %s", mountPoint.c_str());
                Controler::instance()->queueEvent(new HalDeviceEvent(devName, HalDeviceEvent::Add, HalDeviceEvent::VolumeT,
                                    hotplug, device, "Disc: " + label, mountPoint));
            }
        }
        // handle soundcards
/*        else if (string(m_properties[devName]["info.category"].reader().get_string()) == "oss") {
            if (string(m_properties[devName]["oss.type"].reader().get_string()) == "pcm" &&
                m_properties[devName]["oss.device"].reader().get_int32() == 0) {
                string device = string(m_properties[devName]["oss.device_file"].reader().get_string());
                if (m_properties[devName]["oss.card"].reader().get_int32() == 0) {
                    TRACE("HalManager::handleDevice() adding first sound card with device: %s", device.c_str());
                    Controler::instance()->queueEvent(new HalDeviceEvent(HalDeviceEvent::SoundT,
                                    hotplug, device));
                }
                else if (m_properties[devName]["oss.card"].reader().get_int32() == 1) {
                    TRACE("HalManager::handleDevice() adding second sound card with device: %s", device.c_str());
                    Controler::instance()->queueEvent(new HalDeviceEvent(HalDeviceEvent::SoundT,
                                    hotplug, device));
                }
            }*/
        else if (string(m_properties[devName]["info.category"].reader().get_string()) == "alsa") {
            if (string(m_properties[devName]["alsa.type"].reader().get_string()) == "playback") {
                int cardNum = m_properties[devName]["alsa.card"].reader().get_int32();
                if (cardNum > maxAudioCard) {
                    maxAudioCard = cardNum;
                    TRACE("HalManager::handleDevice() adding sound card number: %i", maxAudioCard);
                    Controler::instance()->queueEvent(new HalDeviceEvent(devName, HalDeviceEvent::Add, HalDeviceEvent::SoundT,
                                    hotplug, ""));
                }
            }
        }
    }
    else {
        TRACE("HalManager::handleDevice() found device without \"info.category\"");
    }
}
