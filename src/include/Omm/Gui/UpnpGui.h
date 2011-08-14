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

#ifndef UpnpGui_INCLUDED
#define UpnpGui_INCLUDED

#include "../Upnp.h"
#include "Gui.h"

namespace Omm {
namespace Gui {

class DeviceGroupModel : public DeviceGroup, public WidgetListModel
{
public:
    DeviceGroupModel(const std::string& deviceType, const std::string& shortName);
    DeviceGroupModel(DeviceGroupDelegate* pDeviceGroupDelegate);

    virtual void addDevice(Device* pDevice, int index, bool begin);
    virtual void removeDevice(Device* pDevice, int index, bool begin);
    virtual void selectDevice(Device* pDevice, int index);
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);

    // WidgetListModel interface
    virtual int totalItemCount();
    virtual void selectItem(int row);

protected:
    virtual void init() {}
};


}  // namespace Omm
}  // namespace Gui

#endif
