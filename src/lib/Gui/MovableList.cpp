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
#include "Gui/ListModel.h"
#include "Gui/View.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ListImpl.h"
#endif


namespace Omm {
namespace Gui {


ListView::ListView(View* pParent) :
View(new ListViewImpl(this, pParent), pParent),
_viewHeight(50),
_rowOffset(0)
{

}


ListView::ListView(ViewImpl* pViewImpl, View* pParent) :
View(pViewImpl, pParent),
_viewHeight(50),
_rowOffset(0)
{

}


void
ListView::setModel(ListModel* pModel)
{
    Log::instance()->gui().debug("list view set model ...");

    View::setModel(pModel);

    // create an initial view pool. This also retrieves the height of the view.
    int rows = visibleRows();
    int rowsFetched = pModel->totalItemCount();

    extendViewPool(rows);

    // insert items that are already in the model.
    Log::instance()->gui().debug("inserting number of items: " + Poco::NumberFormatter::format(pModel->totalItemCount()));
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
ListView::addItemView(View* pView)
{
    Log::instance()->gui().debug("list view init view.");
    static_cast<ListViewImpl*>(_pImpl)->addItemView(pView);
}


void
ListView::moveItemView(int row, View* pView)
{
    static_cast<ListViewImpl*>(_pImpl)->moveItemView(row, pView);
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

    ListModel* pModel = static_cast<ListModel*>(_pModel);

    for (int i = 0; i < n; ++i) {
        Log::instance()->gui().debug("list view extend view create item view ... pModel: " + Poco::NumberFormatter::format(pModel));
        View* pView = pModel->createItemView();
        if (!pView) {
            Log::instance()->gui().error("list view failed to create view for pool (ignoring)");
            return;
        }
        Log::instance()->gui().debug("list view extend view created item view " + pView->getName());
        pView->hide();
        _viewPool.push_back(pView);
        _freeViews.push(pView);
        addItemView(pView);
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
//    pView->setRow(row);
    moveItemView(row, pView);
}


void
ListView::insertItem(int row)
{
    Log::instance()->gui().debug("list view insert item: " + Poco::NumberFormatter::format(row));
    
    ListModel* pModel = static_cast<ListModel*>(_pModel);

    // if view is not lazy, view pool has to be extended when too small and new views are inserted.
    if (_visibleViews.size() >= _viewPool.size()) {
        extendViewPool();
    }

    // attach item to a free (not visible) view
    if (_freeViews.size()) {
        View* pView = _freeViews.top();
        Log::instance()->gui().debug("list view got free view: " + pView->getName());
        _freeViews.pop();
        _visibleViews.insert(_visibleViews.begin() + visibleIndex(row), pView);
        pView->setModel(pModel->getItemModel(row));
        Log::instance()->gui().debug("list view creating list item controller ...");
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
        Log::instance()->gui().error("list view failed to attach view to item, view pool is empty (ignoring)");
    }
}


void
ListView::removeItem(int row)
{
    ListModel* pModel = static_cast<ListModel*>(_pModel);

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