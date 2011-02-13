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
#include <Omm/X/Webradio.h>

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
        _pluginOption(""),
        _name("")
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
                           Option("option", "o", "option passed to plugin")
                           .required(false)
                           .repeatable(false)
                           .argument("plugin option", true));
        options.addOption(
                           Option("name", "n", "friendly name of UPnP device")
                           .required(false)
                           .repeatable(false)
                           .argument("friendly name", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);
        
        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "option") {
            _pluginOption = value;
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
        helpFormatter.setHeader("A UPnP Device that implements a UPnP-AV Media Webradio Server.");
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
            Omm::Av::AbstractMediaObject* pObject = new WebradioServer;
            
            std::string home = Poco::Environment::get("HOME");
            _name = "Web Radio";
            if (_pluginOption == "") {
                _pluginOption = home + "/.omm/webradio.conf";
            }
            pObject->setOption("basePath", _pluginOption);
            pObject->setTitle(_name);
            
            Omm::Av::AvServer myMediaServer;
            myMediaServer.setRoot(pObject);
            myMediaServer.setFriendlyName(_name);
            Omm::Icon* pIcon = new Omm::Icon(22, 22, 8, "image/png", "device.png");
            myMediaServer.addIcon(pIcon);
            // FIXME: only start web radio server, if internet is available
            // check this in AvServer::setRoot() by calling for example MediaObject::isAvailable()
            myMediaServer.start();
            waitForTerminationRequest();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool            _helpRequested;
    std::string     _pluginOption;
    std::string     _name;
};


int main(int argc, char** argv)
{
    UpnpAvServerApplication app;
    return app.run(argc, argv);
}