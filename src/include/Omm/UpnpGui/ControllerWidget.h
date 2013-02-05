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

#ifndef ControllerWidget_INCLUDED
#define ControllerWidget_INCLUDED

#include "../Upnp.h"
#include "../UpnpAvRenderer.h"
#include "../UpnpAvCtlRenderer.h"
#include "../UpnpAvServer.h"

#include "../Gui/Cluster.h"
#include "../Gui/View.h"
#include "../Gui/WebBrowser.h"


namespace Omm {

class UpnpApplication;
class GuiVisual;
class MediaRendererGroupWidget;
class MediaRendererView;
class MediaServerDevice;
class MediaServerGroupWidget;
class PlaylistEditor;
class PlaylistEditorView;
class ActivityIndicator;
class TransportStateNotification;
class TrackNotification;
class PlaylistNotification;


class ControllerWidget : public Controller, public Gui::ClusterView
{
friend class PlaylistEditor;

public:
    ControllerWidget(UpnpApplication* pApplication);

    void setState(State newState);

    Av::MediaRenderer* getLocalRenderer();
    GuiVisual* getLocalRendererVisual();
    Gui::View* getControlPanel();
    Gui::View* getStatusBar();
    void setDefaultRenderer(Av::MediaRenderer* pRenderer);
    void newStreamType(Av::StreamTypeNotification* pNotification);
//    void newTransportState(TransportStateNotification* pNotification);
//    void newTrack(TrackNotification* pNotification);
//    void newPlaylist(PlaylistNotification* pNotification);
    void showMainMenu();
//    void showOnlyBasicDeviceGroups(bool show = false);
    void showOnlyRendererVisual(bool show = false);
    void navigateListWithKey(Gui::Controller::KeyCode key);
    void back();
    virtual void signalNetworkActivity(bool on);

    virtual std::stringstream* getPlaylistResource();
    MediaServerDevice* getServer(const std::string& uuid);
    Av::CtlMediaRenderer* getSelectedRenderer();

private:
    void playlistNotification(PlaylistNotification* pNotification);

    MediaServerGroupWidget*     _pMediaServerGroupWidget;
    MediaRendererGroupWidget*   _pMediaRendererGroupWidget;
    PlaylistEditor*             _pPlaylistEditor;
    PlaylistEditorView*         _pPlaylistEditorView;
    GuiVisual*                  _pVisual;
    Gui::WebBrowser*            _pConfigBrowser;
    MediaRendererView*          _pControlPanel;
    ActivityIndicator*          _pActivityIndicator;
    std::string                 _localRendererUuid;
    UpnpApplication*            _pApplication;
};


class KeyController : public Gui::Controller
{
public:
    KeyController(ControllerWidget* pControllerWidget) : _pControllerWidget(pControllerWidget) {}

private:
    virtual void keyPressed(KeyCode key);

    ControllerWidget*   _pControllerWidget;
};


}  // namespace Omm


#endif
