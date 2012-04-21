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
    virtual void keyPressed(KeyCode key);

    ListView*   _pListView;
};


void
ListScrollAreaController::scrolled(int xOffset, int yOffset)
{
    Log::instance()->gui().debug("list scroll area scrolled xOffset: " + Poco::NumberFormatter::format(xOffset) + ", yOffset: " + Poco::NumberFormatter::format(yOffset));
    Log::instance()->gui().debug("list scroll area scrolled row offset: " + Poco::NumberFormatter::format(yOffset / _pListView->getItemViewHeight()));
    _pListView->scrollToRowOffset(yOffset / _pListView->getItemViewHeight());
}


void
ListScrollAreaController::resized(int width, int height)
{
    Log::instance()->gui().debug("list scroll area resized width: " + Poco::NumberFormatter::format(width) + ", height: " + Poco::NumberFormatter::format(height));
    _pListView->resize(width, height);
}


void
ListScrollAreaController::presented()
{
    Log::instance()->gui().debug("list scroll area presented");
    resized(_pListView->width(), _pListView->height());
}


void
ListScrollAreaController::keyPressed(KeyCode key)
{
    Log::instance()->gui().debug("list scroll area key pressed");
    switch (key) {
        case Controller::KeyUp:
            _pListView->highlightItem(_pListView->_highlightedRow - 1);
            break;
        case Controller::KeyDown:
            _pListView->highlightItem(_pListView->_highlightedRow + 1);
            break;
        case Controller::KeyReturn:
            _pListView->selectHighlightedItem();
            break;
    }
}


ListView::ListView(View* pParent) :
ScrollAreaView(pParent),
_itemViewHeight(50),
_rowOffset(0),
_bottomRows(2),
_pHighlightedView(0),
_highlightedRow(-1)
{
    attachController(new ListScrollAreaController(this));
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


void
ListView::selectRow(int row)
{
    selectedItem(row);
}


void
ListView::resetListView()
{
    _rowOffset = 0;
    if (_pHighlightedView) {
        _pHighlightedView->setHighlighted(false);
        _pHighlightedView = 0;
    }
    _highlightedRow = -1;
    scrollContentsTo(0, 0);
}


int
ListView::viewPortHeightInRows()
{
    return getViewportHeight() / getItemViewHeight() + _bottomRows;
}


void
ListView::addItemView(View* pView)
{
//    Log::instance()->gui().debug("list view add item view.");

    pView->resize(getViewportWidth(), getItemViewHeight());
    addSubview(pView);
}


void
ListView::moveItemView(int row, View* pView)
{
    pView->move(0, getItemViewHeight() * row);
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
    if (!_pModel) {
        return;
    }
    resizeScrollArea(getViewportWidth(), pModel->totalItemCount() * getItemViewHeight());
}


void
ListView::scrollOneRow(int direction)
{
    Log::instance()->gui().debug("list view scroll direction: " + Poco::NumberFormatter::format(direction) + ", offset: " + Poco::NumberFormatter::format(_rowOffset));

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    Log::instance()->gui().debug("total item count: " + Poco::NumberFormatter::format(pModel->totalItemCount()) + ", last visible row: " + Poco::NumberFormatter::format(lastVisibleRow()));
    if (direction > 0) {
        if (pModel->totalItemCount() <= lastVisibleRow()) {
            return;
        }
        // detach model from first visible view
        View* pView = _visibleViews.front();
        pView->hide(false);
        pModel->getItemModel(_rowOffset)->detachView(pView);
        // move first view to the end
        moveItemView(lastVisibleRow(), pView);
        // attach model
        pView->setModel(pModel->getItemModel(lastVisibleRow()));
        pView->show(false);
        _itemControllers[pView]->setRow(lastVisibleRow());
        // move view to end of visible rows
        _visibleViews.erase(_visibleViews.begin());
        _visibleViews.push_back(pView);
        _rowOffset++;
    }
    else if (direction < 0) {
        // detach model from last visible view
        View* pView = _visibleViews.back();
        pView->hide(false);
        pModel->getItemModel(lastVisibleRow() - 1)->detachView(pView);
        // move last view to the beginning
        moveItemView(_rowOffset - 1, pView);
        // attach model
        pView->setModel(pModel->getItemModel(_rowOffset - 1));
        pView->show(false);
        _itemControllers[pView]->setRow(_rowOffset - 1);
        // move view to beginning of visible rows
        _visibleViews.erase(_visibleViews.end() - 1);
        _visibleViews.insert(_visibleViews.begin(), pView);
        _rowOffset--;
    }
}


void
ListView::scrollToRowOffset(int rowOffset)
{
    Log::instance()->gui().debug("list view scroll to row offset: " + Poco::NumberFormatter::format(rowOffset));

    if (rowOffset < 0) {
        return;
    }
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (pModel->totalItemCount() < viewPortHeightInRows()) {
        return;
    }
    if (rowOffset > pModel->totalItemCount() - viewPortHeightInRows()) {
        rowOffset = pModel->totalItemCount() - viewPortHeightInRows();
    }
    int rowDelta = rowOffset - _rowOffset;
    if (rowDelta == 0) {
        return;
    }
    int rowDeltaAbsolute = std::abs(rowDelta);
    Log::instance()->gui().debug("list scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDelta));
    if (rowDeltaAbsolute > viewPortHeightInRows()) {
        // far jump
        _rowOffset = rowOffset;
        for (int i = 0; i < countVisibleViews(); i++) {
            _visibleViews[i]->hide();
            _visibleViews[i]->detachModel();
        }
        syncViewImpl();
    }
    else {
        while (rowDeltaAbsolute--) {
            scrollOneRow(rowDelta);
        }
    }
    if (_pHighlightedView) {
        _pHighlightedView->setHighlighted(itemIsVisible(_highlightedRow));
    }
}


void
ListView::resize(int width, int height)
{
    setItemViewWidth(width);
    updateScrollWidgetSize();
    extendViewPool();

    Log::instance()->gui().debug(
          "list view resize, view port height (rows): " + Poco::NumberFormatter::format(viewPortHeightInRows())
        + ", visible views: " + Poco::NumberFormatter::format(_visibleViews.size())
        + ", view pool size: " + Poco::NumberFormatter::format(_viewPool.size())
    );
    // finish if all item views fit into viewport
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (!pModel) {
        return;
    }

    if (pModel->totalItemCount() < viewPortHeightInRows() && _visibleViews.size() != 0) {
        return;
    }

    int rowDelta = std::min(viewPortHeightInRows(), pModel->totalItemCount() - _rowOffset) - _visibleViews.size();
    if (rowDelta >= 0) {
        for (int i = 0; i < rowDelta; i++) {
            Log::instance()->gui().debug("growing view");
            View* pView = getFreeView();
            if (!pView) {
                return;
            }
            moveItemView(lastVisibleRow(), pView);
            pView->setModel(pModel->getItemModel(lastVisibleRow()));
            _itemControllers[pView]->setRow(lastVisibleRow());
            pView->show();
            _visibleViews.push_back(pView);
        }
    } else {
        for (int i = 0; i < -rowDelta; i++) {
            Log::instance()->gui().debug("shrinking view");
            View* pView = _visibleViews.back();
            putFreeView(pView);
            pView->hide();
            _visibleViews.pop_back();
        }
    }
}


void
ListView::extendViewPool()
{
    int n = viewPortHeightInRows() - _viewPool.size();
    if (n <= 0) {
        return;
    }
    Log::instance()->gui().debug("list view \"" + getName() + "\" extend view pool by number of views: " + Poco::NumberFormatter::format(n));

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (!pModel) {
        return;
    }

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
        putFreeView(pView);
        addItemView(pView);

//        Log::instance()->gui().debug("list view creating list item controller ...");
        ListItemController* pItemController = new ListItemController;
        pItemController->_pListView = this;
        _itemControllers[pView] = pItemController;
        pView->attachController(pItemController);

//        Log::instance()->gui().debug("allocate view[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pView));
    }
}


View*
ListView::getFreeView()
{
    if (_freeViews.empty()) {
        Log::instance()->gui().error("list view, could not get free view");
        return 0;
    }
    View* pView = _freeViews.top();
    _freeViews.pop();
    return pView;
}


void
ListView::putFreeView(View* pView)
{
    _freeViews.push(pView);
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
    return _rowOffset <= row && row < _rowOffset + viewPortHeightInRows();
}


int
ListView::lastVisibleRow()
{
    return _rowOffset + _visibleViews.size();
}


void
ListView::clearVisibleViews()
{
    int visibleViewCount = _visibleViews.size();
    for (int i = 0; i < visibleViewCount; i++) {
        _visibleViews.back()->hide();
        putFreeView(_visibleViews.back());
        _visibleViews.pop_back();
    }
}


void
ListView::setItemViewWidth(int width)
{
    Omm::Gui::Log::instance()->gui().debug("list view set item view width: " + Poco::NumberFormatter::format(width));
    if (!_viewPool.size()) {
        return;
    }
    for (std::vector<View*>::iterator it = _viewPool.begin(); it != _viewPool.end(); ++it) {
        (*it)->setWidth(width);
    }
}


void
ListView::selectedItem(int row)
{
    Log::instance()->gui().debug("list view selected item: " + Poco::NumberFormatter::format(row));

//    if (row < _rowOffset || row > lastVisibleRow()) {
//        return;
//    }

    highlightItem(row);
    NOTIFY_CONTROLLER(ListController, selectedItem, row);
}


void
ListView::highlightItem(int row)
{
//    Log::instance()->gui().debug("list view highlight row: " + Poco::NumberFormatter::format(row)
//                                + ", _rowOffset: " + Poco::NumberFormatter::format(_rowOffset)
//                                + ", lastVisibleRow: " + Poco::NumberFormatter::format(lastVisibleRow())
//                                + ", last _highlightedRow: " + Poco::NumberFormatter::format(_highlightedRow)
//                                + ", viewport height (rows): " + Poco::NumberFormatter::format(viewPortHeightInRows())
//                                + ", item view height: " + Poco::NumberFormatter::format(getItemViewHeight())
//    );
    if (row < 0) {
        return;
    }
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (row >= pModel->totalItemCount()) {
        return;
    }

    if (row < _rowOffset) {
        scrollContentsTo(0, row * getItemViewHeight());
    }
    if (row >= lastVisibleRow() - _bottomRows || (row >= lastVisibleRow() && row < pModel->totalItemCount())) {
        scrollContentsTo(0, (row - viewPortHeightInRows() + _bottomRows + 1) * getItemViewHeight() - (getViewportHeight() % getItemViewHeight()));
    }

    if (_highlightedRow >= 0 && itemIsVisible(_highlightedRow)) {
        View* pLastHighlightedView = visibleView(visibleIndex(_highlightedRow));
        if (pLastHighlightedView) {
            pLastHighlightedView->setHighlighted(false);
        }
    }

    View* pHighlightedView = visibleView(visibleIndex(row));
    if (pHighlightedView) {
        pHighlightedView->setHighlighted(true);
        _pHighlightedView = pHighlightedView;
    }
    _highlightedRow = row;
}


void
ListView::selectHighlightedItem()
{
    if (_highlightedRow >= 0) {
        NOTIFY_CONTROLLER(ListController, selectedItem, _highlightedRow);
    }
}


void
ListView::scale(float factor)
{
    setItemViewHeight(factor * getItemViewHeight());
    View::scale(factor);
}


void
ListView::syncViewImpl()
{
    Log::instance()->gui().debug("list view sync view impl of \"" + getName() + "\"");

    ListModel* pModel = static_cast<ListModel*>(_pModel);

    clearVisibleViews();
    int lastRow = std::min(pModel->totalItemCount(), _rowOffset + viewPortHeightInRows()) - 1;
    for (int row = _rowOffset; row <= lastRow; row++) {
        View* pView = getFreeView();
        if (!pView) {
            return;
        }
        _visibleViews.push_back(pView);
        Model* pItemModel = pModel->getItemModel(row);
        if (!pItemModel) {
            Log::instance()->gui().warning("list view sync view impl could not get item in row: " + Poco::NumberFormatter::format(row));
            putFreeView(pView);
            continue;
        }
        pView->setModel(pItemModel);
        _itemControllers[pView]->setRow(row);
        moveItemView(row, pView);
        pView->show();
    }

    // resize view to the size with this item added
    updateScrollWidgetSize();

//    int lastRow = std::min(pModel->totalItemCount(), _rowOffset + viewPortHeightInRows()) - 1;
//    for (int row = _rowOffset; row <= lastRow; row++) {
//        View* pView;
//        if (row - _rowOffset >= _visibleViews.size() && _freeViews.size()) {
//            pView = getFreeView();
//            _visibleViews.push_back(pView);
//        }
//        else if (_visibleViews.size() > row - _rowOffset) {
//            pView = _visibleViews[row - _rowOffset];
//        }
//        else {
//            return;
//        }
//        pView->setModel(pModel->getItemModel(row));
//        _itemControllers[pView]->setRow(row);
//        moveItemView(row, pView);
//        pView->show();
//    }
    // resize view to the size with this item added
//    updateScrollWidgetSize();
}


} // namespace Gui
} // namespace Omm
