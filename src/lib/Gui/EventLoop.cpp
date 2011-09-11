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

#include "Gui/EventLoop.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/EventLoopImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/EventLoopImpl.h"
#endif


namespace Omm {
namespace Gui {


EventLoop::EventLoop(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop ctor ...");
    _pEventLoopImpl = new EventLoopImpl(argc, argv);
    Omm::Gui::Log::instance()->gui().debug("event loop ctor finished.");
}


EventLoop::~EventLoop()
{
}


void
EventLoop::run()
{
    _pEventLoopImpl->run();
}


} // namespace Gui
} // namespace Omm
