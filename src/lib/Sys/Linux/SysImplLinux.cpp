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

#include "Poco/Environment.h"

#include "Log.h"
#include "Sys/SysLogger.h"
#include "SysImplLinux.h"

namespace Omm {
namespace Sys {


const std::string Sys::System::DeviceTypeOther("");
const std::string Sys::System::DeviceTypeDvb("dvb");
const std::string Sys::System::DeviceTypeDisk("block");


SysImpl::SysImpl()
{
    _pUdev = udev_new();
    if (!_pUdev) {
        LOG(sys, error, "initialization of udev failed");
    }
}


SysImpl::~SysImpl()
{
    udev_unref(_pUdev);
}


const std::string
SysImpl::getPath(SysPath::Location loc)
{
    switch (loc) {
        case SysPath::Home:
            return Poco::Environment::get("HOME", "");
        case SysPath::Cache:
            return "/var/cache";
        case SysPath::Tmp:
            return "/tmp";
        default:
            return "";
    }
}


void
SysImpl::getDevicesForType(std::vector<Device*>& devices, const std::string& deviceType)
{
    struct udev_enumerate* pUdevEnumerate = udev_enumerate_new(_pUdev);

    udev_enumerate_add_match_subsystem(pUdevEnumerate, deviceType.c_str());
	udev_enumerate_scan_devices(pUdevEnumerate);
    struct udev_list_entry* deviceList;
    deviceList = udev_enumerate_get_list_entry(pUdevEnumerate);

    struct udev_list_entry* deviceIterator;
    udev_list_entry_foreach(deviceIterator, deviceList) {
        std::string deviceId(udev_list_entry_get_name(deviceIterator));
		udev_device* device = udev_device_new_from_syspath(_pUdev, deviceId.c_str());
        std::string deviceNode(udev_device_get_devnode(device));

        devices.push_back(new Device(deviceId, deviceType, deviceNode));

        udev_device_unref(device);
    }

    udev_enumerate_unref(pUdevEnumerate);
}


}  // namespace Sys
} // namespace Omm
