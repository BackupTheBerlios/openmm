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

#include "ViewImpl.h"
#include "Gui/View.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ViewImpl::ViewImpl(View* pView) :
_pNativeView(new NativeView(this, pView->getParent())),
_pView(pView)
{
    Omm::Gui::Log::instance()->gui().debug("view implementation ctor (view).");
}


ViewImpl::ViewImpl(QWidget* pNativeView) :
_pNativeView(pNativeView)
{
    Omm::Gui::Log::instance()->gui().debug("view implementation ctor (native view).");
}


ViewImpl::ViewImpl(View* pView, QWidget* pNativeView) :
_pView(pView),
_pNativeView(pNativeView)
{
    Omm::Gui::Log::instance()->gui().debug("view implementation ctor (view, native view)");
}


ViewImpl::~ViewImpl()
{
    delete _pNativeView;
}


View*
ViewImpl::getView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


QWidget*
ViewImpl::getNativeView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation get native view: " + Poco::NumberFormatter::format(_pNativeView));
    return _pNativeView;
}


void
ViewImpl::setNativeView(QWidget* pView)
{
    _pNativeView = pView;
}


void
ViewImpl::showView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation show view _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    _pNativeView->show();
    Omm::Gui::Log::instance()->gui().debug("view implementation show view finished.");
}


void
ViewImpl::hideView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation hide.");
    _pNativeView->hide();
}


int
ViewImpl::widthView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation width.");
    return _pNativeView->width();
}


int
ViewImpl::heightView()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation height.");
    return _pNativeView->height();
}


void
ViewImpl::resizeView(int width, int height)
{
    Omm::Gui::Log::instance()->gui().debug("view implementation resize.");
    _pNativeView->resize(width, height);
}


void
ViewImpl::moveView(int x, int y)
{
    Omm::Gui::Log::instance()->gui().debug("view implementation move.");
    _pNativeView->move(x, y);
}


void
ViewImpl::select()
{
    Omm::Gui::Log::instance()->gui().debug("view implementation select.");
    _pView->select();
}


NativeView::NativeView(ViewImpl* pViewImpl, View* pParent) :
QWidget(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
_pViewImpl(pViewImpl)
{
    Omm::Gui::Log::instance()->gui().debug("native view implementation ctor.");
}

void
NativeView::mousePressEvent(QMouseEvent* pMouseEvent)
{
    _pViewImpl->select();
    QWidget::mousePressEvent(pMouseEvent);
}


}  // namespace Omm
}  // namespace Gui
