/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
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


#include "jammlight_ctrl_impl.h"

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class MyController : public Jamm::Controller
{
    virtual void deviceAdded(Jamm::DeviceRoot* pDeviceRoot)
    {
        std::cout << "MyController::deviceAdded()" << std::endl;
        std::cout << "uuid: " << pDeviceRoot->getRootDevice()->getUuid() << std::endl;
        std::cout << "type: " << pDeviceRoot->getRootDevice()->getDeviceType() << std::endl;
//         std::cout << "friendly name: " << pDeviceRoot->getRootDevice()->getFriendlyName() << std::endl;
        
        // NOTE: could write a DeviceFactory here ...
        // NOTE: could iterate through all devices of DeviceRoot here ...
        if (pDeviceRoot->getRootDevice()->getDeviceType() == "urn:schemas-upnp-org:device:DimmableLight:1") {
            Poco::Timestamp t;
            std::cout << "TIME: " << t.elapsed() << std::endl;
            DimmableLightController* pDimmableLightCtrl = new DimmableLightController(pDeviceRoot->getRootDevice(), new SwitchPowerControllerImpl);
            
            bool status;
            std::cout << "sync call of GetStatus() started" << std::endl;
            std::cout << "TIME: " << t.elapsed() << std::endl;
            pDimmableLightCtrl->SwitchPower()->GetStatus(status);
            std::cout << "TIME: " << t.elapsed() << std::endl;
            std::cout << "sync call of GetStatus() finished" << std::endl;
            std::cout << "sync call of GetStatus() returns: " << status << std::endl;
            std::cout << "async request _reqGetStatus() started" << std::endl;
            std::cout << "TIME: " << t.elapsed() << std::endl;
            pDimmableLightCtrl->SwitchPower()->_reqGetStatus();
            std::cout << "TIME: " << t.elapsed() << std::endl;
            std::cout << "async request _reqGetStatus() finished" << std::endl;
            std::cout << "state variable Status lookup" << std::endl;
            status = pDimmableLightCtrl->SwitchPower()->_getStatus();
            std::cout << "state variable Status value: " << status << std::endl;
        }
    }
    
    virtual void deviceRemoved(Jamm::DeviceRoot* pDeviceRoot)
    {
        std::cerr << "MyController::deviceRemoved() uuid: " << pDeviceRoot->getRootDevice()->getUuid() << std::endl;
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