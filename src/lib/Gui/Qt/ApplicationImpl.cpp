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

#include <QtGui>

#include "ApplicationImpl.h"
#include "Gui/Application.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {



ApplicationImpl::ApplicationImpl(Application* pApplication) :
_pApplication(pApplication)
{
    Omm::Gui::Log::instance()->gui().debug("application impl ctor");
}


ApplicationImpl::~ApplicationImpl()
{
}


void
ApplicationImpl::resize(int width, int height)
{
    _width = width;
    _height = height;
}


int
ApplicationImpl::run(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    _pQtApplication = new QApplication(argc, argv);
    _pMainWindow = new QMainWindow;
    _pMainWindow->setCentralWidget(static_cast<QWidget*>(_pApplication->createMainView()->getNativeView()));
    _pMainWindow->resize(_width, _height);
    _pMainWindow->show();
    int ret = _pQtApplication->exec();
    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
    return ret;
}



}  // namespace Omm
}  // namespace Gui
