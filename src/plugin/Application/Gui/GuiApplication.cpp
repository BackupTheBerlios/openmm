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


GuiApplication::GuiApplication()
{
}


GuiApplication::~GuiApplication()
{
}


void
GuiApplication::initApplication(int argc, char** argv)
{
    Omm::Log::instance()->upnp().debug("init gui application ...");

    _pEventLoop = new Omm::Gui::EventLoop(argc, argv);
    _pMainWindow = new Omm::Gui::MainWindow;
    _pMainWindow->resize(800, 480);
    _pMainWindow->show();

    Omm::Log::instance()->upnp().debug("finished init gui application.");
}


void
GuiApplication::resize(int width, int height)
{
    _pMainWindow->resize(width, height);
}


Omm::Controller*
GuiApplication::createController()
{
    Omm::Log::instance()->upnp().debug("application gui create controller ...");
    return new Omm::ControllerWidget;
    Omm::Log::instance()->upnp().debug("application gui create controller finished.");
}


void
GuiApplication::addController()
{
    Omm::Log::instance()->upnp().debug("application gui add controller");
    _pMainWindow->setMainView(static_cast<Omm::ControllerWidget*>(_pController));
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
    Omm::Log::instance()->upnp().debug("gui application entering event loop ...");
    _pEventLoop->run();
}


//void
//ApplicationGui::quit()
//{
////    emit doQuit();
//}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::UpnpApplication)
POCO_EXPORT_CLASS(GuiApplication)
POCO_END_MANIFEST
#endif
