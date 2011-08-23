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
#include <Omm/UpnpAvLogger.h>

#include "ListViewImpl.h"
#include "Gui/ListView.h"
#include "Gui/GuiLogger.h"
#include "Gui/ListWidget.h"

namespace Omm {
namespace Gui {


ListViewImpl::ListViewImpl(Widget* pWidget, bool movableWidgets, Widget* pParent) :
QGraphicsView(static_cast<QWidget*>(pParent ? pParent->getNativeWidget() : 0)),
WidgetImpl(pWidget, this),
_movableWidgets(movableWidgets)
{
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    _pGraphicsScene = new QGraphicsScene;
    setScene(_pGraphicsScene);

    connect(this, SIGNAL(moveWidgetSignal(int, ListWidget*)), this, SLOT(moveWidgetSlot(int, ListWidget*)));
    connect(this, SIGNAL(extendPoolSignal()), this, SLOT(extendPoolSlot()));
}


ListViewImpl::~ListViewImpl()
{
}


int
ListViewImpl::visibleRows()
{
    ListView* pListView =  static_cast<ListView*>(_pWidget);
    int rows = viewport()->geometry().height() / pListView->_widgetHeight;
    Omm::Gui::Log::instance()->gui().debug("widget canvas number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
ListViewImpl::initWidget(ListWidget* pWidget)
{
//    QtListWidget* pQtWidget = static_cast<QtListWidget*>(pWidget);
//    pQtWidget->resize(viewport()->width(), _widgetHeight);
    ListView* pListView =  static_cast<ListView*>(_pWidget);
    pWidget->resize(viewport()->width(), pListView->_widgetHeight);

    QWidget* pQWidget = static_cast<QWidget*>(pWidget->getNativeWidget());
    if (_movableWidgets) {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQWidget, Qt::Window);
    }
    else {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQWidget);
   }
}


void
ListViewImpl::moveWidget(int row, ListWidget* pWidget)
{
    Omm::Gui::Log::instance()->gui().debug("widget canvas move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pWidget);
}


void
ListViewImpl::extendWidgetPool()
{
    emit extendPoolSignal();
}


void
ListViewImpl::moveWidgetSlot(int row, ListWidget* pWidget)
{
    ListView* pListView =  static_cast<ListView*>(_pWidget);
    _proxyWidgets[pWidget]->setPos(0, pListView->_widgetHeight * row);
}


void
ListViewImpl::extendPoolSlot()
{
    ListView* pListView =  static_cast<ListView*>(_pWidget);
    pListView->extendWidgetPool(visibleRows());
}


//LazyListViewImpl::LazyListViewImpl(Widget* pWidget, Widget* pParent) :
//QScrollArea(static_cast<QWidget*>(pParent ? pParent->getNativeWidget() : 0)),
//_pScrollWidget(0)
//{
//    _pScrollWidget = new QWidget;
//    _pScrollWidget->resize(viewport()->size());
//    setWidget(_pScrollWidget);
//
//    connect(this, SIGNAL(moveWidgetSignal(int, ListWidget*)), this, SLOT(moveWidgetSlot(int, ListWidget*)));
//    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledSlot(int)));
//}
//
//
//LazyListViewImpl::~LazyListViewImpl()
//{
//}
//
//
//int
//LazyListViewImpl::visibleRows()
//{
//    int rows = viewport()->geometry().height() / _widgetHeight;
//    Omm::Gui::Log::instance()->gui().debug("widget list number of visible rows: " + Poco::NumberFormatter::format(rows));
//    return rows;
//}
//
//
//void
//LazyListViewImpl::initWidget(ListWidget* pWidget)
//{
//    static_cast<QtListWidget*>(pWidget)->resize(viewport()->width(), _widgetHeight);
//    static_cast<QtListWidget*>(pWidget)->setParent(_pScrollWidget);
//}
//
//
//void
//LazyListViewImpl::moveWidget(int row, ListWidget* pWidget)
//{
//    Omm::Gui::Log::instance()->gui().debug("widget list move item widget to row: " + Poco::NumberFormatter::format(row));
//    emit moveWidgetSignal(row, pWidget);
//}
//
//
//void
//LazyListViewImpl::updateScrollWidgetSize()
//{
//   _pScrollWidget->resize(geometry().width(), _pModel->totalItemCount() * _widgetHeight);
//}
//
//
//int
//LazyListViewImpl::getOffset()
//{
////    Omm::Gui::Log::instance()->gui().debug("scroll widget offset: " + Poco::NumberFormatter::format(_pScrollWidget->geometry().y()));
//    return _pScrollWidget->geometry().y();
//}
//
//
//void
//LazyListViewImpl::moveWidgetSlot(int row, ListWidget* pWidget)
//{
//    static_cast<QtListWidget*>(pWidget)->move(0, _widgetHeight * row);
//}
//
//
//void
//LazyListViewImpl::viewScrolledSlot(int value)
//{
//    scrolledToRow(-getOffset() / _widgetHeight);
//}
//
//
//void
//LazyListViewImpl::resizeEvent(QResizeEvent* pEvent)
//{
//    int rows = pEvent->size().height() / _widgetHeight;
//    Omm::Gui::Log::instance()->gui().debug("Qt widget list resize: " + Poco::NumberFormatter::format(rows));
//    if (pEvent->oldSize().height() > 0) {
//        WidgetListView::resize(rows);
//    }
//}


}  // namespace Omm
}  // namespace Gui
