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

#include "Gui/MovableList.h"
#include "Gui/GuiLogger.h"
#include "Gui/ListModel.h"
#include "Gui/View.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/MovableListImpl.h"
#endif


namespace Omm {
namespace Gui {


MovableListView::MovableMovableListView(View* pParent) :
View(new MovableListViewImpl(this, pParent), pParent),
_itemViewHeight(50),
_rowOffset(0)
{

}


MovableListView::MovableListView(ViewImpl* pViewImpl, View* pParent) :
View(pViewImpl, pParent),
_itemViewHeight(50),
_rowOffset(0)
{

}


void
MovableListView::setModel(MovableListModel* pModel)
{
    LOG(gui, debug, "MovableList view set model ...");

    View::setModel(pModel);

    // create an initial view pool. This also retrieves the height of the view.
    int rows = visibleRows();
    int rowsFetched = pModel->totalItemCount();

    extendViewPool(rows);

    // insert items that are already in the model.
    LOG(gui, debug, "inserting number of items: " + Poco::NumberFormatter::format(pModel->totalItemCount()));
//    for (int i = 0; i < std::min(_pModel->totalItemCount(), rows); i++) {
    for (int i = 0; i < rowsFetched; i++) {
        insertItem(i);
    }

    LOG(gui, debug, "MovableList view set model finished.");
}


int
MovableListView::visibleRows()
{
    return static_cast<MovableListViewImpl*>(_pImpl)->visibleRows();
}


void
MovableListView::addItemView(View* pView)
{
    LOG(gui, debug, "MovableList view init view.");
    static_cast<MovableListViewImpl*>(_pImpl)->addItemView(pView);
}


void
MovableListView::moveItemView(int row, View* pView)
{
    static_cast<MovableListViewImpl*>(_pImpl)->moveItemView(row, pView);
}


void
MovableListView::extendViewPool()
{
    static_cast<MovableListViewImpl*>(_pImpl)->extendViewPool();
}


int
MovableListView::viewPoolSize()
{
    return _viewPool.size();
}


void
MovableListView::extendViewPool(int n)
{
    LOG(gui, debug, "MovableList view extend view pool by number of views: " + Poco::NumberFormatter::format(n));

    MovableListModel* pModel = static_cast<MovableListModel*>(_pModel);

    for (int i = 0; i < n; ++i) {
        LOG(gui, debug, "MovableList view extend view create item view ... pModel: " + Poco::NumberFormatter::format(pModel));
        View* pView = pModel->createItemView();
        if (!pView) {
            LOG(gui, error, "MovableList view failed to create view for pool (ignoring)");
            return;
        }
        LOG(gui, debug, "MovableList view extend view created item view " + pView->getName());
        pView->hide();
        _viewPool.push_back(pView);
        _freeViews.push(pView);
        addItemView(pView);
        LOG(gui, debug, "allocate view[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pView));
    }
}


int
MovableListView::visibleIndex(int row)
{
    return row - _rowOffset;
}


int
MovableListView::countVisibleViews()
{
    return _visibleViews.size();
}


View*
MovableListView::visibleView(int index)
{
    if (0 <= index && index < _visibleViews.size()) {
        return _visibleViews[index];
    }
    else {
        LOG(gui, error, "MovableList view failed to retrieve visible view, out of range (ignoring)");
        return 0;
    }
}


bool
MovableListView::itemIsVisible(int row)
{
    return _rowOffset <= row && row < _rowOffset + visibleRows();
}


void
MovableListView::moveViewToRow(int row, View* pView)
{
//    pView->setRow(row);
    moveItemView(row, pView);
}


void
MovableListView::insertItem(int row)
{
    LOG(gui, debug, "MovableList view insert item: " + Poco::NumberFormatter::format(row));
    
    MovableListModel* pModel = static_cast<MovableListModel*>(_pModel);

    // if view is not lazy, view pool has to be extended when too small and new views are inserted.
    if (_visibleViews.size() >= _viewPool.size()) {
        extendViewPool();
    }

    // attach item to a free (not visible) view
    if (_freeViews.size()) {
        View* pView = _freeViews.top();
        LOG(gui, debug, "MovableList view got free view: " + pView->getName());
        _freeViews.pop();
        _visibleViews.insert(_visibleViews.begin() + visibleIndex(row), pView);
        pView->setModel(pModel->getItemModel(row));
        LOG(gui, debug, "MovableList view creating MovableList item controller ...");
//        MovableListItemController itemController;
        MovableListItemController* pItemController = new MovableListItemController;
//        MovableListItemModel* pItemModel = new MovableListItemModel;
//        MovableListController* pLC = new MovableListController;

//        int* pInt = new int;
//        Log::instance()->gui().debug("MovableList view creating MovableList item controller finished, pItemController: " + Poco::NumberFormatter::format(pItemController)
//        + ", size: " + Poco::NumberFormatter::format(sizeof(*pItemController)));
//        Controller* pItemController = new Controller;
//        pItemController->setRow(row);
//        pView->attachController(pItemController);
        // FIXME: move all views below one down
        // FIXME: detach last view if not visible anymore
        moveViewToRow(row, pView);

        // FIXME: allocation of MovableList* makes pView->show() crash
        pView->show();
    }
    else {
        LOG(gui, error, "MovableList view failed to attach view to item, view pool is empty (ignoring)");
    }
}


void
MovableListView::removeItem(int row)
{
    MovableListModel* pModel = static_cast<MovableListModel*>(_pModel);

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
        LOG(gui, debug, "MovableList view reuse removed item view");
//        pModel->attachView(lastRow - 1, pView);
        pView->setModel(pModel->getItemModel(lastRow - 1));
        _visibleViews.push_back(pView);
        moveViewToRow(lastRow - 1, pView);
    }
    else {
        LOG(gui, debug, "MovableList view free removed item view");
        // otherwise free view
        _freeViews.push(pView);
    }
}



} // namespace Gui
} // namespace Omm
