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

#include "Gui/View.h"
#include "Gui/GuiLogger.h"
#include "Gui/Model.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ViewImpl.h"
#endif


namespace Omm {
namespace Gui {


View::View(View* pParent) :
_pParent(pParent),
_pImpl(new ViewImpl(this)),
_pModel(0)
{
    Omm::Gui::Log::instance()->gui().debug("view ctor (parent).");
}


View::View(ViewImpl* pViewImpl, View* pParent) :
_pParent(pParent),
_pImpl(pViewImpl)
{
    Omm::Gui::Log::instance()->gui().debug("view ctor (view impl, parent).");
}


View::~View()
{
    if (_pImpl) {
//        delete _pImpl;
    }
}


void*
View::getNativeView()
{
    Omm::Gui::Log::instance()->gui().debug("view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pImpl->getNativeView();
}


View*
View::getParent()
{
    Omm::Gui::Log::instance()->gui().debug("view get parent: " + Poco::NumberFormatter::format(_pParent));
    return _pParent;
}


void
View::show()
{
    Omm::Gui::Log::instance()->gui().debug("view show ...");
    syncView();
    _pImpl->showView();
    Omm::Gui::Log::instance()->gui().debug("view show finished.");
}


void
View::hide()
{
    Omm::Gui::Log::instance()->gui().debug("view hide.");
    _pImpl->hideView();
}


int
View::width()
{
    return _pImpl->widthView();
}


int
View::height()
{
    return _pImpl->heightView();
}


void
View::resize(int width, int height)
{
    Omm::Gui::Log::instance()->gui().debug("view resize.");
    _pImpl->resizeView(width, height);
}


void
View::move(int x, int y)
{
    Omm::Gui::Log::instance()->gui().debug("view move: " + Poco::NumberFormatter::format(x) + ", " + Poco::NumberFormatter::format(y));
    _pImpl->moveView(x, y);
}


Model*
View::getModel()
{
    return _pModel;
}


void
View::setModel(Model* pModel)
{
    _pModel = pModel;
    _pModel->_pView = this;
}


} // namespace Gui
} // namespace Omm
