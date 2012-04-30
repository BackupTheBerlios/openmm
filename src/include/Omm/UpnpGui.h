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

#include "Upnp.h"
#include "UpnpAvCtlRenderer.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvCtlServer.h"
#include "UpnpAvCtlObject2.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvServer.h"
#include "Sys.h"

#include "Gui/Application.h"
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
#include "Gui/Image.h"


namespace Omm {

class UpnpApplication;
class ControllerWidget;
class MediaObjectModel;
class GuiVisual;
class MediaServerGroupWidget;
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

    UpnpApplication(int argc, char** argv);

    void setIgnoreConfig(bool ignore = true);
    void enableController(bool enable = true);
    void showRendererVisualOnly(bool show = true);
    std::string getConfigHttpUri();

private:
    // Poco::Util::Application interface
    virtual void defineOptions(Poco::Util::OptionSet& options);
    virtual void handleOption(const std::string& name, const std::string& value);
    virtual int main(const std::vector<std::string>& args);

    void displayHelp();
    void printConfig();
    void loadConfig();
    void saveConfig();
    std::stringstream* getConfigForm(const Poco::Net::HTMLForm& form);

    // Omm::Gui::Application interface
    virtual Omm::Gui::View* createMainView();
    virtual void presentedMainView();
    virtual void start();
    virtual void stop();

    void addLocalRenderer(const std::string& name, const std::string& uuid);
    void addLocalRenderer();
    void addLocalServer(const std::string& name, const std::string& uuid, const std::string& pluginName, const std::string& basePath);

    int                                         _argc;
    char**                                      _argv;
    bool                                        _helpRequested;
    bool                                        _ignoreConfig;
    Poco::Util::PropertyFileConfiguration*      _pConf;
    std::string                                 _confFilePath;

    ControllerWidget*                           _pControllerWidget;
    bool                                        _enableController;

    DeviceServer                                _localDeviceServer;
    DeviceContainer                             _localDeviceContainer;
    Av::MediaRenderer                           _mediaRenderer;
    bool                                        _enableRenderer;
    std::string                                 _rendererName;
    std::string                                 _rendererUuid;
    bool                                        _showRendererVisualOnly;
    std::map<std::string, Av::MediaServer*>     _mediaServers;
    bool                                        _enableServer;

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


class PlaylistNotification : public Poco::Notification
{
public:
    PlaylistNotification(MediaObjectModel* pMediaObject);

    MediaObjectModel*   _pMediaObject;
};


class ControllerWidget : public Controller, public Gui::Tab
{
friend class PlaylistEditor;

public:
    ControllerWidget(UpnpApplication* pApplication);

    GuiVisual* getLocalRendererVisual();
    MediaRendererView* getControlPanel();
    Gui::View* getStatusBar();
    void setDefaultRenderer(Omm::Av::MediaRenderer* pRenderer);
    void newTransportState(TransportStateNotification* pNotification);
    void newPlaylist(PlaylistNotification* pNotification);
    void showMainMenu();
    void showOnlyBasicDeviceGroups(bool show = false);
    void showOnlyRendererVisual(bool show = false);
    void navigateListWithKey(Gui::Controller::KeyCode key);
    void back();
    virtual void signalNetworkActivity(bool on);

    virtual std::stringstream* getPlaylistResource();

private:
    MediaServerGroupWidget*     _pMediaServerGroupWidget;
    MediaRendererGroupWidget*   _pMediaRendererGroupWidget;
    PlaylistEditor*             _pPlaylistEditor;
    GuiVisual*                  _pVisual;
    MediaRendererView*          _pControlPanel;
    ActivityIndicator*          _pActivityIndicator;
    std::string                 _localRendererUuid;
    bool                        _fullscreen;
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

    virtual void showDeviceGroup();

    // ListModel interface
    virtual int totalItemCount();

    // ListController interface
    virtual void selectedItem(int row);

    // NavigatorController delegate interface
    virtual void changedSearchText(const std::string& searchText) {}

    void setDefaultDevice(Device* pDevice);

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
public:
    MediaRendererDevice() : _transportState(""), _volume(-1) {}

    std::string getTransportState();
    ui2 getVolume();

private:
    virtual void newPosition(int duration, int position) {}
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album);
    virtual void newVolume(const int volume);
    virtual void newTransportState(const std::string& transportState);

    std::string _transportState;
    int         _volume;
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

    // NavigatorController delegate
    virtual void changedSearchText(const std::string& searchText);
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


class MediaContainerWidget : public Gui::ListView, Gui::ListModel, Gui::ListController
{
    friend class MediaServerGroupWidget;

public:
    MediaContainerWidget(View* pParent = 0);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

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


class MediaObjectView : public Gui::ListItemView
{
public:
    MediaObjectView(View* pParent = 0);

private:
    Gui::Button*         _pPlaylistButton;
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

    void playlistNotification(PlaylistNotification* pNotification);

    void setPlaylistContainer(MediaObjectModel* pPlaylistContainer);
    std::stringstream* getPlaylistResource();
    std::string getPlaylistResourceUri();

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
