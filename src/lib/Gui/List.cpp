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

#include "Gui/List.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"


namespace Omm {
namespace Gui {


class ListItemController : public Controller
{
    friend class ListView;
    
public:
    void setRow(int row) { _row = row; }

private:
    virtual void selected();

    ListView*     _pListView;
    int           _row;
};


void
ListItemController::selected()
{
    Log::instance()->gui().debug("list item controller selected row: " + Poco::NumberFormatter::format(_row));
    _pListView->selectedItem(_row);
}


class ListScrollAreaController : public ScrollAreaController
{
    friend class ListView;

    ListScrollAreaController(ListView* pListView) : _pListView(pListView) {}

    virtual void scrolled(int xOffset, int yOffset);
    virtual void resized(int width, int height);
    virtual void presented();

    ListView*   _pListView;
};


void
ListScrollAreaController::scrolled(int xOffset, int yOffset)
{
    _pListView->scrolledToRow(yOffset / _pListView->getItemViewHeight());
}


void
ListScrollAreaController::resized(int width, int height)
{
    _pListView->resizeScrollArea(width, _pListView->getScrollAreaHeight());
    _pListView->resize(width, height);
}


void
ListScrollAreaController::presented()
{
    resized(_pListView->width(), _pListView->height());
}


ListView::ListView(View* pParent) :
ScrollAreaView(pParent),
_itemViewHeight(50),
_rowOffset(0),
_pSelectedView(0),
_selectedRow(-1),
_lastVisibleRows(0)
{
    attachController(new ListScrollAreaController(this));
}


void
ListView::setModel(ListModel* pModel)
{
    Log::instance()->gui().debug("list view set model ...");

    View::setModel(pModel);

    int rows = visibleRows();
    _lastVisibleRows = rows;
    extendViewPool(rows);

    // insert items that are already in the model.
    Log::instance()->gui().debug("inserting number of items: " + Poco::NumberFormatter::format(pModel->totalItemCount()));
    for (int i = 0; i < std::min(pModel->totalItemCount(), rows); i++) {
        insertItem(i);
    }

    Log::instance()->gui().debug("list view set model finished.");
}


int
ListView::getItemViewHeight()
{
    return _itemViewHeight;
}


void
ListView::setItemViewHeight(int height)
{
    _itemViewHeight = height;
}


int
ListView::visibleRows()
{
    return getViewportHeight() / _itemViewHeight + 2;
}


void
ListView::addItemView(View* pView)
{
//    Log::instance()->gui().debug("list view add item view.");

    pView->resize(getViewportWidth(), _itemViewHeight);
    addSubview(pView);
}


void
ListView::moveItemView(int row, View* pView)
{
    pView->move(0, _itemViewHeight * row);
}


int
ListView::getOffset()
{
    return getYOffset();
}


void
ListView::updateScrollWidgetSize()
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    resizeScrollArea(getViewportWidth(), pModel->totalItemCount() * _itemViewHeight);
}


void
ListView::scrollDelta(int rowDelta)
{
//   Log::instance()->gui().debug("list scroll delta view");
    
   ListModel* pModel = static_cast<ListModel*>(_pModel);
   if (rowDelta > 0) {
        // detach model from first visible view
        View* pView = _visibleViews.front();
        pModel->getItemModel(_rowOffset)->detachView(pView);
        // move first view to the end
        int lastRow = _rowOffset + _visibleViews.size();
        moveViewToRow(lastRow, pView);
        // attach model
        pView->setModel(pModel->getItemModel(lastRow));
        _itemControllers[pView]->setRow(lastRow);
        // move view to end of visible rows
        _visibleViews.erase(_visibleViews.begin());
        _visibleViews.push_back(pView);
        _rowOffset++;
    }
    else if (rowDelta < 0) {
        // detach model from last visible view
        View* pView = _visibleViews.back();
        int lastRow = _rowOffset + _visibleViews.size() - 1;
        pModel->getItemModel(lastRow)->detachView(pView);
        // move last view to the beginning
        moveViewToRow(_rowOffset - 1, pView);
        // attach model
        pView->setModel(pModel->getItemModel(_rowOffset - 1));
        _itemControllers[pView]->setRow(_rowOffset - 1);
        // move view to beginning of visible rows
        _visibleViews.erase(_visibleViews.end() - 1);
        _visibleViews.insert(_visibleViews.begin(), pView);
        _rowOffset--;
    }
}


void
ListView::scrolledToRow(int rowOffset)
{
//    Log::instance()->gui().debug("list scroll view");

    if (rowOffset < 0) {
        return;
    }

    ListModel* pModel = static_cast<ListModel*>(_pModel);

    int rowDelta = rowOffset - _rowOffset;

    if (rowDelta == 0) {
        return;
    }
    if (rowOffset + _visibleViews.size() > pModel->totalItemCount()) {
        return;
    }

    int rowDeltaAbsolute = std::abs(rowDelta);
//    Log::instance()->gui().debug("list scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
    while (rowDeltaAbsolute--) {
        scrollDelta(rowDelta);
    }

    handleSelectionHighlight();
}


void
ListView::resizeDelta(int rowDelta, int width)
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    extendViewPool(rowDelta);
    for (std::vector<View*>::iterator it = _viewPool.begin(); it != _viewPool.end(); ++it) {
        (*it)->resize(width, _itemViewHeight);
    }
    if (rowDelta <= 0) {
        return;
    }
//    Log::instance()->gui().debug("list view resize visible views: " + Poco::NumberFormatter::format(_visibleViews.size())
//        + ", view height in rows: " + Poco::NumberFormatter::format(static_cast<ListViewImpl*>(_pImpl)->visibleRows()));
//    if (pModel->totalItemCount() < visibleRows()) {
    if (pModel->totalItemCount() < visibleRows()) {
        return;
    }
    _lastVisibleRows += rowDelta;
    for (int i = 0; i < rowDelta; i++) {
        View* pView = _freeViews.top();
        _freeViews.pop();
        int lastRow = _rowOffset + _visibleViews.size();
        moveViewToRow(lastRow, pView);
        pView->setModel(pModel->getItemModel(lastRow));
        _itemControllers[pView]->setRow(lastRow);
        pView->show();
        _visibleViews.push_back(pView);
    }
}


void
ListView::extendViewPool(int n)
{
    Log::instance()->gui().debug("list view extend view pool by number of views: " + Poco::NumberFormatter::format(n));

    ListModel* pModel = static_cast<ListModel*>(_pModel);

    for (int i = 0; i < n; ++i) {
//        Log::instance()->gui().debug("list view extend view create item view ... pModel: " + Poco::NumberFormatter::format(pModel));
        View* pView = pModel->createItemView();
        if (!pView) {
            Log::instance()->gui().error("list view failed to create view for pool (ignoring)");
            return;
        }
//        Log::instance()->gui().debug("list view extend view created item view " + pView->getName());
        pView->hide();
        _viewPool.push_back(pView);
        _freeViews.push(pView);
        addItemView(pView);

//        Log::instance()->gui().debug("list view creating list item controller ...");
        ListItemController* pItemController = new ListItemController;
        pItemController->_pListView = this;
        _itemControllers[pView] = pItemController;
        pView->attachController(pItemController);

//        Log::instance()->gui().debug("allocate view[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pView));
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


View*
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


bool
ListView::itemIsVisible(int row)
{
    return _rowOffset <= row && row < _rowOffset + visibleRows();
}


void
ListView::moveViewToRow(int row, View* pView)
{
    moveItemView(row, pView);
}


void
ListView::handleSelectionHighlight()
{
    if (!_pSelectedView) {
        return;
    }
    if (itemIsVisible(_selectedRow)) {
        _pSelectedView->setHighlighted(true);
    }
    else {
        _pSelectedView->setHighlighted(false);
    }
}


void
ListView::selectedItem(int row)
{
    Log::instance()->gui().debug("list view selected item: " + Poco::NumberFormatter::format(row));

    if (_selectedRow >= 0 && itemIsVisible(_selectedRow)) {
        View* pLastSelectedView = visibleView(visibleIndex(_selectedRow));
        pLastSelectedView->setHighlighted(false);
    }

    View* pSelectedView = visibleView(visibleIndex(row));
    if (pSelectedView) {
        pSelectedView->setHighlighted(true);
        _pSelectedView = pSelectedView;
        _selectedRow = row;
        NOTIFY_CONTROLLER(ListController, selectedItem, row);
    }
}


void
ListView::resize(int width, int height)
{
    int rows = height / _itemViewHeight;
    Omm::Gui::Log::instance()->gui().debug("list view resize rows: " + Poco::NumberFormatter::format(rows));
    int rowDelta = rows - _lastVisibleRows;
    Log::instance()->gui().debug("list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
    resizeDelta(rowDelta, width);
}


void
ListView::insertItem(int row)
{
    Log::instance()->gui().debug("list view insert item: " + Poco::NumberFormatter::format(row));
    
    ListModel* pModel = static_cast<ListModel*>(_pModel);

    // resize view to the size with this item added
    updateScrollWidgetSize();
    // check if item is visible
    if (!itemIsVisible(row)) {
        Log::instance()->gui().debug("list view insert item that is not visible (ignoring)");
        return;
    }

    // attach item to a free (not visible) view
    if (_freeViews.size()) {
        View* pView = _freeViews.top();
        Log::instance()->gui().debug("list view got free view: " + pView->getName());
        _freeViews.pop();
        _visibleViews.insert(_visibleViews.begin() + visibleIndex(row), pView);
        pView->setModel(pModel->getItemModel(row));
        _itemControllers[pView]->setRow(row);

        // FIXME: move all views below one down
        // FIXME: detach last view if not visible anymore
        moveViewToRow(row, pView);

        // FIXME: allocation of List* makes pView->show() crash
        pView->show();
    }
    else {
        Log::instance()->gui().error("list view failed to attach view to item, view pool is empty (ignoring)");
    }
}


void
ListView::removeItem(int row)
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);

    updateScrollWidgetSize();
    if (!itemIsVisible(row)) {
        Log::instance()->gui().debug("list view remove item that is not visible (ignoring)");
        return;
    }

    // remove view from this position in visible views
    int index = visibleIndex(row);
    View* pView = _visibleViews[index];
    int lastRow = _rowOffset + _visibleViews.size();
    // move all views below one up
    for (int i = index + 1; i < countVisibleViews(); i++) {
        moveViewToRow(row + i - index - 1, visibleView(i));
    }
//    pModel->detachView(row);
    pView->hide();
    _visibleViews.erase(_visibleViews.begin() + visibleIndex(row));

    if (pModel->totalItemCount() - lastRow > 0) {
        // FIXME: something's going wrong with removal of rows, duplicate rows appear and crash
        // reuse and attach view below last view cause it is now becoming visible
        Log::instance()->gui().debug("list view reuse removed item view");
//        pModel->attachView(lastRow - 1, pView);
        pView->setModel(pModel->getItemModel(lastRow - 1));
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
