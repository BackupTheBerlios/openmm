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

#include "Gui/ListView.h"
#include "Gui/GuiLogger.h"
#include "Gui/ListModel.h"
#include "Gui/Widget.h"

//#ifdef __GUI_QT_PLATFORM__
//#include "Qt/ListViewImpl.h"
//#endif


namespace Omm {
namespace Gui {


ListView::ListView(int widgetHeight, bool lazy) :
_lazy(lazy),
_pModel(0),
_widgetHeight(widgetHeight),
_rowOffset(0)
{
}


void
ListView::setModel(ListModel* pModel)
{
    Log::instance()->gui().debug("widget list view set model ...");

    // double link model and view.
    _pModel = pModel;
    if (_pModel) {
        _pModel->_pView = this;
    }
    else {
        Log::instance()->gui().error("widget list view failed to set model (ignoring)");
        return;
    }

    // create an initial widget pool. This also retrieves the height of the widget.
    int rows = visibleRows();
    int rowsFetched = _pModel->fetch(std::min(_pModel->totalItemCount(), rows));

    extendWidgetPool(rows);

    // insert items that are already in the model.
    Log::instance()->gui().debug("inserting number of items: " + Poco::NumberFormatter::format(_pModel->totalItemCount()));
//    for (int i = 0; i < std::min(_pModel->totalItemCount(), rows); i++) {
    for (int i = 0; i < rowsFetched; i++) {
        insertItem(i);
    }

    Log::instance()->gui().debug("widget list view set model finished.");
}


void
ListView::resize(int rows)
{
    int rowDelta = rows - widgetPoolSize();
    Log::instance()->gui().debug("widget list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
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
ListView::widgetPoolSize()
{
    return _widgetPool.size();
}


void
ListView::extendWidgetPool(int n)
{
    Log::instance()->gui().debug("widget list view extend widget pool by number of widgets: " + Poco::NumberFormatter::format(n));

    for (int i = 0; i < n; ++i) {
        ListWidget* pWidget = _pModel->createWidget();
        if (!pWidget) {
            Log::instance()->gui().error("widget list view failed to create widget for pool (ignoring)");
            return;
        }
        pWidget->hide();
        _widgetPool.push_back(pWidget);
        _freeWidgets.push(pWidget);
        initWidget(pWidget);
        pWidget->connect(Poco::Observer<ListView, ListWidget::RowSelectNotification>(*this, &ListView::selectNotificationHandler));
        Log::instance()->gui().debug("allocate widget[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
    }
}


int
ListView::visibleIndex(int row)
{
    return row - _rowOffset;
}


int
ListView::countVisibleWidgets()
{
    return _visibleWidgets.size();
}


ListWidget*
ListView::visibleWidget(int index)
{
    if (0 <= index && index < _visibleWidgets.size()) {
        return _visibleWidgets[index];
    }
    else {
        Log::instance()->gui().error("widget list view failed to retrieve visible widget, out of range (ignoring)");
        return 0;
    }
}


void
ListView::scrolledToRow(int rowOffset)
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

    Log::instance()->gui().debug("scroll widget to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
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
ListView::itemIsVisible(int row)
{
    return _rowOffset <= row && row < _rowOffset + visibleRows();
}


void
ListView::moveWidgetToRow(int row, ListWidget* pWidget)
{
    pWidget->setRow(row);
    moveWidget(row, pWidget);
}


void
ListView::selectNotificationHandler(ListWidget::RowSelectNotification* pSelectNotification)
{
    _pModel->selectItem(pSelectNotification->_row);
}


void
ListView::insertItem(int row)
{
    if (_lazy) {
        // resize view to the size with this item added
        updateScrollWidgetSize();
        // check if item is visible
        if (!itemIsVisible(row)) {
            Log::instance()->gui().debug("widget list view insert item that is not visible (ignoring)");
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
        Log::instance()->gui().error("widget list view failed to attach widget to item, widget pool is empty (ignoring)");
    }
}


void
ListView::removeItem(int row)
{
    if (_lazy) {
        // resize view to the size with this item added
        updateScrollWidgetSize();
        // check if item is visible
        if (!itemIsVisible(row)) {
            Log::instance()->gui().debug("widget list view remove item that is not visible (ignoring)");
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
        Log::instance()->gui().debug("widget list view reuse removed item widget");
        _pModel->attachWidget(lastRow - 1, pWidget);
        _visibleWidgets.push_back(pWidget);
        moveWidgetToRow(lastRow - 1, pWidget);
    }
    else {
        Log::instance()->gui().debug("widget list view free removed item widget");
        // otherwise free widget
        _freeWidgets.push(pWidget);
    }
}



} // namespace Gui
} // namespace Omm
