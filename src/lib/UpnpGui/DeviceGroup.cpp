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

#include "Poco/Util/Application.h"

#include "UpnpGui/DeviceGroup.h"

#include "Util.h"
#include "Gui/GuiLogger.h"

#include "MediaImages.h"


namespace Omm {


DeviceGroupWidget::DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    LOGNS(Gui, gui, debug, "device group widget ctor (delegate)");
    attachController(this);
    if (Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        setBackgroundColor(Gui::Color(200, 200, 200, 255));
    }
//    else {
//        _deviceGroupListView.setBackgroundColor(Gui::Color("white"));
//    }
}


void
DeviceGroupWidget::addDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        syncViews();
        if (pDevice->getUuid() == _defaultDeviceUuid) {
            _deviceGroupListView.selectRow(index);
        }
    }
}


void
DeviceGroupWidget::removeDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        if (getSelectedDevice() && pDevice->getUuid() == getSelectedDevice()->getUuid()) {
            popToRoot();
        }
        syncViews();
    }
}


//void
//DeviceGroupWidget::showDeviceGroup()
//{
//    LOGNS(Gui, gui, debug, "device group widget show device group");
//    ControllerWidget* pController = static_cast<ControllerWidget*>(getController());
//    pController->insertView(this, shortName());
//}


int
DeviceGroupWidget::totalItemCount()
{
    LOGNS(Gui, gui, debug, "device group widget total item count returns: " + Poco::NumberFormatter::format(getDeviceCount()));
    return getDeviceCount();
}


void
DeviceGroupWidget::selectedItem(int row)
{
    LOGNS(Gui, gui, debug, "device group widget selected device");
    Device* pDevice = static_cast<Device*>(getDevice(row));
    DeviceGroup::selectDevice(pDevice);
}


void
DeviceGroupWidget::setDefaultDevice(Device* pDevice)
{
    _defaultDeviceUuid = pDevice->getUuid();
}


Gui::ListView&
DeviceGroupWidget::getDeviceGroupView()
{
    return _deviceGroupListView;
}


DeviceGroupNavigatorController::DeviceGroupNavigatorController(DeviceGroupWidget* pDeviceGroupWidget) :
_pDeviceGroupWidget(pDeviceGroupWidget)
{
}


void
DeviceGroupNavigatorController::changedSearchText(const std::string& searchText)
{
    _pDeviceGroupWidget->changedSearchText(searchText);
}


void
DeviceGroupNavigatorController::rightButtonPushed()
{
    _pDeviceGroupWidget->rightButtonPushed();
}

} // namespace Omm
