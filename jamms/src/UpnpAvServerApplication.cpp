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

#include <Poco/Types.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/StreamCopier.h>
// #include <sstream>
// #include <fstream>

#include "UpnpAvServer.h"
#include "UpnpAvServerImpl.h"

#include "Web/WebRadio.h"

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class UpnpAvServerApplication: public Poco::Util::ServerApplication
{
public:
    UpnpAvServerApplication(): _helpRequested(false)
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
        helpFormatter.setHeader("A UPnP Device that implements a UPnP-AV Media S.");
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
            std::cerr << "UpnpAvServerApplication::main()" << std::endl;
            
            AVTransportImplementation       myAVTransportImplementation;
            ConnectionManagerImplementation myConnectionManagerImplementation;
            ContentDirectoryImplementation  myContentDirectoryImplementation;
            
            UpnpAvServer myMediaServer(
                &myContentDirectoryImplementation,
                &myConnectionManagerImplementation,
                &myAVTransportImplementation
                );
            
            WebRadio webRadio;
            myMediaServer.setRoot(&webRadio);
            
            myMediaServer.start();
            waitForTerminationRequest();
            // myMediaServer.stop();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool _helpRequested;
};


int main(int argc, char** argv)
{
    UpnpAvServerApplication app;
    return app.run(argc, argv);
}