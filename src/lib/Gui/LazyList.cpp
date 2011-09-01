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

#include "Gui/LazyList.h"
#include "Gui/GuiLogger.h"
#include "Gui/ListModel.h"
#include "Gui/View.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/LazyListImpl.h"
#endif


namespace Omm {
namespace Gui {


LazyListView::LazyListView(View* pParent) :
ListView(new LazyListViewImpl(this, pParent), pParent)
{

}


void
LazyListView::setModel(LazyListModel* pModel)
{
    Log::instance()->gui().debug("lazy list view set model ...");

    View::setModel(pModel);

    // create an initial view pool. This also retrieves the height of the view.
    int rows = visibleRows();
    int rowsFetched = pModel->fetch(std::min(pModel->totalItemCount(), rows));

    extendViewPool(rows);

    // insert items that are already in the model.
    Log::instance()->gui().debug("inserting number of items: " + Poco::NumberFormatter::format(rowsFetched));
    for (int i = 0; i < rowsFetched; i++) {
        insertItem(i);
    }

    Log::instance()->gui().debug("lazy list view set model finished.");
}


void
LazyListView::insertItem(int row)
{
    Log::instance()->gui().debug("lazy list view insert item: " + Poco::NumberFormatter::format(row));

    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);

    // resize view to the size with this item added
    updateScrollWidgetSize();
    // check if item is visible
    if (!itemIsVisible(row)) {
        Log::instance()->gui().debug("lazy list view insert item that is not visible (ignoring)");
        return;
    }

    // attach item to a free (not visible) view
    if (_freeViews.size()) {
        View* pView = _freeViews.top();
        Log::instance()->gui().debug("lazy list view got free view: " + pView->getName());
        _freeViews.pop();
        _visibleViews.insert(_visibleViews.begin() + visibleIndex(row), pView);
        pView->setModel(pModel->getItemModel(row));
        Log::instance()->gui().debug("lazy list view creating list item controller ...");
//        ListItemController itemController;
        ListItemController* pItemController = new ListItemController;
//        ListItemModel* pItemModel = new ListItemModel;
//        ListController* pLC = new ListController;

//        int* pInt = new int;
//        Log::instance()->gui().debug("list view creating list item controller finished, pItemController: " + Poco::NumberFormatter::format(pItemController)
//        + ", size: " + Poco::NumberFormatter::format(sizeof(*pItemController)));
//        Controller* pItemController = new Controller;
//        pItemController->setRow(row);
//        pView->attachController(pItemController);
        // FIXME: move all views below one down
        // FIXME: detach last view if not visible anymore
        moveViewToRow(row, pView);

        // FIXME: allocation of List* makes pView->show() crash
        pView->show();
    }
    else {
        Log::instance()->gui().error("lazy list view failed to attach view to item, view pool is empty (ignoring)");
    }
}


void
LazyListView::removeItem(int row)
{
    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);

    // resize view to the size with this item added
    updateScrollWidgetSize();
    // check if item is visible
    if (!itemIsVisible(row)) {
        Log::instance()->gui().debug("lazy list view remove item that is not visible (ignoring)");
        return;
    }

    // detach item from visible view
//    View* pView = pModel->getChildView(row);

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
        Log::instance()->gui().debug("lazy list view reuse removed item view");
//        pModel->attachView(lastRow - 1, pView);
        pView->setModel(pModel->getItemModel(lastRow - 1));
        _visibleViews.push_back(pView);
        moveViewToRow(lastRow - 1, pView);
    }
    else {
        Log::instance()->gui().debug("lazy list view free removed item view");
        // otherwise free view
        _freeViews.push(pView);
    }
}


int
LazyListView::visibleRows()
{
    return static_cast<LazyListViewImpl*>(_pImpl)->visibleRows();
}


void
LazyListView::addItemView(View* pView)
{
    Log::instance()->gui().debug("lazy list view add item view.");
    static_cast<LazyListViewImpl*>(_pImpl)->addItemView(pView);
}


void
LazyListView::moveItemView(int row, View* pView)
{
    Log::instance()->gui().debug("lazy list view move item view.");
    static_cast<LazyListViewImpl*>(_pImpl)->moveItemView(row, pView);
}


int
LazyListView::getOffset()
{
    static_cast<LazyListViewImpl*>(_pImpl)->getOffset();
}


void
LazyListView::updateScrollWidgetSize()
{
    static_cast<LazyListViewImpl*>(_pImpl)->updateScrollWidgetSize();
}


void
LazyListView::scrolledToRow(int rowOffset)
{
    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);
    
    int rowDelta = rowOffset - _rowOffset;

    if (rowDelta == 0) {
        return;
    }
    if (rowOffset + _visibleViews.size() > pModel->totalItemCount()) {
        return;
    }

    int rowDeltaAbsolute = std::abs(rowDelta);

    if (rowOffset + _visibleViews.size() + rowDeltaAbsolute >= pModel->lastFetched()) {
        pModel->fetch(_visibleViews.size() + rowDeltaAbsolute);
    }

    Log::instance()->gui().debug("scroll view to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", delta: " + Poco::NumberFormatter::format(rowDeltaAbsolute));
    while (rowDeltaAbsolute--) {
        if (rowDelta > 0) {
            // detach model from first visible view
            View* pView = _visibleViews.front();
            pModel->getItemModel(_rowOffset)->detachView(pView);
            // move first view to the end
            int lastRow = _rowOffset + _visibleViews.size();
            moveViewToRow(lastRow, pView);
            // attach model
            pView->setModel(pModel->getItemModel(lastRow));
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
            // move view to beginning of visible rows
            _visibleViews.erase(_visibleViews.end() - 1);
            _visibleViews.insert(_visibleViews.begin(), pView);
            _rowOffset--;
        }
    }
}




} // namespace Gui
} // namespace Omm
