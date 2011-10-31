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

#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>

#include "UpnpGui.h"
#include "Gui/GuiLogger.h"
#include "UpnpAv.h"
#include "UpnpAvCtlServer.h"
#include "MediaImages.h"

namespace Omm {


ControllerWidget::ControllerWidget()
{
    Gui::Log::instance()->gui().debug("controller widget register device groups ...");
    _pMediaServerGroupWidget = new MediaServerGroupWidget;
    registerDeviceGroup(_pMediaServerGroupWidget);
    _pMediaRendererGroupWidget = new MediaRendererGroupWidget;
    registerDeviceGroup(_pMediaRendererGroupWidget);
    _pVisual = new GuiVisual;
    addView(_pVisual, "Video");

    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, TransportStateNotification>(*this, &ControllerWidget::newTransportState));
    attachController(new KeyController(this));
}


GuiVisual*
ControllerWidget::getLocalRendererVisual()
{
    return _pVisual;
}


void
ControllerWidget::setDefaultRenderer(Omm::Av::MediaRenderer* pRenderer)
{
    _localRendererUuid = pRenderer->getUuid();
    _pMediaRendererGroupWidget->setDefaultDevice(pRenderer);
}


void
ControllerWidget::newTransportState(TransportStateNotification* pNotification)
{
    Gui::Log::instance()->gui().debug("controller widget device: " + pNotification->_uuid + " got new transport state: " + pNotification->_transportState);
    Gui::Log::instance()->gui().debug("local renderer uuid: " + _localRendererUuid);
    if (pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING && pNotification->_uuid == _localRendererUuid) {
        setCurrentView(_pVisual);
    }
}


void
ControllerWidget::showMainMenu()
{
    setCurrentView(_pMediaServerGroupWidget);
}


void
ControllerWidget::navigateListWithKey(Gui::Controller::KeyCode key)
{
    if (_pMediaServerGroupWidget->getVisibleView()) {
        _pMediaServerGroupWidget->getVisibleView()->triggerKeyEvent(key);
    }
}


void
ControllerWidget::back()
{
    _pMediaServerGroupWidget->pop();
}


void
KeyController::keyPressed(KeyCode key)
{
    Gui::Log::instance()->gui().debug("key controller, key pressed: " + Poco::NumberFormatter::format(key));
    switch (key) {
        case Gui::Controller::KeyMenu:
            _pControllerWidget->showMainMenu();
            break;
        case Gui::Controller::KeyLeft:
            if (_pControllerWidget->getCurrentTab()) {
                _pControllerWidget->setCurrentTab(_pControllerWidget->getCurrentTab() - 1);
            }
            break;
        case Gui::Controller::KeyRight:
            if (_pControllerWidget->getCurrentTab() < _pControllerWidget->getTabCount() - 1) {
                _pControllerWidget->setCurrentTab(_pControllerWidget->getCurrentTab() + 1);
            }
            break;
        case Gui::Controller::KeyUp:
        case Gui::Controller::KeyDown:
        case Gui::Controller::KeyReturn:
            _pControllerWidget->navigateListWithKey(key);
            break;
        case Gui::Controller::KeyBack:
            _pControllerWidget->back();
            break;
    }
}


DeviceGroupWidget::DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    Gui::Log::instance()->gui().debug("device group widget ctor (delegate)");
    attachController(this);
}


void
DeviceGroupWidget::addDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        syncViews();
        if (pDevice->getUuid() == _defaultDeviceUuid) {
            selectDevice(pDevice);
        }
    }
}


void
DeviceGroupWidget::removeDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        syncViews();
    }
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
    Gui::Log::instance()->gui().debug("device group widget selected device");
    Device* pDevice = static_cast<Device*>(getDevice(row));
    DeviceGroup::selectDevice(pDevice);
}


void
DeviceGroupWidget::setDefaultDevice(Device* pDevice)
{
    _defaultDeviceUuid = pDevice->getUuid();
}


MediaRendererGroupWidget::MediaRendererGroupWidget() :
DeviceGroupWidget(new Av::MediaRendererGroupDelegate)
{
//    Gui::Log::instance()->gui().debug("media renderer group widget ctor");
    View::setName("media renderer group view");
    _deviceGroupListView.setName("media renderer group view");
    push(&_deviceGroupListView, "Player");

    _deviceGroupListView.setItemViewHeight(50);
    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
}


Device*
MediaRendererGroupWidget::createDevice()
{
//    Gui::Log::instance()->gui().debug("media renderer group widget create renderer device.");
    return new MediaRendererDevice;
}


Gui::View*
MediaRendererGroupWidget::createItemView()
{
//    Gui::Log::instance()->gui().debug("media renderer group widget create renderer view.");
    return new MediaRendererView;
}


Gui::Model*
MediaRendererGroupWidget::getItemModel(int row)
{
    return static_cast<MediaRendererDevice*>(getDevice(row));
}


void
MediaRendererDevice::newTransportState(const std::string& transportState)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new transport state: " + transportState);
    _transportState = transportState;
    syncViews();
    Poco::NotificationCenter::defaultCenter().postNotification(new TransportStateNotification(getUuid(), transportState));
}


std::string
MediaRendererDevice::getTransportState()
{
    return _transportState;
}


class BackButton : public Gui::Button
{
public:
    BackButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-backward.png"));
        setImage(&_image);
        setSizeConstraint(25, height(Gui::View::Pref), Gui::View::Pref);
        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
//        pRenderer->playPressed();
    }

    Gui::Image _image;
};


class PlayButton : public Gui::Button
{
public:
    PlayButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-start.png"));
        setImage(&_image);
        setSizeConstraint(30, height(Gui::View::Pref), Gui::View::Pref);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->playPressed();
    }

    virtual bool getEnabled()
    {
        Gui::Log::instance()->gui().debug("media renderer play button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            Gui::Log::instance()->gui().debug("media renderer play button get enabled, transport state: " + transportState);
            return (transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
    }

    Gui::Image _image;
};


class StopButton : public Gui::Button
{
public:
    StopButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-stop.png"));
        setImage(&_image);
        setSizeConstraint(25, height(Gui::View::Pref), Gui::View::Pref);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->stopPressed();
    }

    virtual bool getEnabled()
    {
        Gui::Log::instance()->gui().debug("media renderer stop button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            Gui::Log::instance()->gui().debug("media renderer stop button get enabled, transport state: " + transportState);
            return (transportState != Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
    }

    Gui::Image _image;
};


class ForwButton : public Gui::Button
{
public:
    ForwButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-forward.png"));
        setImage(&_image);
        setSizeConstraint(25, height(Gui::View::Pref), Gui::View::Pref);
        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
//        pRenderer->playPressed();
    }

    Gui::Image _image;
};


class RendererButton : public Gui::Button
{
public:
    RendererButton(Gui::View* pParent = 0) : Gui::Button(pParent) {}

    virtual void pushed()
    {
        MediaRendererView* pRenderer = static_cast<MediaRendererView*>(_pParent);
        pRenderer->selectedRenderer();
    }
};


class VolSlider : public Gui::Slider
{
public:
    VolSlider(Gui::View* pParent = 0) : Gui::Slider(pParent) {}

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->volumeChanged(value);
    }
};


class SeekSlider : public Gui::Slider
{
public:
    SeekSlider(Gui::View* pParent = 0) : Gui::Slider(pParent) {}

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->positionMoved(value);
    }
};


class RendererName : public Gui::Label
{
public:
    RendererName(Gui::View* pParent = 0) : Gui::Label(pParent) {}

    virtual std::string getLabel()
    {
//        Gui::Log::instance()->gui().debug("media renderer name get label");
        return static_cast<MediaRendererDevice*>(static_cast<MediaRendererView*>(_pParent)->getModel())->getFriendlyName();
    }
};


MediaRendererView::MediaRendererView()
{
    setName("media renderer view");

    _pBackButton = new BackButton(this);
    _pPlayButton = new PlayButton(this);
    _pStopButton = new StopButton(this);
    _pForwButton = new ForwButton(this);

    _pVolSlider = new VolSlider(this);
//    _pSeekSlider = new SeekSlider(this);

    _pRendererName = new RendererName(this);
    _pRendererName->setAlignment(Gui::View::AlignCenter);

    setLayout(&_layout);
}


void
MediaRendererView::selectedRenderer()
{
    NOTIFY_CONTROLLER(Gui::Controller, selected);
}


void
MediaRendererView::syncViewImpl()
{
    Gui::Log::instance()->gui().debug("media renderer view sync view impl");
    _pRendererName->syncViewImpl();
    _pPlayButton->syncViewImpl();
    _pStopButton->syncViewImpl();
}


MediaServerGroupWidget::MediaServerGroupWidget() :
DeviceGroupWidget(new Av::MediaServerGroupDelegate)
{
//    Gui::Log::instance()->gui().debug("media server group widget ctor");
    View::setName("media server group view");
    _deviceGroupListView.setName("media server group view");
    push(&_deviceGroupListView, "Media");

    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
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
MediaServerGroupWidget::selectedItem(int row)
{
    Gui::Log::instance()->gui().debug("media server group widget selected device");
    MediaServerDevice* pServer = static_cast<MediaServerDevice*>(getDevice(row));
    DeviceGroup::selectDevice(pServer);
    pServer->browseRootObject();
    MediaObjectModel* pRootObject = static_cast<MediaObjectModel*>(pServer->getRootObject());
    if (pRootObject->isContainer()) {
        Gui::Log::instance()->gui().debug("media server group widget selected device has container as root object");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        push(pContainer, pServer->getFriendlyName());

        pContainer->_pObjectModel = pRootObject;
        pContainer->_pServerGroup = this;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
    }
}


void
MediaServerDevice::initController()
{
    Gui::LabelModel* pLabelModel = new Gui::LabelModel;
    pLabelModel->setLabel(getFriendlyName());
    setLabelModel(pLabelModel);

    Gui::ImageModel* pImageModel = new Gui::ImageModel;
    pImageModel->setData(MediaImages::instance()->getResource("media-server.png"));
    setImageModel(pImageModel);
}


Av::CtlMediaObject2*
MediaServerDevice::createMediaObject()
{
    return new MediaObjectModel;
}


MediaContainerWidget::MediaContainerWidget(View* pParent) :
LazyListView(pParent)
{
//    setItemViewHeight(30);
}


int
MediaContainerWidget::totalItemCount()
{
    Gui::Log::instance()->gui().debug("media container widget total item count: " + Poco::NumberFormatter::format(_pObjectModel->getTotalChildCount()));
    return _pObjectModel->getTotalChildCount();
}


Gui::View*
MediaContainerWidget::createItemView()
{
    return new MediaObjectView;
}


Gui::Model*
MediaContainerWidget::getItemModel(int row)
{
    Gui::Log::instance()->gui().debug("media container widget get item model in row: " + Poco::NumberFormatter::format(row));
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pObjectModel->getChildForRow(row));
    // FIXME: this can be done once, when the model is created.
    Av::AbstractProperty* pClass = pModel->getProperty(Av::AvProperty::CLASS);
    if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::CONTAINER)) {
        pModel->setImageModel(MediaObjectModel::_pContainerImageModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::AUDIO_ITEM)) {
        pModel->setImageModel(MediaObjectModel::_pItemAudioItemModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::IMAGE_ITEM)) {
        pModel->setImageModel(MediaObjectModel::_pItemImageItemModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::VIDEO_ITEM)) {
        pModel->setImageModel(MediaObjectModel::_pItemVideoItemModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::AUDIO_BROADCAST)) {
        pModel->setImageModel(MediaObjectModel::_pItemAudioBroadcastModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::VIDEO_BROADCAST)) {
        pModel->setImageModel(MediaObjectModel::_pItemVideoBroadcastModel);
    }
    else {
        pModel->setImageModel(MediaObjectModel::_pItemImageModel);
    }
    return pModel;
}


int
MediaContainerWidget::fetch(int rowCount, bool forward)
{
    Gui::Log::instance()->gui().debug("media container widget fetch count items: " + Poco::NumberFormatter::format(rowCount));
    if (!forward) {
        return 0;
    }
    return _pObjectModel->fetchChildren(rowCount);
}


int
MediaContainerWidget::lastFetched(bool forward)
{
    Gui::Log::instance()->gui().debug("media container widget last fetched: " + Poco::NumberFormatter::format(_pObjectModel->getChildCount()));
    return _pObjectModel->getChildCount();
}


void
MediaContainerWidget::selectedItem(int row)
{
    Gui::Log::instance()->gui().debug("media container widget selected media object");
    MediaObjectModel* pChildObject = static_cast<MediaObjectModel*>(getItemModel(row));
    if (pChildObject->isContainer()) {
        Gui::Log::instance()->gui().debug("media container widget selected media container");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        _pServerGroup->push(pContainer, pChildObject->getTitle());

        pContainer->_pObjectModel = pChildObject;
        pContainer->_pServerGroup = _pServerGroup;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
    }
    else {
        Gui::Log::instance()->gui().debug("media container widget selected media item");
        _pObjectModel->getServer()->selectMediaObject(pChildObject);
    }
}


Gui::ImageModel*   MediaObjectModel::_pContainerImageModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemImageModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemAudioItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemImageItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemVideoItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemAudioBroadcastModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemVideoBroadcastModel = 0;

MediaObjectModel::MediaObjectModel()
{
    setLabelModel(new MediaObjectLabelModel(this));

    if (!_pContainerImageModel) {
        _pContainerImageModel = new Gui::Image;
        _pContainerImageModel->setData(MediaImages::instance()->getResource("media-container.png"));
    }
    if (!_pItemImageModel) {
        _pItemImageModel = new Gui::Image;
        _pItemImageModel->setData(MediaImages::instance()->getResource("media-item.png"));
    }
    if (!_pItemAudioItemModel) {
        _pItemAudioItemModel = new Gui::Image;
        _pItemAudioItemModel->setData(MediaImages::instance()->getResource("media-audio-item.png"));
    }
    if (!_pItemImageItemModel) {
        _pItemImageItemModel = new Gui::Image;
        _pItemImageItemModel->setData(MediaImages::instance()->getResource("media-image-item.png"));
    }
    if (!_pItemVideoItemModel) {
        _pItemVideoItemModel = new Gui::Image;
        _pItemVideoItemModel->setData(MediaImages::instance()->getResource("media-video-item.png"));
    }
    if (!_pItemAudioBroadcastModel) {
        _pItemAudioBroadcastModel = new Gui::Image;
        _pItemAudioBroadcastModel->setData(MediaImages::instance()->getResource("media-audio-broadcast.png"));
    }
    if (!_pItemVideoBroadcastModel) {
        _pItemVideoBroadcastModel = new Gui::Image;
        _pItemVideoBroadcastModel->setData(MediaImages::instance()->getResource("media-video-broadcast.png"));
    }
}


std::string
MediaObjectModel::MediaObjectLabelModel::getLabel()
{
    Av::AbstractProperty* pArtist = _pSuperModel->getProperty(Av::AvProperty::ARTIST);
    if (pArtist) {
        return  pArtist->getValue() + " - " + _pSuperModel->getTitle();
    }
    else {
        return _pSuperModel->getTitle();
    }
}


GuiVisual::GuiVisual(Gui::View* pParent)
{
    setBackgroundColor(Gui::Color("black"));
}


GuiVisual::~GuiVisual()
{
}


void
GuiVisual::show()
{
    Gui::View::show();
}


void
GuiVisual::hide()
{
    Gui::View::hide();
}


GuiVisual::WindowHandle
GuiVisual::getWindow()
{
    return (GuiVisual::WindowHandle)Gui::View::getNativeWindowId();
//    return winId();
}


Sys::Visual::VisualType
GuiVisual::getType()
{
    // GuiVisual is multi-platform, and type of visual is platform dependent.
#ifdef __LINUX__
    return Omm::Sys::Visual::VTX11;
#elif __DARWIN__
    return Omm::Sys::Visual::VTOSX;
#elif __WINDOWS__
    return Omm::Sys::Visual::VTWin;
#else
    return Omm::Sys::Visual::VTNone;
#endif
}


void
GuiVisual::renderImage(const std::string& imageData)
{
}


void
GuiVisual::blank()
{
}


} // namespace Omm
