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

#include "Gui/UpnpGui.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {

DeviceGroupModel::DeviceGroupModel(const std::string& deviceType, const std::string& shortName) :
DeviceGroup(deviceType, shortName)
{

}


DeviceGroupModel::DeviceGroupModel(DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    
}


void
DeviceGroupModel::addDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        // WidgetListModel interface
        insertItem(index);
    }
}


void
DeviceGroupModel::removeDevice(Device* pDevice, int index, bool begin)
{
    if (begin) {
        // WidgetListModel interface
        removeItem(index);
    }
}


void
DeviceGroupModel::selectDevice(Device* pDevice, int index)
{
}


void
DeviceGroupModel::addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin)
{
}


void
DeviceGroupModel::removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin)
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
    Device* pDevice = getDevice(row);
    DeviceGroup::selectDevice(pDevice);
}


ControllerGui::ControllerGui()
{
    registerDeviceGroup(new MediaRendererGroupView);
}


MediaRendererGroupView::MediaRendererGroupView() :
DeviceGroupModel(new Av::MediaRendererGroupDelegate),
ListView(50)
{
    // TODO: this populates the list view, need to fully implement renderer view first.
//    setModel(this);
}


ListWidget*
MediaRendererGroupView::createWidget()
{
    Log::instance()->gui().debug("media renderer group view create renderer view.");
    return new MediaRendererView;
}


ListWidget*
MediaRendererGroupView::getChildWidget(int row)
{
    MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(getDevice(row));
//    return pRenderer->getDeviceWidget();
}


Device*
MediaRendererGroupView::createDevice()
{
    Log::instance()->gui().debug("media renderer group view create renderer device.");
    return new MediaRendererDevice;
}


} // namespace Gui
} // namespace Omm
