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

#include <Poco/Exception.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <Omm/UpnpAvController.h>
#include <Omm/UpnpAvRenderer.h>
#include <Omm/Util.h>


class OmmApplication : public Poco::Util::Application
{
public:
    OmmApplication():
        _helpRequested(false),
        _fullscreen(false),
        _width(800),
        _height(480),
        _name("OMM Renderer")
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
//                            .argument("plugin option", true));
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
            Omm::Util::PluginLoader<Omm::Av::AvUserInterface> pluginLoader;
            Omm::Av::AvUserInterface* pUserInterface;
            try {
                pUserInterface = pluginLoader.load("avinterface-qt", "AvInterface");
            }
            catch(Poco::NotFoundException) {
                Omm::Av::Log::instance()->upnpav().error("controller application could not find plugin for user interface");
                return 1;
            }

            Omm::Util::PluginLoader<Omm::Av::Engine> enginePluginLoader;
            Omm::Av::Engine* pEnginePlugin;
            try {
                pEnginePlugin = enginePluginLoader.load("engine-vlc");
            }
            catch(Poco::NotFoundException) {
                Omm::Av::Log::instance()->upnpav().error("controller application could not find plugin for engine");
                return 1;
            }

            Omm::Av::AvController controller;
            controller.setUserInterface(pUserInterface);
            pUserInterface->initGui();
            pUserInterface->resize(_width, _height);
            pUserInterface->setFullscreen(_fullscreen);

            pEnginePlugin->setVisual(pUserInterface->getVisual());
            pEnginePlugin->createPlayer();

            Omm::Av::AvRenderer mediaRenderer(pEnginePlugin);
            Omm::Icon* pIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
            mediaRenderer.addIcon(pIcon);
            if (_name != "") {
                mediaRenderer.setProperty("friendlyName", _name);
            }

            // create a device container and put media renderer device in it.
            Omm::DeviceContainer rendererContainer;
            rendererContainer.addDevice(&mediaRenderer);
            // set media renderer device as root device
            rendererContainer.setRootDevice(&mediaRenderer);

            // create a runnable device server and add media server container
            Omm::DeviceServer localDevices;
            localDevices.addDeviceContainer(&rendererContainer);

            pUserInterface->showMainWindow();
            controller.start();
            localDevices.init();
            localDevices.start();

            Omm::Av::Log::instance()->upnpav().debug("ControllerApplication: starting event loop");
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
};


int main(int argc, char** argv)
{
    OmmApplication app;
    app.init(argc, argv);
    return app.run();
}

