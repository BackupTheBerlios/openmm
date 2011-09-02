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

#include "Gui/Button.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ListImpl.h"
#endif


namespace Omm {
namespace Gui {


class LazyListItemController : public ButtonController
{
public:
    void setRow(int row) { _row = row; }

private:
    virtual void selectedRow(int row);
    virtual void pushed();

    int _row;
};


void
LazyListItemController::pushed()
{
    selectedRow(_row);
}


void
LazyListItemController::selectedRow(int row)
{
    Log::instance()->gui().debug("lazy list item controller selected row: " + Poco::NumberFormatter::format(row));
}


LazyListView::LazyListView(View* pParent) :
ListView(new ListViewImpl(this, pParent), pParent)
{

}


void
LazyListView::setModel(LazyListModel* pModel)
{
    Log::instance()->gui().debug("lazy list view set model ...");

    View::setModel(pModel);

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
}


void
LazyListView::resize(int rows)
{
    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);

    int rowDelta = rows - viewPoolSize();
    Log::instance()->gui().debug("lazy list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));
    if (rowDelta > 0) {
        if (_rowOffset + _visibleViews.size() + rowDelta >= pModel->lastFetched()) {
            pModel->fetch(_visibleViews.size() + rowDelta);
        }
        extendViewPool(rowDelta);
        for (int i = 0; i < rowDelta; i++) {
            View* pView = _freeViews.top();
            _freeViews.pop();
            int lastRow = _rowOffset + _visibleViews.size();
            moveViewToRow(lastRow, pView);
            pView->setModel(pModel->getItemModel(lastRow));
            pView->show();
            _visibleViews.push_back(pView);
        }
    }
}


} // namespace Gui
} // namespace Omm
