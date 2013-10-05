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

#ifndef UpnpApplication_INCLUDED
#define UpnpApplication_INCLUDED

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

#include "../Upnp.h"
#include "../UpnpAvCtlRenderer.h"
#include "../UpnpAvRenderer.h"
#include "../UpnpAvCtlServer.h"
#include "../UpnpAvCtlObject.h"
#include "../UpnpAvRenderer.h"
#include "../UpnpAvServer.h"

#include "../Gui/Application.h"


namespace Omm {

class ControllerWidget;
class WebSetup;

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

    // Omm::Gui::Application interface
    virtual void start();
    virtual void stop();

    void setLockInstance(bool lock = true);
    void setIgnoreConfig(bool ignore = true);
    bool getIgnoreConfig();
    void enableController(bool enable = true);
    void showRendererVisualOnly(bool show = true);
    std::string getAppHttpUri();
    std::string getConfigHttpUri();
    std::string getMode();
    Poco::Util::PropertyFileConfiguration* getFileConfiguration();
    Av::MediaRenderer* getLocalRenderer();
    void restartLocalDeviceContainer();
    void setLocalDeviceContainerState(const std::string& state);
    Av::MediaServer* getLocalMediaServer(const std::string& uuid);

private:
    // Poco::Util::Application interface
    virtual void defineOptions(Poco::Util::OptionSet& options);
    virtual void handleOption(const std::string& name, const std::string& value);
    virtual int main(const std::vector<std::string>& args);

    // Omm::Gui::Application interface
    virtual Omm::Gui::View* createMainView();
    virtual void presentedMainView();

    // application configuration
    void displayHelp();
    void printConfig();
    void printForm(const Poco::Net::HTMLForm& form);
    void loadConfig();
    void initConfig();
    void saveConfig();

    // local devices
    void initLocalDevices();
    void setLocalRenderer(const std::string& name, const std::string& uuid, const std::string& pluginName);
    void setLocalRenderer();
    void addLocalServer(const std::string& id);

    // other stuff
    void startAppHttpServer();
    void stopAppHttpServer();
    bool instanceAlreadyRunning();
    void installGlobalErrorHandler();

    int                                         _argc;
    char**                                      _argv;
    bool                                        _helpRequested;
    bool                                        _lockInstance;

    bool                                        _ignoreConfig;
    Poco::Util::PropertyFileConfiguration*      _pConf;
    std::string                                 _confFilePath;
    std::string                                 _mode;
    WebSetup*                                   _pWebSetup;

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


}  // namespace Omm


#endif
