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
GuiApplication::init()
{
    Omm::Log::instance()->upnp().debug("init gui application ...");

    // TODO: command line arguments can be passed after Poco::Application has processed them.
    _pEventLoop = new Omm::Gui::EventLoop(0, 0);
    _pMainWindow = new Omm::Gui::MainWindow;

    _pMainWindow->showWidget();

    Omm::Log::instance()->upnp().debug("finished init gui application.");
}


Omm::Controller*
GuiApplication::createController()
{
//    return new QtController(this);
}


void
GuiApplication::addController()
{
//    _pMainWidget->addWidget(static_cast<QtController*>(_pController));
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
      _pEventLoop->run();
//    _pApp->exec();
}


//void
//GuiApplication::quit()
//{
////    emit doQuit();
//}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::UpnpApplication)
POCO_EXPORT_CLASS(GuiApplication)
POCO_END_MANIFEST
#endif
