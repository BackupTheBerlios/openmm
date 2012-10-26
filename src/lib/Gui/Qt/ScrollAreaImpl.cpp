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

#include <QtGui>
#include <Poco/NumberFormatter.h>
//include <qt4/QtGui/qabstractslider.h>

#include "ColorImpl.h"
#include "ScrollAreaImpl.h"
#include "QtScrollAreaImpl.h"
#include "Gui/ScrollArea.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


//class QtScrollArea : public QScrollArea
//{
//public:
//    virtual void scrollContentsBy(int dx, int dy)
//    {
//        QScrollArea::sc
//    }
//};


ScrollAreaViewImpl::ScrollAreaViewImpl(View* pView)
{
    QScrollArea* pNativeView = new QtViewImpl<QScrollArea>(this);
    ScrollAreaSignalProxy* pSignalProxy = new ScrollAreaSignalProxy(this);

    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(pNativeView->viewport()->size());
//    _pScrollWidget->setPalette(QPalette(QColor(255, 255, 255)));
    pNativeView->setWidget(_pScrollWidget);
    pNativeView->setPalette(QPalette(QColor(255, 255, 255)));

    // FIXME: this prevents setting color of subviews of scrollarea
//    pNativeView->setBackgroundRole(QPalette::Base);

    initViewImpl(pView, pNativeView, pSignalProxy);
}


ScrollAreaViewImpl::~ScrollAreaViewImpl()
{
}


int
ScrollAreaViewImpl::getViewportWidth()
{
    return static_cast<QScrollArea*>(_pNativeView)->viewport()->geometry().width();
}


int
ScrollAreaViewImpl::getViewportHeight()
{
    return static_cast<QScrollArea*>(_pNativeView)->viewport()->geometry().height();
}


int
ScrollAreaViewImpl::getXOffset()
{
    return - _pScrollWidget->geometry().x();
}


int
ScrollAreaViewImpl::getYOffset()
{
    return - _pScrollWidget->geometry().y();
}


int
ScrollAreaViewImpl::getScrollAreaWidth()
{
    return _pScrollWidget->geometry().width();
}


int
ScrollAreaViewImpl::getScrollAreaHeight()
{
    return _pScrollWidget->geometry().height();
}


void
ScrollAreaViewImpl::resizeScrollArea(int width, int height)
{
    _pScrollWidget->resize(width, height);
}


void
ScrollAreaViewImpl::scrollContentsTo(int x, int y)
{
    static_cast<QScrollArea*>(_pNativeView)->horizontalScrollBar()->setSliderPosition(x);
    static_cast<QScrollArea*>(_pNativeView)->verticalScrollBar()->setSliderPosition(y);
//    static_cast<QScrollArea*>(_pNativeView)->scroll(x, y);
//    static_cast<QScrollArea*>(_pNativeView)->scrollContentsBy(x, y);
//    static_cast<QScrollArea*>(_pNativeView)->ensureVisible(x, y);
}


void
ScrollAreaViewImpl::showScrollBars(bool show)
{
    if (!show) {
        static_cast<QScrollArea*>(_pNativeView)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        static_cast<QScrollArea*>(_pNativeView)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else {
        static_cast<QScrollArea*>(_pNativeView)->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        static_cast<QScrollArea*>(_pNativeView)->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}


void
ScrollAreaViewImpl::setBackgroundColor(const Color& color)
{
    _pScrollWidget->setPalette(QPalette(*static_cast<QColor*>(color._pImpl->getNativeColor())));
}


void
ScrollAreaViewImpl::addSubview(View* pView)
{
    static_cast<QWidget*>(pView->getNativeView())->setParent(_pScrollWidget);
}


void
ScrollAreaSignalProxy::init()
{
    LOG(gui, debug, "button view impl, init signal proxy");
    SignalProxy::init();
    QScrollArea* pNativeView = static_cast<QScrollArea*>(_pViewImpl->getNativeView());
    connect(pNativeView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledXSlot(int)));
    connect(pNativeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledYSlot(int)));
}


void
ScrollAreaSignalProxy::viewScrolledXSlot(int value)
{
    ScrollAreaViewImpl* pViewImpl = static_cast<ScrollAreaViewImpl*>(_pViewImpl);
    PROXY_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, pViewImpl->getXOffset(), pViewImpl->getYOffset());
}


void
ScrollAreaSignalProxy::viewScrolledYSlot(int value)
{
    ScrollAreaViewImpl* pViewImpl = static_cast<ScrollAreaViewImpl*>(_pViewImpl);
    PROXY_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, pViewImpl->getXOffset(), pViewImpl->getYOffset());
}


}  // namespace Omm
}  // namespace Gui
