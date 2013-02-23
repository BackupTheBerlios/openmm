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
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>

#include "Log.h"
#include "Gui/Cluster.h"
#include "Gui/GuiLogger.h"
#include "Gui/Layout.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/Splitter.h"
#include "Gui/Label.h"
#include "Gui/Button.h"
#include "Gui/ListItem.h"
#include "Gui/Drag.h"
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

        std::string label = pDrag->getSource()->getName();
        ListItemModel* pHandleModel = dynamic_cast<ListItemModel*>(pDrag->getModel());
        if (pHandleModel) {
            label = pHandleModel->getLabelModel()->getLabel();
        }

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
            pNewCluster->insertView(pDrag->getSource(), label, 0);
        }
        _pColumnClusterViewImpl->changedConfiguration();
    }

    virtual void insertedView(View* pView)
    {
//        LOG(gui, debug, "column cluster inserted view: " + pView->getName());
        _pColumnClusterViewImpl->movedView(pView);
    }

    virtual void changedConfiguration()
    {
        _pColumnClusterViewImpl->changedConfiguration();
    }

    ClusterView*            _pClusterView;
    ColumnView*             _pColumnView;
    ColumnClusterViewImpl*  _pColumnClusterViewImpl;
};


class ClusterCoordinate
{
public:
    ClusterCoordinate(const ClusterCoordinate& coord) : _column(coord._column), _cluster(coord._cluster), _index(coord._index) {}
    ClusterCoordinate(int column, int cluster, int index) : _column(column), _cluster(cluster), _index(index) {}

    std::string write() const;

    bool equal(const ClusterCoordinate& coord) const
    {
        return _column == coord._column && _cluster == coord._cluster && _index == coord._index;
    }

    int     _column;
    int     _cluster;
    int     _index;
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
    ClusterConfiguration() : _width(0), _height(0) {}

    void addView(const std::string& name, const ClusterCoordinate& coord);
    std::string write();

    int countColumns();
    int countClusters(int column);
    int countViews(int column, int cluster);
    std::string subClusterConfiguration(int column, int cluster);

    std::map<std::string, ClusterCoordinate>                                _coords;
    std::map< int, std::map< int, std::map< int, std::string > > >          _views;
    int                                                                     _width;
    int                                                                     _height;
    std::vector<float>                                                      _colWidth;
    std::vector< std::vector<float > >                                      _clusterHeight;
};


void
ClusterConfiguration::addView(const std::string& name, const ClusterCoordinate& coord)
{
    _coords.insert(std::make_pair(name, coord));
    _views[coord._column][coord._cluster][coord._index] = name;
}


std::string
ClusterConfiguration::write()
{
    std::string res = "col ";
    for (int col = 0; col < countColumns(); ++col) {
        for (int cluster = 0; cluster < countClusters(col); ++cluster) {
            res += "[" + Poco::NumberFormatter::format(col) + "," + Poco::NumberFormatter::format(cluster) + "] "
                    + subClusterConfiguration(col, cluster) + " ";
        }
    }
    if (_width && _height) {
        res += "{" + Poco::NumberFormatter::format(_width) + ";" + Poco::NumberFormatter::format(_height) + "} ";
    }
    if (_colWidth.size()) {
        res += "{";
        for (std::vector<float>::const_iterator it = _colWidth.begin(); it != _colWidth.end(); ++it) {
            res += Poco::NumberFormatter::format(*it, 2);
            if (it + 1 != _colWidth.end()) {
                res += ";";
            }
        }
        res += "} ";
    }
    if (_clusterHeight.size()) {
        for (std::vector< std::vector<float > >::const_iterator cit = _clusterHeight.begin(); cit != _clusterHeight.end(); ++cit) {
            res += "{";
            for (std::vector<float>::const_iterator it = cit->begin(); it != cit->end(); ++it) {
                res += Poco::NumberFormatter::format(*it, 2);
                if (it + 1 != cit->end()) {
                    res += ";";
                }
            }
            res += "} ";
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


int
ClusterConfiguration::countViews(int column, int cluster)
{
    return _views[column][cluster].size();
}


std::string
ClusterConfiguration::subClusterConfiguration(int column, int cluster)
{
    // we know, that subclusters have "tab" type configurations, so we generate the simple form w/o "tab" prefix
    // instead of calling getConfiguration() of subcluster
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
    _pViewImpl->layoutViews(_pView->width(), _pView->height());
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
PlainViewImpl(pView),
_pTargetConfiguration(0)
{
}


ColumnClusterViewImpl::~ColumnClusterViewImpl()
{
    if (_pTargetConfiguration) {
        delete _pTargetConfiguration;
    }
}


void
ColumnClusterViewImpl::init()
{
    _pSplitter = new SplitterView(_pView, View::Horizontal);
    _pView->setLayout(new HorizontalLayout);
    _pSplitter->setName("column cluster");
    _pSplitter->setLayout(new ColumnClusterLayout(this));
    ColumnView* pCol = getColumn();
    _pSplitter->insertView(pCol, 0);
    _grid.push_back(pCol);
}


void
ColumnClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
    LOG(gui, debug, "column cluster view impl insert view: " + pView->getName());

    getOriginCluster()->insertView(pView, label, index);
    _visibleViews.insert(_visibleViews.begin() + index, pView);
}


void
ColumnClusterViewImpl::removeView(View* pView)
{
    LOG(gui, debug, "column cluster view impl remove view: " + pView->getName());

    // TODO: do we need to also remove the view from the subcluster (and clean up columns)?

    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (it != _visibleViews.end()) {
        _visibleViews.erase(it);
    }
}


void
ColumnClusterViewImpl::showViewAtIndex(View* pView, int index)
{
//    LOG(gui, debug, "column cluster view impl show view: " + pView->getName());
//
//    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
//    if (it == _visibleViews.end()) {
//        _visibleViews.insert(_visibleViews.begin() + index, pView);
//        _hiddenViews.erase(pView);
//    }
//    ClusterView* pCluster = getCluster(pView);
//    if (pCluster) {
//        pCluster->showViewAtIndex(pView, index);
//    }
}


void
ColumnClusterViewImpl::hideView(View* pView)
{
//    LOG(gui, debug, "column cluster view impl hide view: " + pView->getName());
//
//    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
//    if (it != _visibleViews.end()) {
//        _visibleViews.erase(it);
//        _hiddenViews.insert(pView);
//    }
//    ClusterView* pCluster = getCluster(pView);
//    if (pCluster) {
//        pCluster->hideView(pView);
//    }
}


std::string
ColumnClusterViewImpl::getConfiguration()
{
    ClusterConfiguration res;
    getCurrentConfiguration(res);
    return res.write();
}


void
ColumnClusterViewImpl::setConfiguration(const std::string& configuration)
{
    Poco::StringTokenizer columnTokens(configuration, " ");
    if (!columnTokens.count()) {
        LOG(gui, error, "column cluster configuration empty");
        return;
    }
    std::string configType = *columnTokens.begin();
    if (configType != "col") {
        LOG(gui, error, "column cluster cannot parse configuration of type: " + configType);
        return;
    }

    if (_pTargetConfiguration) {
        delete _pTargetConfiguration;
    }
    _pTargetConfiguration = new ClusterConfiguration;
    int i = 1;
    while (i < columnTokens.count() && columnTokens[i][0] != '{') {
        Poco::StringTokenizer coords(columnTokens[i].substr(1, columnTokens[i].length() - 2), ",");
        int col = Poco::NumberParser::parse(coords[0]);
        int cluster = Poco::NumberParser::parse(coords[1]);
        Poco::StringTokenizer views(columnTokens[i + 1], ",");
        for (int index = 0; index < views.count(); ++index) {
            ClusterCoordinate coord(col, cluster, index);
            _pTargetConfiguration->addView(views[index], coord);
        }
        i += 2;
    }
    if (i < columnTokens.count()) {
        Poco::StringTokenizer viewSize(columnTokens[i].substr(1, columnTokens[i].length() - 2), ";");
        _pTargetConfiguration->_width = Poco::NumberParser::parse(viewSize[0]);
        _pTargetConfiguration->_height = Poco::NumberParser::parse(viewSize[1]);
        i++;
    }
    if (i < columnTokens.count()) {
        Poco::StringTokenizer colSizes(columnTokens[i].substr(1, columnTokens[i].length() - 2), ";");
        std::vector<float> sizes;
        for (Poco::StringTokenizer::Iterator it = colSizes.begin(); it != colSizes.end(); ++it) {
            sizes.push_back(Poco::NumberParser::parseFloat(*it));
        }
        _pTargetConfiguration->_colWidth = sizes;
        i++;
    }
    while (i < columnTokens.count()) {
        Poco::StringTokenizer colSizes(columnTokens[i].substr(1, columnTokens[i].length() - 2), ";");
        std::vector<float> sizes;
        for (Poco::StringTokenizer::Iterator it = colSizes.begin(); it != colSizes.end(); ++it) {
            sizes.push_back(Poco::NumberParser::parseFloat(*it));
        }
        _pTargetConfiguration->_clusterHeight.push_back(sizes);
        i++;
    }
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
    return _visibleViews.size();
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
    int topColClusterIndex = 0;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        int rowClusterIndex = 0;
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
            int index = (*it)->getIndexFromView(pView);
            if (index >= 0) {
                LOG(gui, debug, "column cluster view impl get current view index: " + Poco::NumberFormatter::format(topColClusterIndex + rowClusterIndex + index));
                return topColClusterIndex + rowClusterIndex + index;
            }
            else {
                rowClusterIndex += (*it)->getViewCount();
            }
        }
        topColClusterIndex += rowClusterIndex;
    }
    LOG(gui, debug, "column cluster view impl get current view index: -1");
    return -1;
}


View*
ColumnClusterViewImpl::getViewFromIndex(int index)
{
    int topColClusterIndex = 0;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        int rowClusterIndex = 0;
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
            if (index < topColClusterIndex + rowClusterIndex + (*it)->getViewCount()) {
                return (*it)->getViewFromIndex(index - topColClusterIndex - rowClusterIndex);
            }
            rowClusterIndex += (*it)->getViewCount();
        }
        topColClusterIndex += rowClusterIndex;
    }
    return 0;
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

    ColumnView* pCol = getColumn();
    _pSplitter->insertView(pCol, column);
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
//    return dynamic_cast<ClusterView*>(pView->getParent());
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++colIt) {
        for (ColumnView::ClusterIterator it = (*colIt)->beginCluster(); it != (*colIt)->endCluster(); ++it) {
            if ((*it)->getIndexFromView(pView) != -2) {
                return *it;
            }
        }
    }
    return 0;
}


//void
//ColumnClusterViewImpl::mergeClusterWithCluster(ClusterView* pCluster, ClusterView* pTargetCluster)
//{
//    if (pCluster == pTargetCluster) {
//        return;
//    }
//    for (int viewIndex = 0; viewIndex < pCluster->getViewCount(); ++viewIndex) {
//        View* pView = pCluster->getViewFromIndex(viewIndex);
//        pTargetCluster->insertView(pView, pView->getName(), pTargetCluster->getViewCount());
//    }
//    removeCluster(pCluster);
//}


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
ColumnClusterViewImpl::changedConfiguration()
{
    if (_pTargetConfiguration) {
        delete _pTargetConfiguration;
    }
    _pTargetConfiguration = new ClusterConfiguration;
    getCurrentConfiguration(*_pTargetConfiguration);

    IMPL_NOTIFY_CONTROLLER(ClusterController, changedConfiguration);

    LOG(gui, debug, "column cluster target configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + _pTargetConfiguration->write());
}


void
ColumnClusterViewImpl::getOriginConfiguration(ClusterConfiguration& configuration)
{
    int i = 0;
    for (std::vector<View*>::iterator it = _visibleViews.begin(); it != _visibleViews.end(); ++i, ++it) {
        ClusterCoordinate coord(0, 0, i);
        configuration.addView((*it)->getName(), coord);
    }
}


void
ColumnClusterViewImpl::getCurrentConfiguration(ClusterConfiguration& configuration)
{
    configuration._width = _pSplitter->width();
    configuration._height = _pSplitter->height();
    configuration._colWidth = _pSplitter->getSizes();

    int column = 0;
    for (std::vector<ColumnView*>::iterator colIt = _grid.begin(); colIt != _grid.end(); ++column, ++colIt) {
        configuration._clusterHeight.push_back((*colIt)->getSizes());
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
    for (std::vector<View*>::iterator it = _visibleViews.begin(); it != _visibleViews.end(); ++i, ++it) {
        ClusterCoordinate coord(i, 0, 0);
        configuration.addView((*it)->getName(), coord);
    }
}


void
ColumnClusterViewImpl::getTransitionConfiguration(int width, int height, ClusterConfiguration& targetConfiguration, ClusterConfiguration& transitionConfiguration)
{
    // we need metrical (not only topological) information to calculate transitional configurations
    if (!(targetConfiguration._width || targetConfiguration._colWidth.size() || targetConfiguration._clusterHeight.size())) {
        transitionConfiguration = targetConfiguration;
        return;
    }

    // calculate the number of columns we need for the transition configuration
    int widthSum = 0;
    int maxColCount = 0;
    for (std::vector<float>::const_iterator it = targetConfiguration._colWidth.begin(); it != targetConfiguration._colWidth.end(); ++it) {
        maxColCount++;
        widthSum += *it * targetConfiguration._width;
        if (widthSum > width) {
            break;
        }
    }
    // move the view names of folded columns to the last visible column, first cluster
//    LOG(gui, debug, "column cluster transition configuration column count: " + Poco::NumberFormatter::format(maxColCount));
    for (int col = 0; col < maxColCount; ++col) {
        transitionConfiguration._views[col] = targetConfiguration._views[col];
    }
    int lastIndex = transitionConfiguration.countViews(maxColCount - 1, 0);
    for (int col = maxColCount; col < targetConfiguration._colWidth.size(); ++col) {
        for (std::map< int, std::map< int, std::string > >::const_iterator it = targetConfiguration._views[col].begin(); it != targetConfiguration._views[col].end(); ++it) {
            for (std::map<int, std::string>::const_iterator cit = it->second.begin(); cit != it->second.end(); ++cit) {
//                LOG(gui, debug, "column cluster transition configuration insert view " + cit->second + " into index: " + Poco::NumberFormatter::format(lastIndex));
                transitionConfiguration._views[maxColCount - 1][0][lastIndex] = cit->second;
                lastIndex++;
            }
        }
    }
    // calculate the coordinate map for the transition configuration
    for (int col = 0; col < transitionConfiguration.countColumns(); ++col) {
        for (int cluster = 0; cluster < transitionConfiguration.countClusters(col); ++cluster) {
            for (int index = 0; index < transitionConfiguration.countViews(col, cluster); ++index) {
                transitionConfiguration._coords.insert(std::make_pair(transitionConfiguration._views[col][cluster][index], ClusterCoordinate(col, cluster, index)));
            }
        }
    }
    // adapt column widths
    float columnTotal = 0.0;
    for (int col = 0; col < maxColCount; ++col) {
        columnTotal += targetConfiguration._colWidth[col];
    }
    for (int col = 0; col < maxColCount; ++col) {
        transitionConfiguration._colWidth.push_back(targetConfiguration._colWidth[col] / columnTotal);
    }
    // TODO: should adapt cluster heights, leaving them as they are by now ...
    transitionConfiguration._clusterHeight = targetConfiguration._clusterHeight;
}


void
ColumnClusterViewImpl::layoutViews(int width, int height)
{
    if (!_visibleViews.size()) {
        return;
    }
    ClusterConfiguration* pConfiguration;
    if (_pTargetConfiguration) {
        pConfiguration = _pTargetConfiguration;
    }
    else {
        pConfiguration = new ClusterConfiguration;
        getOriginConfiguration(*pConfiguration);
    }
    LOG(gui, debug, "column cluster target configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + pConfiguration->write());

    ClusterConfiguration transitionConfiguration;
    getTransitionConfiguration(width, height, *pConfiguration, transitionConfiguration);
    LOG(gui, debug, "column cluster transition configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + transitionConfiguration.write());

//    layoutViews(*pConfiguration);
    layoutViews(transitionConfiguration);
    if (pConfiguration != _pTargetConfiguration) {
        delete pConfiguration;
    }
}


void
ColumnClusterViewImpl::layoutViews(ClusterConfiguration& targetConfiguration)
{
    ClusterConfiguration currentConfiguration;
    getCurrentConfiguration(currentConfiguration);
    LOG(gui, debug, "column cluster current configuration:" + Poco::LineEnding::NEWLINE_DEFAULT + currentConfiguration.write());

    for (int col = _grid.size(); col < targetConfiguration.countColumns(); ++col) {
        createClusterInNewColumn(col);
    }

    for (int col = 0; col < _grid.size(); ++col) {
        for (int cluster = _grid[col]->getSize(); cluster < targetConfiguration.countClusters(col); ++cluster) {
            createClusterInRow(col, cluster);
        }
    }

    for (std::vector<View*>::iterator it = _visibleViews.begin(); it != _visibleViews.end(); ++it) {
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
                ClusterView* pCluster = getCluster(targetPos->second._column, targetPos->second._cluster);
                pCluster->insertView(*it, (*it)->getName());
            }
        }
    }

    removeEmptyCols();

    if (targetConfiguration._colWidth.size()) {
        _pSplitter->setSizes(targetConfiguration._colWidth);
    }
    for (int col = 0; col < _grid.size(); ++col) {
        if (targetConfiguration._clusterHeight.size() && targetConfiguration._clusterHeight[col].size()) {
            _grid[col]->setSizes(targetConfiguration._clusterHeight[col]);
        }
        for (int cluster = 0; cluster < _grid[col]->getSize(); ++cluster) {
            getCluster(col, cluster)->setConfiguration(targetConfiguration.subClusterConfiguration(col, cluster));
        }
    }
}


} // namespace Gui
} // namespace Omm
