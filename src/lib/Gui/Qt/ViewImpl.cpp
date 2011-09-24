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


ViewImpl::~ViewImpl()
{
    Omm::Gui::Log::instance()->gui().debug("view impl dtor");
//    delete _pNativeView;
}


void
ViewImpl::initViewImpl(View* pView, QWidget* pNative)
{
    _pView = pView;
    _pNativeView = pNative;
    connect(this, SIGNAL(showViewSignal()), _pNativeView, SLOT(show()));
        _pNativeView->setBackgroundRole(QPalette::Highlight);

    if (pView->getParent()) {
        QWidget* pParentWidget = static_cast<QWidget*>(pView->getParent()->getNativeView());
        _pNativeView->resize(pParentWidget->size());
        _pNativeView->setParent(pParentWidget);
    }
    else {
        _pNativeView->resize(250, 400);
    }
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
ViewImpl::setHighlighted(bool highlighted)
{
    Omm::Gui::Log::instance()->gui().debug("view impl set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
    if (highlighted) {
        _pNativeView->setBackgroundRole(QPalette::Highlight);
//        _pNativeView->update();
    }
    else {
        _pNativeView->setBackgroundRole(QPalette::Window);
    }
}


void
ViewImpl::setBackgroundColor(const Color& color)
{
//    QColor* pColor = static_cast<QColor*>(color.getNativeColor());
//    _pNativeView->setBackgroundRole(QPalette::Highlight);
//    _pNativeView->setBackgroundRole(QApplication::palette()->color(QPalette::Highlight));
//    _pNativeView->setPalette(QPalette(*static_cast<QColor*>(color.getNativeColor())));
//    _pNativeView->setAutoFillBackground(true);
}


PlainViewImpl::PlainViewImpl(View* pView)
{
    initViewImpl(pView, this);
}


void
PlainViewImpl::mousePressEvent(QMouseEvent* pMouseEvent)
{
    _pView->selected();
    QWidget::mousePressEvent(pMouseEvent);
}

}  // namespace Omm
}  // namespace Gui
