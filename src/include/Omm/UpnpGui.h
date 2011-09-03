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

#include "Upnp.h"
#include "UpnpAvCtlRenderer.h"

#include "Gui/Tab.h"
#include "Gui/ListModel.h"
#include "Gui/List.h"
#include "Gui/ListItem.h"
#include "Gui/Button.h"


namespace Omm {


class ControllerWidget : public Controller, public Gui::Tab
{
public:
    ControllerWidget();

//    virtual void addDeviceGroup(DeviceGroup* pDeviceGroup, bool begin);
};


class DeviceGroupWidget : public DeviceGroup, public Gui::ListView, public Gui::ListController, public Gui::ListModel
{
public:
    DeviceGroupWidget(const std::string& deviceType, const std::string& shortName);
    DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate);

    // DeviceGroup interface
    virtual void addDevice(Device* pDevice, int index, bool begin);
    virtual void removeDevice(Device* pDevice, int index, bool begin);
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer, int index, bool begin);
    virtual void showDeviceGroup();

    // ListModel interface
    virtual int totalItemCount();

    // ListController interface
    virtual void selectedItem(int row);

protected:
    virtual void init() {}
};


class MediaRendererGroupWidget : public DeviceGroupWidget
{
public:
    MediaRendererGroupWidget();

    // Omm::DeviceGroup interface
    virtual Device* createDevice();

    // ListModel interface
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);
};


class MediaRendererDevice : public Av::CtlMediaRenderer, public Gui::ButtonModel
{
public:
    virtual void initController();
};


class MediaRendererView : public Gui::ButtonView
{

};


}  // namespace Omm

#endif
