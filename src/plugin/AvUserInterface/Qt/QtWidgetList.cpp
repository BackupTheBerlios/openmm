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


QtWidgetList::QtWidgetList(QWidget* pParent) :
QScrollArea(pParent),
WidgetListView(50, true),
_pScrollWidget(0)
{
    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(viewport()->size());
    setWidget(_pScrollWidget);

    connect(this, SIGNAL(moveWidgetSignal(int, Omm::Util::Widget*)), this, SLOT(moveWidgetSlot(int, Omm::Util::Widget*)));
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
QtWidgetList::initWidget(Omm::Util::Widget* pWidget)
{
    static_cast<QtWidget*>(pWidget)->resize(viewport()->width(), _widgetHeight);
    static_cast<QtWidget*>(pWidget)->setParent(_pScrollWidget);
}


void
QtWidgetList::moveWidget(int row, Omm::Util::Widget* pWidget)
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
QtWidgetList::moveWidgetSlot(int row, Omm::Util::Widget* pWidget)
{
    static_cast<QtWidget*>(pWidget)->move(0, _widgetHeight * row);
}


void
QtWidgetList::viewScrolledSlot(int value)
{
    scrolledToRow(-getOffset() / _widgetHeight);
}


QtWidgetCanvas::QtWidgetCanvas(bool movableWidgets, QWidget* pParent) :
QGraphicsView(pParent),
WidgetListView(50, false),
_movableWidgets(movableWidgets)
{
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    _pGraphicsScene = new QGraphicsScene;
    setScene(_pGraphicsScene);

    connect(this, SIGNAL(moveWidgetSignal(int, Omm::Util::Widget*)), this, SLOT(moveWidgetSlot(int, Omm::Util::Widget*)));
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
QtWidgetCanvas::initWidget(Omm::Util::Widget* pWidget)
{
    QtWidget* pQtWidget = static_cast<QtWidget*>(pWidget);
    pQtWidget->resize(viewport()->width(), _widgetHeight);

    if (_movableWidgets) {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQtWidget, Qt::Window);
    }
    else {
        _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pQtWidget);
   }
}


void
QtWidgetCanvas::moveWidget(int row, Omm::Util::Widget* pWidget)
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
QtWidgetCanvas::moveWidgetSlot(int row, Omm::Util::Widget* pWidget)
{
    _proxyWidgets[pWidget]->setPos(0, _widgetHeight * row);
}


void
QtWidgetCanvas::extendPoolSlot()
{
    Omm::Util::WidgetListView::extendWidgetPool(visibleRows());
}