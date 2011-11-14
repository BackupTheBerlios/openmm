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

#include <Poco/NumberFormatter.h>

#include "Gui/Application.h"
#include "Gui/GuiLogger.h"
#include "ApplicationImpl.h"


namespace Omm {
namespace Gui {


Application::Application() :
_pImpl(new ApplicationImpl(this)),
_pMainView(0),
_scaleFactor(1.0)
{
//    Omm::Gui::Log::instance()->gui().debug("application ctor.");
}


Application::~Application()
{
}


void
Application::resizeMainView(int width, int height)
{
    _pImpl->resize(width, height);
}


void
Application::scaleMainView(float factor)
{
    _scaleFactor = factor;
}


void
Application::setFullscreen(bool fullscreen)
{
    _pImpl->setFullscreen(fullscreen);
}


View*
Application::getMainView()
{
    return _pMainView;
}


int
Application::width()
{
    return _pImpl->width();
}


int
Application::height()
{
    return _pImpl->height();
}


void
Application::setToolBar(View* pView)
{
    _pImpl->addToolBar(pView);
}


void
Application::showToolBar(bool show)
{
    _pImpl->showToolBar(show);
}


int
Application::run(int argc, char** argv)
{
    return _pImpl->run(argc, argv);
}


void
Application::quit()
{
    _pImpl->quit();
}


void
Application::createdMainView()
{
    _pMainView->scale(_scaleFactor);
}


void
Application::receivedSignal(SignalType signal)
{
    Omm::Gui::Log::instance()->gui().debug("application signal received.");
    switch (signal) {
        case Sys::SignalHandler::SigInt:
        case Sys::SignalHandler::SigQuit:
        case Sys::SignalHandler::SigTerm:
        quit();
        break;
    }
}


} // namespace Gui
} // namespace Omm
