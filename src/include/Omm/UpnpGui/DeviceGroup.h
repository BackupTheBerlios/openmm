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

#ifndef DeviceGroup_INCLUDED
#define DeviceGroup_INCLUDED

#include "../Upnp.h"

#include "../Gui/Navigator.h"
#include "../Gui/ListModel.h"
#include "../Gui/List.h"


namespace Omm {

class DeviceGroupWidget : public DeviceGroup, public Gui::NavigatorView, public Gui::ListModel, public Gui::ListController
{
public:
    DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate);

    // DeviceGroup interface
    // explicit add / remove is not necessary, view pulls any changes done in the model via syncViewImpl()
    // TODO: only locking of model should be done, when updating the model or pulling data from the model.
    virtual void addDevice(Device* pDevice, int index, bool begin);
    virtual void removeDevice(Device* pDevice, int index, bool begin);
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin) {}

//    virtual void showDeviceGroup();

    // ListModel interface
    virtual int totalItemCount();

    // ListController interface
    virtual void selectedItem(int row);

    // NavigatorController delegate interface
    virtual void changedSearchText(const std::string& searchText) {}
    virtual void rightButtonPushed() {}

    void setDefaultDevice(Device* pDevice);
    Gui::ListView& getDeviceGroupView();

protected:
    virtual void init() {}

    Gui::ListView   _deviceGroupListView;
    std::string     _defaultDeviceUuid;
};


class DeviceGroupNavigatorController : public Gui::NavigatorController
{
public:
    DeviceGroupNavigatorController(DeviceGroupWidget* pDeviceGroupWidget);

    virtual void changedSearchText(const std::string& searchText);
    virtual void rightButtonPushed();

private:
    DeviceGroupWidget*  _pDeviceGroupWidget;
};


}  // namespace Omm


#endif
