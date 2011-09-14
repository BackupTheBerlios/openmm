/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#ifndef GuiApplication_INCLUDED
#define GuiApplication_INCLUDED

//#include <Omm/Gui/EventLoop.h>
//#include <Omm/Gui/MainWindow.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/View.h>

#include <Omm/Upnp.h>
#include <Omm/UpnpApplication.h>
#include <Omm/UpnpGui.h>


class GuiApplication : public Omm::UpnpApplication, public Omm::Gui::Application
{
public:
    GuiApplication();
    virtual ~GuiApplication();

    virtual void eventLoop();

private:
    // UpnpApplication interface
    virtual void initApplication(int argc = 0, char** argv = 0);
    virtual void resize(int width, int height);
    virtual void setWindowTitle(const std::string& title);
    virtual Omm::Controller* createController();
    virtual void addController();
    virtual void removeController();

    virtual Omm::Gui::View* createMainView();

//    Omm::Gui::EventLoop*     _pEventLoop;
//    Omm::Gui::MainWindow*    _pMainWindow;
//    Omm::Gui::Application*    _pApplication;
//    Omm::Gui::Application       _application;
    int                       _argc;
    char**                    _argv;
};


#endif
