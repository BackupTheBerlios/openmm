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
    _pListView->scrolledToRow(yOffset / _pListView->getItemViewHeight());
}


void
ListScrollAreaController::resized(int width, int height)
{
    Log::instance()->gui().debug("list scroll area resized width: " + Poco::NumberFormatter::format(width) + ", height: " + Poco::NumberFormatter::format(height));
    _pListView->resizeScrollArea(width, _pListView->getScrollAreaHeight());
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
            _pListView->highlightItem(_pListView->_selectedRow - 1);
            break;
        case Controller::KeyDown:
            _pListView->highlightItem(_pListView->_selectedRow + 1);
            break;
        case Controller::KeyReturn:
            _pListView->selectedItemNotify();
            break;
    }
}


ListView::ListView(View* pParent) :
ScrollAreaView(pParent),
_itemViewHeight(50),
_rowOffset(0),
_pSelectedView(0),
_selectedRow(-1)
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


int
ListView::visibleViews()
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
   Log::instance()->gui().debug("list view scroll delta: " + Poco::NumberFormatter::format(rowDelta));
    
   ListModel* pModel = static_cast<ListModel*>(_pModel);
   if (rowDelta > 0) {
        // detach model from first visible view
        View* pView = _visibleViews.front();
        pModel->getItemModel(_rowOffset)->detachView(pView);
        // move first view to the end
        int lastRow = _rowOffset + _visibleViews.size();
        moveItemView(lastRow, pView);
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
        moveItemView(_rowOffset - 1, pView);
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
    Log::instance()->gui().debug("list view scroll to row offset: " + Poco::NumberFormatter::format(rowOffset));

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
    Log::instance()->gui().debug("list scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDelta));
    while (rowDeltaAbsolute--) {
        scrollDelta(rowDelta);
    }

    handleSelectionHighlight();
}


void
ListView::resizeDelta(int rowDelta, int width)
{
    Log::instance()->gui().debug("list view resize visible views: " + Poco::NumberFormatter::format(_visibleViews.size())
        + ", view height in rows: " + Poco::NumberFormatter::format(visibleViews()));

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    extendViewPool(rowDelta);
    for (std::vector<View*>::iterator it = _viewPool.begin(); it != _viewPool.end(); ++it) {
        (*it)->resize(width, _itemViewHeight);
    }
    if (rowDelta <= 0) {
        return;
    }
    if (pModel->totalItemCount() < visibleViews()) {
        return;
    }
//    _lastVisibleRow += rowDelta;
    for (int i = 0; i < rowDelta; i++) {
        View* pView = _freeViews.top();
        _freeViews.pop();
        int lastRow = _rowOffset + _visibleViews.size();
        moveItemView(lastRow, pView);

        Model* pViewModel = pModel->getItemModel(lastRow);
        if (pViewModel) {
            pView->setModel(pViewModel);
        }
        else {
            Log::instance()->gui().warning("list view resize could not get item model in last row: " + Poco::NumberFormatter::format(lastRow));
            return;
        }
        
        _itemControllers[pView]->setRow(lastRow);
        pView->show();
        _visibleViews.push_back(pView);
    }
}


void
ListView::extendViewPool(int n)
{
    Log::instance()->gui().debug("list view \"" + getName() + "\" extend view pool by number of views: " + Poco::NumberFormatter::format(n));

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
    return _rowOffset <= row && row < _rowOffset + visibleViews();
}


void
ListView::handleSelectionHighlight()
{
    if (!_pSelectedView) {
        return;
    }
    _pSelectedView->setHighlighted(itemIsVisible(_selectedRow));
}


int
ListView::lastVisibleRow()
{
    return _rowOffset + _visibleViews.size();
}


void
ListView::setItemViewWidth(int width)
{
    Omm::Gui::Log::instance()->gui().debug("list view set item view width: " + Poco::NumberFormatter::format(width) + " ...");
    if (!_viewPool.size()) {
        return;
    }
    for (std::vector<View*>::iterator it = _viewPool.begin(); it != _viewPool.end(); ++it) {
        (*it)->setWidth(width);
    }
    _itemViewHeight = _viewPool[0]->height();
    int row = 0;
    for (std::vector<View*>::iterator it = _visibleViews.begin(); it != _visibleViews.end(); ++it, ++row) {
        (*it)->move(0, _itemViewHeight * row);
    }
    Omm::Gui::Log::instance()->gui().debug("list view set item view width finished.");
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
    if (row < _rowOffset) {
        scrolledToRow(row);
    }
    if (row > lastVisibleRow()) {
        scrolledToRow(row);
    }

    if (_selectedRow >= 0 && itemIsVisible(_selectedRow)) {
        View* pLastSelectedView = visibleView(visibleIndex(_selectedRow));
        pLastSelectedView->setHighlighted(false);
    }

    View* pSelectedView = visibleView(visibleIndex(row));
    if (pSelectedView) {
        pSelectedView->setHighlighted(true);
        _pSelectedView = pSelectedView;
    }
    _selectedRow = row;
}


void
ListView::selectedItemNotify()
{
    if (_selectedRow >= 0) {
        NOTIFY_CONTROLLER(ListController, selectedItem, _selectedRow);
    }
}


void
ListView::resize(int width, int height)
{
    setItemViewWidth(width);
    int rows = height / _itemViewHeight;
    Omm::Gui::Log::instance()->gui().debug("list view resize rows: " + Poco::NumberFormatter::format(rows));
    int rowDelta = rows - lastVisibleRow();
    Log::instance()->gui().debug("list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
    resizeDelta(rowDelta, width);
}


void
ListView::scale(float factor)
{
    _itemViewHeight *= factor;
    View::scale(factor);
}


void
ListView::syncViewImpl()
{
    Log::instance()->gui().debug("list view sync view impl of \"" + getName() + "\"");

    ListModel* pModel = static_cast<ListModel*>(_pModel);
    // resize view to the size with this item added
    updateScrollWidgetSize();

    int lastRow = std::min(pModel->totalItemCount(), _rowOffset + visibleViews()) - 1;
    for (int row = _rowOffset; row <= lastRow; row++) {
        if (_freeViews.size()) {
            View* pView = _freeViews.top();
            Log::instance()->gui().debug("list view got free view: " + pView->getName());
            _freeViews.pop();
            _visibleViews.push_back(pView);

            Model* pViewModel = pModel->getItemModel(row);
            if (pViewModel) {
                pView->setModel(pViewModel);
            }
            else {
                Log::instance()->gui().warning("list view insert item could not get item model in last row: " + Poco::NumberFormatter::format(row));
                return;
            }
            _itemControllers[pView]->setRow(row);
            moveItemView(row, pView);
            pView->show();
        }
        else {
            Log::instance()->gui().error("list view failed to attach view to item, view pool of \"" + getName() + "\" is empty (ignoring)");
        }        
    }
}


} // namespace Gui
} // namespace Omm
