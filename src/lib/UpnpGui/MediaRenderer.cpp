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
    _deviceGroupListView.setItemViewHeight(88);
#else
    _deviceGroupListView.setItemViewHeight(44);
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
    LOGNS(Gui, gui, debug, "media renderer group widget selected renderer: " + Poco::NumberFormatter::format(row));
    DeviceGroupWidget::selectedItem(row);
    _pControllerWidget->getControlPanel()->setModel(static_cast<MediaRendererDevice*>(getDevice(row)));
}


void
MediaRendererDevice::initController()
{
    _rendererName.setLabel(getFriendlyName());
    _volume.setValue(getVolume());
    _position.setValue(0);
}


void
MediaRendererDevice::newUri(const std::string& uri)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new uri: " + uri);
//    _trackName.setLabel(uri);
    Av::Connection* pConnection = getConnectionManager()->getConnection(0);
    if (pConnection) {
        std::string serverUuid = pConnection->getServer().getConnectionManagerId().getUuid();
        MediaServerDevice* pServer = _pControllerWidget->getServer(serverUuid);
        if (pServer) {
            LOGNS(Gui, gui, debug, "media renderer device connected to server: \"" + pServer->getFriendlyName() + "\"");
            Av::CtlMediaObject* pObject = pServer->getMediaObjectFromResource(uri);
            if (pObject) {
                LOGNS(Gui, gui, debug, "media renderer device playing object with title: \"" + pObject->getTitle() + "\"");
                _trackName.setLabel(pObject->getTitle());
            }
        }
    }
    syncViews();
}


void
MediaRendererDevice::newTrack(const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new track: " + title + ", " + artist + ", " + album + ", " + objectClass);
    _trackName.setLabel(artist == "" ? title : artist + " - " + title);
    syncViews();
    Poco::NotificationCenter::defaultCenter().postNotification(new TrackNotification(getUuid(), title, artist, album, objectClass));
}


void
MediaRendererDevice::newPosition(int duration, int position)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new position: " + Poco::NumberFormatter::format(position) + ", duration: " + Poco::NumberFormatter::format(duration));
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" position slider: " + Poco::NumberFormatter::format(((float)position / duration) * 100.0));
    _duration = duration;
    if (duration == 0) {
        _position.setValue(0);
    }
    else {
        _position.setValue(((float)position / duration) * 100);
    }
    syncViews();
}


void
MediaRendererDevice::newVolume(const int volume)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new volume: " + Poco::NumberFormatter::format(volume));
//    _volume = volume;
    _volume.setValue(volume);
    syncViews();
}


void
MediaRendererDevice::newTransportState(const std::string& transportState)
{
    LOGNS(Gui, gui, debug, "media renderer device \"" + getFriendlyName() + "\" new transport state: " + transportState);
    _transportState = transportState;
    syncViews();
    // TODO: deactivated position timer for now, should fix it.
//    startPositionTimer(transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING);
    Poco::NotificationCenter::defaultCenter().postNotification(new TransportStateNotification(getUuid(), transportState));
    // FIXME: from "new transport state STOPPED" to UPNP.CONTROL action response sent (StopResponse) it takes nearly one
    // second, sometimes.
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
    BackButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-backward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setSizeConstraint(45, 40, Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->backPressed();
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
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->playPressed();
    }

    virtual bool getEnabled()
    {
        return true;

        LOGNS(Gui, gui, debug, "media renderer play button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            LOGNS(Gui, gui, debug, "media renderer play button get enabled, transport state: " + transportState);
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
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->stopPressed();
    }

    virtual bool getEnabled()
    {
        return true;

        LOGNS(Gui, gui, debug, "media renderer stop button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            LOGNS(Gui, gui, debug, "media renderer stop button get enabled, transport state: " + transportState);
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
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->forwardPressed();
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
//        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->volumeChanged(value);
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
//        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->positionMoved(value / 100.0 * pRenderer->_duration);
        // TODO: seek position value should be multiplied with lenght of track
    }
};


MediaRendererView::MediaRendererView()
{
    setName("media renderer view");

//    setBackgroundColor(Gui::Color("blue"));

    _pBackButton = new BackButton(this);
    _pPlayButton = new PlayButton(this);
    _pStopButton = new StopButton(this);
    _pForwButton = new ForwButton(this);
    _pVolSlider = new VolSlider(this);
    _pSeekSlider = new SeekSlider(this);
    _pRendererName = new Gui::LabelView(this);
//    _pRendererName = new RendererName(this);
//    _pRendererName->setAlignment(Gui::View::AlignCenter);
    _pRendererName->setAlignment(Gui::View::AlignLeft);
    _pTrackName = new Gui::LabelView(this);
    _pTrackName->setAlignment(Gui::View::AlignCenter);
//    _pTrackName->setAlignment(Gui::View::AlignLeft);

#ifdef __IPHONE__
    _pBackButton->resize(40, 40);
    _pBackButton->move(0, 2);
    _pPlayButton->resize(40, 40);
    _pPlayButton->move(40, 2);
    _pStopButton->resize(40, 40);
    _pStopButton->move(80, 2);
    _pForwButton->resize(40, 40);
    _pForwButton->move(120, 2);

    _pVolSlider->resize(70, 40);
    _pVolSlider->move(160, 2);
    _pSeekSlider->resize(70, 40);
    _pSeekSlider->move(230, 2);

    _pRendererName->resize(120, 40);
    _pRendererName->move(0, 44);
    _pTrackName->resize(120, 40);
    _pTrackName->move(122, 44);

    resize(300, 88);
#else
    _pBackButton->resize(45, 40);
    _pBackButton->move(0, 2);
    _pPlayButton->resize(45, 40);
    _pPlayButton->move(45, 2);
    _pStopButton->resize(45, 40);
    _pStopButton->move(90, 2);
    _pForwButton->resize(45, 40);
    _pForwButton->move(135, 2);

    _pVolSlider->resize(100, 40);
    _pVolSlider->move(180, 2);
    _pSeekSlider->resize(100, 40);
    _pSeekSlider->move(280, 2);

    _pRendererName->resize(120, 40);
    _pRendererName->move(384, 2);
    _pTrackName->resize(120, 40);
    _pTrackName->move(508, 2);

    resize(620, 44);
#endif

//    setSizeConstraint(400, 40, Gui::View::Pref);
//    resize(800, 20);
//    setLayout(&_layout);
}


void
MediaRendererView::setModel(Gui::Model* pModel)
{
    _pRendererName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_rendererName);
    _pTrackName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_trackName);
    _pSeekSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_position);
    _pVolSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_volume);
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
    _pSeekSlider->syncViewImpl();
    _pVolSlider->syncViewImpl();
    _pTrackName->syncViewImpl();
}


} // namespace Omm
