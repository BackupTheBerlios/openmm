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

#include "ListImpl.h"
#include "Gui/List.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtScrollArea : public QScrollArea
{
    friend class ListViewImpl;

    QtScrollArea(QWidget* pParent = 0) : QScrollArea(pParent) {}
    
    void resizeEvent(QResizeEvent* pEvent)
    {
        if (pEvent->oldSize().height() > 0) {
//            _pListViewImpl->resized(_pListViewImpl->widthView(), _pListViewImpl->heightView());
            int width = pEvent->size().width();
            int height = pEvent->size().height();
            _pListViewImpl->resized(width, height);
//            getWidget()->resize(width, getWidget()->height());
        }
    }

    void showEvent(QShowEvent* event)
    {
        _pListViewImpl->resized(_pListViewImpl->widthView(), _pListViewImpl->heightView());
    }
    
    ListViewImpl*   _pListViewImpl;
};


ListViewImpl::ListViewImpl(View* pView)
{
    QtScrollArea* pNativeView = new QtScrollArea;
    pNativeView->_pListViewImpl = this;

    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(pNativeView->viewport()->size());
    pNativeView->setBackgroundRole(QPalette::Base);
    pNativeView->setWidget(_pScrollWidget);
    connect(pNativeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledSlot(int)));

    initViewImpl(pView, pNativeView);
}


ListViewImpl::~ListViewImpl()
{
}


int
ListViewImpl::getOffset()
{
//    Omm::Gui::Log::instance()->gui().debug("scroll widget offset: " + Poco::NumberFormatter::format(_pScrollWidget->geometry().y()));
    return _pScrollWidget->geometry().y();
}


int
ListViewImpl::getViewportWidth()
{
    return static_cast<QScrollArea*>(_pNativeView)->viewport()->geometry().width();
}


int
ListViewImpl::getViewportHeight()
{
    return static_cast<QScrollArea*>(_pNativeView)->viewport()->geometry().height();
}


int
ListViewImpl::getScrollAreaWidth()
{
    return _pScrollWidget->geometry().width();
}


int
ListViewImpl::getScrollAreaHeight()
{
    return _pScrollWidget->geometry().height();
}


void
ListViewImpl::resizeScrollArea(int width, int height)
{
    _pScrollWidget->resize(width, height);
}



void
ListViewImpl::addSubview(View* pView)
{
    static_cast<QWidget*>(pView->getNativeView())->setParent(_pScrollWidget);
}


void
ListViewImpl::viewScrolledSlot(int value)
{
//    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, value);

//    ListView* pListView =  static_cast<ListView*>(_pView);
//    pListView->scrolledToRow(-getOffset() / pListView->_itemViewHeight);
}


void
ListViewImpl::resized(int width, int height)
{
//    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, resized, width, height);
    
    ListView* pListView =  static_cast<ListView*>(_pView);
    pListView->resized(width, height);
    _pScrollWidget->resize(width, _pScrollWidget->height());
}


}  // namespace Omm
}  // namespace Gui
