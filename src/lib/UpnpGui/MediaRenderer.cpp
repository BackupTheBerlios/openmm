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

#include <cmath>

#include "Util.h"
#include "Gui/GuiLogger.h"

#include "UpnpGui/ControllerWidget.h"
#include "UpnpGui/MediaRenderer.h"
#include "UpnpGui/MediaServer.h"

#include "MediaImages.h"


namespace Omm {

MediaRendererGroupWidget::MediaRendererGroupWidget(ControllerWidget* pControllerWidget) :
DeviceGroupWidget(new Av::MediaRendererGroupDelegate),
_pControllerWidget(pControllerWidget)
{
//    LOGNS(Gui, gui, debug, "media renderer group widget ctor");
    View::setName("Player");
    _deviceGroupListView.setName("Player");
    showNavigatorBar(false);
    push(&_deviceGroupListView, "Player");

#ifdef __IPHONE__
    _deviceGroupListView.setItemViewHeight(3 * 60);
#else
    _deviceGroupListView.setItemViewHeight(3 * 30);
#endif
//    _deviceGroupListView.setSelectionType(Gui::ListView::Frame);
    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
}


Device*
MediaRendererGroupWidget::createDevice()
{
//    LOGNS(Gui, gui, debug, "media renderer group widget create renderer device.");
    return new MediaRendererDevice(_pControllerWidget);
}


Gui::View*
MediaRendererGroupWidget::createItemView()
{
//    LOGNS(Gui, gui, debug, "media renderer group widget create renderer view.");
    return new MediaRendererView(true);
}


Gui::Model*
MediaRendererGroupWidget::getItemModel(int row)
{
    return static_cast<MediaRendererDevice*>(getDevice(row));
}


void
MediaRendererGroupWidget::selectedItem(int row)
{
    LOGNS(Gui, gui, debug, "media renderer group widget selected renderer: " + Poco::NumberFormatter::format(row));
    DeviceGroupWidget::selectedItem(row);
    _pControllerWidget->getControlPanel()->setModel(static_cast<MediaRendererDevice*>(getDevice(row)));
}


void
MediaRendererDevice::initController()
{
    _rendererName.setLabel(getFriendlyName());
    _trackName.setLabel("- no track -");
    _volumeLabel.setLabel(Poco::NumberFormatter::format(getVolume()) + "%");
    _positionLabel.setLabel("00:00:00/00:00:00");
    _volume.setValue(getVolume());
    _position.setValue(0);
}


void
MediaRendererDevice::newUri(const std::string& uri)
{
    // set track name for tracks in a playlist (only uri of current track is known, nothing else)
    // connection via connection manager must be available
    if (!_featureTrackInfoFromConnection) {
        return;
    }
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new uri: " + uri);
    Av::Connection* pConnection = getConnectionManager()->getConnection(0);
    if (pConnection) {
        std::string serverUuid = pConnection->getServer().getConnectionManagerId().getUuid();
        MediaServerDevice* pServer = _pControllerWidget->getServer(serverUuid);
        if (pServer) {
            LOGNS(Gui, gui, debug, "media renderer device connected to server: \"" + pServer->getFriendlyName() + "\"");
            Av::CtlMediaObject* pObject = pServer->getMediaObjectFromResource(uri);
            if (pObject) {
                LOGNS(Gui, gui, debug, "media renderer device playing object with title: \"" + pObject->getTitle() + "\"");
                Av::AbstractProperty* pArtist = pObject->getProperty(Av::AvProperty::ARTIST);
                Av::AbstractProperty* pAlbum = pObject->getProperty(Av::AvProperty::ALBUM);
                _trackName.setLabel(pServer->getFriendlyName() + ": " + (pArtist ? pArtist->getValue() + " - " + pObject->getTitle() : pObject->getTitle()));
                syncViews();
            }
        }
    }
}


void
MediaRendererDevice::newTrack(const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass, const std::string& server, const std::string& uri)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new track: " + title + ", " + artist + ", " + album + ", " + objectClass + ", " + server + ", " + uri);

    // set track name, when metadata of track is available for renderer
    if (!_featureTrackInfoFromConnection || !Av::AvClass::matchClass(objectClass, Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)) {
        _trackName.setLabel((server.size() ? server + ": " : "") + (artist == "" ? title : artist + " - " + title));
        syncViews();
    //    Poco::NotificationCenter::defaultCenter().postNotification(new TrackNotification(getUuid(), title, artist, album, objectClass));
    }
}


void
MediaRendererDevice::newPosition(r8 duration, r8 position)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new position: " + Poco::NumberFormatter::format(position, 1) + ", duration: " + Poco::NumberFormatter::format(duration, 1));
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" position slider: " + Poco::NumberFormatter::format(((r8)position / duration) * 100.0));
    _duration = duration;
    if (duration == 0.0) {
        _position.setValue(0);
    }
    else {
        _position.setValue(((r8)position / duration) * 100);
    }
    _positionLabel.setLabel(formatDuration(position) + "/" + formatDuration(duration));
    syncViews();
}


void
MediaRendererDevice::newVolume(const int volume)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new volume: " + Poco::NumberFormatter::format(volume));
    _volumeLabel.setLabel(Poco::NumberFormatter::format(volume) + "%");
    _volume.setValue(volume);
    syncViews();
}


void
MediaRendererDevice::newTransportState(const std::string& transportState)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new transport state: " + transportState);
    _transportState = transportState;
    syncViews();
    if (_featurePollPosition) {
        startPositionTimer(transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING);
    }
    Poco::NotificationCenter::defaultCenter().postNotification(new TransportStateNotification(getUuid(), transportState));
    // FIXME: from "new transport state STOPPED" to UPNP.CONTROL action response sent (StopResponse) it takes nearly one
    // second, sometimes.
}


std::string
MediaRendererDevice::formatDuration(r8 duration)
{
    int hours = duration / 3600.0;
    int minutes = (duration - hours * 3600) / 60.0;
    int seconds = floor(duration - hours * 3600 - minutes * 60);
    return
        Poco::NumberFormatter::format0(hours, 2) + ":" +
        Poco::NumberFormatter::format0(minutes, 2) + ":" +
        Poco::NumberFormatter::format0(seconds, 2);
}


std::string
MediaRendererDevice::getTransportState()
{
    // FIXME: track name should be retrieved from CtlService::TransportState of the corresponding AVTransport instance.
    //        Don't store it in a separate class member.
    return _transportState;
}


class BackButton : public Gui::Button
{
public:
    BackButton(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::Button(pParent), _pRendererView(pRendererView)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-backward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setSizeConstraint(45, 40, Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pRendererView->getModel());
        if (pRenderer) {
            pRenderer->backPressed();
        }
    }

    MediaRendererView*      _pRendererView;
    Gui::Image              _image;
};


class PlayButton : public Gui::Button
{
public:
    PlayButton(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::Button(pParent), _pRendererView(pRendererView)
    {
        _image.setData(MediaImages::instance()->getResource("media-start.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pRendererView->getModel());
        if (pRenderer) {
            pRenderer->playPressed();
        }
    }

    virtual bool getEnabled()
    {
        return true;

        LOGNS(Gui, gui, debug, "media renderer play button get enabled");
        if (_pRendererView && _pRendererView->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pRendererView->getModel())->getTransportState();
            LOGNS(Gui, gui, debug, "media renderer play button get enabled, transport state: " + transportState);
            return (transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
    }

    MediaRendererView*      _pRendererView;
    Gui::Image              _image;
};


class StopButton : public Gui::Button
{
public:
    StopButton(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::Button(pParent), _pRendererView(pRendererView)
    {
        _image.setData(MediaImages::instance()->getResource("media-stop.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pRendererView->getModel());
        if (pRenderer) {
            pRenderer->stopPressed();
        }
    }

    virtual bool getEnabled()
    {
        return true;

        LOGNS(Gui, gui, debug, "media renderer stop button get enabled");
        if (_pRendererView && _pRendererView->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pRendererView->getModel())->getTransportState();
            LOGNS(Gui, gui, debug, "media renderer stop button get enabled, transport state: " + transportState);
            return (transportState != Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
        return false;
    }

    MediaRendererView*      _pRendererView;
    Gui::Image              _image;
};


class ForwButton : public Gui::Button
{
public:
    ForwButton(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::Button(pParent), _pRendererView(pRendererView)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-forward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pRendererView->getModel());
        if (pRenderer) {
            pRenderer->forwardPressed();
        }
    }

    MediaRendererView*      _pRendererView;
    Gui::Image              _image;
};


class VolSeekView : public Gui::View, public Gui::Controller
{
    friend class MediaRendererView;

    VolSeekView(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::View(pParent), _pRendererView(pRendererView)
    {
    }

    virtual void selected()
    {
        _pRendererView->switchVolSeekSlider();
    }

    MediaRendererView*      _pRendererView;
};


class VolLabel : public Gui::LabelView
{
public:
    VolLabel(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::LabelView(pParent), _pRendererView(pRendererView)
    {
//        _image.setData(MediaImages::instance()->getResource("media-skip-forward.png"));
//        setImage(&_image);
//        setLabel("00%");
        setSizeConstraint(75, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(true);
    }

    void select(bool select)
    {
        if (select) {
            setBackgroundColor(Gui::Color(200, 200, 200));
        }
        else {
            setBackgroundColor(Gui::Color("white"));
        }
    }
//    virtual void selected()
//    {
//        _pRendererView->switchVolSeekSlider();
//    }

    MediaRendererView*      _pRendererView;
};


class SeekLabel : public Gui::LabelView
{
public:
    SeekLabel(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::LabelView(pParent), _pRendererView(pRendererView)
    {
//        _image.setData(MediaImages::instance()->getResource("media-skip-forward.png"));
//        setImage(&_image);
//        setLabel("00:00:00");
        setSizeConstraint(75, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(true);
    }

    void select(bool select)
    {
        if (select) {
            setBackgroundColor(Gui::Color(200, 200, 200));
        }
        else {
            setBackgroundColor(Gui::Color("white"));
        }
    }
//    virtual void selected()
//    {
//        _pRendererView->switchVolSeekSlider();
//    }

    MediaRendererView*      _pRendererView;
};


class VolSeekSlider : public Gui::Slider
{
public:
    VolSeekSlider(MediaRendererView* pRendererView, Gui::View* pParent = 0) : Gui::Slider(pParent), _pRendererView(pRendererView)
    {
        setBackgroundColor(Gui::Color(200, 200, 200));
//        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pRendererView->getModel());
        if (pRenderer) {
            if (_pRendererView->_modeVolume) {
                pRenderer->volumeChanged(value);
            }
            else {
                int sliderMaxVal = 100;
                pRenderer->positionMoved((float)value / sliderMaxVal * pRenderer->_duration);
            }
        }
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

    MediaRendererView*      _pRendererView;
};


class MediaRendererLayout : public Gui::Layout
{
    virtual void layoutView()
    {
        MediaRendererView* pRendererView = static_cast<MediaRendererView*>(_pView);

        int margin = 5;
        int width = pRendererView->width() - 2 * margin;
        int height = pRendererView->height() - 2 * margin;
        if (pRendererView->_lineBreak) {
            pRendererView->_pButtonPanel->resize(width, height / 3);
            pRendererView->_pButtonPanel->move(margin, margin);
            pRendererView->_pSliderPanel->resize(width, height / 3);
            pRendererView->_pSliderPanel->move(margin, margin + height / 3);
            pRendererView->_pLabelPanel->resize(width, height / 3);
            pRendererView->_pLabelPanel->move(margin, margin + 2 * height / 3);
        }
        else {
            pRendererView->_pButtonPanel->resize(width / 3, height);
            pRendererView->_pButtonPanel->move(margin, margin);
            pRendererView->_pSliderPanel->resize(width / 3, height);
            pRendererView->_pSliderPanel->move(margin + width / 3, margin);
            pRendererView->_pLabelPanel->resize(width / 3, height);
            pRendererView->_pLabelPanel->move(margin + 2 * width / 3, margin);
        }
    }

};


class MediaRendererController : public Gui::Controller
{
    friend class MediaRendererView;

    MediaRendererController(MediaRendererView* pRendererView) : _pRendererView(pRendererView) {}

    virtual void selected()
    {
        _pRendererView->selectedRenderer();
    }

    MediaRendererView*  _pRendererView;
};


MediaRendererView::MediaRendererView(bool lineBreak) :
_lineBreak(lineBreak),
_modeVolume(true)
{
    setName("media renderer view");

    _pButtonPanel = new Gui::View(this);
    _pButtonPanel->setLayout(new Gui::HorizontalLayout);
    _pButtonPanel->attachController(new MediaRendererController(this));

    _pRendererName = new Gui::LabelView(_pButtonPanel);
//    _pRendererName = new RendererName(this);
//    _pRendererName->setAlignment(Gui::View::AlignCenter);
    _pRendererName->setAlignment(Gui::View::AlignLeft);
    _pBackButton = new BackButton(this, _pButtonPanel);
    _pPlayButton = new PlayButton(this, _pButtonPanel);
    _pStopButton = new StopButton(this, _pButtonPanel);
    _pForwButton = new ForwButton(this, _pButtonPanel);

    _pSliderPanel = new Gui::View(this);
    _pSliderPanel->setLayout(new Gui::HorizontalLayout);
    _pSliderPanel->attachController(new MediaRendererController(this));

    VolSeekView* pVolSeekView = new VolSeekView(this, _pSliderPanel);
    pVolSeekView->setLayout(new Gui::VerticalLayout);
    pVolSeekView->setName("VolSeekView");
    pVolSeekView->attachController(pVolSeekView);
    _pVolLabel = new VolLabel(this, pVolSeekView);
    _pVolLabel->setName("VolLabel");
    _pVolLabel->select(true);
    _pVolLabel->attachController(new MediaRendererController(this));
    _pSeekLabel = new SeekLabel(this, pVolSeekView);
    _pSeekLabel->setName("SeekLabel");
    _pSeekLabel->attachController(new MediaRendererController(this));
    _pVolSeekSlider = new VolSeekSlider(this, _pSliderPanel);

    _pLabelPanel = new Gui::View(this);
    _pLabelPanel->setLayout(new Gui::HorizontalLayout);
    _pLabelPanel->attachController(new MediaRendererController(this));

    _pTrackName = new Gui::LabelView(_pLabelPanel);
//    _pTrackName->setAlignment(Gui::View::AlignCenter);
    _pTrackName->setAlignment(Gui::View::AlignLeft);

    setLayout(new MediaRendererLayout);

#ifdef __IPHONE__
    resize(300, 60);
#else
    resize(620, 30);
#endif
}


void
MediaRendererView::setModel(Gui::Model* pModel)
{
    _pRendererName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_rendererName);
    _pTrackName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_trackName);
    _pVolLabel->setModel(&static_cast<MediaRendererDevice*>(pModel)->_volumeLabel);
    _pSeekLabel->setModel(&static_cast<MediaRendererDevice*>(pModel)->_positionLabel);
    if (_modeVolume) {
        _pVolSeekSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_volume);
    }
    else {
        _pVolSeekSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_position);
    }
    Gui::View::setModel(pModel);
}


void
MediaRendererView::selectedRenderer()
{
    NOTIFY_CONTROLLER(Gui::Controller, selected);
}


void
MediaRendererView::syncViewImpl()
{
//    LOGNS(Gui, gui, debug, "media renderer view sync view impl");
    // FIXME: submodels should by synced implicitely
    _pRendererName->syncViewImpl();
    _pPlayButton->syncViewImpl();
    _pStopButton->syncViewImpl();
    _pVolLabel->syncViewImpl();
    _pSeekLabel->syncViewImpl();
    _pVolSeekSlider->syncViewImpl();
    _pTrackName->syncViewImpl();
}


void
MediaRendererView::switchVolSeekSlider()
{
    _modeVolume = !_modeVolume;
    if (_modeVolume) {
        _pVolSeekSlider->setModel(&static_cast<MediaRendererDevice*>(getModel())->_volume);
        _pVolLabel->select(true);
        _pSeekLabel->select(false);
    }
    else {
        _pVolSeekSlider->setModel(&static_cast<MediaRendererDevice*>(getModel())->_position);
        _pVolLabel->select(false);
        _pSeekLabel->select(true);
    }
}


} // namespace Omm
