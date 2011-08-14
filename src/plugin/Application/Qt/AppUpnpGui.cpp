/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#include "AppUpnpGui.h"


DeviceGroupModel::DeviceGroupModel(const std::string& deviceType, const std::string& shortName) :
DeviceGroup(deviceType, shortName)
{

}


DeviceGroupModel::DeviceGroupModel(Omm::DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    
}


void
DeviceGroupModel::addDevice(Omm::Device* pDevice, int index, bool begin)
{
    if (!begin) {
        // WidgetListModel interface
        insertItem(index);
    }
}


void
DeviceGroupModel::removeDevice(Omm::Device* pDevice, int index, bool begin)
{
    if (begin) {
        // WidgetListModel interface
        removeItem(index);
    }
}


void
DeviceGroupModel::selectDevice(Omm::Device* pDevice, int index)
{
}


void
DeviceGroupModel::addDeviceContainer(Omm::DeviceContainer* pDeviceContainer, int index, bool begin)
{
}


void
DeviceGroupModel::removeDeviceContainer(Omm::DeviceContainer* pDeviceContainer, int index, bool begin)
{
}


int
DeviceGroupModel::totalItemCount()
{
    return getDeviceCount();
}


void
DeviceGroupModel::selectItem(int row)
{
    Omm::Device* pDevice = getDevice(row);
    Omm::DeviceGroup::selectDevice(pDevice);
}

