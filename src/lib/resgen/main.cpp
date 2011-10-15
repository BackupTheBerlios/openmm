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
        _outputDirectory("./")
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
    }
    
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("[-o OUTPUT_DIRECTORY] FILE_NAME_1 FILE_NAME_2 ..." + Poco::LineEnding::NEWLINE_DEFAULT
                                + "resource header file is put into OUTPUT_DIRECTORY, files can be any type of file"
                                );
        helpFormatter.setHeader("A resource generator.");
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
            for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it) {
                std::ifstream ifs((*it).c_str());
                std::string fileName = Poco::Path(*it).getFileName();
                std::string data = "char " + fileName + "[] = {";
                const int bufSize = 512;
                char buf[bufSize];
                while(ifs) {
                    ifs.read(buf, bufSize);
                    for (int c = 0; c < ifs.gcount(); c++) {
                        data += "0x" + Poco::NumberFormatter::formatHex(buf[c]) + (c == ifs.gcount() - 1 ? "" : ", ");
                    }
                }
                data += "};" + Poco::LineEnding::NEWLINE_DEFAULT;
                std::cout << data << std::endl;
            }
        }
        return Application::EXIT_OK;
    }
    
private:
    bool                        _helpRequested;
    std::string                 _outputDirectory;
};


int main(int argc, char** argv)
{
    ResGenApplication app;
    app.init(argc, argv);
    return app.run();
}
