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

#include <Omm/Util.h>

#include "ScrollAreaImpl.h"
#include "Gui/ScrollArea.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtScrollArea : public QScrollArea
{
    friend class ScrollAreaViewImpl;

    QtScrollArea(QWidget* pParent = 0) : QScrollArea(pParent) {}

    void resizeEvent(QResizeEvent* pEvent)
    {
        if (pEvent->oldSize().height() > 0) {
            _pViewImpl->resized(pEvent->size().width(), pEvent->size().height());
        }
    }

    void showEvent(QShowEvent* event)
    {
        _pViewImpl->presented();
    }

    ScrollAreaViewImpl*   _pViewImpl;
};


ScrollAreaViewImpl::ScrollAreaViewImpl(View* pView)
{
    QtScrollArea* pNativeView = new QtScrollArea;
    pNativeView->_pViewImpl = this;
    
    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(pNativeView->viewport()->size());
    pNativeView->setWidget(_pScrollWidget);
    pNativeView->setBackgroundRole(QPalette::Base);
    connect(pNativeView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledXSlot(int)));
    connect(pNativeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledYSlot(int)));

    initViewImpl(pView, pNativeView);
}


ScrollAreaViewImpl::~ScrollAreaViewImpl()
{
}


int
ScrollAreaViewImpl::getViewportWidth()
{
    return static_cast<QtScrollArea*>(_pNativeView)->viewport()->geometry().width();
}


int
ScrollAreaViewImpl::getViewportHeight()
{
    return static_cast<QtScrollArea*>(_pNativeView)->viewport()->geometry().height();
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
ScrollAreaViewImpl::viewScrolledXSlot(int value)
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, getXOffset(), getYOffset());
}


void
ScrollAreaViewImpl::viewScrolledYSlot(int value)
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, getXOffset(), getYOffset());
}


void
ScrollAreaViewImpl::resized(int width, int height)
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, resized, width, height);
}


void
ScrollAreaViewImpl::presented()
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, presented);
}


}  // namespace Omm
}  // namespace Gui
