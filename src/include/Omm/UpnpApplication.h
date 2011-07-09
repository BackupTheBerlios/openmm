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

#ifndef UpnpApplication_INCLUDED
#define	UpnpApplication_INCLUDED

#include <Poco/Exception.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include "Upnp.h"

namespace Omm {


class ControllerSubSystem : public Poco::Util::Subsystem
{

};


class DevicesSubSystem : public Poco::Util::Subsystem
{

};


class UpnpApplication : public Poco::Util::ServerApplication
{
public:
    UpnpApplication();
    virtual ~UpnpApplication();

    virtual void init() {}
//    virtual void showMainWindow() {}
//    virtual void hideMainWindow() {}
    virtual void resize(int width, int height) {}
    virtual void setFullscreen(bool fullscreen) {}

    void enableController(bool enable = true);
    void enableDevices(bool enable = false);

protected:
    virtual int eventLoop();
    virtual Controller* createController() { return new Controller; }
    virtual void addController() {}
    virtual void removeController() {}
    virtual Sys::Visual* createVisual() { return 0; }

    virtual void defineOptions(Poco::Util::OptionSet& options);
    virtual void handleOption(const std::string& name, const std::string& value);

    Controller*             _pController;
    DeviceServer*           _pDevices;

private:
    virtual int main(const std::vector<std::string>& args);
    virtual void initialize(Poco::Util::Application& self);
    virtual void uninitialize();
    virtual void displayHelp();

    void start();
    void stop();
    
    bool                    _helpRequested;
};


} // namespace Omm

#endif

