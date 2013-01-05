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
#include <stack>
#include <Poco/LineEndingConverter.h>

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


namespace Omm {
namespace Gui {


class ColumnView : public SplitterView
{
public:
    typedef std::vector<ClusterView*>::iterator ClusterIterator;

    ColumnView(ColumnClusterViewImpl* pColumnClusterViewImpl);

    ClusterIterator beginCluster();
    ClusterIterator endCluster();

    int getSize();
    int getRow(ClusterView* pCluster);
    ClusterView* getTopCluster();
//    ClusterView* createCluster();
    void insertInitialCluster();
    ClusterView* createCluster(int row);
    void removeCluster(ClusterView* pCluster);
    void removeEmptyCluster();

private:
    ClusterView* getCluster();
    void putCluster(ClusterView* pCluster);

    static int                  _clusterNumber;
    static int                  _columnNumber;
    ColumnClusterViewImpl*      _pColumnClusterViewImpl;
    std::vector<ClusterView*>   _col;
    std::stack<ClusterView*>    _clusterPool;
};


class ColumnClusterController : public ClusterController
{
public:
    ColumnClusterController(ClusterView* pClusterView, ColumnView* pColumnView, ColumnClusterViewImpl* pColumnClusterViewImpl) : _pClusterView(pClusterView), _pColumnView(pColumnView), _pColumnClusterViewImpl(pColumnClusterViewImpl) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "column cluster controller drag started in source view: " + _pClusterView->getName());
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "column cluster controller drag moved in target view: " + _pClusterView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "column cluster controller dropped in target view: " + _pClusterView->getName()
                + ", source view: " + pDrag->getSource()->getName() + ", source cluster: " + pDrag->getSource()->getParent()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
        bool horizontal = pos.y() < _pClusterView->getHandleHeight() + _pClusterView->height() / 2;

//        ClusterView* pSourceCluster = dynamic_cast<ClusterView*>(pDrag->getSource()->getParent());
//        if (pSourceCluster && pSourceCluster->getViewCount() == 1) {
//            pSourceCluster->hide(false);
//        }

        ClusterView* pNewCluster = 0;
        int row = _pColumnView->getRow(_pClusterView);
        int col = _pColumnClusterViewImpl->getColumnIndex(_pColumnView);
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

    virtual void insertedView(View* pView)
    {
//        LOG(gui, debug, "column cluster inserted view: " + pView->getName());
        _pColumnClusterViewImpl->movedView(pView);
    }


    ClusterView*            _pClusterView;
    ColumnView*             _pColumnView;
    ColumnClusterViewImpl*  _pColumnClusterViewImpl;
};


class ClusterCoordinate
{
public:
//    ClusterCoordinate() : _column(0), _cluster(0), _index(0) {}
    ClusterCoordinate(const ClusterCoordinate& coord) : _column(coord._column), _cluster(coord._cluster), _index(coord._index) {}
    ClusterCoordinate(int column, int cluster, int index) : _column(column), _cluster(cluster), _index(index) {}

//    bool
//    operator()(const ClusterCoordinate& c1, const ClusterCoordinate& c2) const
//    {
//        return c1._column < c2._column && c1._cluster < c2._cluster && c1._index < c2._index;
//    }

    void read(const std::string& configuration);
    std::string write() const;

    bool equal(const ClusterCoordinate& coord) const
    {
        return _column == coord._column && _cluster == coord._cluster && _index == coord._index;
    }

    int     _column;
    int     _cluster;
    int     _index;
};


struct ClusterCoordinateCompare
{
    bool
    operator()(const ClusterCoordinate& c1, const ClusterCoordinate& c2) const
    {
        return c1._column < c2._column && c1._cluster < c2._cluster && c1._index < c2._index;
    }
};


std::string
ClusterCoordinate::write() const
{
    return "[" + Poco::NumberFormatter::format(_column) + ", "
            + Poco::NumberFormatter::format(_cluster) + ", "
            + Poco::NumberFormatter::format(_index) + "]";
}


class ClusterConfiguration
{
public:
    void addView(const std::string& name, const ClusterCoordinate& coord);
    void read(const std::string& configuration);
    std::string write();

    int countColumns();
    int countClusters(int column);
    std::string clusterConfiguration(int column, int cluster);

    std::map<std::string, ClusterCoordinate>                               _coords;
    std::map< int, std::map< int, std::map< int, std::string > > >         _views;
//    std::map<ClusterCoordinate, std::string, ClusterCoordinateCompare>     _views;
};


void
ClusterConfiguration::addView(const std::string& name, const ClusterCoordinate& coord)
{
    _coords.insert(std::make_pair(name, coord));
    _views[coord._column][coord._cluster][coord._index] = name;
//    _views.insert(std::make_pair(coord, name));
}


std::string
ClusterConfiguration::write()
{
    std::string res;
//    for (std::map<ClusterCoordinate, std::string, ClusterCoordinateCompare>::const_iterator it = _views.begin(); it != _views.end(); ++it) {
//        res += it->first.write() + " " + it->second + Poco::LineEnding::NEWLINE_DEFAULT;
//    }
//    for (std::map<std::string, ClusterCoordinate>::const_iterator it = _coords.begin(); it != _coords.end(); ++it) {
//        res += it->first + " " + it->second.write() + Poco::LineEnding::NEWLINE_DEFAULT;
//    }
    for (int col = 0; col < countColumns(); ++col) {
        for (int cluster = 0; cluster < countClusters(col); ++cluster) {
            res += "[" + Poco::NumberFormatter::format(col) + "," + Poco::NumberFormatter::format(cluster) + "] "
                    + clusterConfiguration(col, cluster) + Poco::LineEnding::NEWLINE_DEFAULT;
        }
    }
    return res;
}


int
ClusterConfiguration::countColumns()
{
    return _views.size();
}


int
ClusterConfiguration::countClusters(int column)
{
    return _views[column].size();
}


std::string
ClusterConfiguration::clusterConfiguration(int column, int cluster)
{
    std::string res;
    for (std::map<int, std::string>::iterator it = _views[column][cluster].begin(); it != _views[column][cluster].end(); ++it) {
        res += it->second + ",";
    }
    return res.substr(0, res.length() - 1);
}


class ColumnClusterLayout : public Layout
{
public:
    ColumnClusterLayout(ColumnClusterViewImpl* pViewImpl) : _pViewImpl(pViewImpl) {}

    virtual void layoutView();

    ColumnClusterViewImpl*     _pViewImpl;
};


void
ColumnClusterLayout::layoutView()
{
    LOG(gui, debug, "column cluster view update layout with current size [" + Poco::NumberFormatter::format(_pView->width()) + ", " + Poco::NumberFormatter::format(_pView->height()) + "]");
    static_cast<ColumnClusterViewImpl*>(_pView->getViewImpl())->layoutViews(_pView->width(), _pView->height());
}


int ColumnView::_clusterNumber = 0;
int ColumnView::_columnNumber = 0;

ColumnView::ColumnView(ColumnClusterViewImpl* pColumnClusterViewImpl) :
SplitterView(0, View::Vertical),
_pColumnClusterViewImpl(pColumnClusterViewImpl)
{
    setName("column " + Poco::NumberFormatter::format(_columnNumber));
    LOG(gui, debug, "column cluster create new column: " + getName());
    _columnNumber++;
}


ColumnView::ClusterIterator
ColumnView::beginCluster()
{
    return _col.begin();
}


ColumnView::ClusterIterator
ColumnView::endCluster()
{
    return _col.end();
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
    return _col[0];
}


//ClusterView*
//ColumnView::createCluster()
//{
////    ClusterView* pCluster = new ClusterView(0, ClusterView::Generic);
//    ClusterView* pCluster = getCluster();
//    pCluster->setAcceptDrops(true);
//    pCluster->attachController(new ColumnClusterController(pCluster, this, _pColumnClusterViewImpl));
//    return pCluster;
//}


void
ColumnView::insertInitialCluster()
{
    ClusterView* pCluster = getCluster();
    SplitterView::insertView(pCluster, 0);
    _col.push_back(pCluster);
}


ClusterView*
ColumnView::createCluster(int row)
{
    if (row < 0 || row > _col.size()) {
        return 0;
    }
//    ClusterView* pCluster = createCluster();
    ClusterView* pCluster = getCluster();
    SplitterView::insertView(pCluster, row);
    _col.insert(_col.begin() + row, pCluster);
    return pCluster;
}


void
ColumnView::removeCluster(ClusterView* pCluster)
{
    std::vector<ClusterView*>::iterator it = std::find(_col.begin(), _col.end(), pCluster);
    if (it != _col.end()) {
        putCluster(*it);
        _col.erase(it);
    }
}


void
ColumnView::removeEmptyCluster()
{
    std::stack<std::vector<ClusterView*>::iterator> emptyCluster;
    for (ClusterIterator it = beginCluster(); it != endCluster(); ++it) {
        if (!(*it)->getViewCount()) {
            emptyCluster.push(it);
        }
    }
    while (emptyCluster.size()) {
        putCluster(*emptyCluster.top());
        _col.erase(emptyCluster.top());
        emptyCluster.pop();
    }
}


ClusterView*
ColumnView::getCluster()
{
    ClusterView* pCluster;
    if (_clusterPool.size()) {
        pCluster = _clusterPool.top();
        _clusterPool.pop();
        pCluster->show(false);
    }
    else {
        pCluster = new ClusterView(0, ClusterView::Generic);
//        pCluster->setParent(this);
        pCluster->setName("cluster " + Poco::NumberFormatter::format(_clusterNumber));
        pCluster->setAcceptDrops(true);
        pCluster->attachController(new ColumnClusterController(pCluster, this, _pColumnClusterViewImpl));
        _clusterNumber++;
    }
    return pCluster;
}


void
ColumnView::putCluster(ClusterView* pCluster)
{
    _clusterPool.push(pCluster);
}


ColumnClusterViewImpl::ColumnClusterViewImpl(View* pView) :
SplitterViewImpl(pView, View::Horizontal),
_layoutNeedsUpdate(true)
//_pTargetLayout(new ClusterLayout)
{
}


ColumnClusterViewImpl::~ColumnClusterViewImpl()
{
//    delete _pTargetLayout;
}


void
ColumnClusterViewImpl::init()
{
    _pView->setName("column cluster");
    _pView->setLayout(new ColumnClusterLayout(this));
//    ColumnView* pCol = new ColumnView(this);
    ColumnView* pCol = getColumn();
    SplitterViewImpl::insertView(pCol, 0);
    _grid.push_back(pCol);
}


void
ColumnClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
    LOG(gui, debug, "column cluster view impl insert view: " + pView->getName());

    getOriginCluster()->insertView(pView, label, index);
    _views.insert(_views.begin() + index, pView);
    _viewMap.insert(std::make_pair(pView->getName(), pView));
}


void
ColumnClusterViewImpl::removeView(View* pView)
{
    LOG(gui, debug, "column cluster view impl remove view: " + pView->getName());

    // TODO: do we need to also remove the view from the subcluster (and clean up columns)?

    std::vector<View*>::iterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        _views.erase(it);
    }
    _viewMap.erase(pView->getName());
}


void
ColumnClusterViewImpl::setConfiguration(const std::string& configuration)
{

}


int
ColumnClusterViewImpl::getViewCount()
{
//    int viewCount = 0;
//    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
//        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
//            viewCount += (*it)->getViewCount();
//        }
//    }
//    return viewCount;
    return _views.size();
}


int
ColumnClusterViewImpl::getCurrentViewIndex()
{
    // FIXME: this is not the view with focus, just the visible view of the first cluster
    return getOriginCluster()->getCurrentViewIndex();
}


void
ColumnClusterViewImpl::setCurrentViewIndex(int index)
{
    LOG(gui, debug, "column cluster view impl set current view index: " + Poco::NumberFormatter::format(index));

    if (index == -1) {
        return;
    }
    int topColClusterIndex = 0;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        int rowClusterIndex = 0;
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
            if (topColClusterIndex + rowClusterIndex + (*it)->getViewCount() > index) {
                (*it)->setCurrentViewIndex(index - rowClusterIndex - topColClusterIndex);
                return;
            }
            else {
                rowClusterIndex += (*it)->getViewCount();
            }
        }
        topColClusterIndex += rowClusterIndex;
    }
}


int
ColumnClusterViewImpl::getIndexFromView(View* pView)
{
//    int topColClusterIndex = 0;
//    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
//        int rowClusterIndex = 0;
//        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
//            int index = (*it)->getIndexFromView(pView);
//            if (index != -1) {
//                return topColClusterIndex + rowClusterIndex + index;
//            }
//            else {
//                rowClusterIndex += (*it)->getViewCount();
//            }
//        }
//        topColClusterIndex += rowClusterIndex;
//    }
    std::vector<View*>::iterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        return it - _views.begin();
    }
    else {
        return -1;
    }
}


View*
ColumnClusterViewImpl::getViewFromIndex(int index)
{
//    int topColClusterIndex = 0;
//    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
//        int rowClusterIndex = 0;
//        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
//            if (index < topColClusterIndex + rowClusterIndex + (*it)->getViewCount()) {
//                return (*it)->getViewFromIndex(index - topColClusterIndex - rowClusterIndex);
//            }
//            rowClusterIndex += (*it)->getViewCount();
//        }
//        topColClusterIndex += rowClusterIndex;
//    }
//    return 0;
    if (0 <= index && index < _views.size()) {
        return _views[index];
    }
    else {
        return 0;
    }
}


void
ColumnClusterViewImpl::setHandlesHidden(bool hidden)
{
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
            (*it)->setHandlesHidden(hidden);
        }
    }
}


const int
ColumnClusterViewImpl::getHandleHeight()
{
    return getOriginCluster()->getHandleHeight();
}


//std::string
//ColumnClusterViewImpl::writeLayout()
//{
////    std::string res;
////    int column = 0;
////    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++column, ++colIt) {
////        int cluster = 0;
////        res += "[" + Poco::NumberFormatter::format(column);
////        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++cluster, ++it) {
////            res += "(" + Poco::NumberFormatter::format(cluster) + ":\"" + (*it)->getName() + "\"" + (*it)->writeLayout() + ")";
////        }
////        res += "]";
////    }
////    return res;
//
//    ClusterLayout layout;
//    int column = 0;
//    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++column, ++colIt) {
//        int cluster = 0;
//        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++cluster, ++it) {
//            for (int index = 0; index < (*it)->getViewCount(); ++index) {
//                ClusterCoordinate coord(column, cluster, index);
//                layout.addCluster((*it)->getViewFromIndex(index)->getName(), coord);
//            }
//        }
//    }
//    return layout.write();
//}


int
ColumnClusterViewImpl::getColumnCount()
{
    return _grid.size();
}


int
ColumnClusterViewImpl::getColumnIndex(ColumnView* pColumn)
{
    std::vector<ColumnView*>::iterator it = std::find(_grid.begin(), _grid.end(), pColumn);
    return it - _grid.begin();
}


ColumnView*
ColumnClusterViewImpl::getColumn()
{
    ColumnView* pColumn;
    if (_columnPool.size()) {
        pColumn = _columnPool.top();
        LOG(gui, debug, "column cluster get column from pool: " + pColumn->getName());
        _columnPool.pop();
    }
    else {
        pColumn = new ColumnView(this);
    }
    pColumn->insertInitialCluster();
    return pColumn;
}


void
ColumnClusterViewImpl::putColumn(ColumnView* pColumn)
{
    LOG(gui, debug, "column cluster put column to pool: " + pColumn->getName());
    _columnPool.push(pColumn);
}


ClusterView*
ColumnClusterViewImpl::getOriginCluster()
{
    return _grid[0]->getTopCluster();
}


ClusterView*
ColumnClusterViewImpl::createClusterInNewColumn(int column)
{
    if (column < 0 || column > _grid.size()) {
        return 0;
    }
    LOG(gui, debug, "column cluster create column: " + Poco::NumberFormatter::format(column));

//    ColumnView* pCol = new ColumnView(this);
    ColumnView* pCol = getColumn();
    SplitterViewImpl::insertView(pCol, column);
    pCol->show(false);
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


ClusterView*
ColumnClusterViewImpl::getCluster(int column, int cluster)
{
    return *(_grid[column]->beginCluster() + cluster);
}


ClusterView*
ColumnClusterViewImpl::getCluster(View* pView)
{
    return dynamic_cast<ClusterView*>(pView->getParent());
}


void
ColumnClusterViewImpl::mergeClusterWithCluster(ClusterView* pCluster, ClusterView* pTargetCluster)
{
    if (pCluster == pTargetCluster) {
        return;
    }
    for (int viewIndex = 0; viewIndex < pCluster->getViewCount(); ++viewIndex) {
        View* pView = pCluster->getViewFromIndex(viewIndex);
        pTargetCluster->insertView(pView, pView->getName(), pTargetCluster->getViewCount());
    }
    removeCluster(pCluster);
}


void
ColumnClusterViewImpl::removeCluster(ClusterView* pCluster)
{
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        (*colIt)->removeCluster(pCluster);
    }
}


void
ColumnClusterViewImpl::removeEmptyCols()
{
    std::stack<std::vector<ColumnView*>::iterator> emptyCols;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        (*colIt)->removeEmptyCluster();
        if (!(*colIt)->getSize() && colIt != _grid.begin()) {
            emptyCols.push(colIt);
        }
    }
    while (emptyCols.size()) {
        (*emptyCols.top())->hide(false);
        putColumn(*emptyCols.top());
        _grid.erase(emptyCols.top());
        emptyCols.pop();
    }
}


void
ColumnClusterViewImpl::movedView(View* pView)
{
    IMPL_NOTIFY_CONTROLLER(ClusterController, movedView, pView);
}


void
ColumnClusterViewImpl::getCurrentConfiguration(ClusterConfiguration& configuration)
{
    int column = 0;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++column, ++colIt) {
        int cluster = 0;
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++cluster, ++it) {
            for (int index = 0; index < (*it)->getViewCount(); ++index) {
                ClusterCoordinate coord(column, cluster, index);
                configuration.addView((*it)->getViewFromIndex(index)->getName(), coord);
            }
        }
    }
}


void
ColumnClusterViewImpl::getDefaultConfiguration(ClusterConfiguration& configuration)
{
    int i = 0;
    for (std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++i, ++it) {
        ClusterCoordinate coord(0, 0, i);
        configuration.addView((*it)->getName(), coord);
    }
}


void
ColumnClusterViewImpl::getTargetConfiguration(ClusterConfiguration& configuration)
{
    int i = 0;
    for (std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++i, ++it) {
        ClusterCoordinate coord(i, 0, 0);
        configuration.addView((*it)->getName(), coord);
    }
}


void
ColumnClusterViewImpl::layoutViews(int width, int height)
{
//    int widthOriginMin = _views[0]->width(View::Pref) * 0.66;
    if (!_views.size()) {
        return;
    }

//    int index = 0;
//    for (std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++index, ++it) {
//        getOriginCluster()->insertView(*it, (*it)->getName(), index);
//    }
//    removeEmptyCols();
//
//    int widthOriginMax = _views[0]->width(View::Pref) * 1.33;
//
//    int viewIndex = _views.size() - 1;
//    int widthSum = widthOriginMax;
//    while (viewIndex && widthSum < width) {
//        widthSum += _views[viewIndex]->width(View::Pref) * 1.33;
//        viewIndex--;
//    }
//
//    for (int i = 0; i < _views.size(); ++i) {
//        if (i <= viewIndex) {
//            getOriginCluster()->insertView(_views[i], _views[i]->getName(), i);
//        }
//        else {
//            ClusterView* pCluster = createClusterInNewColumn(i - viewIndex);
//            pCluster->insertView(_views[i], _views[i]->getName());
//        }
//    }

// ----------------------------------

//    int firstUnfoldedCluster = 4;
//    if (_layoutNeedsUpdate) {
//        for (int i = firstUnfoldedCluster; i < _views.size(); ++i) {
//            ClusterView* pCluster = createClusterInNewColumn(i - firstUnfoldedCluster + 1);
//            pCluster->insertView(_views[i], _views[i]->getName());
//        }
//        _layoutNeedsUpdate = false;
//    }

//    LOG(gui, debug, "column cluster layout:" + Poco::LineEnding::NEWLINE_DEFAULT + writeLayout());

// ----------------------------------
    ClusterConfiguration targetConfiguration;
    if (width < 500) {
        getDefaultConfiguration(targetConfiguration);
    }
    else {
        getTargetConfiguration(targetConfiguration);
    }
    layoutViews(targetConfiguration);

}


void
ColumnClusterViewImpl::layoutViews(ClusterConfiguration& targetConfiguration)
{
    ClusterConfiguration currentConfiguration;
    getCurrentConfiguration(currentConfiguration);

    LOG(gui, debug, "column cluster current configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + currentConfiguration.write());
    LOG(gui, debug, "column cluster target configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + targetConfiguration.write());

    for (int col = _grid.size(); col < targetConfiguration.countColumns(); ++col) {
        createClusterInNewColumn(col);
    }

    for (int col = 0; col < _grid.size(); ++col) {
        for (int cluster = _grid[col]->getSize(); cluster < targetConfiguration.countClusters(col); ++cluster) {
            createClusterInRow(col, cluster);
        }
    }

    for (std::vector<View*>::iterator it = _views.begin(); it != _views.end(); ++it) {
        std::map<std::string, ClusterCoordinate>::const_iterator currentPos = currentConfiguration._coords.find((*it)->getName());
        std::map<std::string, ClusterCoordinate>::const_iterator targetPos = targetConfiguration._coords.find((*it)->getName());
        if (targetPos == targetConfiguration._coords.end()) {
            // move view to invisible area
        }
        else {
            if (currentPos != currentConfiguration._coords.end() && currentPos->second.equal(targetPos->second)) {
                // do nothing, view is at same position as before
                continue;
            }
            else {
                // move view to new destination
//                ClusterView* pCluster = *(_grid[targetPos->second._column]->beginCluster() + targetPos->second._cluster);
                ClusterView* pCluster = getCluster(targetPos->second._column, targetPos->second._cluster);
                pCluster->insertView(*it, (*it)->getName());
            }
        }
    }

    removeEmptyCols();

    for (int col = 0; col < _grid.size(); ++col) {
        for (int cluster = 0; cluster < _grid[col]->getSize(); ++cluster) {
            getCluster(col, cluster)->setConfiguration(targetConfiguration.clusterConfiguration(col, cluster));
        }
    }
}


//void
//ColumnClusterViewImpl::layoutViews(int width, int height)
//{
//    View::SizeConstraint widthConstrained;
//    View::SizeConstraint heightConstrained;
//    View* pViewToMove = 0;
//    ClusterView* pClusterToMove = 0;
//    std::stack<ClusterView*> clustersToMove;
//    int column = 0;
//    int columnToInsert = 0;
//    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++column, ++colIt) {
//        int cluster = 0;
//        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++cluster, ++it) {
//            View::SizeConstraint wC;
//            View::SizeConstraint hC;
//            (*it)->sizeConstraintReached(wC, hC);
//            if (wC != View::None) {
//                widthConstrained = wC;
//            }
//            if (hC != View::None) {
//                heightConstrained = hC;
//            }
//            if (wC != View::None || hC != View::None) {
//                LOG(gui, debug, "column cluster size constraint reached in cluster [" + Poco::NumberFormatter::format(column) + ", " + Poco::NumberFormatter::format(cluster) + "]");
////                clustersToMove.push(*it);
//                if (*it != getOriginCluster() && !pClusterToMove) {
//                    pClusterToMove = *it;
//                }
//            }
//            if ((*it)->getViewCount() > 1) {
//                pViewToMove = (*it)->getViewFromIndex((*it)->getViewCount() - 1);
//                columnToInsert = column + 1;
//                LOG(gui, debug, "column cluster view move candidate in column " + Poco::NumberFormatter::format(column) + ", cluster " + Poco::NumberFormatter::format(cluster) + ": " + pViewToMove->getName());
//            }
//        }
//    }
//
//    if (pViewToMove) {
//        LOG(gui, debug, "column cluster view to move: " + pViewToMove->getName());
//    }
//    ClusterView* pNewCluster = 0;
//    if (widthConstrained == View::Min) {
//        LOG(gui, debug, "column cluster MIN PREF WIDTH CONSTRAINT reached");
////        for (ColumnView::ClusterIterator it = _grid.back()->beginCluster(); it != _grid.back()->endCluster(); ++it) {
////            mergeClusterWithCluster(*it, getOriginCluster());
////        }
////        while (clustersToMove.size()) {
////            mergeClusterWithCluster(clustersToMove.top(), getOriginCluster());
////            clustersToMove.pop();
////        }
//        if (pClusterToMove) {
//            mergeClusterWithCluster(pClusterToMove, getOriginCluster());
//        }
//    }
////    else if (widthConstrained == View::Max) {
////        LOG(gui, debug, "column cluster MAX WIDTH CONSTRAINT reached");
//    else if (widthConstrained == View::Max) {
//        LOG(gui, debug, "column cluster MAX PREF WIDTH CONSTRAINT reached");
//        if (pViewToMove) {
//            pNewCluster = createClusterInNewColumn(columnToInsert);
//            pNewCluster->insertView(pViewToMove, pViewToMove->getName());
//            pViewToMove = 0;
//        }
//    }
//    else if (heightConstrained == View::Min) {
////    if (heightConstrained == View::Min) {
////        LOG(gui, debug, "column cluster MIN PREF HEIGHT CONSTRAINT reached");
////        while (clustersToMove.size()) {
////            mergeClusterWithCluster(clustersToMove.top(), getOriginCluster());
////            clustersToMove.pop();
////        }
////        if (pClusterToMove) {
////            mergeClusterWithCluster(pClusterToMove, getOriginCluster());
////        }
//    }
////    else if (heightConstrained == View::Max) {
////        LOG(gui, debug, "column cluster MAX HEIGHT CONSTRAINT reached");
//    else if (heightConstrained == View::Max) {
////        LOG(gui, debug, "column cluster MAX PREF HEIGHT CONSTRAINT reached");
////        if (pViewToMove) {
////            pNewCluster = createClusterInRow(0, _grid[0]->getSize());
////            pNewCluster->insertView(pViewToMove, pViewToMove->getName());
////        }
//    }
//
//    removeEmptyCols();
//
//    LOG(gui, debug, "column cluster layout: " + writeLayout());
//}


} // namespace Gui
} // namespace Omm
