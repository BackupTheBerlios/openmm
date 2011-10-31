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

#include <Poco/Notification.h>

#include "Upnp.h"
#include "UpnpAvCtlRenderer.h"
#include "UpnpAvCtlServer.h"
#include "UpnpAvCtlObject2.h"
#include "Sys.h"

#include "Gui/Tab.h"
#include "Gui/Navigator.h"
#include "Gui/ListModel.h"
#include "Gui/List.h"
#include "Gui/LazyList.h"
#include "Gui/ListItem.h"
#include "Gui/Button.h"
#include "Gui/Label.h"
#include "Gui/Slider.h"
#include "Gui/HorizontalLayout.h"


namespace Omm {

class MediaObjectModel;
class GuiVisual;
class MediaServerGroupWidget;
class MediaRendererGroupWidget;


class TransportStateNotification : public Poco::Notification
{
public:
    TransportStateNotification(const std::string& uuid, const std::string& transportState) : _uuid(uuid), _transportState(transportState) {}

    std::string         _uuid;
    std::string         _transportState;
};


class ControllerWidget : public Controller, public Gui::Tab
{
public:
    ControllerWidget();

    GuiVisual* getLocalRendererVisual();
    void setLocalRendererUuid(const std::string& uuid);
    void newTransportState(TransportStateNotification* pNotification);
    void showMainMenu();

private:
    MediaServerGroupWidget*     _pMediaServerGroupWidget;
    MediaRendererGroupWidget*   _pMediaRendererGroupWidget;
    GuiVisual*                  _pVisual;
    std::string                 _localRendererUuid;
};


class KeyController : public Gui::Controller
{
public:
    KeyController(ControllerWidget* pControllerWidget) : _pControllerWidget(pControllerWidget) {}

private:
    virtual void keyPressed(KeyCode key);

    ControllerWidget*   _pControllerWidget;
};


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

    virtual void showDeviceGroup();

    // ListModel interface
    virtual int totalItemCount();

    // ListController interface
    virtual void selectedItem(int row);

protected:
    virtual void init() {}

    Gui::ListView   _deviceGroupListView;
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


class MediaRendererDevice : public Av::CtlMediaRenderer, public Gui::Model
{
public:
    MediaRendererDevice() : _transportState("") {}

    std::string getTransportState();

private:
    virtual void newTransportState(const std::string& transportState);

    std::string _transportState;
};


class MediaRendererView : public Gui::View
{
public:
    MediaRendererView();

    void selectedRenderer();

private:
    virtual void syncViewImpl();

    Gui::HorizontalLayout   _layout;
    Gui::Button*            _pBackButton;
    Gui::Button*            _pPlayButton;
    Gui::Button*            _pStopButton;
    Gui::Button*            _pForwButton;
    Gui::Slider*            _pVolSlider;
    Gui::Slider*            _pSeekSlider;
    Gui::Label*             _pRendererName;
};


class MediaServerGroupWidget : public DeviceGroupWidget
{
public:
    MediaServerGroupWidget();

    // Omm::DeviceGroup interface
    virtual Device* createDevice();

    // ListModel interface
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
    virtual void selectedItem(int row);
};


class MediaServerDevice : public Av::CtlMediaServer, public Gui::ListItemModel
{
public:
    virtual void initController();
    virtual Av::CtlMediaObject2* createMediaObject();
};


class MediaServerView : public Gui::ListItemView
{

};


class MediaContainerWidget : public Gui::LazyListView, Gui::LazyListModel, Gui::ListController
{
    friend class MediaServerGroupWidget;

public:
    MediaContainerWidget(View* pParent = 0);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // LazyListModel interface
    virtual int fetch(int rowCount = 10, bool forward = true);
    virtual int lastFetched(bool forward = true);

    // ListController interface
    virtual void selectedItem(int row);

private:
    MediaObjectModel*           _pObjectModel;
    MediaServerGroupWidget*     _pServerGroup;
};


class MediaObjectModel : public Av::CtlMediaObject2, public Gui::ListItemModel
{
    friend class MediaContainerWidget;

public:
    MediaObjectModel();

private:
    class MediaObjectLabelModel : public Gui::LabelModel
    {
    public:
        MediaObjectLabelModel(MediaObjectModel* pSuperModel) : _pSuperModel(pSuperModel) {}

        virtual std::string getLabel();

        MediaObjectModel*   _pSuperModel;
    };

    MediaContainerWidget*     _pContainer;
    static Gui::ImageModel*   _pContainerImageModel;
    static Gui::ImageModel*   _pItemImageModel;
    static Gui::ImageModel*   _pItemAudioItemModel;
    static Gui::ImageModel*   _pItemImageItemModel;
    static Gui::ImageModel*   _pItemVideoItemModel;
    static Gui::ImageModel*   _pItemAudioBroadcastModel;
    static Gui::ImageModel*   _pItemVideoBroadcastModel;
};


class MediaObjectView : public Gui::ListItemView
{

};


class GuiVisual : public Gui::Image, public Sys::Visual
{
public:
    GuiVisual(Gui::View* pParent = 0);
    virtual ~GuiVisual();

    virtual void show();
    virtual void hide();

    virtual WindowHandle getWindow();
    virtual VisualType getType();
    virtual void renderImage(const std::string& imageData);
    virtual void blank();
};


}  // namespace Omm

#endif
