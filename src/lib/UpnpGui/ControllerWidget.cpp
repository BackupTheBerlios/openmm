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

#include <iostream>

#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/StreamCopier.h>
#include <Poco/NamedMutex.h>
#include <Poco/Util/Application.h>


#include "Util.h"
#include "UpnpAv.h"
#include "UpnpAvCtlServer.h"

#include "Gui/GuiLogger.h"
#include "Gui/View.h"

#include "UpnpGui/ActivityIndicator.h"
#include "UpnpGui/DeviceGroup.h"
#include "UpnpGui/GuiVisual.h"
#include "UpnpGui/MediaObject.h"
#include "UpnpGui/MediaRenderer.h"
#include "UpnpGui/MediaServer.h"
#include "UpnpGui/Playlist.h"
#include "UpnpGui/Setup.h"
#include "UpnpGui/UpnpApplication.h"
#include "UpnpGui/ControllerWidget.h"

#include "MediaImages.h"


namespace Omm {


class ControllerWidgetClusterController : public Omm::Gui::ClusterController
{
    friend class ControllerWidget;

    ControllerWidgetClusterController(ControllerWidget* pControllerWidget) : _pControllerWidget(pControllerWidget) {}

    virtual void movedView(Omm::Gui::View* pView)
    {
        LOGNS(Gui, gui, debug, "controller widget moved view: " + pView->getName());

        GuiVisual* pVisual = dynamic_cast<GuiVisual*>(pView);
        if (pVisual && _pControllerWidget->getLocalRenderer() && _pControllerWidget->getLocalRenderer()->getEngine()) {
            _pControllerWidget->getLocalRenderer()->getEngine()->setVisual(pVisual);
        }
    }

    ControllerWidget* _pControllerWidget;
};


ControllerWidget::ControllerWidget(UpnpApplication* pApplication) :
_pApplication(pApplication)
{
    LOGNS(Gui, gui, debug, "controller widget register device groups ...");

    if (!Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        _pSetup = new GuiSetup(_pApplication);
        _pSetup->setName("Setup");
        insertView(_pSetup, "Setup");
    }

    _pVisual = new GuiVisual;
    insertView(_pVisual, "Video");

    _pMediaRendererGroupWidget = new MediaRendererGroupWidget(this);
    registerDeviceGroup(_pMediaRendererGroupWidget);
    if (!Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        insertView(_pMediaRendererGroupWidget, "Player");
    }

    Omm::Gui::SplitterView* pSplitterView = new Omm::Gui::SplitterView(0, Omm::Gui::View::Vertical);
    pSplitterView->setName("Media");
    _pMediaServerGroupWidget = new MediaServerGroupWidget;
    registerDeviceGroup(_pMediaServerGroupWidget);
    pSplitterView->insertView(_pMediaServerGroupWidget, 0);
    if (!Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        _pPlaylistEditor = new PlaylistEditor(this);
        _pPlaylistEditorView = new PlaylistEditorView(_pPlaylistEditor);
        _pPlaylistEditorView->hide();
        pSplitterView->insertView(_pPlaylistEditorView, 1);
        Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget,
        PlaylistNotification>(*this, &ControllerWidget::playlistNotification));
    }
    insertView(pSplitterView, "Media");
    std::vector<float> splitterSizes;
    splitterSizes.push_back(0.8);
    splitterSizes.push_back(0.2);
    pSplitterView->setSizes(splitterSizes);

    _pControlPanel = new MediaRendererView;
    _pActivityIndicator = new ActivityIndicator;
    setCurrentViewIndex(getIndexFromView(_pMediaServerGroupWidget));
//    setConfiguration(Poco::Util::Application::instance().config().getString("application.cluster", "[0,0] Media,Setup [0,1] Player [1,0] List [1,1] Video"));
    if (!Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        setConfiguration(Poco::Util::Application::instance().config().getString("application.cluster", "[0,0] Media,Setup,Player,List,Video {800;480} {1,00} {1,00}"));
    }
    else {
        setConfiguration("[0,0] Media,Video {800;480} {1,00} {1,00}");
    }

    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, Av::StreamTypeNotification>(*this, &ControllerWidget::newStreamType));
//    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, TransportStateNotification>(*this, &ControllerWidget::newTransportState));
//    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, TrackNotification>(*this, &ControllerWidget::newTrack));
//    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, PlaylistNotification>(*this, &ControllerWidget::newPlaylist));
    attachController(new KeyController(this));
    attachController(new ControllerWidgetClusterController(this));
}


void
ControllerWidget::setState(State newState)
{
    LOG(upnp, debug, "controller widget state change: " + _state + " -> " + newState);
    if (_state == newState) {
        LOG(upnp, debug, "new state equal to old state, ignoring");
        return;
    }
    Controller::setState(newState);
    if (newState == Stopped) {

        // don't stop selected renderer, it should continue playing though the controller is stopped
//        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pMediaRendererGroupWidget->getSelectedDevice());
//        if (pRenderer) {
//            pRenderer->stopPressed();
//        }
        // stop position info timer for all renderer devices that controller knows of (renderers continue to play)
        for (int r = 0; r < _pMediaRendererGroupWidget->getDeviceCount(); r++) {
            MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pMediaRendererGroupWidget->getDevice(r));
            // TODO: deactivated position timer for now, should fix it.
//            pRenderer->startPositionTimer(false);
        }
        // clear device views and remove controller's device containers
        _pMediaServerGroupWidget->popToRoot();
        _pMediaServerGroupWidget->clearDevices();
//        _pMediaServerGroupWidget->getVisibleView()->syncView();
        _pMediaServerGroupWidget->getDeviceGroupView().syncView();
        _pMediaRendererGroupWidget->clearDevices();
//        _pMediaRendererGroupWidget->getVisibleView()->syncView();
        _pMediaRendererGroupWidget->getDeviceGroupView().syncView();
    }
    LOG(upnp, debug, "controller widget state change finished");
}


Av::MediaRenderer*
ControllerWidget::getLocalRenderer()
{
    return _pApplication->getLocalRenderer();
}


GuiVisual*
ControllerWidget::getLocalRendererVisual()
{
    return _pVisual;
}


Gui::View*
ControllerWidget::getControlPanel()
{
//    View* pView = new View;
//    pView->setBackgroundColor(Gui::Color("blue"));
//    pView->resize(100, 20);
//    return pView;
//    _pControlPanel->resize(800, 80);
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
ControllerWidget::newStreamType(Av::StreamTypeNotification* pNotification)
{
    LOGNS(Gui, gui, debug, "controller widget stream type notification, instance id: "
        + Poco::NumberFormatter::format(pNotification->_instanceId) + ", transport state: " + pNotification->_transportState + ", stream type: " + pNotification->_streamType);
    if (pNotification->_streamType == Av::Engine::StreamTypeVideo && pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING) {
        setCurrentViewIndex(getIndexFromView(_pVisual));
    }
    else if (pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED) {
        showMainMenu();
    }
}


//void
//ControllerWidget::newTransportState(TransportStateNotification* pNotification)
//{
//    LOGNS(Gui, gui, debug, "controller widget device: " + pNotification->_uuid + " got new transport state: " + pNotification->_transportState);
//    LOGNS(Gui, gui, debug, "new transport state, local renderer has uuid: " + _localRendererUuid);
//    if (pNotification->_uuid == _localRendererUuid && pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED) {
//        showMainMenu();
//    }
//}


//void
//ControllerWidget::newTrack(TrackNotification* pNotification)
//{
//    if (pNotification->_uuid == _localRendererUuid) {
//        LOGNS(Gui, gui, debug, "new track on local renderer");
//        Av::CtlMediaRenderer* pRenderer = static_cast<Av::CtlMediaRenderer*>(_pMediaRendererGroupWidget->getDevice(pNotification->_uuid));
//        if (pRenderer) {
//            LOGNS(Gui, gui, debug, "new track on local renderer, playing object: " + pNotification->_title + ", class: " + pNotification->_objectClass);
//            if (Av::AvClass::matchClass(pNotification->_objectClass, Av::AvClass::ITEM, Av::AvClass::VIDEO_ITEM)
//                || Av::AvClass::matchClass(pNotification->_objectClass, Av::AvClass::ITEM, Av::AvClass::VIDEO_BROADCAST)
//                || Av::AvClass::matchClass(pNotification->_objectClass, Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)) {
//                setCurrentView(_pVisual);
//            }
//        }
//    }
//}


//void
//ControllerWidget::newPlaylist(PlaylistNotification* pNotification)
//{
//    // write resource of playlist container
//    // TODO: move this to playlist editor, assign a playlist container to playlist editor.
////    pModel->writeResource(getControllerHttpUri());
//}


void
ControllerWidget::showMainMenu()
{
    setCurrentViewIndex(getIndexFromView(_pMediaServerGroupWidget));
}


//void
//ControllerWidget::showOnlyBasicDeviceGroups(bool show)
//{
//    insertView(_pMediaRendererGroupWidget, "Player");
//    insertView(_pPlaylistEditor, "List");
//    insertView(_pConfigBrowser, "Setup");
//}


void
ControllerWidget::showOnlyRendererVisual(bool show)
{
    insertView(_pMediaServerGroupWidget, "Media");
    insertView(_pMediaRendererGroupWidget, "Player");
    insertView(_pPlaylistEditor, "List");
    insertView(_pSetup, "Setup");
//    insertView(_pConfigBrowser, "Setup");
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


std::stringstream*
ControllerWidget::getPlaylistResource()
{
    return _pPlaylistEditor->getPlaylistResource();
}


MediaServerDevice*
ControllerWidget::getServer(const std::string& uuid)
{
    return static_cast<MediaServerDevice*>(_pMediaServerGroupWidget->getDevice(uuid));
}


Av::CtlMediaRenderer*
ControllerWidget::getSelectedRenderer()
{
    return static_cast<Av::CtlMediaRenderer*>(_pMediaRendererGroupWidget->getSelectedDevice());
}


void
ControllerWidget::playlistNotification(PlaylistNotification* pNotification)
{
    MediaObjectModel* pModel = pNotification->_pMediaObject;
    if (pModel->isContainer()) {
        if (Av::AvClass::matchClass(pModel->getClass(), Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)) {
            _pPlaylistEditorView->show();
//            if (pModel->getResource() && pModel->getResource()->getAttributeValue(Av::AvProperty::IMPORT_URI) != "") {
//                LOGNS(Gui, gui, debug, "playlist editor load playlist: " + pModel->getTitle());
//                setPlaylistContainer(pModel);
//                Av::AbstractMediaObject* pObject = pModel->getChildForRow(0);
//                LOGNS(Gui, gui, debug, "media object playlist button pushed, container with count children: " + Poco::NumberFormatter::format(pModel->getChildCount()));
//                for (int r = 0; r < pModel->getChildCount(); r++) {
//                    LOGNS(Gui, gui, debug, "title: " + pModel->getChildForRow(r)->getTitle());
//                    _playlistItems.push_back(new MediaObjectModel(*static_cast<MediaObjectModel*>(pModel->getChildForRow(r))));
//                }
//            }
        }
    }
//    else if (_pPlaylistContainer) {
//        LOGNS(Gui, gui, debug, "media object playlist add item with title: " + pModel->getTitle());
//        _playlistItems.push_back(new MediaObjectModel(*pModel));
//        writePlaylistResource();
//
//        // FIXME: why does this crash?
////        _pPlaylistContainer->writeResource(getPlaylistResourceUri());
//    }
//    syncViewImpl();
}


void
KeyController::keyPressed(KeyCode key)
{
    LOGNS(Gui, gui, debug, "key controller, key pressed: " + Poco::NumberFormatter::format(key));
    Av::CtlMediaRenderer* pRenderer = _pControllerWidget->getSelectedRenderer();

    switch (key) {
        case Gui::Controller::KeyM:
        case Gui::Controller::KeyMenu:
            _pControllerWidget->showMainMenu();
            break;
        case Gui::Controller::KeyLeft:
            if (_pControllerWidget->getCurrentViewIndex()) {
                _pControllerWidget->setCurrentViewIndex(_pControllerWidget->getCurrentViewIndex() - 1);
            }
            break;
        case Gui::Controller::KeyRight:
            if (_pControllerWidget->getCurrentViewIndex() < _pControllerWidget->getViewCount() - 1) {
                _pControllerWidget->setCurrentViewIndex(_pControllerWidget->getCurrentViewIndex() + 1);
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
        case Gui::Controller::KeyP:
        case Gui::Controller::KeyPlay:
            if (pRenderer) {
                pRenderer->playPressed();
            }
            break;
        case Gui::Controller::KeyS:
        case Gui::Controller::KeyStop:
            if (pRenderer) {
                pRenderer->stopPressed();
            }
            break;
        case Gui::Controller::KeyH:
        case Gui::Controller::KeyMute:
            if (pRenderer) {
                pRenderer->setMute(!pRenderer->getMute());
            }
            break;
        case Gui::Controller::KeyU:
        case Gui::Controller::KeyVolUp:
            if (pRenderer) {
                pRenderer->volumeChanged(pRenderer->getVolume() + 10);
            }
            break;
        case Gui::Controller::KeyD:
        case Gui::Controller::KeyVolDown:
            if (pRenderer) {
                pRenderer->volumeChanged(pRenderer->getVolume() - 10);
            }
            break;
    }
}


} // namespace Omm
