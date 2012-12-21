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

//class ClusterModel : public Model
//{
//public:
//    virtual int getViewCount() { return 0; }
//    virtual View* getView(int index) { return 0; }
//    virtual std::string getName(int index) { return ""; }
//    virtual ImageModel* getImageModel(int index) { return 0; }
//};


class ClusterView : public View
{
    friend class ClusterViewImpl;
    friend class ClusterStackedLayout;
    friend class SubClusterView;

public:
    static const std::string Native;
    static const std::string Generic;
    static const std::string Column;
    static const std::string Tree;

    ClusterView(View* pParent = 0, const std::string& type = Native, bool createInitialCluster = true);
    virtual ~ClusterView();

    void insertView(View* pView, const std::string& name = "", int index = 0);
    int getViewCount();
    int getCurrentViewIndex(); /// current view has focus
    void setCurrentViewIndex(int index);
    int getIndexFromView(View* pView);

    void setHandlesHidden(bool hidden = true);
    const int getHandleHeight();

private:
    virtual void removedSubview(View* pView);

//    SubClusterView* addSubCluster(bool horizontalLayout);
//
//    SubClusterView*                         _pSubCluster;
//    ClusterView*                            _pFirstCluster;
//    ClusterView*                            _pSecondCluster;
};


class Cluster : public Widget<ClusterView, Controller, Model>
{
public:
    Cluster(View* pParent = 0) : Widget<ClusterView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
