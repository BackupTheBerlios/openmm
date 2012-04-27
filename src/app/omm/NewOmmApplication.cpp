/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/Controller.h>
#include <Omm/UpnpGui.h>
#include <Omm/UpnpAvRenderer.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/Util.h>
#ifdef __IPHONE__
#include <Omm/X/EngineMPMoviePlayer.h>
#else
#include <Omm/X/EngineVlc.h>
#include <Omm/X/EnginePhonon.h>
#endif


class OmmApplication;

//class OmmKeyController : public Omm::Gui::Controller
//{
//public:
//    OmmKeyController(OmmApplication* pOmmApplication) : _pOmmApplication(pOmmApplication), _appStarted(true) {}
//
//private:
//    virtual void keyPressed(KeyCode key);
//
//    OmmApplication*     _pOmmApplication;
//    bool                _appStarted;
//};


class OmmApplication : public Omm::Gui::Application
{
    friend class Application;

public:

    OmmApplication() :
    _fullscreen(false),
    _enableRenderer(false),
    _enableServer(false)
    {
    }

    virtual Omm::Gui::View* createMainView()
    {
        _pControllerWidget = new Omm::ControllerWidget;
        setToolBar(_pControllerWidget->getControlPanel());
        setStatusBar(_pControllerWidget->getStatusBar());
//        _pControllerWidget->attachController(new OmmKeyController(this));
        return _pControllerWidget;
    }

    virtual void presentedMainView()
    {
        _pControllerWidget->setTabBarHidden(_fullscreen);
        _pControllerWidget->showOnlyBasicDeviceGroups(_fullscreen);
        _pControllerWidget->init();
        _pControllerWidget->setLocalDeviceServer(&_localDeviceServer);
    }

    virtual void finishedEventLoop()
    {
    }

    virtual void stop()
    {
        Omm::Av::Log::instance()->upnpav().debug("omm application stopping ...");
        _localDeviceServer.stop();
        _pControllerWidget->stop();
    }

    virtual void start()
    {
        Omm::Av::Log::instance()->upnpav().debug("omm application starting ...");
//#ifndef __IPHONE__
        if (_enableRenderer) {
            addLocalRenderer();
        }
        if (_enableRenderer || _enableServer) {
            _localDeviceServer.addDeviceContainer(&_localDeviceContainer);
        }
//#endif
        _localDeviceServer.init();
//#ifndef __IPHONE__
        if (_enableRenderer) {
            _pControllerWidget->setDefaultRenderer(&_mediaRenderer);
        }
//#endif

        _pControllerWidget->start();
        _localDeviceServer.start();
    }

    void setFullscreen(bool fullscreen)
    {
        Omm::Gui::Application::setFullscreen(fullscreen);
        _fullscreen = fullscreen;
    }

    void addLocalRenderer(const std::string& name, const std::string& uuid)
    {
        _enableRenderer = true;
        _rendererName = name;
        _rendererUuid = uuid;
    }

    void addLocalRenderer()
    {
        Omm::Av::Log::instance()->upnpav().debug("omm application add local renderer ...");
        Omm::Av::Engine* pEngine;
#ifdef __IPHONE__
//        pEngine = new MPMoviePlayerEngine;
#else
        pEngine = new VlcEngine;
//        pEngine = new PhononEngine;
#endif
        pEngine->setVisual(_pControllerWidget->getLocalRendererVisual());
        pEngine->createPlayer();

        _mediaRenderer.addEngine(pEngine);
        Omm::Icon* pRendererIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
        _mediaRenderer.addIcon(pRendererIcon);
        _mediaRenderer.setFriendlyName(_rendererName);
        _mediaRenderer.setUuid(_rendererUuid);
        _localDeviceContainer.addDevice(&_mediaRenderer);
        _localDeviceContainer.setRootDevice(&_mediaRenderer);
        Omm::Av::Log::instance()->upnpav().debug("omm application add local renderer finished.");
    }

    void addLocalServer(const std::string& name, const std::string& uuid, const std::string& pluginName, const std::string& basePath)
    {
        Omm::Av::Log::instance()->upnpav().debug("omm application add local server ...");

        Omm::Av::AbstractDataModel* pDataModel;
        Omm::Util::PluginLoader<Omm::Av::AbstractDataModel> pluginLoader;
        try {
            pDataModel = pluginLoader.load(pluginName);
        }
        catch(Poco::NotFoundException) {
            Omm::Av::Log::instance()->upnpav().error("could not find server plugin: " + pluginName);
            return;
        }
        Omm::Av::Log::instance()->upnpav().information("container plugin: " + pluginName + " loaded successfully");

        _enableServer = true;
        Omm::Av::MediaServer* pMediaServer = new Omm::Av::MediaServer;

        Omm::Av::ServerContainer* pContainer = new Omm::Av::ServerContainer(pMediaServer);
        pContainer->setTitle(name);
        pContainer->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER));
        pContainer->setDataModel(pDataModel);
        std::string path = basePath;
        // if relative path then search file in config directory
        if (basePath[0] != '/') {
            path = Omm::Util::Home::instance()->getConfigDirPath("/") + basePath;
        }
        pContainer->setBasePath(path);

        pMediaServer->setRoot(pContainer);
        pMediaServer->setFriendlyName(name);
        pMediaServer->setUuid(uuid);
        Omm::Icon* pIcon = new Omm::Icon(32, 32, 8, "image/png", "server.png");
        pMediaServer->addIcon(pIcon);

        _localDeviceContainer.addDevice(pMediaServer);

        Omm::Av::Log::instance()->upnpav().debug("omm application add local server finished.");
    }

    Omm::ControllerWidget*  _pControllerWidget;
    Omm::Av::MediaRenderer  _mediaRenderer;
    Omm::DeviceContainer    _localDeviceContainer;
    Omm::DeviceServer       _localDeviceServer;
    bool                    _fullscreen;
    bool                    _enableRenderer;
    bool                    _enableServer;
    std::string             _rendererName;
    std::string             _rendererUuid;
};


//void
//OmmKeyController::keyPressed(KeyCode key)
//{
//    if (key == Omm::Gui::Controller::KeyX) {
//        _appStarted ? _pOmmApplication->stop() : _pOmmApplication->start();
//        _appStarted = !_appStarted;
//    }
//}


class Application :  public Poco::Util::Application
{
public:

    Application(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    _helpRequested(false),
//#ifdef __IPHONE__
//    _rendererName("iPhone Renderer"),
//#else
//    _rendererName("OMM Renderer"),
//#endif
    _fullscreen(false),
    _scale(1.0)
    {
        setUnixOptions(true);
    }

    void initialize(Application& self)
    {
        Poco::Util::Application::initialize(self);

        _confFilePath = Omm::Util::Home::instance()->getConfigDirPath("/") + "omm.properties";
        _pConf = new Poco::Util::PropertyFileConfiguration;
        try {
            _pConf->load(_confFilePath);
        }
        catch (Poco::Exception& e) {
            Omm::Av::Log::instance()->upnpav().debug("no config file present");
        }
//        config().addWriteable(_pConf, -200);
        config().addWriteable(_pConf, 0);

        printConfig();
    }

    void uninitialize()
    {
        Poco::Util::Application::uninitialize();

        _pConf->save(_confFilePath);
    }

    void defineOptions(Poco::Util::OptionSet& options)
    {
        Poco::Util::Application::defineOptions(options);

        options.addOption(
                           Poco::Util::Option("help", "", "display help information on command line arguments")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("renderername", "r", "friendly name of UPnP-AV renderer")
                           .binding("renderer.friendlyName")
                           .required(false)
                           .repeatable(false)
                           .argument("renderername", true));
        options.addOption(
                           Poco::Util::Option("fullscreen", "f", "option passed to plugin")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("width", "w", "width of application window")
                           .binding("application.width")
                           .required(false)
                           .repeatable(false)
                           .argument("width", true));
        options.addOption(
                           Poco::Util::Option("height", "h", "height of application window")
                           .binding("application.height")
                           .required(false)
                           .repeatable(false)
                           .argument("height", true));
        options.addOption(
                           Poco::Util::Option("scale", "s", "application window scale factor")
                           .required(false)
                           .repeatable(false)
                           .argument("scale", true));
    }

    void handleOption(const std::string& name, const std::string& value)
    {
        Poco::Util::Application::handleOption(name, value);

        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "fullscreen") {
            _fullscreen = true;
        }
        else if (name == "scale") {
            _scale = Poco::NumberParser::parseFloat(value);
        }
    }

    void displayHelp()
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("OMM application running a UPnP controller, renderer and servers.");
        helpFormatter.format(std::cout);
    }

    void printConfig()
    {
        std::vector<std::string> rootKeys;
        config().keys(rootKeys);
        for (std::vector<std::string>::iterator it = rootKeys.begin(); it != rootKeys.end(); ++it) {
            Omm::Av::Log::instance()->upnpav().debug("omm config, root keys: " + *it);
        }

        std::vector<std::string> confKeys;
        _pConf->keys(confKeys);
        for (std::vector<std::string>::iterator it = confKeys.begin(); it != confKeys.end(); ++it) {
            Omm::Av::Log::instance()->upnpav().debug("omm config, config file keys: " + *it + ", value: " + _pConf->getString(*it, ""));
        }

        std::vector<std::string> appKeys;
        config().keys("application", appKeys);
        for (std::vector<std::string>::iterator it = appKeys.begin(); it != appKeys.end(); ++it) {
            Omm::Av::Log::instance()->upnpav().debug("omm config, application keys: " + *it + ", value: " + config().getString("application." + *it, ""));
        }

        std::vector<std::string> sysKeys;
        config().keys("system", sysKeys);
        for (std::vector<std::string>::iterator it = sysKeys.begin(); it != sysKeys.end(); ++it) {
            Omm::Av::Log::instance()->upnpav().debug("omm config, system keys: " + *it + ", value: " + config().getString("system." + *it, ""));
        }
    }

    int main(const std::vector<std::string>& args)
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            initialize(*this);

            OmmApplication app;

            if (config().getBool("renderer.enable", false)) {
                app.addLocalRenderer(config().getString("renderer.friendlyName", "OMM Renderer"),
                        config().getString("renderer.uuid", ""));
            }

            std::vector<std::string> servers;
            config().keys("server", servers);
            for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it) {
                Omm::Av::Log::instance()->upnpav().debug("omm config, server: " + *it);
                if (config().getBool("server." + *it + ".enable", false)) {
                    app.addLocalServer(config().getString("server." + *it + ".friendlyName", "OMM Server"),
                            config().getString("server." + *it + ".uuid", ""),
                            config().getString("server." + *it + ".plugin", "model-webradio"),
                            config().getString("server." + *it + ".basePath", "webradio.conf"));
                }
            }

//            if (config().getBool("server.enable", false)) {
//                app.addLocalServer(config().getString("server.friendlyName", "OMM Server"),
//                        config().getString("server.uuid", ""),
//                        config().getString("server.plugin", "model-webradio"),
//                        config().getString("server.basePath", "webradio.conf"));
//            }

            app.setFullscreen(_fullscreen);
            app.resizeMainView(config().getInt("application.width", 800), config().getInt("application.height", 480));
            app.scaleMainView(_scale);

            app.run(_argc, _argv);

//            _pConf->setInt("width", app.width());
//            _pConf->setInt("height", app.height());
            // FIXME: main view is smaller than app window
            _pConf->setInt("application.width", app.getMainView()->width());
            _pConf->setInt("application.height", app.getMainView()->height());
//            config().setInt("width", app.getMainView()->width());
//            config().setInt("height", app.getMainView()->height());

            uninitialize();
        }
        return Application::EXIT_OK;
    }

    int                                         _argc;
    char**                                      _argv;
    bool                                        _helpRequested;
    Poco::Util::PropertyFileConfiguration*      _pConf;
    std::string                                 _confFilePath;
    std::string                                 _rendererName;
    bool                                        _fullscreen;
    int                                         _width;
    int                                         _height;
    float                                       _scale;
};


int main(int argc, char** argv)
{
    Application app(argc, argv);
    app.init(argc, argv);
    return app.run();
}

