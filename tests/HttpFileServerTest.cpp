/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009 - 2010                                                |
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


#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <Upnp.h>
#include <UpnpAvServer.h>

class HttpFileServerTest: public Poco::Util::ServerApplication
{
public:
    HttpFileServerTest(): _helpRequested(false)
    {
    }
    
    ~HttpFileServerTest()
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
    
    void defineOptions(Poco::Util::OptionSet& options)
    {
        ServerApplication::defineOptions(options);
        
        options.addOption(
                           Poco::Util::Option("help", "h", "display help information on command line arguments")
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
        Poco::Util::HelpFormatter helpFormatter(options());
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
            
            Jamm::Av::HttpFileServer fileServer;
            fileServer.start();
            std::clog << "HttpFileServerTest::main() file server started on port: " << fileServer.getPort() << std::endl;
            
//             fileServer.registerFile("foo", "/home/jb/tmp/hal.log");
            std::clog << "HttpFileServerTest::main() waiting for termination request" << std::endl;
            waitForTerminationRequest();
        }
        return Application::EXIT_OK;
    }
    
private:
    bool _helpRequested;
};


int main(int argc, char** argv)
{
    HttpFileServerTest app;
    return app.run(argc, argv);
}
