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

#include "LazyListImpl.h"
#include "Gui/LazyList.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


LazyListViewImpl::LazyListViewImpl(View* pView, View* pParent) :
QScrollArea(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
ViewImpl(pView, this),
_pScrollWidget(0)
{
    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(viewport()->size());
    setWidget(_pScrollWidget);

    connect(this, SIGNAL(moveWidgetSignal(int, View*)), this, SLOT(moveWidgetSlot(int, View*)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledSlot(int)));
}


LazyListViewImpl::~LazyListViewImpl()
{
}


int
LazyListViewImpl::visibleRows()
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    int rows = viewport()->geometry().height() / pListView->_viewHeight;
    Omm::Gui::Log::instance()->gui().debug("widget list number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
LazyListViewImpl::addItemView(View* pView)
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    static_cast<QWidget*>(pView->getNativeView())->resize(viewport()->width(), pListView->_viewHeight);
    static_cast<QWidget*>(pView->getNativeView())->setParent(_pScrollWidget);
}


void
LazyListViewImpl::moveItemView(int row, View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("widget list move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pView);
}


void
LazyListViewImpl::updateScrollWidgetSize()
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    LazyListModel* pListModel = static_cast<LazyListModel*>(_pView->getModel());
   _pScrollWidget->resize(geometry().width(), pListModel->totalItemCount() * pListView->_viewHeight);
}


int
LazyListViewImpl::getOffset()
{
//    Omm::Gui::Log::instance()->gui().debug("scroll widget offset: " + Poco::NumberFormatter::format(_pScrollWidget->geometry().y()));
    return _pScrollWidget->geometry().y();
}


void
LazyListViewImpl::moveWidgetSlot(int row, View* pView)
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    static_cast<QWidget*>(pView->getNativeView())->move(0, pListView->_viewHeight * row);
}


void
LazyListViewImpl::viewScrolledSlot(int value)
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    pListView->scrolledToRow(-getOffset() / pListView->_viewHeight);
}


void
LazyListViewImpl::resizeEvent(QResizeEvent* pEvent)
{
    LazyListView* pListView =  static_cast<LazyListView*>(_pView);
    int rows = pEvent->size().height() / pListView->_viewHeight;
    Omm::Gui::Log::instance()->gui().debug("Qt widget list resize: " + Poco::NumberFormatter::format(rows));
    if (pEvent->oldSize().height() > 0) {
//        WidgetListView::resize(rows);
    }
}


}  // namespace Omm
}  // namespace Gui
