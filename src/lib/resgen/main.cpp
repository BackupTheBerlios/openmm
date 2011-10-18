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
#include <fstream>
#include <Poco/NumberFormatter.h>

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/LineEndingConverter.h"
#include "Poco/Exception.h"
#include "Poco/Path.h"


using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


class ResGenApplication : public Poco::Util::Application
{
public:
    ResGenApplication() :
        _helpRequested(false),
        _outputDirectory("./"),
        _resourceName("OmmResources")
    {
        setUnixOptions(true);
    }
    
    ~ResGenApplication()
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
        options.addOption(Option("output-directory", "o", "output directory for generated stub files")
                          .required(false)
                          .repeatable(false)
                          .argument("output directory", true));
        options.addOption(Option("resource-name", "r", "name of resource, used for class and source file names")
                          .required(false)
                          .repeatable(false)
                          .argument("resource name", true));
    }
    
    void handleOption(const std::string& name, const std::string& value)
    {
        Application::handleOption(name, value);
            
        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "output-directory") {
            _outputDirectory = value;
        }
        else if (name == "resource-name") {
            _resourceName = value;
        }
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("[-o OUTPUT_DIRECTORY] [-r RESOURCE_NAME] FILE_NAME_1 FILE_NAME_2 ..." + Poco::LineEnding::NEWLINE_DEFAULT
                                + "resource header file is put into OUTPUT_DIRECTORY, files can be any type of file" + Poco::LineEnding::NEWLINE_DEFAULT
                                + "resource name is used for class name and names of generated source and header files."
                                );
        helpFormatter.setHeader("A resource generator.");
        helpFormatter.format(std::cout);
    }
    
    std::string writeResourceHeader()
    {
        std::string resourceH =
                "#include <string>" + Poco::LineEnding::NEWLINE_DEFAULT
                + "#include <map>" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + "class " + _resourceName + Poco::LineEnding::NEWLINE_DEFAULT
                + "{" + Poco::LineEnding::NEWLINE_DEFAULT
                + "public:" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    " + _resourceName + "* instance();" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    const std::string& getResource(const std::string& resourceName);" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + "private:" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    " + _resourceName + "();" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    static " + _resourceName + "* _pInstance;" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    std::map<std::string, std::string> _resources;" + Poco::LineEnding::NEWLINE_DEFAULT
                + "};" + Poco::LineEnding::NEWLINE_DEFAULT
                ;
        return resourceH;
    }
    
    
    std::string writeResourceProlog()
    {
        std::string prolog =
                "#include \"" + _resourceName + ".h\"" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + _resourceName + "* " + _resourceName + "::_pInstance = 0;" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + _resourceName + "*" + Poco::LineEnding::NEWLINE_DEFAULT
                + _resourceName + "::instance()" + Poco::LineEnding::NEWLINE_DEFAULT
                + "{" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    if (!_pInstance) {" + Poco::LineEnding::NEWLINE_DEFAULT
                + "        _pInstance = new " + _resourceName + ";" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    }" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    return _pInstance;" + Poco::LineEnding::NEWLINE_DEFAULT
                + "}" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + "const std::string&" + Poco::LineEnding::NEWLINE_DEFAULT
                + _resourceName + "::getResource(const std::string& resourceName)" + Poco::LineEnding::NEWLINE_DEFAULT
                + "{" + Poco::LineEnding::NEWLINE_DEFAULT
                + "    return _resources[resourceName];" + Poco::LineEnding::NEWLINE_DEFAULT
                + "}" + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                + Poco::LineEnding::NEWLINE_DEFAULT
                ;
        return prolog;
    }
    
    
    int main(const std::vector<std::string>& args)
    {

        if (_helpRequested || args.size() == 0)
        {
            displayHelp();
        }
        else
        {
            std::cout << "resgen writing header to: " + _outputDirectory + "/" + _resourceName + ".h" << std::endl;
            std::ofstream resourceHFile((_outputDirectory + "/" + _resourceName + ".h").c_str());
            resourceHFile << writeResourceHeader();
            
            std::string resourceCpp = 
                    _resourceName + "::" + _resourceName + "()" + Poco::LineEnding::NEWLINE_DEFAULT
                    + "{" + Poco::LineEnding::NEWLINE_DEFAULT
                    ;
            
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it) {
                std::ifstream ifs((*it).c_str());
                std::string fileName = Poco::Path(*it).getFileName();
                resourceCpp += "    _resources[\"" + fileName + "\"] = std::string(";
                const int bufSize = 512;
                char buf[bufSize];
                while(ifs) {
                    ifs.read(buf, bufSize);
//                    for (int c = 0; c < ifs.gcount(); c++) {
//                        resourceCpp += "0x" + Poco::NumberFormatter::formatHex(buf[c]) + (c == ifs.gcount() - 1 ? "" : ", ");
//                    }
                }
                resourceCpp += ");" + Poco::LineEnding::NEWLINE_DEFAULT;
            }
            resourceCpp += "}" + Poco::LineEnding::NEWLINE_DEFAULT;
                    
            std::cout << "resgen writing source to: " + _outputDirectory + "/" + _resourceName + ".cpp" << std::endl;
            std::ofstream resourceCppFile((_outputDirectory + "/" + _resourceName + ".cpp").c_str());
            resourceCppFile << writeResourceProlog() << resourceCpp;
        }
        return Application::EXIT_OK;
    }
    
private:
    bool                        _helpRequested;
    std::string                 _outputDirectory;
    std::string                 _resourceName;
};


int main(int argc, char** argv)
{
    ResGenApplication app;
    app.init(argc, argv);
    return app.run();
}
