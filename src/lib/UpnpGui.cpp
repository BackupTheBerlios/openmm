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

#include "UpnpGui.h"
#include "Gui/GuiLogger.h"


namespace Omm {


ControllerWidget::ControllerWidget()
{
    Gui::Log::instance()->gui().debug("controller widget register device groups ...");
    registerDeviceGroup(new MediaServerGroupWidget);
    registerDeviceGroup(new MediaRendererGroupWidget);
}


DeviceGroupWidget::DeviceGroupWidget(const std::string& deviceType, const std::string& shortName) :
DeviceGroup(deviceType, shortName)
{
    Gui::Log::instance()->gui().debug("device group widget ctor (device type, short name)");
}


DeviceGroupWidget::DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    Gui::Log::instance()->gui().debug("device group widget ctor (delegate)");
}


void
DeviceGroupWidget::addDevice(Device* pDevice, int index, bool begin)
{
    Gui::Log::instance()->gui().debug("device group widget add device");
    if (!begin) {
        Gui::ListModel::insertItem(index);
    }
}


void
DeviceGroupWidget::removeDevice(Device* pDevice, int index, bool begin)
{
    if (begin) {
        Gui::ListModel::removeItem(index);
    }
}


void
DeviceGroupWidget::addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin)
{
}


void
DeviceGroupWidget::removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin)
{
}


void
DeviceGroupWidget::showDeviceGroup()
{
    Gui::Log::instance()->gui().debug("device group widget show device group");
    ControllerWidget* pController = static_cast<ControllerWidget*>(getController());
    pController->addView(this, shortName());
}


int
DeviceGroupWidget::totalItemCount()
{
    Gui::Log::instance()->gui().debug("device group widget total item count returns: " + Poco::NumberFormatter::format(getDeviceCount()));
    return getDeviceCount();
}


void
DeviceGroupWidget::selectedItem(int row)
{
    Device* pDevice = getDevice(row);
    DeviceGroup::selectDevice(pDevice);
}


MediaRendererGroupWidget::MediaRendererGroupWidget() :
DeviceGroupWidget(new Av::MediaRendererGroupDelegate)
{
    Gui::Log::instance()->gui().debug("media renderer group widget ctor");
    View::setName("media renderer group view");
    _deviceGroupList.setModel(this);
    push(&_deviceGroupList, ">");
}


Device*
MediaRendererGroupWidget::createDevice()
{
    Gui::Log::instance()->gui().debug("media renderer group widget create renderer device.");
    return new MediaRendererDevice;
}


Gui::View*
MediaRendererGroupWidget::createItemView()
{
    Gui::Log::instance()->gui().debug("media renderer group widget create renderer view.");
    return new MediaRendererView;
}


Gui::Model*
MediaRendererGroupWidget::getItemModel(int row)
{
    return static_cast<MediaRendererDevice*>(getDevice(row));
}


void
MediaRendererDevice::initController()
{
    setLabel(getFriendlyName());
}


MediaServerGroupWidget::MediaServerGroupWidget() :
DeviceGroupWidget(new Av::MediaServerGroupDelegate)
{
    Gui::Log::instance()->gui().debug("media server group widget ctor");
    View::setName("media server group view");
    _deviceGroupList.setModel(this);
    push(&_deviceGroupList, ">");
}


Device*
MediaServerGroupWidget::createDevice()
{
    Gui::Log::instance()->gui().debug("media server group widget create server device.");
    return new MediaServerDevice;
}


Gui::View*
MediaServerGroupWidget::createItemView()
{
    Gui::Log::instance()->gui().debug("media server group widget create server view.");
    return new MediaServerView;
}


Gui::Model*
MediaServerGroupWidget::getItemModel(int row)
{
    return static_cast<MediaServerDevice*>(getDevice(row));
}


void
MediaServerDevice::initController()
{
    setLabel(getFriendlyName());
}


} // namespace Omm
