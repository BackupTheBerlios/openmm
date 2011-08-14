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

#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Environment.h>
#include <Poco/NumberFormatter.h>

#include <Omm/UpnpAvLogger.h>

#include "AppGui.h"



Widget::Widget()
{
}


Widget::~Widget()
{
}


Widget::SelectNotification::SelectNotification()
{
}


void
Widget::registerEventNotificationHandler(const Poco::AbstractObserver& observer)
{
    _eventNotificationCenter.addObserver(observer);
}


void
Widget::select()
{
    _eventNotificationCenter.postNotification(new SelectNotification);
}


ListWidget::ListWidget() :
_row(0)
{

}


int
ListWidget::getRow()
{
    return _row;
}


void
ListWidget::setRow(int row)
{
    _row = row;
}


ListWidget::RowSelectNotification::RowSelectNotification(int row) :
_row(row)
{
}


void
ListWidget::select()
{
    Widget::select();
    _eventNotificationCenter.postNotification(new RowSelectNotification(_row));
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
        if (_pView) {
            _pView->insertItem(row);
        }
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
        if (_pView) {
            _pView->removeItem(row);
        }
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list model tries to remove item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
WidgetListModel::setWidgetFactory(ListWidgetFactory* pWidgetFactory)
{
    _pWidgetFactory = pWidgetFactory;
}


ListWidget*
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
    Omm::Av::Log::instance()->upnpav().debug("widget list view set model ...");

    // double link model and view.
    _pModel = pModel;
    if (_pModel) {
        _pModel->_pView = this;
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to set model (ignoring)");
        return;
    }

    // create an initial widget pool. This also retrieves the height of the widget.
    int rows = visibleRows();
    int rowsFetched = _pModel->fetch(std::min(_pModel->totalItemCount(), rows));

    extendWidgetPool(rows);

    // insert items that are already in the model.
    Omm::Av::Log::instance()->upnpav().debug("inserting number of items: " + Poco::NumberFormatter::format(_pModel->totalItemCount()));
//    for (int i = 0; i < std::min(_pModel->totalItemCount(), rows); i++) {
    for (int i = 0; i < rowsFetched; i++) {
        insertItem(i);
    }

    Omm::Av::Log::instance()->upnpav().debug("widget list view set model finished.");
}


void
WidgetListView::resize(int rows)
{
    int rowDelta = rows - widgetPoolSize();
    Omm::Av::Log::instance()->upnpav().debug("widget list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
    if (rowDelta > 0) {
        if (_rowOffset + _visibleWidgets.size() + rowDelta >= _pModel->lastFetched()) {
            _pModel->fetch(_visibleWidgets.size() + rowDelta);
        }
        extendWidgetPool(rowDelta);
        for (int i = 0; i < rowDelta; i++) {
            ListWidget* pWidget = _freeWidgets.top();
            _freeWidgets.pop();
            int lastRow = _rowOffset + _visibleWidgets.size();
            moveWidgetToRow(lastRow, pWidget);
            _pModel->attachWidget(lastRow, pWidget);
            _visibleWidgets.push_back(pWidget);
        }
    }
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
        ListWidget* pWidget = _pModel->createWidget();
        if (!pWidget) {
            Omm::Av::Log::instance()->upnpav().error("widget list view failed to create widget for pool (ignoring)");
            return;
        }
        pWidget->hideWidget();
        _widgetPool.push_back(pWidget);
        _freeWidgets.push(pWidget);
        initWidget(pWidget);
        pWidget->registerEventNotificationHandler(Poco::Observer<WidgetListView, ListWidget::RowSelectNotification>(*this, &WidgetListView::selectNotificationHandler));
        Omm::Av::Log::instance()->upnpav().debug("allocate widget[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
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


ListWidget*
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
    if (rowOffset + _visibleWidgets.size() > _pModel->totalItemCount()) {
        return;
    }

    int rowDeltaAbsolute = std::abs(rowDelta);

    if (rowOffset + _visibleWidgets.size() + rowDeltaAbsolute >= _pModel->lastFetched()) {
        _pModel->fetch(_visibleWidgets.size() + rowDeltaAbsolute);
    }

    Omm::Av::Log::instance()->upnpav().debug("scroll widget to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
    while (rowDeltaAbsolute--) {
        if (rowDelta > 0) {
            // detach first visible widget
            ListWidget* pWidget = _visibleWidgets.front();
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
            ListWidget* pWidget = _visibleWidgets.back();
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
WidgetListView::moveWidgetToRow(int row, ListWidget* pWidget)
{
    pWidget->setRow(row);
    moveWidget(row, pWidget);
}


void
WidgetListView::selectNotificationHandler(ListWidget::RowSelectNotification* pSelectNotification)
{
    _pModel->selectItem(pSelectNotification->_row);
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
        ListWidget* pWidget = _freeWidgets.top();
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
    ListWidget* pWidget = _pModel->getChildWidget(row);

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
        // FIXME: something's going wrong with removal of rows, duplicate rows appear and crash
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


Navigable::Navigable() :
_pNavigator(0)
{

}


Navigator*
Navigable::getNavigator() const
{
    return _pNavigator;
}


Navigator::Navigator()
{
}


Navigator::~Navigator()
{
}


void
Navigator::push(Navigable* pNavigable)
{
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " ...");
//    Omm::Util::Log::instance()->plugin().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString());

    pNavigable->_pNavigator = this;
    if (pNavigable->getWidget()) {
//        Omm::Av::Log::instance()->upnpav().debug("Qt navigator add widget: " + Poco::NumberFormatter::format(pNavigable->getWidget()));
        pushImpl(pNavigable);
    }
    _navigableStack.push(pNavigable);
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator showing widget ...");
    pNavigable->show();
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " finished.");
}


