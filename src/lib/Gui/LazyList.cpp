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


namespace Omm {
namespace Gui {


LazyListView::LazyListView(View* pParent) :
ListView(pParent)
{

}


void
LazyListView::setModel(LazyListModel* pModel)
{
    Log::instance()->gui().debug("lazy list view set model ...");

    int rows = viewPortHeightInRows();
    int rowsFetched = pModel->fetch(std::min(pModel->totalItemCount(), rows));

    View::setModel(pModel);

    Log::instance()->gui().debug("lazy list view set model finished.");
}


void
LazyListView::syncViewImpl()
{
    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);

    int rows = viewPortHeightInRows();
    int rowsToFetch = std::min(pModel->totalItemCount() - _rowOffset, rows);

    if (_rowOffset + rowsToFetch > pModel->lastFetched(true)) {
        int rowsFetched = pModel->fetch(rowsToFetch, true);
    }
    else if (_rowOffset < pModel->lastFetched(false)) {
        int rowsFetched = pModel->fetch(rowsToFetch, false);
    }

    ListView::syncViewImpl();
}


void
LazyListView::scrollToRowOffset(int rowOffset)
{
    Log::instance()->gui().debug("lazy list view scroll to row offset: " + Poco::NumberFormatter::format(rowOffset) + ", visible views: " + Poco::NumberFormatter::format(_visibleViews.size()));

    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);
    int rowDelta = rowOffset - _rowOffset;
    int rowDeltaAbsolute = std::abs(rowDelta);
    if (rowDelta > 0 && rowOffset + _visibleViews.size() + rowDeltaAbsolute >= pModel->lastFetched(true)) {
        pModel->fetch(_visibleViews.size() + rowDeltaAbsolute, true);
    }
    else if (rowDelta < 0 && rowOffset < pModel->lastFetched(false)) {
        pModel->fetch(rowDeltaAbsolute, false);
    }
    ListView::scrollToRowOffset(rowOffset);
}


void
LazyListView::resize(int width, int height)
{
    int rowDelta = viewPortHeightInRows() - _viewPool.size();
    Log::instance()->gui().debug("lazy list view resize row delta: " + Poco::NumberFormatter::format(rowDelta));

    LazyListModel* pModel = static_cast<LazyListModel*>(_pModel);
    if (rowDelta > 0 && pModel) {
        if (lastVisibleRow() + rowDelta >= pModel->lastFetched()) {
            pModel->fetch(_visibleViews.size() + rowDelta);
        }
        ListView::resize(width, height);
    }
}


} // namespace Gui
} // namespace Omm
