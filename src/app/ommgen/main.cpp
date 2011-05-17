/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/LineEndingConverter.h"
#include "Poco/Exception.h"

#include "ommgen.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class OmmGenApplication : public Poco::Util::Application
{
public:
    OmmGenApplication() :
        _helpRequested(false),
        _outputPath("./")
    {
        setUnixOptions(true);
    }
    
    ~OmmGenApplication()
    {
    }
    
protected:
    void initialize(Application& self)
    {
        Application::initialize(self);
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
        options.addOption(Option("output-path", "o", "output path for generated stub files")
                          .required(false)
                          .repeatable(false)
                          .argument("output path", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        Application::handleOption(name, value);
            
        if (name == "help") {
            _helpRequested = true;
        } else if (name == "output-path") {
            _outputPath = value;
        }
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("[-o OUTPUT_DIRECTORY] DEVICE_DESCRIPTION_FILE_PATH" + Poco::LineEnding::NEWLINE_DEFAULT
                                + "SCPDURLs in device description file must be relative to device description file path"
                                );
        helpFormatter.setHeader("A stub generator for OMM UPnP.");
        helpFormatter.format(std::cout);
    }
    
    int main(const std::vector<std::string>& args)
    {

        if (_helpRequested || args.size() == 0)
        {
            displayHelp();
        }
        else
        {
            std::string descriptionPath = args[0];

            Omm::UriDescriptionReader descriptionReader;
            Omm::DeviceContainer* pDeviceContainer = descriptionReader.deviceRoot("file:" + descriptionPath);
            pDeviceContainer->rewriteDescriptions();

            _stubWriters.push_back(new DeviceH(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceCpp(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceImplH(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceImplCpp(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceDescH(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceCtrlImplH(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceCtrlImplCpp(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceCtrlH(pDeviceContainer, _outputPath));
            _stubWriters.push_back(new DeviceCtrlCpp(pDeviceContainer, _outputPath));

            for (std::vector<StubWriter*>::iterator i = _stubWriters.begin(); i != _stubWriters.end(); ++i) {
                (*i)->write();
            }
        }
        return Application::EXIT_OK;
    }
    
private:
    bool                        _helpRequested;
    std::string                 _descriptionPath;
    std::string                 _outputPath;
    std::vector<StubWriter*>    _stubWriters;
};


int main(int argc, char** argv)
{
    OmmGenApplication app;
    app.init(argc, argv);
    return app.run();
}
