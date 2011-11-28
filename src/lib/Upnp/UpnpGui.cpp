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
    _pMediaRendererGroupWidget = new MediaRendererGroupWidget(this);
    registerDeviceGroup(_pMediaRendererGroupWidget);
    _pVisual = new GuiVisual;
    addView(_pVisual, "Video");
    _pControlPanel = new MediaRendererView;
    _pActivityIndicator = new ActivityIndicator;
//    _pStatusBar->resize(20, 20);

    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, TransportStateNotification>(*this, &ControllerWidget::newTransportState));
    attachController(new KeyController(this));
}


GuiVisual*
ControllerWidget::getLocalRendererVisual()
{
    return _pVisual;
}


MediaRendererView*
ControllerWidget::getControlPanel()
{
    return _pControlPanel;
}


Gui::View*
ControllerWidget::getStatusBar()
{
    return _pActivityIndicator;
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
ControllerWidget::signalNetworkActivity(bool on)
{
    on ? _pActivityIndicator->startActivity() : _pActivityIndicator->stopActivity();
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
            _deviceGroupListView.selectRow(index);
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


MediaRendererGroupWidget::MediaRendererGroupWidget(ControllerWidget* pControllerWidget) :
DeviceGroupWidget(new Av::MediaRendererGroupDelegate),
_pControllerWidget(pControllerWidget)
{
//    Gui::Log::instance()->gui().debug("media renderer group widget ctor");
    View::setName("media renderer group view");
    _deviceGroupListView.setName("media renderer group view");
    push(&_deviceGroupListView, "Player");

    _deviceGroupListView.setItemViewHeight(80);
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
MediaRendererGroupWidget::selectedItem(int row)
{
    DeviceGroupWidget::selectedItem(row);
    _pControllerWidget->getControlPanel()->setModel(static_cast<MediaRendererDevice*>(getDevice(row)));
}


void
MediaRendererDevice::newTrack(const std::string& title, const std::string& artist, const std::string& album)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new track: " + title + ", " + artist + ", " + album);
}


void
MediaRendererDevice::newVolume(const int volume)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new volume: " + Poco::NumberFormatter::format(volume));
    _volume = volume;
    syncViews();
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


ui2
MediaRendererDevice::getVolume()
{
    if (_volume == -1) {
        return Av::CtlMediaRenderer::getVolume();
    }
    else {
        return _volume;
    }
}


class BackButton : public Gui::Button
{
public:
    BackButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-backward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
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
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
        setEnabled(false);
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
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
        setEnabled(false);
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
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
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
    VolSlider(Gui::View* pParent = 0) : Gui::Slider(pParent)
    {
        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->volumeChanged(value);
    }

    virtual const int getValue() const
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(static_cast<MediaRendererView*>(_pParent)->getModel());
        return pRenderer->getVolume();
    }

    virtual bool getEnabled()
    {
        if (_pModel) {
            return true;
        }
        else {
            return false;
        }
    }
};


class SeekSlider : public Gui::Slider
{
public:
    SeekSlider(Gui::View* pParent = 0) : Gui::Slider(pParent)
    {
        setEnabled(false);
    }

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

    setSizeConstraint(800, 40, Gui::View::Pref);
    resize(800, 40);
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
    _pVolSlider->syncViewImpl();
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
        pContainer->_pObjectModel = pRootObject;
        pContainer->_pServerGroup = this;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        push(pContainer, pServer->getFriendlyName());
    }
}


void
MediaServerDevice::initController()
{
    Gui::LabelModel* pLabelModel = new Gui::LabelModel;
    pLabelModel->setLabel(getFriendlyName());
    setLabelModel(pLabelModel);

    Icon* pIcon = 0;
    for (IconIterator it = beginIcon(); it != endIcon(); ++it) {
        pIcon = *it;
    }
    Gui::ImageModel* pImageModel = new Gui::ImageModel;
    if (pIcon) {
        pImageModel->setData(pIcon->getBuffer());
    }
    else {
        pImageModel->setData(MediaImages::instance()->getResource("media-server.png"));
    }
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
        Omm::Icon* pIcon = pModel->getIcon();
//        Omm::Icon* pIcon = pModel->getImageRepresentation();
        if (pIcon) {
            Omm::Gui::ImageModel* pImage = new Omm::Gui::ImageModel;
            pImage->setData(pIcon->getBuffer());
            pModel->setImageModel(pImage);
        }
        else {
            pModel->setImageModel(MediaObjectModel::_pItemImageItemModel);
        }
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
        pContainer->_pObjectModel = pChildObject;
        pContainer->_pServerGroup = _pServerGroup;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        _pServerGroup->push(pContainer, pChildObject->getTitle());
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


void*
GuiVisual::getWindow()
{
    return Gui::View::getNativeView();
}


GuiVisual::WindowHandle
GuiVisual::getWindowId()
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


ActivityIndicator::ActivityIndicator(Gui::View* pParent) :
Gui::ImageView(pParent),
_indicateDuration(250),
_activityInProgress(false),
_indicatorOn(false),
_timerActive(false),
_pOffTimer(0),
_stopIndicatorCallback(*this, &ActivityIndicator::stopIndicator)
{
    _pActivityOffModel = new Gui::ImageModel;
    _pActivityOffModel->setData(MediaImages::instance()->getResource("activity-off.png"));
    _pActivityOnModel = new Gui::ImageModel;
    _pActivityOnModel->setData(MediaImages::instance()->getResource("activity-on.png"));
//    resize(15, 15);
    setModel(_pActivityOffModel);
}


ActivityIndicator::~ActivityIndicator()
{
}


void
ActivityIndicator::startActivity()
{
//    Gui::Log::instance()->gui().debug("activity indicator start activity");

    setActivityInProgress(true);
    if (!indicatorOn()) {
        setIndicatorOn(true);
        setModel(_pActivityOnModel);
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator already on, do nothing");
    }
}


void
ActivityIndicator::stopActivity()
{
//    Gui::Log::instance()->gui().debug("activity indicator stop activity");

    setActivityInProgress(false);
    if (indicatorOn() && !timerActive()) {
//        Gui::Log::instance()->gui().debug("turn off indicator after short delay ...");
        setTimerActive(true);
        if (_pOffTimer) {
            delete _pOffTimer;
        }
        _pOffTimer = new Poco::Timer;
        _pOffTimer->setStartInterval(_indicateDuration);
        _pOffTimer->start(_stopIndicatorCallback);
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator already off or timer running, do nothing");
    }
}


void
ActivityIndicator::stopIndicator(Poco::Timer& timer)
{
//    Gui::Log::instance()->gui().debug("activity indicator stop timer callback ...");

    if (!activityInProgress() && indicatorOn()) {
        setModel(_pActivityOffModel);
        setIndicatorOn(false);
//        Gui::Log::instance()->gui().debug("indicator turned off, no activity in progress anymore");
    }
    else {
//        Gui::Log::instance()->gui().debug("turn off indicator ignored, activity still in progress or indicator already off");
    }
    setTimerActive(false);
//    Gui::Log::instance()->gui().debug("activity indicator stop timer callback finished.");
}


void
ActivityIndicator::setActivityInProgress(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    if (set) {
//        Gui::Log::instance()->gui().debug("indicator flag \"activity in progress\" set to true");
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator flag \"activity in progress\" set to false");
    }
    _activityInProgress = set;
}


bool
ActivityIndicator::activityInProgress()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    return _activityInProgress;
}


void
ActivityIndicator::setIndicatorOn(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    if (set) {
//        Gui::Log::instance()->gui().debug("flag \"indicator on\" set to true");
    }
    else {
//        Gui::Log::instance()->gui().debug("flag \"indicator on\" set to false");
    }
    _indicatorOn = set;
}


bool
ActivityIndicator::indicatorOn()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    return _indicatorOn;
}


bool
ActivityIndicator::timerActive()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    return _timerActive;
}


void
ActivityIndicator::setTimerActive(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    _timerActive = set;
}



} // namespace Omm
