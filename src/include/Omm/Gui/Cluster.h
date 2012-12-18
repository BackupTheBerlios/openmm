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

#ifndef Cluster_INCLUDED
#define Cluster_INCLUDED

#include <map>
#include <vector>

#include "View.h"
#include "Model.h"
#include "Image.h"

namespace Omm {
namespace Gui {


class Label;
class Button;
class ListItemView;
class SubClusterView;

class ClusterModel : public Model
{
public:
    virtual int getViewCount() { return 0; }
    virtual View* getView(int index) { return 0; }
    virtual std::string getName(int index) { return ""; }
    virtual ImageModel* getImageModel(int index) { return 0; }
};


class ClusterView : public View
{
    friend class ClusterViewImpl;
    friend class ClusterStackedLayout;
    friend class SubClusterView;

public:
//    static const std::string Flat;
//    static const std::string Tree;

    ClusterView(View* pParent = 0);
    virtual ~ClusterView();

    void addView(View* pView, const std::string& name = "", bool show = true);
//    void insertView(View* pView, const std::string& name = "", int index);
    void removeView(View* pView);

    int getTabCount();
//    getViewCount();
    int getCurrentTab();
//    getCurrentViewIndex(); /// current view has the focus
    void setCurrentTab(int index);
//    setCurrentViewIndex(int index);
    void setCurrentView(View* pView);
//    getIndexFromView(View* pView);

    void setTabBarHidden(bool hidden = true);
//    setHandlesHidden(bool hidden = true);
    const int getHandleHeight() const;

protected:
//    void insertHandle(int index, const std::string& name = "");
//    void removeHandle(int index);
    virtual void syncViewImpl();

private:
    int                                 _handleHeight;
    int                                 _handleWidth;
    bool                                _handleBarHidden;
    std::map<View*, ListItemView*>      _handles;
    std::vector<View*>                  _views;
    int                                 _currentView;


//    SubClusterView* addSubCluster(bool horizontalLayout);
//
//    SubClusterView*                         _pSubCluster;
//    ClusterView*                            _pFirstCluster;
//    ClusterView*                            _pSecondCluster;
};


class Cluster : public Widget<ClusterView, Controller, ClusterModel>
{
public:
    Cluster(View* pParent = 0) : Widget<ClusterView, Controller, ClusterModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
