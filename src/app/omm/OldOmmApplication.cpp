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

#include <Poco/Exception.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <Omm/UpnpAvController.h>
#include <Omm/UpnpAvRenderer.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/Util.h>


class OmmApplication : public Poco::Util::Application
{
public:
    OmmApplication():
        _helpRequested(false),
        _fullscreen(false),
        _width(800),
        _height(480),
        _name("OMM"),
        _gui("avinterface-qt")
    {
        setUnixOptions(true);
    }

    ~OmmApplication()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        Application::initialize(self);
    }

    void uninitialize()
    {
        Application::uninitialize();
    }

    void defineOptions(Poco::Util::OptionSet& options)
    {
        Application::defineOptions(options);

        options.addOption(
                           Poco::Util::Option("help", "", "display help information on command line arguments")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("fullscreen", "f", "option passed to plugin")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("width", "w", "width of video window")
                           .required(false)
                           .repeatable(false)
                           .argument("width", true));
        options.addOption(
                           Poco::Util::Option("height", "h", "height of video window")
                           .required(false)
                           .repeatable(false)
                           .argument("height", true));
        options.addOption(
                           Poco::Util::Option("name", "n", "friendly name of UPnP-AV renderer")
                           .required(false)
                           .repeatable(false)
                           .argument("name", true));
        options.addOption(
                   Poco::Util::Option("gui", "g", "user interface for application")
                   .required(false)
                   .repeatable(false)
                   .argument("gui", true));
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
        else if (name == "width") {
            _width = Poco::NumberParser::parse(value);
        }
        else if (name == "height") {
            _height = Poco::NumberParser::parse(value);
        }
        else if (name == "name") {
            _name = value;
        }
        else if (name == "gui") {
            _gui = value;
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

    int main(const std::vector<std::string>& args)
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            ///////// set up controller with its user interface /////////
            Omm::Util::PluginLoader<Omm::Av::AvUserInterface> pluginLoader;
            Omm::Av::AvUserInterface* pUserInterface;

            try {
                pUserInterface = pluginLoader.load("avinterface-qt", "AvInterface");
            }
            catch(Poco::NotFoundException) {
                LOGNS(Omm::Av, upnpav, error, "controller application could not find plugin for user interface");
                return 1;
            }

            Omm::Av::AvController controller;
            controller.setUserInterface(pUserInterface);
//            Omm::Av::AvApplication application;
//            application.setUserInterface(pUserInterface);
            pUserInterface->initGui();
            pUserInterface->resize(_width, _height);
            pUserInterface->setFullscreen(_fullscreen);

            ///////// add a media renderer /////////
            Omm::Util::PluginLoader<Omm::Av::Engine> enginePluginLoader;
            Omm::Av::Engine* pEnginePlugin;
            try {
                pEnginePlugin = enginePluginLoader.load("engine-vlc");
            }
            catch(Poco::NotFoundException) {
                LOGNS(Omm::Av, upnpav, error, "controller application could not find plugin for engine");
                return 1;
            }

            pEnginePlugin->setVisual(pUserInterface->getVisual());
            pEnginePlugin->createPlayer();

            Omm::Av::MediaRenderer mediaRenderer;
            mediaRenderer.addEngine(pEnginePlugin);
            Omm::Icon* pRendererIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
            mediaRenderer.addIcon(pRendererIcon);
            if (_name != "") {
                mediaRenderer.setFriendlyName(_name);
            }
            else {
                mediaRenderer.setFriendlyName("OMM Renderer");
            }

            // create a device container and put media renderer device in it.
            Omm::DeviceContainer rendererContainer;
            rendererContainer.addDevice(&mediaRenderer);
            // set media renderer device as root device
            rendererContainer.setRootDevice(&mediaRenderer);

//            ///////// add a file server /////////
//            Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> serverPluginLoader;
//            Omm::Av::AbstractMediaObject* pContainerPlugin;
//            try {
//                pContainerPlugin = serverPluginLoader.load("server-file");
//            }
//            catch(Poco::NotFoundException) {
//                std::cerr << "Error could not find server plugin: " << "server-file" << std::endl;
//                return 1;
//            }
//
//            pContainerPlugin->setOption("basePath", Poco::Environment::get("HOME") + "/music");
//            pContainerPlugin->setTitle("Music");
//            // create a media server device
//            Omm::Av::MediaServer mediaServer;
//            mediaServer.setRoot(pContainerPlugin);
//            mediaServer.setFriendlyName("OMM Music Collection");
//            Omm::Icon* pServerIcon = new Omm::Icon(32, 32, 8, "image/png", "device.png");
//            mediaServer.addIcon(pServerIcon);
//            rendererContainer.addDevice(&mediaServer);
//
//            ///////// add a webradio server /////////
//            Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> webradioPluginLoader;
//            Omm::Av::AbstractMediaObject* pWebradioPlugin;
//            try {
//                pWebradioPlugin = webradioPluginLoader.load("server-webradio");
//            }
//            catch(Poco::NotFoundException) {
//                std::cerr << "Error could not find server plugin: " << "server-webradio" << std::endl;
//                return 1;
//            }
//
//            pWebradioPlugin->setOption("basePath", Poco::Environment::get("HOME") + "/.omm/webradio.conf");
//            pWebradioPlugin->setTitle("Web Radio");
//            Omm::Av::MediaServer webradioServer;
//            webradioServer.setRoot(pWebradioPlugin);
//            webradioServer.setFriendlyName("OMM Webradio");
//            Omm::Icon* pWebradioIcon = new Omm::Icon(32, 32, 8, "image/png", "device.png");
//            webradioServer.addIcon(pWebradioIcon);
//            rendererContainer.addDevice(&webradioServer);

            ///////// create a runnable device server and add container with local devices /////////
            Omm::DeviceServer localDevices;
            localDevices.addDeviceContainer(&rendererContainer);

            ///////// start the whole thing /////////
            pUserInterface->showMainWindow();
            controller.init();
            controller.start();
            localDevices.init();
            localDevices.start();

            LOGNS(Omm::Av, upnpav, debug, "ControllerApplication: starting event loop");
            int ret = pUserInterface->eventLoop();

            controller.stop();
            localDevices.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool            _helpRequested;
    bool            _fullscreen;
    int             _width;
    int             _height;
    std::string     _name;
    std::string     _gui;
};


int main(int argc, char** argv)
{
    OmmApplication app;
    app.init(argc, argv);
    return app.run();
}

