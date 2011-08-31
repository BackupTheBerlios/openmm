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
#include "Gui/Layout.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ViewImpl.h"
#endif


namespace Omm {
namespace Gui {


View::View(View* pParent) :
_pParent(pParent),
_pImpl(new ViewImpl(this)),
pModel(0),
_pLayout(0)
{
    Omm::Gui::Log::instance()->gui().debug("view ctor (parent).");
    if (_pParent) {
        Omm::Gui::Log::instance()->gui().debug("adding view as child to parent view.");
        _pParent->_children.push_back(this);
    }
}


View::View(ViewImpl* pViewImpl, View* pParent) :
_pParent(pParent),
_pImpl(pViewImpl),
pModel(0),
_pLayout(0)
{
    Omm::Gui::Log::instance()->gui().debug("view ctor (view impl, parent).");
    if (_pParent) {
        Omm::Gui::Log::instance()->gui().debug("adding view as child to parent view.");
        _pParent->_children.push_back(this);
    }
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
    Omm::Gui::Log::instance()->gui().debug("view show ... " + getName());
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
View::getModel() const
{
    return pModel;
}


void
View::setModel(Model* pModel)
{
    if (pModel) {
        pModel->attachView(this);
        syncView(pModel);
    }
    // if pModel == 0, detach the model from the view
    else if (pModel) {
        // if there was a model attached previously, detach this view from it
        pModel->detachView(this);
    }
    pModel = pModel;
}


void
View::attachController(Controller* pController)
{
    _controllers.push_back(pController);
}


void
View::detachController(Controller* pController)
{
    _controllers.erase(std::find(_controllers.begin(), _controllers.end(), pController));
}


Layout*
View::getLayout() const
{
    return _pLayout;
}


void
View::setLayout(Layout* pLayout)
{
    _pLayout = pLayout;
    _pLayout->_pView = this;
    _pLayout->layoutView();
}


const std::string&
View::getName() const
{
    return _name;
}


void
View::setName(const std::string& name)
{
    _name = name;
}


View::ChildIterator
View::beginChild()
{
    return _children.begin();
}


View::ChildIterator
View::endChild()
{
    return _children.end();
}


int
View::childCount()
{
    return _children.size();
}


View::ControllerIterator
View::beginController()
{
    return _controllers.begin();
}


View::ControllerIterator
View::endController()
{
    return _controllers.end();
}


void
View::syncViewWithModel(Model* pModel)
{
    if (pModel) {
        syncView(pModel);
    }
    else {
        syncView(getModel());
    }
}


} // namespace Gui
} // namespace Omm
