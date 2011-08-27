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
#include "Gui/View.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ListViewImpl.h"
#endif


namespace Omm {
namespace Gui {


ListView::ListView(int viewHeight, bool movableViews, View* pParent) :
View(new ListViewImpl(this, pParent), pParent),
_pModel(0),
_viewHeight(viewHeight),
_rowOffset(0)
//_lazy(false)
{
}


void
ListView::setModel(ListModel* pModel)
{
    Log::instance()->gui().debug("list view set model ...");

    // double link model and view.
    _pModel = pModel;
    if (_pModel) {
        _pModel->_pView = this;
    }
    else {
        Log::instance()->gui().error("list view failed to set model (ignoring)");
        return;
    }

    // create an initial view pool. This also retrieves the height of the view.
    int rows = visibleRows();
    int rowsFetched = _pModel->fetch(std::min(_pModel->totalItemCount(), rows));

    extendViewPool(rows);

    // insert items that are already in the model.
    Log::instance()->gui().debug("inserting number of items: " + Poco::NumberFormatter::format(_pModel->totalItemCount()));
//    for (int i = 0; i < std::min(_pModel->totalItemCount(), rows); i++) {
    for (int i = 0; i < rowsFetched; i++) {
        insertItem(i);
    }

    Log::instance()->gui().debug("list view set model finished.");
}


int
ListView::visibleRows()
{
    return static_cast<ListViewImpl*>(_pImpl)->visibleRows();
}


void
ListView::initView(ListItemView* pView)
{
    static_cast<ListViewImpl*>(_pImpl)->initView(pView);
}


void
ListView::moveView(int row, ListItemView* pView)
{
    static_cast<ListViewImpl*>(_pImpl)->moveView(row, pView);
}


void
ListView::resize(int rows)
{
    int rowDelta = rows - viewPoolSize();
    Log::instance()->gui().debug("list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
    if (rowDelta > 0) {
        if (_rowOffset + _visibleViews.size() + rowDelta >= _pModel->lastFetched()) {
            _pModel->fetch(_visibleViews.size() + rowDelta);
        }
        extendViewPool(rowDelta);
        for (int i = 0; i < rowDelta; i++) {
            ListItemView* pView = _freeViews.top();
            _freeViews.pop();
            int lastRow = _rowOffset + _visibleViews.size();
            moveViewToRow(lastRow, pView);
            _pModel->attachView(lastRow, pView);
            _visibleViews.push_back(pView);
        }
    }
}


void
ListView::extendViewPool()
{
    static_cast<ListViewImpl*>(_pImpl)->extendViewPool();
}


int
ListView::viewPoolSize()
{
    return _viewPool.size();
}


void
ListView::extendViewPool(int n)
{
    Log::instance()->gui().debug("list view extend view pool by number of views: " + Poco::NumberFormatter::format(n));

    for (int i = 0; i < n; ++i) {
        ListItemView* pView = _pModel->createView();
        if (!pView) {
            Log::instance()->gui().error("list view failed to create view for pool (ignoring)");
            return;
        }
        pView->hide();
        _viewPool.push_back(pView);
        _freeViews.push(pView);
        initView(pView);
//        pView->connect(Poco::Observer<ListView, ListItemView::RowSelectNotification>(*this, &ListView::selectNotificationHandler));
        Log::instance()->gui().debug("allocate view[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pView));
    }
}


int
ListView::visibleIndex(int row)
{
    return row - _rowOffset;
}


int
ListView::countVisibleViews()
{
    return _visibleViews.size();
}


ListItemView*
ListView::visibleView(int index)
{
    if (0 <= index && index < _visibleViews.size()) {
        return _visibleViews[index];
    }
    else {
        Log::instance()->gui().error("list view failed to retrieve visible view, out of range (ignoring)");
        return 0;
    }
}


//void
//ListView::scrolledToRow(int rowOffset)
//{
//    int rowDelta = rowOffset - _rowOffset;
//
//    if (rowDelta == 0) {
//        return;
//    }
//    if (rowOffset + _visibleWidgets.size() > _pModel->totalItemCount()) {
//        return;
//    }
//
//    int rowDeltaAbsolute = std::abs(rowDelta);
//
//    if (rowOffset + _visibleWidgets.size() + rowDeltaAbsolute >= _pModel->lastFetched()) {
//        _pModel->fetch(_visibleWidgets.size() + rowDeltaAbsolute);
//    }
//
//    Log::instance()->gui().debug("scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
//    while (rowDeltaAbsolute--) {
//        if (rowDelta > 0) {
//            // detach first visible view
//            ListWidget* pView = _visibleWidgets.front();
//            _pModel->detachWidget(_rowOffset);
//            // move first view to the end
//            int lastRow = _rowOffset + _visibleWidgets.size();
//            moveWidgetToRow(lastRow, pView);
//            // attach view
//            _pModel->attachWidget(lastRow, pView);
//            // move view to end of visible rows
//            _visibleWidgets.erase(_visibleWidgets.begin());
//            _visibleWidgets.push_back(pView);
//            _rowOffset++;
//        }
//        else if (rowDelta < 0) {
//            // detach last visible view
//            ListWidget* pView = _visibleWidgets.back();
//            int lastRow = _rowOffset + _visibleWidgets.size() - 1;
//            _pModel->detachWidget(lastRow);
//            // move last view to the beginning
//            moveWidgetToRow(_rowOffset - 1, pView);
//            // attach view
//            _pModel->attachWidget(_rowOffset - 1, pView);
//            // move view to beginning of visible rows
//            _visibleWidgets.erase(_visibleWidgets.end() - 1);
//            _visibleWidgets.insert(_visibleWidgets.begin(), pView);
//            _rowOffset--;
//        }
//    }
//}


bool
ListView::itemIsVisible(int row)
{
    return _rowOffset <= row && row < _rowOffset + visibleRows();
}


void
ListView::moveViewToRow(int row, ListItemView* pView)
{
    pView->setRow(row);
    moveView(row, pView);
}


void
ListView::selectNotificationHandler(ListItemView::RowSelectNotification* pSelectNotification)
{
    _pModel->selectItem(pSelectNotification->_row);
}


void
ListView::insertItem(int row)
{
//    if (_lazy) {
//        // resize view to the size with this item added
//        updateScrollWidgetSize();
//        // check if item is visible
//        if (!itemIsVisible(row)) {
//            Log::instance()->gui().debug("list view insert item that is not visible (ignoring)");
//            return;
//        }
//    }
//    else {
        // if view is not lazy, view pool has to be extended when too small and new views are inserted.
        if (_visibleViews.size() >= _viewPool.size()) {
            extendViewPool();
        }
//    }

    // attach item to a free (not visible) view
    if (_freeViews.size()) {
        ListItemView* pView = _freeViews.top();
        _freeViews.pop();
        _visibleViews.insert(_visibleViews.begin() + visibleIndex(row), pView);
        _pModel->attachView(row, pView);
        // FIXME: move all views below one down
        // FIXME: detach last view if not visible anymore
        moveViewToRow(row, pView);
    }
    else {
        Log::instance()->gui().error("list view failed to attach view to item, view pool is empty (ignoring)");
    }
}


void
ListView::removeItem(int row)
{
//    if (_lazy) {
//        // resize view to the size with this item added
//        updateScrollWidgetSize();
//        // check if item is visible
//        if (!itemIsVisible(row)) {
//            Log::instance()->gui().debug("list view remove item that is not visible (ignoring)");
//            return;
//        }
//    }

    // detach item from visible view
    ListItemView* pView = _pModel->getChildView(row);

    // remove view from this position in visible views
    int index = visibleIndex(row);
    int lastRow = _rowOffset + _visibleViews.size();
    // move all views below one up
    for (int i = index + 1; i < countVisibleViews(); i++) {
        moveViewToRow(row + i - index - 1, visibleView(i));
    }
    _pModel->detachView(row);
    _visibleViews.erase(_visibleViews.begin() + visibleIndex(row));

    if (_pModel->totalItemCount() - lastRow > 0) {
        // FIXME: something's going wrong with removal of rows, duplicate rows appear and crash
        // reuse and attach view below last view cause it is now becoming visible
        Log::instance()->gui().debug("list view reuse removed item view");
        _pModel->attachView(lastRow - 1, pView);
        _visibleViews.push_back(pView);
        moveViewToRow(lastRow - 1, pView);
    }
    else {
        Log::instance()->gui().debug("list view free removed item view");
        // otherwise free view
        _freeViews.push(pView);
    }
}



} // namespace Gui
} // namespace Omm
