/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#include <fstream>
#include <iostream>

#include <Poco/Types.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/StreamCopier.h>
#include <sstream>

#include <Omm/UpnpAvRenderer.h>
#include <Omm/Util.h>


using Poco::UInt8;
using Poco::StreamCopier;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using std::stringstream;


class MediaRendererApplication : public Poco::Util::ServerApplication
{
public:
    MediaRendererApplication():
        _helpRequested(false),
        _enginePlugin(""),
        _fullscreen(false),
        _width(1020),
        _height(576),
        _name("")
    {
        setUnixOptions(true);
    }
    
    ~MediaRendererApplication()
    {
    }
    
protected:
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present
        ServerApplication::initialize(self);
    }
    
    void uninitialize()
    {
        ServerApplication::uninitialize();
    }
    
    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);
        
        options.addOption(
                           Option("help", "", "display help information on command line arguments")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Option("plugin", "p", "load plugin")
                           .required(false)
                           .repeatable(false)
                           .argument("plugin name", true));
        options.addOption(
                           Option("fullscreen", "f", "option passed to plugin")
                           .required(false)
                           .repeatable(false));
//                            .argument("plugin option", true));
        options.addOption(
                           Option("width", "w", "width of video window")
                           .required(false)
                           .repeatable(false)
                           .argument("width", true));
        options.addOption(
                           Option("height", "h", "height of video window")
                           .required(false)
                           .repeatable(false)
                           .argument("height", true));
        options.addOption(
                           Option("name", "n", "friendly name of UPnP-AV renderer")
                           .required(false)
                           .repeatable(false)
                           .argument("name", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);
        
        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "plugin") {
            _enginePlugin = value;
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
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A UPnP Device that implements a UPnP-AV Media Renderer.");
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
            if (_enginePlugin == "") {
//                 _enginePlugin = "engine-avstream";
                _enginePlugin = "engine-vlc";
            }
            
            Omm::Util::PluginLoader<Omm::Av::Engine> pluginLoader;
            Omm::Av::Engine* pEnginePlugin;
            try {
                if (_enginePlugin == "engine-avstream") {
                    pEnginePlugin = pluginLoader.load(_enginePlugin, "Engine", "AvStream");
                }
                else {
                    pEnginePlugin = pluginLoader.load(_enginePlugin);
                }
            }
            catch(Poco::NotFoundException) {
                std::cerr << "Error could not find engine plugin: " << _enginePlugin << std::endl;
                return 1;
            }
            std::clog << "engine plugin: " << _enginePlugin << " loaded successfully" << std::endl;

            std::string visualPlugin = "visual-x11";

            Omm::Util::PluginLoader<Omm::Sys::Visual> visualPluginLoader;
            Omm::Sys::Visual* pVisualPlugin;
            try {
                pVisualPlugin = visualPluginLoader.load(visualPlugin, "Visual", "X11");
            }
            catch(Poco::NotFoundException) {
                std::cerr << "Error could not find visual plugin: " << visualPlugin << std::endl;
                return 1;
            }
            std::clog << "visual plugin: " << visualPlugin << " loaded successfully" << std::endl;

            pEnginePlugin->setVisual(pVisualPlugin);

            if (_fullscreen) {
                pVisualPlugin->setFullscreen();
            }
            else {
                pVisualPlugin->setWidth(_width);
                pVisualPlugin->setHeight(_height);
            }
            pEnginePlugin->createPlayer();

            // create a media renderer device
            Omm::Av::MediaRenderer mediaRenderer;
            mediaRenderer.addEngine(pEnginePlugin);
            Omm::Icon* pIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
            mediaRenderer.addIcon(pIcon);
            if (_name != "") {
                mediaRenderer.setFriendlyName(_name);
            }

            // create a device container and put media renderer device in it.
            Omm::DeviceContainer rendererContainer;
            rendererContainer.addDevice(&mediaRenderer);
            rendererContainer.setRootDevice(&mediaRenderer);

            // create a runnable device server and add media server container
            Omm::DeviceServer renderer;
            renderer.addDeviceContainer(&rendererContainer);
            // start runnable server
            renderer.init();
            renderer.start();

            waitForTerminationRequest();

            renderer.stop();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool            _helpRequested;
    std::string     _enginePlugin;
    bool            _fullscreen;
    int             _width;
    int             _height;
    std::string     _name;
};


int main(int argc, char** argv)
{
    MediaRendererApplication app;
    return app.run(argc, argv);
}