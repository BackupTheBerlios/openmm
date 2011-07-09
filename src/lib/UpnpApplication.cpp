/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                           |
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

#include <Poco/Util/HelpFormatter.h>

#include "UpnpApplication.h"

namespace Omm {


UpnpApplication::UpnpApplication() :
_pController(0),
_pDevices(0),
_helpRequested(false)
{
    setUnixOptions(true);
}


UpnpApplication::~UpnpApplication()
{
    if (_pController) {
        delete _pController;
    }
    if (_pDevices) {
        delete _pDevices;
    }
}


int
UpnpApplication::eventLoop()
{
    waitForTerminationRequest();
}


void
UpnpApplication::initialize(Poco::Util::Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}


void
UpnpApplication::uninitialize()
{
    ServerApplication::uninitialize();
}


void
UpnpApplication::defineOptions(Poco::Util::OptionSet& options)
{
    ServerApplication::defineOptions(options);
    
    options.addOption(
        Poco::Util::Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false));
}


void
UpnpApplication::handleOption(const std::string& name, const std::string& value)
{
    ServerApplication::handleOption(name, value);
    
    if (name == "help") {
        _helpRequested = true;
    }
}


void
UpnpApplication::displayHelp()
{
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A UPnP Application with optionally a controller and local devices.");
    helpFormatter.format(std::cout);
}


int
UpnpApplication::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
    {
        displayHelp();
    }
    else
    {
        start();

        int ret = eventLoop();

        stop();
    }
    return Poco::Util::Application::EXIT_OK;
}


void
UpnpApplication::enableController(bool enable)
{
    if (enable && !_pController) {
        _pController = createController();
        addController();
        _pController->init();
    }
    else if (!enable && _pController) {
        _pController->stop();
        removeController();
        delete _pController;
        _pController = 0;
    }
}


void
UpnpApplication::enableDevices(bool enable)
{
    if (enable && !_pDevices) {
        _pDevices = new DeviceServer;
        _pDevices->init();
    }
    else if (!enable && _pDevices) {
        _pDevices->stop();
        delete _pDevices;
        _pDevices = 0;
    }
}


void
UpnpApplication::start()
{
    if (_pController) {
        _pController->start();
    }
    if (_pDevices) {
        _pDevices->startAsThread();
    }
}


void
UpnpApplication::stop()
{
    if (_pDevices) {
        _pDevices->stopThread();
    }
    if (_pController) {
        _pController->stop();
    }
}


} // namespace Omm
