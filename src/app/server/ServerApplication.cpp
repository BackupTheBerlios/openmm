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

#include <iostream>

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
        _pluginName("model-file"),
        _basePath(""),
        _name("")
    {
        setUnixOptions(true);
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
        options.addOption(
                           Option("basepath", "b", "base path for data (directory / meta data file depending on type of server)")
                           .required(false)
                           .repeatable(false)
                           .argument("base path", true));
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
        else if (name == "plugin") {
            _pluginName = value;
        }
        else if (name == "basepath") {
            _basePath = value;
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
            Omm::Av::AbstractDataModel* pDataModel;
            Omm::Util::PluginLoader<Omm::Av::AbstractDataModel> pluginLoader;
            try {
                pDataModel = pluginLoader.load(_pluginName);
            }
            catch(Poco::NotFoundException) {
                std::cerr << "error could not find server plugin: " << _pluginName << std::endl;
                return 1;
            }
            std::clog << "container plugin: " << _pluginName << " loaded successfully" << std::endl;

//            Omm::Util::PluginLoader<Omm::Av::ServerContainer> pluginLoader;
//            Omm::Av::ServerContainer* pContainerPlugin;
//            try {
//                pContainerPlugin = pluginLoader.load(_containerPlugin);
//            }
//            catch(Poco::NotFoundException) {
//                std::cerr << "error could not find server plugin: " << _containerPlugin << std::endl;
//                return 1;
//            }
//            std::clog << "container plugin: " << _containerPlugin << " loaded successfully" << std::endl;

            std::string home = Poco::Environment::get("HOME");
            if (_pluginName == "model-dvb") {
                if (_name == "") {
                    _name = "Digital TV";
                }
                if (_basePath == "") {
                    _basePath = home + "/.omm/channels.conf";
                }
            }
//            else if (_containerPlugin == "server-file") {
//                if (_name == "") {
//                    _name = "Collection";
//                }
//                if (_pluginOption == "") {
//                    _pluginOption = home + "/music";
////                     _pluginOption = home;
//                }
//            }
            else if (_pluginName == "model-webradio") {
                // FIXME: only start web radio server, if internet is available
                // check this in AvServer::setRoot() by calling for example MediaObject::isAvailable()
                if (_name == "") {
                    _name = "Web Radio";
                }
                if (_basePath == "") {
                    _basePath = home + "/.omm/webradio.conf";
                }
            }
            else if (_pluginName == "model-file") {
                if (_name == "") {
                    _name = "Collection";
                }
                if (_basePath == "") {
                    _basePath = home + "/music";
                }
            }
            else if (_name == "") {
                _name = "OMM Server";
            }

            Omm::Av::CachedServerContainer* pContainer = new Omm::Av::CachedServerContainer;
            pContainer->setTitle(_name);
            pContainer->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER));
            pContainer->setDataModel(pDataModel);
            pContainer->setBasePath(_basePath);

            // create a media server device
            Omm::Av::MediaServer mediaServer;
            mediaServer.setRoot(pContainer);
            mediaServer.setFriendlyName(_name);
            Omm::Icon* pIcon = new Omm::Icon(32, 32, 8, "image/png", "server.png");
            mediaServer.addIcon(pIcon);

            // create a device container and put media server device in it.
            Omm::DeviceContainer serverContainer;
            serverContainer.addDevice(&mediaServer);
            // set media server device as root device
            serverContainer.setRootDevice(&mediaServer);

            // create a runnable device server and add media server container
            Omm::DeviceServer server;
            server.addDeviceContainer(&serverContainer);
            // start runnable server
            server.init();
            server.start();

            waitForTerminationRequest();

            server.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool            _helpRequested;
    std::string     _pluginName;
    std::string     _basePath;
    std::string     _name;
};


int main(int argc, char** argv)
{
    UpnpAvServerApplication app;
    return app.run(argc, argv);
}