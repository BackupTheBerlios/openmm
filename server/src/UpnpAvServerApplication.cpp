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

#include <Poco/Types.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <Omm/UpnpAvServer.h>
#include <Omm/Util.h>

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class UpnpAvServerApplication: public Poco::Util::ServerApplication
{
public:
    UpnpAvServerApplication():
        _helpRequested(false),
        _containerPlugin("")
    {
    }
    
    ~UpnpAvServerApplication()
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
                           Option("help", "h", "display help information on command line arguments")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Option("plugin", "p", "load plugin")
                           .required(false)
                           .repeatable(false)
                           .argument("plugin name", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);
        
        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "plugin") {
            _containerPlugin = value;
        }
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A UPnP Device that implements a UPnP-AV Media Server.");
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
            if (_containerPlugin == "") {
//                 _containerPlugin = "server-webradio";
//                 _containerPlugin = "server-webradiosimple";
//                 _containerPlugin = "server-storage";
                _containerPlugin = "server-test";
            }
            
            Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> pluginLoader;
            Omm::Av::AbstractMediaObject* pContainerPlugin;
            try {
                pContainerPlugin = pluginLoader.load(_containerPlugin);
            }
            catch(Poco::NotFoundException) {
                std::cerr << "Error could not find server plugin: " << _containerPlugin << std::endl;
                return 1;
            }
            std::clog << "container plugin: " << _containerPlugin << " loaded successfully" << std::endl;
            
            Omm::Av::UpnpAvServer myMediaServer;
            myMediaServer.setRoot(pContainerPlugin);
            
            myMediaServer.setFriendlyName(pContainerPlugin->getTitle());
            Omm::Icon* pIcon = new Omm::Icon(22, 22, 8, "image/png", "device.png");
            myMediaServer.addIcon(pIcon);
            myMediaServer.start();
            waitForTerminationRequest();
            // myMediaServer.stop();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool            _helpRequested;
    std::string     _containerPlugin;
};


int main(int argc, char** argv)
{
    UpnpAvServerApplication app;
    return app.run(argc, argv);
}