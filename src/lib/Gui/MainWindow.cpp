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

#include "Gui/MainWindow.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/MainWindowImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/MainWindowImpl.h"
#endif


namespace Omm {
namespace Gui {


MainWindow::MainWindow() :
View(new MainWindowImpl(this))
{
    Omm::Gui::Log::instance()->gui().debug("main window ctor.");
}


MainWindow::~MainWindow()
{
}


void
MainWindow::setMainView(View* pView)
{
    static_cast<MainWindowImpl*>(_pImpl)->setMainView(pView);
}


} // namespace Gui
} // namespace Omm
