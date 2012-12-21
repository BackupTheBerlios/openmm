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
#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "Sys/Signal.h"

#include "Upnp.h"
#include "UpnpAvCtlRenderer.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvCtlServer.h"

#include "UpnpAvCtlObject2.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvServer.h"

#include "Gui/Application.h"
#include "Gui/Cluster.h"
#include "Gui/Navigator.h"
#include "Gui/ListModel.h"
#include "Gui/List.h"
#include "Gui/LazyList.h"
#include "Gui/ListItem.h"
#include "Gui/Button.h"
#include "Gui/Label.h"
#include "Gui/TextLine.h"
#include "Gui/Slider.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/Image.h"
#include "Gui/WebBrowser.h"


namespace Omm {

class UpnpApplication;
class ControllerWidget;
class MediaObjectModel;
class GuiVisual;
class MediaServerGroupWidget;
class MediaServerDevice;
class MediaRendererGroupWidget;
class MediaRendererView;
class MediaObjectView;
class PlaylistEditor;
class PlaylistEditorObjectView;
class ActivityIndicator;


class ConfigRequestHandler: public Poco::Net::HTTPRequestHandler
{
public:
    ConfigRequestHandler(UpnpApplication* pApp) : _pApp(pApp) {}

    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    UpnpApplication*        _pApp;
};


class ConfigRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    ConfigRequestHandlerFactory(UpnpApplication* pApp) : _pApp(pApp) {}

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
    UpnpApplication*        _pApp;
};


class UpnpApplication :  public Poco::Util::Application, public Gui::Application
{
    friend class ConfigRequestHandler;

public:
    static const std::string PLAYLIST_URI;
    static const std::string CONFIG_URI;
    static const std::string CONFIG_APP_QUERY;
    static const std::string CONFIG_DEV_QUERY;

    static const std::string ModeFull;
    static const std::string ModeRendererOnly;

    UpnpApplication(int argc, char** argv);
    ~UpnpApplication();

    void setLockInstance(bool lock = true);
    void setIgnoreConfig(bool ignore = true);
    void enableController(bool enable = true);
    void showRendererVisualOnly(bool show = true);
    std::string getAppHttpUri();
    std::string getConfigHttpUri();
    std::string getMode();

private:
    // Poco::Util::Application interface
    virtual void defineOptions(Poco::Util::OptionSet& options);
    virtual void handleOption(const std::string& name, const std::string& value);
    virtual int main(const std::vector<std::string>& args);

    // Omm::Gui::Application interface
    virtual Omm::Gui::View* createMainView();
    virtual void presentedMainView();
    virtual void start();
    virtual void stop();

    // application configuration
    void displayHelp();
    void printConfig();
    void printForm(const Poco::Net::HTMLForm& form);
    void loadConfig();
    void initConfig();
    void saveConfig();
    std::stringstream* generateConfigPage();
    void handleAppConfigRequest(const Poco::Net::HTMLForm& form);
    void handleDevConfigRequest(const Poco::Net::HTMLForm& form);

    // local devices
    void initLocalDevices();
    void setLocalRenderer(const std::string& name, const std::string& uuid, const std::string& pluginName);
    void setLocalRenderer();
    void addLocalServer(const std::string& id);

    // other stuff
    void startAppHttpServer();
    void stopAppHttpServer();
    bool instanceAlreadyRunning();

    int                                         _argc;
    char**                                      _argv;
    bool                                        _helpRequested;
    bool                                        _lockInstance;
    bool                                        _ignoreConfig;
    Poco::Util::PropertyFileConfiguration*      _pConf;
    std::string                                 _confFilePath;
    std::string                                 _mode;

    ControllerWidget*                           _pControllerWidget;
    bool                                        _enableController;

    DeviceServer*                               _pLocalDeviceServer;
    DeviceContainer*                            _pLocalDeviceContainer;
    Av::MediaRenderer*                          _pLocalMediaRenderer;
    bool                                        _enableRenderer;
    std::string                                 _rendererName;
    std::string                                 _rendererUuid;
    std::string                                 _rendererPlugin;
    bool                                        _showRendererVisualOnly;
    bool                                        _enableServer;
    std::string                                 _instanceMutexName;

    // _socket is needed for
    // 1. playlist editor resource imports
    // 2. configuration of app
    int                                         _appStandardPort;
    Poco::Net::ServerSocket                     _socket;
    Poco::Net::HTTPServer*                      _pHttpServer;
};


class TransportStateNotification : public Poco::Notification
{
public:
    TransportStateNotification(const std::string& uuid, const std::string& transportState) : _uuid(uuid), _transportState(transportState) {}

    std::string         _uuid;
    std::string         _transportState;
};


class TrackNotification : public Poco::Notification
{
public:
    TrackNotification(const std::string& uuid, const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass) : _uuid(uuid), _title(title), _artist(artist), _album(album), _objectClass(objectClass) {}

    std::string         _uuid;
    std::string         _title;
    std::string         _artist;
    std::string         _album;
    std::string         _objectClass;
};


class PlaylistNotification : public Poco::Notification
{
public:
    PlaylistNotification(MediaObjectModel* pMediaObject);

    MediaObjectModel*   _pMediaObject;
};


class ControllerWidget : public Controller, public Gui::ClusterView
{
friend class PlaylistEditor;

public:
    ControllerWidget(UpnpApplication* pApplication);

    void setState(State newState);

    GuiVisual* getLocalRendererVisual();
    Gui::View* getControlPanel();
    Gui::View* getStatusBar();
    void setDefaultRenderer(Omm::Av::MediaRenderer* pRenderer);
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
    MediaServerGroupWidget*     _pMediaServerGroupWidget;
    MediaRendererGroupWidget*   _pMediaRendererGroupWidget;
    PlaylistEditor*             _pPlaylistEditor;
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

//    virtual void showDeviceGroup();

    // ListModel interface
    virtual int totalItemCount();

    // ListController interface
    virtual void selectedItem(int row);

    // NavigatorController delegate interface
    virtual void changedSearchText(const std::string& searchText) {}

    void setDefaultDevice(Device* pDevice);
    Gui::ListView& getDeviceGroupView();

protected:
    virtual void init() {}

    Gui::ListView   _deviceGroupListView;
    std::string     _defaultDeviceUuid;
};


class DeviceGroupNavigatorController : public Gui::NavigatorController
{
public:
    DeviceGroupNavigatorController(DeviceGroupWidget* pDeviceGroupWidget);

    virtual void changedSearchText(const std::string& searchText);

private:
    DeviceGroupWidget*  _pDeviceGroupWidget;
};


class MediaRendererGroupWidget : public DeviceGroupWidget
{
public:
    MediaRendererGroupWidget(ControllerWidget* pControllerWidget);

    // Omm::DeviceGroup interface
    virtual Device* createDevice();

    // ListModel interface
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);
    virtual void selectedItem(int row);

private:
    ControllerWidget*   _pControllerWidget;
};


class MediaRendererDevice : public Av::CtlMediaRenderer, public Gui::Model
{
    friend class MediaRendererView;
    friend class SeekSlider;

public:
    MediaRendererDevice(ControllerWidget* pControllerWidget) : _transportState(""), _duration(0), _pControllerWidget(pControllerWidget) {}

    std::string getTransportState();

private:
    virtual void initController();

    virtual void newUri(const std::string& uri);
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass);
    virtual void newPosition(int duration, int position);
    virtual void newVolume(const int volume);
    virtual void newTransportState(const std::string& transportState);

    std::string         _transportState;
    int                 _duration;
    Gui::LabelModel     _rendererName;
    Gui::LabelModel     _trackName;
    Gui::SliderModel    _position;
    Gui::SliderModel    _volume;
    ControllerWidget*   _pControllerWidget;
};


class MediaRendererView : public Gui::View
{
public:
    MediaRendererView();

    // FIXME: add submodels in setModel, no needed if submodels are supported
    virtual void setModel(Gui::Model* pModel);

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
    Gui::LabelView*         _pRendererName;
    Gui::LabelView*         _pTrackName;
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

    // NavigatorController delegate
    virtual void changedSearchText(const std::string& searchText);

private:
    std::string         _searchString;
};


class MediaServerDevice : public Av::CtlMediaServer, public Gui::ListItemModel
{
public:
    MediaServerDevice(MediaServerGroupWidget* pServerGroupWidget) : _pServerGroupWidget(pServerGroupWidget) {}

    virtual void initController();

    virtual Av::CtlMediaObject2* createMediaObject();

    virtual void newSystemUpdateId(ui4 id);

private:
    MediaServerGroupWidget*     _pServerGroupWidget;
};


class MediaServerView : public Gui::ListItemView
{
public:
    MediaServerView();

};


class MediaContainerWidget : public Gui::ListView, Gui::ListModel, Gui::ListController
{
    friend class MediaServerGroupWidget;
    friend class MediaServerDevice;

public:
    MediaContainerWidget(View* pParent = 0);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
    virtual void selectedItem(int row);

    void createPlaylist(const std::string& playlistName);

private:
    MediaObjectModel*           _pObjectModel;
    MediaServerGroupWidget*     _pServerGroup;
};


class MediaContainerPlaylistCreator : public Gui::TextLine
{
public:
    MediaContainerPlaylistCreator(MediaContainerWidget* pMediaContainer);

private:
    virtual void editedText(const std::string& text);

    MediaContainerWidget*       _pMediaContainer;
};


class MediaObjectModel : public Av::CtlMediaObject2, public Gui::ListItemModel
{
    friend class MediaContainerWidget;

public:
    MediaObjectModel();
    MediaObjectModel(const MediaObjectModel& objectModel);

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


class MediaObjectViewPlaylistButtonController : public Gui::ButtonController
{
public:
    MediaObjectViewPlaylistButtonController(MediaObjectView* pMediaObjectView);

private:
    // ButtonController interface
    virtual void pushed();

    MediaObjectView*    _pMediaObjectView;
};


class MediaObjectViewDestroyButtonController : public Gui::ButtonController
{
public:
    MediaObjectViewDestroyButtonController(MediaObjectView* pMediaObjectView);

private:
    // ButtonController interface
    virtual void pushed();

    MediaObjectView*    _pMediaObjectView;
};


class MediaObjectView : public Gui::ListItemView
{
public:
    MediaObjectView(View* pParent = 0);

private:
    Gui::Button*         _pPlaylistButton;
    Gui::Button*         _pDestroyButton;
};


class PlaylistEditor : public Gui::ListView, Gui::ListModel, Gui::ListController
{
    friend class PlaylistEditorDeleteObjectController;

public:
    PlaylistEditor(ControllerWidget* pControllerWidget);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
    virtual void draggedItem(int row);
    virtual void droppedItem(Gui::Model* pModel, int row);

    void playlistNotification(PlaylistNotification* pNotification);

    void setPlaylistContainer(MediaObjectModel* pPlaylistContainer);
    std::stringstream* getPlaylistResource();
    std::string getPlaylistResourceUri();
    void writePlaylistResource();

    void deleteItem(MediaObjectModel* pModel);

private:
    ControllerWidget*                   _pControllerWidget;
    MediaObjectModel*                   _pPlaylistContainer;
    std::vector<MediaObjectModel*>      _playlistItems;
};


class PlaylistEditorDeleteObjectController : public Gui::ButtonController
{
public:
    PlaylistEditorDeleteObjectController(PlaylistEditorObjectView* pPlaylistEditorObjectView);

private:
    // ButtonController interface
    virtual void pushed();

    PlaylistEditorObjectView*    _pPlaylistEditorObjectView;
};


class PlaylistEditorObjectView : public Gui::ListItemView
{
    friend class PlaylistEditorDeleteObjectController;

public:
    PlaylistEditorObjectView(PlaylistEditor* pPlaylistEditor, View* pParent = 0);

private:
    PlaylistEditor*      _pPlaylistEditor;
    Gui::Button*         _pDeleteButton;
};


class GuiVisual : public Gui::Image, public Sys::Visual
{
public:
    GuiVisual(Gui::View* pParent = 0);
    virtual ~GuiVisual();

    virtual void show();
    virtual void hide();

    virtual void* getWindow();
    virtual WindowHandle getWindowId();
    virtual VisualType getType();
    virtual void renderImage(const std::string& imageData);
    virtual void blank();
};


class ActivityIndicator : public Gui::ImageView
{
public:
    ActivityIndicator(Gui::View* pParent = 0);
    ~ActivityIndicator();

public:
    void startActivity();
    void stopActivity();

private:
    void stopIndicator(Poco::Timer& timer);
    void setActivityInProgress(bool set);
    bool activityInProgress();
    void setIndicatorOn(bool set);
    bool indicatorOn();
    bool timerActive();
    void setTimerActive(bool set);

    const int                                   _indicateDuration;
    Gui::ImageModel*                            _pActivityOffModel;
    Gui::ImageModel*                            _pActivityOnModel;
    bool                                        _activityInProgress;
    Poco::FastMutex                             _activityInProgressLock;
    bool                                        _indicatorOn;
    Poco::FastMutex                             _indicatorOnLock;
    Poco::Timer*                                _pOffTimer;
    bool                                        _timerActive;
    Poco::FastMutex                             _timerActiveLock;
    Poco::TimerCallback<ActivityIndicator>      _stopIndicatorCallback;
};


}  // namespace Omm


#endif
