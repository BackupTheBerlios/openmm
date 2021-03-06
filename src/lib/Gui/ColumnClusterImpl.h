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

#ifndef ColumnClusterImpl_INCLUDED
#define ColumnClusterImpl_INCLUDED

#include <map>
#include <vector>
#include <stack>

#include "Gui/View.h"
#include "ViewImpl.h"
#include "AbstractClusterImpl.h"

namespace Omm {
namespace Gui {


class SplitterView;
class ColumnView;
class ClusterConfiguration;

class ColumnClusterViewImpl : public AbstractClusterViewImpl, public PlainViewImpl
{
    friend class ColumnClusterController;
    friend class ColumnClusterLayout;
    friend class ColumnView;

public:
    ColumnClusterViewImpl(View* pView);
    virtual ~ColumnClusterViewImpl();

    virtual void init();
    virtual void insertView(View* pView, const std::string& label = "", int index = 0);
    virtual void removeView(View* pView);
    virtual void showViewAtIndex(View* pView, int index);
    virtual void hideView(View* pView);
    virtual std::string getConfiguration();
    virtual void setConfiguration(const std::string& configuration);

    virtual int getViewCount();
    virtual int getCurrentViewIndex();
    virtual void setCurrentViewIndex(int index);
    virtual int getIndexFromView(View* pView);
    virtual View* getViewFromIndex(int index);

    virtual void setHandlesHidden(bool hidden = true);
    virtual const int getHandleHeight();

private:
    typedef std::vector<ColumnView*>::iterator GridIterator;

    int getColumnCount();
    int getColumnIndex(ColumnView* pColumn);
    ColumnView* getColumn();
    void putColumn(ColumnView* pColumn);

    ClusterView* getOriginCluster();
    ClusterView* createClusterInNewColumn(int column);
    ClusterView* createClusterInRow(int column, int row);
    ClusterView* getCluster(int column, int cluster);
    ClusterView* getCluster(View* pView);
//    void mergeClusterWithCluster(ClusterView* pCluster, ClusterView* pTargetCluster);

    void removeCluster(ClusterView* pCluster);
    void removeEmptyCols();

    void movedView(View* pView);
    void changedConfiguration();
    void getOriginConfiguration(ClusterConfiguration& configuration);
    void getCurrentConfiguration(ClusterConfiguration& configuration);
    void getDefaultConfiguration(ClusterConfiguration& configuration);
    void getTransitionConfiguration(int width, int height, ClusterConfiguration& targetConfiguration, ClusterConfiguration& transitionConfiguration);
//    void getTransition(int width, int height, const ClusterConfiguration& targetConfiguration, ClusterConfiguration& transitionConfiguration);
    void layoutViews(int width, int height);
    void layoutViews(ClusterConfiguration& targetConfiguration);
//    void layoutViews(const ClusterConfiguration& targetConfiguration);

    SplitterView*                                   _pSplitter;
    std::vector<ColumnView*>                        _grid;
    std::stack<ColumnView*>                         _columnPool;
    ClusterConfiguration*                           _pTargetConfiguration;
};


}  // namespace Omm
}  // namespace Gui

#endif
