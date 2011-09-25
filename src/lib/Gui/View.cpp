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
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/ViewImpl.h"
#endif


namespace Omm {
namespace Gui {


View::View(View* pParent) :
_minWidth(50),
_minHeight(35),
_prefWidth(100),
_prefHeight(70),
_maxWidth(200),
_maxHeight(140)
{
//    Omm::Gui::Log::instance()->gui().debug("view ctor (parent).");

    initView(pParent);
    _pImpl = new PlainViewImpl(this);
}


View::View(View* pParent, bool createPlainView) :
_minWidth(50),
_minHeight(35),
_prefWidth(100),
_prefHeight(70),
_maxWidth(200),
_maxHeight(140)
{
//    Omm::Gui::Log::instance()->gui().debug("view ctor (parent, createPlainView).");

    initView(pParent);
    if (createPlainView) {
        _pImpl = new PlainViewImpl(this);
    }
}


View::~View()
{
//    if (_pImpl) {
//        delete _pImpl;
//    }
}


void
View::initView(View* pParent)
{
    _pParent = pParent;
    _pModel = 0;
    _pLayout = 0;

    if (_pParent) {
//        Omm::Gui::Log::instance()->gui().debug("adding view as child to parent view.");
        _pParent->_children.push_back(this);
    }
}


void*
View::getNativeView()
{
//    Omm::Gui::Log::instance()->gui().debug("view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pImpl->getNativeView();
}


View*
View::getParent()
{
//    Omm::Gui::Log::instance()->gui().debug("view get parent: " + Poco::NumberFormatter::format(_pParent));
    return _pParent;
}


void
View::addSubview(View* pView)
{
    _pImpl->addSubview(pView);
}


void
View::show()
{
//    Omm::Gui::Log::instance()->gui().debug("view show ... " + getName());
    _pImpl->showView();
//    Omm::Gui::Log::instance()->gui().debug("view show finished.");
}


void
View::hide()
{
//    Omm::Gui::Log::instance()->gui().debug("view hide.");
    _pImpl->hideView();
}


int
View::width(SizeConstraint size)
{
    switch(size) {
        case Current:
            return _pImpl->widthView();
        case Min:
            return _minWidth;
        case Pref:
            return _prefWidth;
        case Max:
            return _maxWidth;
    }
}


int
View::height(SizeConstraint size)
{
    switch(size) {
        case Current:
            return _pImpl->heightView();
        case Min:
            return _minHeight;
        case Pref:
            return _prefHeight;
        case Max:
            return _maxHeight;
    }
}


void
View::resize(SizeConstraint size)
{
//    Omm::Gui::Log::instance()->gui().debug("view resize.");
    _pImpl->resizeView(width(size), height(size));
    if (_pLayout) {
        _pLayout->layoutView();
    }
}


void
View::resize(int width, int height)
{
//    Omm::Gui::Log::instance()->gui().debug("view resize.");
    _pImpl->resizeView(width, height);
    if (_pLayout) {
        _pLayout->layoutView();
    }
}


void
View::move(int x, int y)
{
//    Omm::Gui::Log::instance()->gui().debug("view move: " + Poco::NumberFormatter::format(x) + ", " + Poco::NumberFormatter::format(y));
    _pImpl->moveView(x, y);
}


Model*
View::getModel() const
{
    return _pModel;
}


void
View::setModel(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("view set model: " + Poco::NumberFormatter::format(pModel));
    if (_pModel) {
        // if there was a model attached previously, detach this view from it
        for(ControllerIterator it = beginController(); it != endController(); ++it) {
            (*it)->detachModel(_pModel);
        }
        _pModel->detachView(this);
    }
    if (pModel) {
        pModel->attachView(this);
        for(ControllerIterator it = beginController(); it != endController(); ++it) {
            (*it)->attachModel(pModel);
        }
        syncView(pModel);
    }
    _pModel = pModel;
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
View::setBackgroundColor(const Color& color)
{
    _pImpl->setBackgroundColor(color);
}


void
View::setHighlighted(bool highlighted)
{
    _pImpl->setHighlighted(highlighted);
}


ViewImpl*
View::getViewImpl()
{
    return _pImpl;
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
