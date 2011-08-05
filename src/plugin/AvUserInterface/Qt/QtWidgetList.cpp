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


Widget::Widget() :
_row(0)
{

}


int
Widget::getRow()
{
    return _row;
}


void
Widget::setRow(int row)
{
    _row = row;
}


WidgetListModel::WidgetListModel() :
_pView(0),
_pWidgetFactory(0)
{
}


void
WidgetListModel::insertItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Omm::Av::Log::instance()->upnpav().debug("widget list model insert row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        _pView->insertItem(row);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list model tries to insert item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
WidgetListModel::removeItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Omm::Av::Log::instance()->upnpav().debug("widget list model remove row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        _pView->removeItem(row);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list model tries to remove item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
WidgetListModel::setWidgetFactory(WidgetFactory* pWidgetFactory)
{
    _pWidgetFactory = pWidgetFactory;
}


Widget*
WidgetListModel::createWidget()
{
    if (_pWidgetFactory) {
        return _pWidgetFactory->createWidget();
    }
    return 0;
}


WidgetListView::WidgetListView(int widgetHeight, bool lazy) :
_lazy(lazy),
_pModel(0),
_widgetHeight(widgetHeight),
_rowOffset(0)
{
}


void
WidgetListView::setModel(WidgetListModel* pModel)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list view set model");
    
    // double link model and view.
    _pModel = pModel;
    _pModel->_pView = this;

    // create an initial widget pool. This also retrieves the height of the widget.
    extendWidgetPool(visibleRows());
}


int
WidgetListView::widgetPoolSize()
{
    return _widgetPool.size();
}


void
WidgetListView::extendWidgetPool(int n)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list view extend widget pool by number of widgets: " + Poco::NumberFormatter::format(n));

    for (int i = 0; i < n; ++i) {
        Widget* pWidget = _pModel->createWidget();
        pWidget->hideWidget();
        _widgetPool.push_back(pWidget);
        _freeWidgets.push(pWidget);
        initWidget(pWidget);
        Omm::Av::Log::instance()->upnpav().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
    }
}


int
WidgetListView::visibleIndex(int row)
{
    return row - _rowOffset;
}


int
WidgetListView::countVisibleWidgets()
{
    return _visibleWidgets.size();
}


Widget*
WidgetListView::visibleWidget(int index)
{
    if (0 <= index && index < _visibleWidgets.size()) {
        return _visibleWidgets[index];
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to retrieve visible widget, out of range (ignoring)");
        return 0;
    }
}


void
WidgetListView::scrolledToRow(int rowOffset)
{
    int rowDelta = rowOffset - _rowOffset;

    if (rowDelta == 0) {
        return;
    }

    int rowDeltaAbsolute = std::abs(rowDelta);
    Omm::Av::Log::instance()->upnpav().debug("scroll widget to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
    while (rowDeltaAbsolute--) {
        if (rowDelta > 0) {
            // detach first visible widget
            Widget* pWidget = _visibleWidgets.front();
            _pModel->detachWidget(_rowOffset);
            // move first widget to the end
            int lastRow = _rowOffset + _visibleWidgets.size();
            moveWidgetToRow(lastRow, pWidget);
            // attach widget
            _pModel->attachWidget(lastRow, pWidget);
            // move widget to end of visible rows
            _visibleWidgets.erase(_visibleWidgets.begin());
            _visibleWidgets.push_back(pWidget);
            _rowOffset++;
        }
        else if (rowDelta < 0) {
            // detach last visible widget
            Widget* pWidget = _visibleWidgets.back();
            int lastRow = _rowOffset + _visibleWidgets.size() - 1;
            _pModel->detachWidget(lastRow);
            // move last widget to the beginning
            moveWidgetToRow(_rowOffset - 1, pWidget);
            // attach widget
            _pModel->attachWidget(_rowOffset - 1, pWidget);
            // move widget to beginning of visible rows
            _visibleWidgets.erase(_visibleWidgets.end() - 1);
            _visibleWidgets.insert(_visibleWidgets.begin(), pWidget);
            _rowOffset--;
        }
    }
}


bool
WidgetListView::itemIsVisible(int row)
{
    return _rowOffset <= row && row < _rowOffset + visibleRows();
}


void
WidgetListView::insertItem(int row)
{
    if (_lazy) {
        // resize view to the size with this item added
        updateScrollWidgetSize();
        // check if item is visible
        if (!itemIsVisible(row)) {
            Omm::Av::Log::instance()->upnpav().debug("widget list view insert item that is not visible (ignoring)");
            return;
        }
    }
    else {
        // if view is not lazy, widget pool has to be extended when too small and new widgets are inserted.
        if (_visibleWidgets.size() >= _widgetPool.size()) {
            extendWidgetPool();
        }
    }

    // attach item to a free (not visible) widget
    if (_freeWidgets.size()) {
        Widget* pWidget = _freeWidgets.top();
        _freeWidgets.pop();
        _visibleWidgets.insert(_visibleWidgets.begin() + visibleIndex(row), pWidget);
        _pModel->attachWidget(row, pWidget);
        // FIXME: move all widgets below one down
        // FIXME: detach last widget if not visible anymore
        moveWidgetToRow(row, pWidget);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to attach widget to item, widget pool is empty (ignoring)");
    }
}


void
WidgetListView::removeItem(int row)
{
    if (_lazy) {
        // resize view to the size with this item added
        updateScrollWidgetSize();
        // check if item is visible
        if (!itemIsVisible(row)) {
            Omm::Av::Log::instance()->upnpav().debug("widget list view remove item that is not visible (ignoring)");
            return;
        }
    }
    
    // detach item from visible widget
    Widget* pWidget = _pModel->getWidget(row);

    // remove widget from this position in visible widgets
    int index = visibleIndex(row);
    int lastRow = _rowOffset + _visibleWidgets.size();
    // move all widgets below one up
    for (int i = index + 1; i < countVisibleWidgets(); i++) {
        moveWidgetToRow(row + i - index - 1, visibleWidget(i));
    }
    _pModel->detachWidget(row);
    _visibleWidgets.erase(_visibleWidgets.begin() + visibleIndex(row));
    
    if (_pModel->totalItemCount() - lastRow > 0) {
        // reuse and attach widget below last widget cause it is now becoming visible
        Omm::Av::Log::instance()->upnpav().debug("widget list view reuse removed item widget");
        _pModel->attachWidget(lastRow - 1, pWidget);
        _visibleWidgets.push_back(pWidget);
        moveWidgetToRow(lastRow - 1, pWidget);
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("widget list view free removed item widget");
        // otherwise free widget
        _freeWidgets.push(pWidget);
    }
}


void
WidgetListView::selectedRow(int row)
{
    _pModel->selectItem(row);
}



void
QtWidget::showWidget()
{
    QWidget::show();
}


void
QtWidget::hideWidget()
{
    QWidget::hide();
}


QtWidgetList::QtWidgetList(QWidget* pParent) :
QScrollArea(pParent),
WidgetListView(50, true),
_pScrollWidget(0)
{
    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(viewport()->size());
    setWidget(_pScrollWidget);

    connect(this, SIGNAL(moveWidget(int, Widget*)), this, SLOT(move(int, Widget*)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolled(int)));
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
QtWidgetList::initWidget(Widget* pWidget)
{
    static_cast<QtWidget*>(pWidget)->resize(viewport()->width(), _widgetHeight);
    static_cast<QtWidget*>(pWidget)->setParent(_pScrollWidget);
}


void
QtWidgetList::moveWidgetToRow(int row, Widget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidget(row, pWidget);
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
QtWidgetList::move(int row, Widget* pWidget)
{
    static_cast<QtWidget*>(pWidget)->move(0, _widgetHeight * row);
}


void
QtWidgetList::viewScrolled(int value)
{
    scrolledToRow(-getOffset() / _widgetHeight);
}


class QtWidgetScene : public QGraphicsScene
{
private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent);
};


void
QtWidgetScene::mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent)
{
    Omm::Av::Log::instance()->upnpav().debug("widget scene mouse pressed");
    itemAt(pMouseEvent->scenePos());

    QGraphicsScene::mousePressEvent(pMouseEvent);
}


class QtWidgetCanvasItem : public QGraphicsProxyWidget
{
    friend class QtWidgetCanvas;
    
public:
    QtWidgetCanvasItem(QGraphicsItem* pParent = 0, Qt::WindowFlags wFlags = 0);

private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent);

    QtWidgetCanvas*     _pWidgetCanvas;
    int                 _row;
};


void
QtWidgetCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent)
{
    Omm::Av::Log::instance()->upnpav().debug("widget canvas item mouse pressed in widget with row: " + Poco::NumberFormatter::format(_row));
    _pWidgetCanvas->selectedRow(_row);
    QGraphicsProxyWidget::mousePressEvent(pMouseEvent);
}


QtWidgetCanvasItem::QtWidgetCanvasItem(QGraphicsItem* pParent, Qt::WindowFlags wFlags) :
QGraphicsProxyWidget(pParent, wFlags)
{

}


QtWidgetCanvas::QtWidgetCanvas(bool movableWidgets, QWidget* pParent) :
QGraphicsView(pParent),
WidgetListView(50, false),
_movableWidgets(movableWidgets)
{
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    _pGraphicsScene = new QGraphicsScene;
//    _pGraphicsScene = new QtWidgetScene;
    setScene(_pGraphicsScene);

    connect(this, SIGNAL(moveWidget(int, Widget*)), this, SLOT(move(int, Widget*)));
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
QtWidgetCanvas::initWidget(Widget* pWidget)
{
    QtWidget* pQtWidget = static_cast<QtWidget*>(pWidget);
    pQtWidget->resize(viewport()->width(), _widgetHeight);

    QtWidgetCanvasItem* pProxyWidget;
    if (_movableWidgets) {
        pProxyWidget = new QtWidgetCanvasItem(0, Qt::Window);
    }
    else {
        pProxyWidget = new QtWidgetCanvasItem;
   }
    pProxyWidget->_pWidgetCanvas = this;
    pProxyWidget->setWidget(pQtWidget);
    _pGraphicsScene->addItem(pProxyWidget);
    _proxyWidgets[pWidget] = pProxyWidget;
//    _proxyWidgets[pWidget] = _pGraphicsScene->addWidget(pWidget);
}


void
QtWidgetCanvas::moveWidgetToRow(int row, Widget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget canvas move item widget to row: " + Poco::NumberFormatter::format(row));
    // with a non-lazy model, move widget is only called when inserting / removing widgets (lazy models also scroll widgets)
    _proxyWidgets[pWidget]->_row = row;
    emit moveWidget(row, pWidget);
}


void
QtWidgetCanvas::extendWidgetPool()
{
    emit extendPoolSignal();
}


void
QtWidgetCanvas::move(int row, Widget* pWidget)
{
    _proxyWidgets[pWidget]->setPos(0, _widgetHeight * row);
}


void
QtWidgetCanvas::extendPoolSlot()
{
    WidgetListView::extendWidgetPool(visibleRows());
}
