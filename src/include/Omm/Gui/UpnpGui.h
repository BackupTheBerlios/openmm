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
#include "../UpnpAvCtlRenderer.h"

#include "TabView.h"
#include "ListModel.h"
#include "ListView.h"
#include "ListItemView.h"


namespace Omm {
namespace Gui {


class ControllerGui : public Controller, public TabView
{
public:
    ControllerGui();
};


class DeviceGroupModel : public DeviceGroup, public ListModel
{
public:
    DeviceGroupModel(const std::string& deviceType, const std::string& shortName);
    DeviceGroupModel(DeviceGroupDelegate* pDeviceGroupDelegate);

    // DeviceGroup interface
    virtual void addDevice(Device* pDevice, int index, bool begin);
    virtual void removeDevice(Device* pDevice, int index, bool begin);
    virtual void selectDevice(Device* pDevice, int index);
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);

    // ListModel interface
    virtual int totalItemCount();
    virtual void selectItem(int row);

protected:
    virtual void init() {}
};


class MediaRendererGroupView : public DeviceGroupModel, public ListView
{
public:
    MediaRendererGroupView();

//    // ListModel interface
    virtual ListItemView* createView();
    virtual ListItemView* getChildView(int row);
//    virtual void attachWidget(int row, ListWidget* pView);
//    virtual void detachWidget(int row);

    // Omm::DeviceGroup interface
    virtual Device* createDevice();
//    virtual void show();
};


class MediaRendererDevice : public Av::CtlMediaRenderer
{
//    void setDeviceWidget(QtMediaRendererWidget* pView);
//    QtMediaRendererWidget* getDeviceWidget();
//
//    virtual void initController();
//
//private:
//    QtMediaRendererWidget*            _pMediaRendererWidget;
};


class MediaRendererView : public ListItemView
{

};


}  // namespace Omm
}  // namespace Gui

#endif
