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

#include "MainWindowImpl.h"
#include "Gui/MainWindow.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


MainWindowImpl::MainWindowImpl() :
WidgetImpl(this)
{
    Omm::Gui::Log::instance()->gui().debug("main window implementation ctor");
}


void
MainWindowImpl::setMainWidget(Widget* pWidget)
{
    Omm::Gui::Log::instance()->gui().debug("main window implementation set main widget");
    QMainWindow::setCentralWidget(static_cast<QWidget*>(pWidget->getNativeWidget()));
}


}  // namespace Omm
}  // namespace Gui
