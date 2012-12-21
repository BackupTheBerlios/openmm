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
#include <map>
#include <vector>

#include "Log.h"
#include "Gui/Cluster.h"
#include "Gui/GuiLogger.h"
#include "Gui/Layout.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/Label.h"
#include "Gui/Button.h"
#include "Gui/ListItem.h"
#include "Gui/Drag.h"
#include "Gui/Splitter.h"
#include "ColumnClusterImpl.h"
#include "Qt/SplitterImpl.h"


namespace Omm {
namespace Gui {


class ColumnView : public SplitterView
{
public:
    ColumnView(ColumnClusterViewImpl* pColumnClusterViewImpl);

    int getSize();
    int getRow(ClusterView* pCluster);
    ClusterView* getTopCluster();
    ClusterView* createCluster();
    ClusterView* createCluster(int row);

private:
    static int                  _clusterNumber;
    ColumnClusterViewImpl*      _pColumnClusterViewImpl;
    std::vector<ClusterView*>   _col;
};


class ColumnClusterController : public Controller
{
public:
    ColumnClusterController(ClusterView* pClusterView, ColumnView* pColumnView, ColumnClusterViewImpl* pColumnClusterViewImpl) : _pClusterView(pClusterView), _pColumnView(pColumnView), _pColumnClusterViewImpl(pColumnClusterViewImpl) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "column cluster drag controller drag started in source view: " + _pClusterView->getName());
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "column cluster drag controller drag moved in target view: " + _pClusterView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "column cluster drag controller dropped in target view: " + _pClusterView->getName() + ", source view: " + pDrag->getSource()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
        bool horizontal = pos.y() < _pClusterView->getHandleHeight() + _pClusterView->height() / 2;

        ClusterView* pNewCluster = 0;
        int row = _pColumnView->getRow(_pClusterView);
        int col = _pColumnClusterViewImpl->getColumn(_pColumnView);
        if (horizontal) {
            pNewCluster = _pColumnClusterViewImpl->createClusterInNewColumn(col + 1);
        }
        else {
            pNewCluster = _pColumnClusterViewImpl->createClusterInRow(col, row + 1);
        }
        if (pNewCluster) {
            pNewCluster->insertView(pDrag->getSource(), pDrag->getSource()->getName(), 0);
        }
    }

    ClusterView*            _pClusterView;
    ColumnView*             _pColumnView;
    ColumnClusterViewImpl*  _pColumnClusterViewImpl;
};


int ColumnView::_clusterNumber = 0;

ColumnView::ColumnView(ColumnClusterViewImpl* pColumnClusterViewImpl) :
SplitterView(0, View::Vertical),
_pColumnClusterViewImpl(pColumnClusterViewImpl)
{
    ClusterView* pCluster = createCluster();
    SplitterView::insertView(pCluster, 0);
    _col.push_back(pCluster);
}


int
ColumnView::getSize()
{
    return _col.size();
}


int
ColumnView::getRow(ClusterView* pCluster)
{
    std::vector<ClusterView*>::iterator it = std::find(_col.begin(), _col.end(), pCluster);
    return it - _col.begin();
}


ClusterView*
ColumnView::getTopCluster()
{
    return *_col.begin();
}


ClusterView*
ColumnView::createCluster()
{
    ClusterView* pCluster = new ClusterView(0, ClusterView::Generic);
    pCluster->setAcceptDrops(true);
    pCluster->attachController(new ColumnClusterController(pCluster, this, _pColumnClusterViewImpl));
    pCluster->setName("cluster " + Poco::NumberFormatter::format(_clusterNumber));
    _clusterNumber++;
    return pCluster;
}


ClusterView*
ColumnView::createCluster(int row)
{
    if (row < 0 || row > _col.size()) {
        return 0;
    }
    ClusterView* pCluster = createCluster();
    SplitterView::insertView(pCluster, row);
    _col.insert(_col.begin() + row, pCluster);
    return pCluster;
}


ColumnClusterViewImpl::ColumnClusterViewImpl(View* pView, bool createInitialCluster) :
//PlainViewImpl(pView),
SplitterViewImpl(pView, View::Horizontal)
{
}


void
ColumnClusterViewImpl::init()
{
    _pView->setName("column cluster");
    ColumnView* pCol = new ColumnView(this);
    SplitterViewImpl::insertView(pCol, 0);
    _grid.push_back(pCol);
}


void
ColumnClusterViewImpl::insertView(View* pView, const std::string& name, int index)
{
    LOG(gui, debug, "column cluster insert view: " + name);

    getFirstCluster()->insertView(pView, name, index);
}


void
ColumnClusterViewImpl::removeView(View* pView)
{

}


int
ColumnClusterViewImpl::getViewCount()
{
}


int
ColumnClusterViewImpl::getCurrentViewIndex()
{
//    LOG(gui, debug, "cluster get current tab: " + Poco::NumberFormatter::format(_currentView));

}


void
ColumnClusterViewImpl::setCurrentViewIndex(int index)
{
    LOG(gui, debug, "cluster set current handle: " + Poco::NumberFormatter::format(index));

    if (index == -1) {
        return;
    }
}


int
ColumnClusterViewImpl::getIndexFromView(View* pView)
{
}


void
ColumnClusterViewImpl::setHandlesHidden(bool hidden)
{
}


const int
ColumnClusterViewImpl::getHandleHeight()
{
    return getFirstCluster()->getHandleHeight();
}


int
ColumnClusterViewImpl::getColumn(ColumnView* pColumn)
{
    std::vector<ColumnView*>::iterator it = std::find(_grid.begin(), _grid.end(), pColumn);
    return it - _grid.begin();
}


ClusterView*
ColumnClusterViewImpl::getFirstCluster()
{
    return (*_grid.begin())->getTopCluster();
}


ClusterView*
ColumnClusterViewImpl::createClusterInNewColumn(int column)
{
    if (column < 0 || column > _grid.size()) {
        return 0;
    }

    ColumnView* pCol = new ColumnView(this);
    SplitterViewImpl::insertView(pCol, column);
    _grid.insert(_grid.begin() + column, pCol);
    return pCol->getTopCluster();
}


ClusterView*
ColumnClusterViewImpl::createClusterInRow(int column, int row)
{
    if (column < 0 || column >= _grid.size()) {
        return 0;
    }
    return _grid[column]->createCluster(row);
}


} // namespace Gui
} // namespace Omm
