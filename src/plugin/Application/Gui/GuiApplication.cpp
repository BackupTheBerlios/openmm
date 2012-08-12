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

#include <Poco/ClassLibrary.h>

#include "GuiApplication.h"


//class GuiApp : public Omm::Gui::Application
//{
//    virtual Omm::Gui::View* createMainView()
//    {
//        return static_cast<Omm::ControllerWidget*>(_pController);
//    }
//};


GuiApplication::GuiApplication()
{
}


GuiApplication::~GuiApplication()
{
}


void
GuiApplication::initApplication(int argc, char** argv)
{
    LOGNS(Omm, upnp, debug, "init gui application ...");

//    _pEventLoop = new Omm::Gui::EventLoop(argc, argv);
//    _pMainWindow = new Omm::Gui::MainWindow;
//    _pMainWindow->resize(800, 480);
//    _pMainWindow->show();
//    _pApplication = new GuiApp;
    _argc = argc;
    _argv = argv;

    LOGNS(Omm, upnp, debug, "finished init gui application.");
}


void
GuiApplication::resize(int width, int height)
{
//    _pMainWindow->resize(width, height);
    Omm::Gui::Application::resizeMainView(width, height);
}


Omm::Controller*
GuiApplication::createController()
{
    LOGNS(Omm, upnp, debug, "application gui create controller ...");
//    return new Omm::ControllerWidget;
    LOGNS(Omm, upnp, debug, "application gui create controller finished.");
}


void
GuiApplication::addController()
{
    LOGNS(Omm, upnp, debug, "application gui add controller");
//    _pMainWindow->setMainView(static_cast<Omm::ControllerWidget*>(_pController));
}


void
GuiApplication::removeController()
{
//    _pMainWidget->removeWidget(static_cast<QtController*>(_pController));
}


void
GuiApplication::setWindowTitle(const std::string& title)
{
//    _pMainWindow->setWindowTitle(QString::fromStdString(title));
}


void
GuiApplication::eventLoop()
{
    LOGNS(Omm, upnp, debug, "gui application entering event loop ...");
//    _pEventLoop->run();
    Omm::Gui::Application::runEventLoop(_argc, _argv);
}


Omm::Gui::View*
GuiApplication::createMainView()
{
//    return static_cast<Omm::ControllerWidget*>(_pController);
    Omm::ControllerWidget* pController = new Omm::ControllerWidget;
    _pController = pController;
    pController->init();
    return pController;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::UpnpApplication)
POCO_EXPORT_CLASS(GuiApplication)
POCO_END_MANIFEST
#endif
