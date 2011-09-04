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
    Omm::Gui::Log::instance()->gui().debug("view impl ctor (view).");
    connect(this, SIGNAL(showViewSignal()), _pNativeView, SLOT(show()));
}


ViewImpl::ViewImpl(View* pView, QWidget* pNativeView) :
_pView(pView),
_pNativeView(pNativeView)
{
    Omm::Gui::Log::instance()->gui().debug("view impl ctor view: " + Poco::NumberFormatter::format(_pView) + ", native view: " + Poco::NumberFormatter::format(_pNativeView));
    if (pNativeView) {
        connect(this, SIGNAL(showViewSignal()), _pNativeView, SLOT(show()));
    }
}


ViewImpl::~ViewImpl()
{
    Omm::Gui::Log::instance()->gui().debug("view impl dtor");
//    delete _pNativeView;
}


View*
ViewImpl::getView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


QWidget*
ViewImpl::getNativeView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl get native view: " + Poco::NumberFormatter::format(_pNativeView));
    return _pNativeView;
}


void
ViewImpl::setNativeView(QWidget* pView)
{
    Omm::Gui::Log::instance()->gui().debug("view impl set native view: " + Poco::NumberFormatter::format(pView));
    _pNativeView = pView;
}


void
ViewImpl::showView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
//    _pNativeView->show();
    emit showViewSignal();
    Omm::Gui::Log::instance()->gui().debug("view impl show finished.");
}


void
ViewImpl::hideView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    _pNativeView->hide();
    Omm::Gui::Log::instance()->gui().debug("view impl hide finished.");
}


int
ViewImpl::widthView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl width.");
    return _pNativeView->width();
}


int
ViewImpl::heightView()
{
    Omm::Gui::Log::instance()->gui().debug("view impl height.");
    return _pNativeView->height();
}


void
ViewImpl::resizeView(int width, int height)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl resize.");
    _pNativeView->resize(width, height);
}


void
ViewImpl::moveView(int x, int y)
{
    Omm::Gui::Log::instance()->gui().debug("view impl move.");
    _pNativeView->move(x, y);
}


void
ViewImpl::selected()
{
    Omm::Gui::Log::instance()->gui().debug("view impl selected event.");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
}


NativeView::NativeView(ViewImpl* pViewImpl, View* pParent) :
QWidget(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
_pViewImpl(pViewImpl)
{
    Omm::Gui::Log::instance()->gui().debug("native view impl ctor.");
}


void
NativeView::mousePressEvent(QMouseEvent* pMouseEvent)
{
    _pViewImpl->selected();
    QWidget::mousePressEvent(pMouseEvent);
}


}  // namespace Omm
}  // namespace Gui
