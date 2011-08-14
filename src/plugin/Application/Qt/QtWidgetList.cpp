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

#include "QtWidgetList.h"
#include "QtListWidget.h"


QtWidgetList::QtWidgetList(QWidget* pParent) :
QScrollArea(pParent),
WidgetListView(32, true),
_pScrollWidget(0)
{
    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(viewport()->size());
    setWidget(_pScrollWidget);

    connect(this, SIGNAL(moveWidgetSignal(int, ListWidget*)), this, SLOT(moveWidgetSlot(int, ListWidget*)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledSlot(int)));
}


QtWidgetList::~QtWidgetList()
{
}


int
QtWidgetList::visibleRows()
{
    int rows = viewport()->geometry().height() / _widgetHeight;
    Omm::Av::Log::instance()->upnpav().debug("widget list number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
QtWidgetList::initWidget(ListWidget* pWidget)
{
    static_cast<QtListWidget*>(pWidget)->resize(viewport()->width(), _widgetHeight);
    static_cast<QtListWidget*>(pWidget)->setParent(_pScrollWidget);
}


void
QtWidgetList::moveWidget(int row, ListWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pWidget);
}


void
QtWidgetList::updateScrollWidgetSize()
{
   _pScrollWidget->resize(geometry().width(), _pModel->totalItemCount() * _widgetHeight);
}


int
QtWidgetList::getOffset()
{
//    Omm::Av::Log::instance()->upnpav().debug("scroll widget offset: " + Poco::NumberFormatter::format(_pScrollWidget->geometry().y()));
    return _pScrollWidget->geometry().y();
}


void
QtWidgetList::moveWidgetSlot(int row, ListWidget* pWidget)
{
    static_cast<QtListWidget*>(pWidget)->move(0, _widgetHeight * row);
}


void
QtWidgetList::viewScrolledSlot(int value)
{
    scrolledToRow(-getOffset() / _widgetHeight);
}


void
QtWidgetList::resizeEvent(QResizeEvent* pEvent)
{
    int rows = pEvent->size().height() / _widgetHeight;
    Omm::Av::Log::instance()->upnpav().debug("Qt widget list resize: " + Poco::NumberFormatter::format(rows));
    if (pEvent->oldSize().height() > 0) {
        WidgetListView::resize(rows);
    }
}


QtWidgetCanvas::QtWidgetCanvas(bool movableWidgets, QWidget* pParent) :
QGraphicsView(pParent),
WidgetListView(50, false),
_movableWidgets(movableWidgets)
{
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    _pGraphicsScene = new QGraphicsScene;
    setScene(_pGraphicsScene);

    connect(this, SIGNAL(moveWidgetSignal(int, ListWidget*)), this, SLOT(moveWidgetSlot(int, ListWidget*)));
    connect(this, SIGNAL(extendPoolSignal()), this, SLOT(extendPoolSlot()));
}


QtWidgetCanvas::~QtWidgetCanvas()
{
}


int
QtWidgetCanvas::visibleRows()
{
    int rows = viewport()->geometry().height() / _widgetHeight;
    Omm::Av::Log::instance()->upnpav().debug("widget canvas number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
QtWidgetCanvas::initWidget(ListWidget* pWidget)
{
    QtListWidget* pQtWidget = static_cast<QtListWidget*>(pWidget);
    pQtWidget->resize(viewport()->width(), _widgetHeight);

    if (_movableWidgets) {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQtWidget, Qt::Window);
    }
    else {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQtWidget);
   }
}


void
QtWidgetCanvas::moveWidget(int row, ListWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget canvas move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pWidget);
}


void
QtWidgetCanvas::extendWidgetPool()
{
    emit extendPoolSignal();
}


void
QtWidgetCanvas::moveWidgetSlot(int row, ListWidget* pWidget)
{
    _proxyWidgets[pWidget]->setPos(0, _widgetHeight * row);
}


void
QtWidgetCanvas::extendPoolSlot()
{
    WidgetListView::extendWidgetPool(visibleRows());
}
