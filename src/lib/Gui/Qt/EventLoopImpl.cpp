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

#include "EventLoopImpl.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


EventLoopImpl::EventLoopImpl(int argc, char** argv) :
QApplication(argc, argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop implementation ctor");
}


void
EventLoopImpl::run()
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    QApplication::exec();
    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
}


}  // namespace Omm
}  // namespace Gui
