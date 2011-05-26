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
#include "UpnpPrivate.h"

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class SsdpTest: public Poco::Util::ServerApplication
{
public:
    SsdpTest(): _helpRequested(false)
    {
    }
    
    ~SsdpTest()
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
        helpFormatter.setHeader("A sniffer for SSDP (Simple Service Discovery Protocol).");
        helpFormatter.format(std::cout);
    }
    
/*    void handleSsdpMessage(const AutoPtr<SsdpMessage>& pNf)
//     void handleSsdpMessage(AutoPtr<SsdpMessage> pNf)
    {
        std::cout << "SsdpTest::handleSsdpMessage() receives message:" << std::endl;
        pNf->toString();
//         std::cout << pNf->toString();
    }*/
    
    void handleSsdpMessage(Omm::SsdpMessage* pNf)
    {
        std::cout << "SSDP message from " << pNf->getSender().toString() << std::endl;
        std::cout << pNf->toString();
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
            
        // set-up a server socket
//             SsdpSocket s(NObserver<SsdpTest, SsdpMessage>(*this, &SsdpTest::handleSsdpMessage));
            Omm::SsdpSocket s;
            s.addObserver(Poco::Observer<SsdpTest, Omm::SsdpMessage>(*this, &SsdpTest::handleSsdpMessage));
            s.start();
            waitForTerminationRequest();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool _helpRequested;
};


int main(int argc, char** argv)
{
    SsdpTest app;
    return app.run(argc, argv);
}