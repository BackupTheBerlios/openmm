/***************************************************************************|
|  Omm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Omm.                                               |
|                                                                           |
|  Omm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Omm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include <iostream>

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "ommgen.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class OmmGenApplication : public Poco::Util::Application
{
public:
    OmmGenApplication() :
        m_helpRequested(false),
        m_description(""),
        m_descriptionPath("./"),
        m_outputPath("./")
    {
    }
    
    ~OmmGenApplication()
    {
    }
    
protected:
    void initialize(Application& self)
    {
//         loadConfiguration(); // load default configuration files, if present
        try {
            Application::initialize(self);
        }
//     catch (Poco::Util::MissingArgumentException) {
        catch (...) {
            displayHelp();
        }
        
    }
    
    void uninitialize()
    {
        Application::uninitialize();
    }
    
    void defineOptions(OptionSet& options)
    {
        Application::defineOptions(options);
        
        options.addOption(Option("help", "h", "display help information on command line arguments")
                            .required(false)
                            .repeatable(false));
        options.addOption(Option("description", "d", "device description file")
                            .required(false)
                            .repeatable(false)
                            .argument("description name", true));
        options.addOption(Option("description-path", "p", "path to device and service description files")
                            .required(false)
                            .repeatable(false)
                            .argument("description path", true));
        options.addOption(Option("output-path", "o", "path to generated stub files")
                          .required(false)
                          .repeatable(false)
                          .argument("output path", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        Application::handleOption(name, value);
        std::cerr << "Application::handleOption() name: " << name << " , value: " << value << std::endl;
            
        if (name == "help") {
            m_helpRequested = true;
        } else if (name == "description") {
            m_description = value;
        } else if (name == "description-path") {
            m_descriptionPath = value;
        } else if (name == "output-path") {
            m_outputPath = value;
        }
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("[OPTIONS] DESCRIPTION_FILE");
        helpFormatter.setHeader("A stub generator for OmmLib UPnP.");
        helpFormatter.format(std::cout);
    }
    
    int main(const std::vector<std::string>& args)
    {
        if (m_helpRequested)
        {
            displayHelp();
        }
        else
        {
//             Omm::UriDescriptionReader descriptionReader(Poco::URI("file:" + m_descriptionPath + "/"), m_description);
            Omm::UriDescriptionReader descriptionReader;
            Omm::DeviceRoot* pDeviceRoot = descriptionReader.deviceRoot("file:" + m_descriptionPath + "/" + m_description);
            
            m_stubWriters.push_back(new DeviceH(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceCpp(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceImplH(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceImplCpp(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceDescH(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceCtrlImplH(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceCtrlImplCpp(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceCtrlH(pDeviceRoot, m_outputPath));
            m_stubWriters.push_back(new DeviceCtrlCpp(pDeviceRoot, m_outputPath));
            
            for (std::vector<StubWriter*>::iterator i = m_stubWriters.begin(); i != m_stubWriters.end(); ++i) {
                (*i)->write();
            }
        }
        return Application::EXIT_OK;
    }
    
private:
    bool                        m_helpRequested;
    std::string                 m_description;
    std::string                 m_descriptionPath;
    std::string                 m_outputPath;
    std::vector<StubWriter*>    m_stubWriters;
};


int main(int argc, char** argv)
{
    OmmGenApplication app;
    app.init(argc, argv);
    return app.run();
}
