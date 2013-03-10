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
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>

#include "Gui/List.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"
#include "Gui/Drag.h"
#include "SelectionView.h"
#include "Log.h"


namespace Omm {
namespace Gui {


class ListItemController : public Controller
{
    friend class ListView;

public:
    void setRow(int row) { _row = row; }

private:
    virtual void selected();
    virtual void activated();
//    virtual void dragStarted();

    ListView*     _pListView;
    int           _row;
};


void
ListItemController::selected()
{
    LOG(gui, debug, "list item controller selected row: " + Poco::NumberFormatter::format(_row));
    _pListView->selectedItem(_row, false);
}


void
ListItemController::activated()
{
    LOG(gui, debug, "list item controller activated row: " + Poco::NumberFormatter::format(_row));
    _pListView->activatedItem(_row, false);
}


//void
//ListItemController::dragStarted()
//{
//    if (_pListView->_dragMode & ListView::DragNone) {
//        return;
//    }
//    else {
//        View* pSource = _pListView->visibleView(_pListView->visibleIndex(_row));
//        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(pSource, pSource->getModel());
//        pDrag->start();
//    }
//}


class ListDragController : public Controller
{
public:
    ListDragController(ListView* pList, View* pItemView) : _pListView(pList), _pItemView(pItemView) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "list drag controller drag started");
        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(_pItemView, _pItemView->getModel());
        _pListView->dragView(_pItemView);
        _pListView->syncViewImpl();
        pDrag->start();
    }

    virtual void dragEntered(const Position& pos, Drag* pDrag, bool& accept)
    {
//        accept = (_pListView->rowFromView(pDrag->getSource()) >= 0);
//        accept = (typeid(*pDrag->getSource()) == typeid(*_pItemView));
        accept = (typeid(*pDrag->getSource()->getModel()) == typeid(*_pItemView->getModel()));
        if (accept) {
//            _pListView->shiftViews(_pItemView);
            _pListView->showDropLine(_pItemView);
        }
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag, bool& accept)
    {
    }

    virtual void dragLeft()
    {
    }

    virtual void dropped(const Position& pos, Drag* pDrag, bool& accept)
    {
//        accept = (_pListView->rowFromView(pDrag->getSource()) >= 0);
//        accept = (typeid(*pDrag->getSource()) == typeid(*_pItemView));
        accept = (typeid(*pDrag->getSource()->getModel()) == typeid(*_pItemView->getModel()));
        if (accept) {
            _pListView->hideDropLine();
            _pListView->dropView(pDrag->getModel(), _pItemView);
            _pListView->syncViewImpl();
        }
    }


    ListView*   _pListView;
    View*       _pItemView;
};


class ListScrollAreaController : public ScrollAreaController
{
    friend class ListView;

    ListScrollAreaController(ListView* pListView) : _pListView(pListView) {}

    virtual void scrolled(int xOffset, int yOffset);
    virtual void resized(int width, int height);
    virtual void shown();
    virtual void keyPressed(KeyCode key);

    ListView*   _pListView;
};


void
ListScrollAreaController::scrolled(int xOffset, int yOffset)
{
    LOG(gui, debug, "list scroll area scrolled xOffset: " + Poco::NumberFormatter::format(xOffset) + ", yOffset: " + Poco::NumberFormatter::format(yOffset));
    LOG(gui, debug, "list scroll area scrolled row offset: " + Poco::NumberFormatter::format(yOffset / _pListView->getItemViewHeight()));
    _pListView->scrollToRowOffset(yOffset / _pListView->getItemViewHeight());
}


void
ListScrollAreaController::resized(int width, int height)
{
    LOG(gui, debug, "list scroll area resized width: " + Poco::NumberFormatter::format(width) + ", height: " + Poco::NumberFormatter::format(height));
    _pListView->onResize(width, height);
}


void
ListScrollAreaController::shown()
{
    LOG(gui, debug, "list scroll area shown");
    resized(_pListView->width(), _pListView->height());
}


void
ListScrollAreaController::keyPressed(KeyCode key)
{
    LOG(gui, debug, "list scroll area key pressed");
    switch (key) {
        case Controller::KeyUp:
            _pListView->highlightItem(_pListView->_highlightedRow - 1, false);
            break;
        case Controller::KeyDown:
            _pListView->highlightItem(_pListView->_highlightedRow + 1, false);
            break;
        case Controller::KeyReturn:
//            _pListView->selectHighlightedItem();
            _pListView->activateHighlightedItem();
            break;
    }
}


ListView::ListView(View* pParent, SelectionType selectionType) :
ScrollAreaView(pParent),
#ifdef __IPHONE__
_itemViewHeight(50),
#else
_itemViewHeight(30),
#endif
_rowOffset(0),
_bottomRows(2),
//_pHighlightedView(0),
_pSelectionView(0),
_pSelectedModel(0),
_highlightedRow(-1),
_pTopView(0),
_selectionType(selectionType),
_pDropLine(0)
{
    attachController(new ListScrollAreaController(this));
//    setBackgroundColor(Color("white"));
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
    selectedItem(row, true);
}


void
ListView::activateRow(int row)
{
    activatedItem(row, true);
}


void
ListView::resetListView()
{
    _rowOffset = 0;
//    if (_pHighlightedView) {
//        _pHighlightedView->setHighlighted(false);
//        _pHighlightedView = 0;
//    }
    if (_pSelectionView) {
        _pSelectionView->hide(false);
//        delete _pSelectionView;
//        _pSelectionView = 0;
    }
    _pSelectedModel = 0;
    _highlightedRow = -1;
    scrollContentsTo(0, 0);
}


void
ListView::addTopView(View* pView)
{
    LOG(gui, debug, "list view add top view.");
    _pTopView = pView;
    addItemView(pView);
}


void
ListView::setDragMode(int dragMode)
{
    _dragMode = dragMode;
}


void
ListView::setSelectionType(SelectionType selectionType)
{
    _selectionType = selectionType;
}


void
ListView::syncViewImpl()
{
    LOG(gui, debug, "list view sync view impl of \"" + getName() + "\"");
    // whipe out everything
    clearVisibleViews();
    if (_pSelectionView) {
        _pSelectionView->hide();
    }

    // FIXME: updateScrollWidgetSize crashes omm when deleting local server:
//17:13:19.298 tristan[12570,0] D GUI list view sync view impl of ""
//17:13:19.299 tristan[12570,0] D GUI list scroll area scrolled xOffset: 0, yOffset: 169
//17:13:19.299 tristan[12570,0] D GUI list scroll area scrolled row offset: 5
//17:13:19.299 tristan[12570,0] D GUI list view scroll to row offset: 5
//17:13:19.299 tristan[12570,0] D GUI list scroll view to row offset: 3, delta: -1
//17:13:19.299 tristan[12570,0] D GUI list view scroll direction: -1, offset: 4
//17:13:19.299 tristan[12570,0] D GUI item count: 10, last visible row: 4
//
//Program received signal SIGSEGV, Segmentation fault.
//0x00007ffff5f69837 in Omm::Gui::ListView::scrollOneRow (this=0x7fc090, direction=-1)
//    at /home/jb/devel/omm/src/lib/Gui/List.cpp:387
//387	        pView->hide(false);
    updateScrollWidgetSize(totalItemCount());
    int visibleItemCount = std::min(totalItemCount(), _rowOffset + viewPortHeightInRows()) - _rowOffset;

    // if scrolled to top and top view is present, show it
    if (_pTopView) {
        if (_rowOffset == 0) {
            LOG(gui, debug, "list view sync top view is visible");
            _visibleViews.push_back(_pTopView);
            _pTopView->move(0, 0);
            _pTopView->show();
            showItemsAt(1, 0, visibleItemCount - 1);
        }
        else {
            showItemsAt(_rowOffset, _rowOffset - 1, visibleItemCount);
        }
    }
    else {
        showItemsAt(_rowOffset, _rowOffset, visibleItemCount);
    }
}


void
ListView::showItemsAt(int rowOffset, int itemOffset, int countItems)
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    for (int index = 0; index < countItems; index++) {
        LOG(gui, debug, "list view show item at index: " + Poco::NumberFormatter::format(index));
        View* pView = getFreeView();
        if (!pView) {
            return;
        }
        Model* pItemModel = pModel->getItemModel(itemOffset + index);
        if (!pItemModel) {
            LOG(gui, warning, "list view sync view impl could not get item model at index: " + Poco::NumberFormatter::format(itemOffset + index));
            putFreeView(pView);
            continue;
        }
        pView->setModel(pItemModel);
        _visibleViews.push_back(pView);
        _itemControllers[pView]->setRow(itemOffset + index);
        moveItemView(rowOffset + index, pView);
        pView->show();
        if (pItemModel == _pSelectedModel) {
            moveItemView(rowOffset + index, _pSelectionView);
            _pSelectionView->show();
            _pSelectionView->raise();
        }
    }
}


void
ListView::scrollOneRow(int direction)
{
    LOG(gui, debug, "list view scroll direction: " + Poco::NumberFormatter::format(direction) + ", offset: " + Poco::NumberFormatter::format(_rowOffset));

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    LOG(gui, debug, "item count: " + Poco::NumberFormatter::format(totalItemCount()) + ", last visible row: " + Poco::NumberFormatter::format(lastVisibleRow()));
    if (direction > 0) {
        if (totalItemCount() <= lastVisibleRow()) {
            return;
        }
        // detach model from first visible view
        View* pView = _visibleViews.front();
        if (pView == _pTopView) {
            pView = getFreeView();
        }
        else {
            pView->hide(false);
            pModel->getItemModel(_rowOffset - (_pTopView ? 1 : 0))->detachView(pView);
        }
        // move first view to the end
        moveItemView(lastVisibleRow(), pView);
        // attach model
        pView->setModel(pModel->getItemModel(lastVisibleRow() - (_pTopView ? 1 : 0)));
        pView->show(false);
        _itemControllers[pView]->setRow(lastVisibleRow() - (_pTopView ? 1 : 0));
        _visibleViews.erase(_visibleViews.begin());
        _visibleViews.push_back(pView);
        _rowOffset++;
    }
    else if (direction < 0) {
        if (_rowOffset <= 0) {
            return;
        }
        // detach model from last visible view
        View* pView = _visibleViews.back();
        pView->hide(false);
        pModel->getItemModel(lastVisibleRow() - 1)->detachView(pView);
        if (_pTopView && _rowOffset == 1) {
            pView = _pTopView;
            putFreeView(_visibleViews.back());
        }
        else {
            // move last view to the beginning
            moveItemView(_rowOffset - 1, pView);
            // attach model
            pView->setModel(pModel->getItemModel(_rowOffset - (_pTopView ? 1 : 0) - 1));
            pView->show(false);
            _itemControllers[pView]->setRow(_rowOffset - (_pTopView ? 1 : 0) - 1);
        }
        // move view to beginning of visible rows
        _visibleViews.erase(_visibleViews.end() - 1);
        _visibleViews.insert(_visibleViews.begin(), pView);
        _rowOffset--;
    }
}


void
ListView::scrollToRowOffset(int rowOffset)
{
    LOG(gui, debug, "list view scroll to row offset: " + Poco::NumberFormatter::format(rowOffset));

    if (rowOffset < 0) {
        return;
    }
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (modelItemCount() < viewPortHeightInRows()) {
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
    LOG(gui, debug, "list scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDelta));
    if (rowDeltaAbsolute > viewPortHeightInRows()) {
        // far jump
        _rowOffset = rowOffset;
        for (int i = 0; i < _visibleViews.size(); i++) {
            if (_visibleViews[i] != _pTopView) {
                _visibleViews[i]->hide();
                _visibleViews[i]->detachModel();
            }
        }
        syncViewImpl();
    }
    else {
        while (rowDeltaAbsolute--) {
            scrollOneRow(rowDelta);
        }
    }
//    if (_pHighlightedView) {
//        _pHighlightedView->setHighlighted(itemIsVisible(_highlightedRow));
//    }
}


void
ListView::onResize(int width, int height)
{
    setItemViewWidth(width);
    updateScrollWidgetSize(totalItemCount());

    LOG(gui, debug, "list view resize, view port height (rows): " + Poco::NumberFormatter::format(viewPortHeightInRows()) + \
            ", visible views: " + Poco::NumberFormatter::format(_visibleViews.size()) + \
            ", view pool size: " + Poco::NumberFormatter::format(_viewPool.size()));
    // finish if all item views fit into viewport
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (!pModel) {
        return;
    }

    if (totalItemCount() < viewPortHeightInRows() && _visibleViews.size() != 0) {
        return;
    }

    int rowDelta = std::min(viewPortHeightInRows(), totalItemCount() - _rowOffset) - _visibleViews.size();
    if (rowDelta >= 0) {
        for (int i = 0; i < rowDelta; i++) {
            LOG(gui, debug, "growing view");
            View* pView = getFreeView();
            if (!pView) {
                return;
            }
            moveItemView(lastVisibleRow(), pView);
            pView->setModel(pModel->getItemModel(lastVisibleRow() - (_pTopView ? 1 : 0)));
            _itemControllers[pView]->setRow(lastVisibleRow() - (_pTopView ? 1 : 0));
            pView->show();
            _visibleViews.push_back(pView);
        }
    } else {
        for (int i = 0; i < -rowDelta; i++) {
            LOG(gui, debug, "shrinking view");
            View* pView = _visibleViews.back();
            putFreeView(pView);
            pView->hide();
            _visibleViews.pop_back();
        }
    }
}


void
ListView::scale(float factor)
{
    setItemViewHeight(factor * getItemViewHeight());
    View::scale(factor);
}


void
ListView::extendViewPool()
{
    int n = viewPortHeightInRows() - _viewPool.size();
    if (n <= 0) {
        return;
    }
    LOG(gui, debug, "list view \"" + getName() + "\" extend view pool by number of views: " + Poco::NumberFormatter::format(n));

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (!pModel) {
        return;
    }

    for (int i = 0; i < n; ++i) {
//        LOG(gui, debug, "list view extend view create item view ... pModel: " + Poco::NumberFormatter::format(pModel));
        View* pView = pModel->createItemView();
        if (!pView) {
            LOG(gui, error, "list view failed to create view for pool (ignoring)");
            return;
        }
//        LOG(gui, debug, "list view extend view created item view " + pView->getName());
        pView->hide(false);
        _viewPool.push_back(pView);
        putFreeView(pView);
        addItemView(pView);

//        LOG(gui, debug, "list view creating list item controller ...");
        ListItemController* pItemController = new ListItemController;
        pItemController->_pListView = this;
        _itemControllers[pView] = pItemController;
        pView->attachController(pItemController);
        if (!(_dragMode & DragNone)) {
            pView->attachController(new ListDragController(this, pView));
        }
        if (_dragMode & DragTarget) {
            pView->setAcceptDrops(true);
        }

//        LOG(gui, debug, "allocate view[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pView));
    }
    if (!_pSelectionView) {
        _pSelectionView = new SelectionView(_selectionType);
        _pSelectionView->setParentView(getAreaView());
        _pSelectionView->hide(false);
    }
    else {
        _pSelectionView->raise(false);
    }
    if (!_pDropLine) {
        _pDropLine = new View(getAreaView());
        _pDropLine->setBackgroundColor(Color(100, 100, 100, 150));
        _pDropLine->hide(false);
    }
    else {
        _pDropLine->raise(false);
    }
}


View*
ListView::getFreeView()
{
    if (_freeViews.empty()) {
        LOG(gui, error, "list view, could not get free view");
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


void
ListView::clearVisibleViews()
{
    int countVisibleViews = _visibleViews.size();
    for (int i = 0; i < countVisibleViews; i++) {
        if (_visibleViews.back() != _pTopView) {
            putFreeView(_visibleViews.back());
        }
        _visibleViews.back()->hide();
        _visibleViews.pop_back();
    }
}


void
ListView::addItemView(View* pView)
{
//    LOG(gui, debug, "list view add item view.");

    pView->resize(getViewportWidth(), getItemViewHeight());
    pView->setSizeConstraint(getViewportWidth(), getItemViewHeight(), View::Pref);
    pView->setParent(getAreaView());
}


void
ListView::moveItemView(int row, View* pView)
{
//    LOG(gui, debug, "move list item view \"" + pView->getName() + "\" to row: " + Poco::NumberFormatter::format(row));
    pView->move(0, getItemViewHeight() * row);
}


int
ListView::getOffset()
{
    return getYOffset();
}


void
ListView::updateScrollWidgetSize(int rows)
{
    getAreaView()->resize(getViewportWidth(), rows * getItemViewHeight());
    extendViewPool();
}


int
ListView::visibleIndex(int row)
{
    return row - _rowOffset + (_pTopView ? 1 : 0);
}


int
ListView::modelItemCount()
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);
    if (!_pModel) {
        return 0;
    }
    return pModel->totalItemCount();
}


int
ListView::totalItemCount()
{
    return modelItemCount() + (_pTopView ? 1 : 0);
}


int
ListView::viewPortHeightInRows()
{
    return getViewportHeight() / getItemViewHeight() + _bottomRows;
}


View*
ListView::visibleView(int index)
{
    if (0 <= index && index < _visibleViews.size()) {
        return _visibleViews[index];
    }
    else {
        LOG(gui, error, "list view failed to retrieve visible view, out of range (ignoring)");
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


int
ListView::rowFromView(View* pView)
{
    std::vector<View*>::iterator pos = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (pos == _visibleViews.end()) {
        return -1;
    }
    else {
//        return pos - _visibleViews.begin() + _rowOffset - (_pTopView ? 1 : 0);
        return pos - _visibleViews.begin() + _rowOffset;
    }
}


void
ListView::setItemViewWidth(int width)
{
//    LOG(gui, debug, "list view set item view width: " + Poco::NumberFormatter::format(width));
    if (!_viewPool.size()) {
        return;
    }
    if (_pTopView) {
        _pTopView->setWidth(width);
    }
    if (_pSelectionView) {
        _pSelectionView->resize(width, getItemViewHeight());
    }
    if (_pDropLine) {
        _pDropLine->resize(width, 1);
    }
    for (std::vector<View*>::iterator it = _viewPool.begin(); it != _viewPool.end(); ++it) {
        (*it)->setWidth(width);
    }
}


void
ListView::selectedItem(int row, bool async)
{
    LOG(gui, debug, "list view selected item: " + Poco::NumberFormatter::format(row));

//    if (row < _rowOffset || row > lastVisibleRow()) {
//        return;
//    }

    highlightItem(row, async);
    NOTIFY_CONTROLLER(ListController, selectedItem, row);
}


void
ListView::activatedItem(int row, bool async)
{
    LOG(gui, debug, "list view activated item: " + Poco::NumberFormatter::format(row));

//    if (row < _rowOffset || row > lastVisibleRow()) {
//        return;
//    }

    highlightItem(row, async);
    NOTIFY_CONTROLLER(ListController, activatedItem, row);
}


void
ListView::highlightItem(int row, bool async)
{
    LOG(gui, debug, "list view highlight row: " + Poco::NumberFormatter::format(row) +\
            ", _rowOffset: " + Poco::NumberFormatter::format(_rowOffset) +\
            ", lastVisibleRow: " + Poco::NumberFormatter::format(lastVisibleRow()) +\
            ", last _highlightedRow: " + Poco::NumberFormatter::format(_highlightedRow) +\
            ", viewport height (rows): " + Poco::NumberFormatter::format(viewPortHeightInRows()) +\
            ", item view height: " + Poco::NumberFormatter::format(getItemViewHeight()));
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
//    if (row >= lastVisibleRow() - _bottomRows || (row >= lastVisibleRow() && row < pModel->totalItemCount())) {
    if (row >= lastVisibleRow() - _bottomRows || (row >= lastVisibleRow() - _bottomRows && row < pModel->totalItemCount())) {
        int y = (row - viewPortHeightInRows() + _bottomRows + 1) * getItemViewHeight() - (getViewportHeight() % getItemViewHeight());
        if (y > 0) {
            scrollContentsTo(0, y);
        }
    }


    if (_pSelectionView) {
        _pSelectionView->move(0, (_pTopView ? row + 1 : row) * getItemViewHeight());
        _pSelectionView->show(async);
        _pSelectionView->raise(async);
    }

    _pSelectedModel = pModel->getItemModel(row);
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
ListView::activateHighlightedItem()
{
    if (_highlightedRow >= 0) {
        NOTIFY_CONTROLLER(ListController, activatedItem, _highlightedRow);
    }
}


void
ListView::dragView(View* pView)
{
    LOG(gui, debug, "list view drag view: " + pView->getName());
    LOG(gui, debug, "list view drag view in row: " + Poco::NumberFormatter::format(rowFromView(pView)));
    NOTIFY_CONTROLLER(ListController, draggedItem, rowFromView(pView) - (_pTopView ? 1 : 0));
}


void
ListView::dropView(Model* pSourceModel, View* pTarget)
{
//    LOG(gui, debug, "list view drop view source model: " + pSourceModel->getName());
    LOG(gui, debug, "list view drop view target: " + pTarget->getName());
    int row = rowFromView(pTarget) - (_pTopView ? 1 : 0);
    LOG(gui, debug, "list view drop view in row: " + Poco::NumberFormatter::format(row));
    NOTIFY_CONTROLLER(ListController, droppedItem, pSourceModel, row);
}


//void
//ListView::shiftViews(View* pFirstView, int pixel)
//{
//    // shift pFirstView and all visible views below number of pixel
//    int index = rowFromView(pFirstView) - _rowOffset;
//    for (int i = 0; i < index; i++) {
//        _visibleViews[i]->move(0, (_rowOffset + i) * _itemViewHeight);
//    }
//    for (int i = index; i < _visibleViews.size(); i++) {
//        _visibleViews[i]->move(0, (_rowOffset + i) * _itemViewHeight + pixel);
//    }
//    if (index == 0) {
//        scrollOneRow(-1);
//        scrollContentsTo(0, (_rowOffset - 1) * _itemViewHeight);
//    }
//    else if (index >= _visibleViews.size() - _bottomRows - 1) {
//        scrollOneRow(1);
//        scrollContentsTo(0, (_rowOffset + 1) * _itemViewHeight);
//    }
//}


void
ListView::showDropLine(View* pView)
{
    if (_pDropLine) {
        _pDropLine->move(0, pView->posY() + _itemViewHeight);
        _pDropLine->show(false);
    }
}


void
ListView::hideDropLine()
{
    if (_pDropLine) {
        _pDropLine->hide(false);
    }
}


} // namespace Gui
} // namespace Omm
