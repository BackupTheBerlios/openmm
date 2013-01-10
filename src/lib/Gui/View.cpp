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

#include <algorithm>

#include <Poco/NumberFormatter.h>
#include <vector>

#include "Gui/View.h"
#include "Gui/GuiLogger.h"
#include "Gui/Model.h"
#include "Gui/Layout.h"
#include "ViewImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


View::View(View* pParent) :
_minWidth(50),
_minHeight(35),
_prefWidth(100),
_prefHeight(70),
_maxWidth(200),
_maxHeight(140),
_stretchFactor(1.0),
_scaleFactor(1.0)
{
//    LOG(gui, debug, "view ctor (parent).");

    initView(pParent);
    _pImpl = new PlainViewImpl(this);
}


View::View(View* pParent, bool createPlainView) :
_minWidth(50),
_minHeight(35),
_prefWidth(100),
_prefHeight(70),
_maxWidth(200),
_maxHeight(140),
_stretchFactor(1.0),
_scaleFactor(1.0),
_pImpl(0)
{
//    LOG(gui, debug, "view ctor (parent, createPlainView).");

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
//        LOG(gui, debug, "adding view as child to parent view.");
        _pParent->_subviews.push_back(this);
    }
}


void*
View::getNativeView()
{
//    LOG(gui, debug, "view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pImpl->getNativeView();
}


#ifdef __WINDOWS__
    void*
#else
    uint32_t
#endif
View::getNativeWindowId()
{
//    LOG(gui, debug, "view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pImpl->getNativeWindowId();
}


View*
View::getParent()
{
//    LOG(gui, debug, "view get parent: " + Poco::NumberFormatter::format(_pParent));
    return _pParent;
}


void
View::setParent(View* pView)
{
    _pImpl->setParent(pView);
    if (_pParent) {
        _pParent->removedSubview(this);
    }
    _pParent = pView;
    _pParent->addedSubview(this);
}


void
View::show(bool async)
{
//    LOG(gui, debug, "view show \"" + getName() + "\" ...");
    _pImpl->showView(async);
//    LOG(gui, debug, "view show finished.");
}


void
View::hide(bool async)
{
//    LOG(gui, debug, "view hide.");
    _pImpl->hideView(async);
}


void
View::raise(bool async)
{
    _pImpl->raise(async);
}


int
View::posX()
{
    return _pImpl->posXView();
}


int
View::posY()
{
    return _pImpl->posYView();
}


int
View::width(SizeConstraint size)
{
    switch(size) {
        case Current:
            return _pImpl->widthView();
        case Min:
            return _minWidth * _scaleFactor;
        case Pref:
            return _prefWidth * _scaleFactor;
        case Max:
            return _maxWidth * _scaleFactor;
    }
}


int
View::height(SizeConstraint size)
{
    switch(size) {
        case Current:
            return _pImpl->heightView();
        case Min:
            return _minHeight * _scaleFactor;
        case Pref:
            return _prefHeight * _scaleFactor;
        case Max:
            return _maxHeight * _scaleFactor;
    }
}


void
View::setSizeConstraint(int width, int height, SizeConstraint size)
{
//    _pImpl->setSizeConstraint(width, height, size);
    switch(size) {
        case Min:
            _minWidth = width;
            _minHeight = height;
            break;
        case Pref:
            _prefWidth = width;
            _prefHeight = height;
            break;
        case Max:
            _maxWidth = width;
            _maxHeight = height;
            break;
    }
}


float
View::stretchFactor()
{
    return _stretchFactor;
}


void
View::setStretchFactor(float stretch)
{
    _stretchFactor = stretch;
}


void
View::setWidth(int width)
{
    _pImpl->resizeView(width, height());
    updateLayout();
}


void
View::setHeight(int height)
{
    _pImpl->resizeView(width(), height);
    updateLayout();
}


void
View::resize(SizeConstraint size)
{
//    LOG(gui, debug, "view resize.");
    _pImpl->resizeView(width(size), height(size));
    updateLayout();
}


void
View::resize(int width, int height)
{
//    LOG(gui, debug, "view resize \"" + getName() + "\" width: " + Poco::NumberFormatter::format(width) + ", height: " + Poco::NumberFormatter::format(height));
    _pImpl->resizeView(width, height);
    updateLayout();
}


void
View::scale(float factor)
{
    // resize view and font (recursively with all subviews)
    _scaleFactor = factor;
    _pImpl->setFontSize(_scaleFactor * _pImpl->getFontSize());
    for (SubviewIterator it = beginSubview(); it != endSubview(); ++it) {
        (*it)->scale(factor);
    }
    if (_pLayout) {
        _pLayout->layoutView();
    }
}


void
View::move(int x, int y)
{
//    LOG(gui, debug, "view move " + _name + ": " + Poco::NumberFormatter::format(x) + ", " + Poco::NumberFormatter::format(y));
    _pImpl->moveView(x, y);
}


Model*
View::getModel() const
{
    return _pModel;
}


void*
View::getNativeModel()
{
    if (_pModel) {
        return _pModel->getNativeModel();
    }
    else {
        return 0;
    }
}


void
View::setModel(Model* pModel)
{
//    LOG(gui, debug, "view \"" + getName() + "\" set model: " + Poco::NumberFormatter::format(pModel) + " ...");
    if (_pModel) {
        // if there was a model attached previously, detach controllers (and this view) from it
        for(ControllerIterator it = beginController(); it != endController(); ++it) {
            (*it)->detachModel(_pModel);
        }
    // FIXME: detaching the view segfaults if old model is already deleted (responsibility of model?)
//        _pModel->detachView(this);
    }
    if (pModel) {
        pModel->attachView(this);
        for(ControllerIterator it = beginController(); it != endController(); ++it) {
            (*it)->attachModel(pModel);
        }
    }
    _pModel = pModel;
//    LOG(gui, debug, "view \"" + getName() + "\" sync view ...");
    syncView();
//    LOG(gui, debug, "view \"" + getName() + "\" set model finished.");
}


void
View::detachModel()
{
    _pModel->detachView(this);
    _pModel = 0;
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
//    _pLayout->layoutViewEquiDistant();
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


View::SubviewIterator
View::beginSubview()
{
    return _subviews.begin();
}


View::SubviewIterator
View::endSubview()
{
    return _subviews.end();
}


void
View::removedSubview(View* pView)
{
    SubviewIterator it = std::find(beginSubview(), endSubview(), pView);
    if (it != endSubview()) {
        _subviews.erase(it);
    }
    updateLayout();
    NOTIFY_CONTROLLER(Controller, removedSubview, pView);
}


void
View::addedSubview(View* pView)
{
    _subviews.push_back(pView);
    pView->_scaleFactor = _scaleFactor;
    updateLayout();
    NOTIFY_CONTROLLER(Controller, addedSubview, pView);
}


int
View::subviewCount()
{
    return _subviews.size();
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
View::resizeNoLayout(int width, int height)
{
    _pImpl->resizeView(width, height);
}


void
View::updateLayout()
{
    if (_pLayout) {
//        LOG(gui, debug, "view \"" + getName() + "\" update layout");
        _pLayout->layoutView();
//        _pLayout->layoutViewEquiDistant();
    }
}


void
View::syncView()
{
//    LOG(gui, debug, "view \"" + getName() + "\" sync view");
    _pImpl->triggerViewSync();
}


bool
View::getEnableRedraw()
{
    return _pImpl->getEnableRedraw();
}


void
View::setEnableRedraw(bool enable)
{
    _pImpl->setEnableRedraw(enable);
}


void
View::triggerKeyEvent(Controller::KeyCode key)
{
    NOTIFY_CONTROLLER(Controller, keyPressed, key);
}


void
View::setAcceptDrops(bool accept)
{
    _pImpl->setAcceptDrops(accept);
}


} // namespace Gui
} // namespace Omm
