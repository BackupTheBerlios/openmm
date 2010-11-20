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

#include <Poco/Types.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/StreamCopier.h>
#include <sstream>

#include <Omm/UpnpAvRenderer.h>
#include <Omm/Util.h>

// #include "AvStreamEngine.h"


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
        _width(""),
        _height(""),
        _name("")
    {
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
            _width = value;
        }
        else if (name == "height") {
            _height = value;
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
            
            if (_fullscreen) {
                pEnginePlugin->setOption("fullscreen", "");
            }
            if (_width != "") {
                pEnginePlugin->setOption("width", _width);
            }
            if (_height != "") {
                pEnginePlugin->setOption("height", _height);
            }
            pEnginePlugin->createPlayer();
            
            Omm::Av::UpnpAvRenderer myMediaRenderer(pEnginePlugin);
            
            
//             std::cerr << "MediaRendererApplication::main()" << std::endl;
//             char* argv[args.size()] = new char*[args.size()];
//             for (int i = 0; i < args.size(); ++i) {
//                 argv[i] = args[i].c_str();
//             }
// //             char* argv[1] = {"ommr"};
//             char** argv
            // TODO: change Engine ctor with arg as std::vector<std::string>
//             Omm::Av::Engine* pEngine = new EngineVlc(1, argv);
// //             Omm::Av::Engine* pEngine = new EngineXine;
//             std::cerr << "MediaRendererApplication::main() pEngine: " << pEngine << std::endl;
//             std::cerr << "MediaRendererApplication::main() engine id: " << pEngine->getEngineId() << std::endl;
            
// //             Omm::Av::UpnpAvRenderer myMediaRenderer(pEngine);
            
//             myMediaRenderer.setFullscreen();
            Omm::Icon* pIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
            myMediaRenderer.addIcon(pIcon);
            if (_name != "") {
                myMediaRenderer.setFriendlyName(_name);
            }
            
            myMediaRenderer.start();
            waitForTerminationRequest();
            // myMediaRenderer.stop();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool            _helpRequested;
    std::string     _enginePlugin;
    bool            _fullscreen;
    std::string     _width;
    std::string     _height;
    std::string     _name;
};


int main(int argc, char** argv)
{
    MediaRendererApplication app;
    return app.run(argc, argv);
}