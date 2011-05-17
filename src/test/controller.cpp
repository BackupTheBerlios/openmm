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


#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "Upnp.h"

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class MyController : public Omm::Controller
{
    virtual void deviceAdded(Omm::DeviceContainer* device)
    {
        std::cerr << "MyController::deviceAdded() uuid: " << device->getRootDevice()->getUuid() << std::endl;
    }
    
    virtual void deviceRemoved(Omm::DeviceContainer* device)
    {
        std::cerr << "MyController::deviceRemoved() uuid: " << device->getRootDevice()->getUuid() << std::endl;
    }
    
};


class ControllerTest: public Poco::Util::ServerApplication
{
public:
    ControllerTest(): _helpRequested(false)
    {
    }
    
    ~ControllerTest()
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
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);
        
        if (name == "help")
            _helpRequested = true;
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A simple UPnP Controller.");
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
        // get parameters from configuration file
//             unsigned short port = (unsigned short) config().getInt("EchoServer.port", 9977);
            MyController controller;
            controller.start();
            std::cerr << "ControllerTest::main() waiting for termination request" << std::endl;
            waitForTerminationRequest();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool _helpRequested;
};


int main(int argc, char** argv)
{
    ControllerTest app;
    return app.run(argc, argv);
}