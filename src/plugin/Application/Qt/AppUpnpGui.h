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

#ifndef AppUpnpGui_INCLUDED
#define AppUpnpGui_INCLUDED

#include <Omm/Upnp.h>
#include "AppGui.h"


class DeviceGroupModel : public Omm::DeviceGroup, public WidgetListModel
{
public:
    DeviceGroupModel(const std::string& deviceType, const std::string& shortName);
    DeviceGroupModel(Omm::DeviceGroupDelegate* pDeviceGroupDelegate);

    virtual void addDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void removeDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void selectDevice(Omm::Device* pDevice, int index);
    virtual void addDeviceContainer(Omm::DeviceContainer* pDeviceContainer, int index, bool begin);
    virtual void removeDeviceContainer(Omm::DeviceContainer* pDeviceContainer, int index, bool begin);

    // WidgetListModel interface
    virtual int totalItemCount();
    virtual void selectItem(int row);

protected:
    virtual void init() {}
};


#endif
