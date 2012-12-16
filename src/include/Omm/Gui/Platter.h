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

#ifndef Platter_INCLUDED
#define Platter_INCLUDED

#include <map>
#include <vector>

#include "View.h"

namespace Omm {
namespace Gui {


class Label;
class Button;
class ListItemView;
class ClusterView;


class PlatterView : public View
{
    friend class PlatterViewImpl;
    friend class PlatterStackedLayout;
    friend class ClusterView;

public:
    PlatterView(View* pParent = 0, bool createCluster = true);
    virtual ~PlatterView();

    void addView(View* pView, const std::string& name = "", bool show = true);
    void removeView(View* pView);
    int getTabCount();
    int getCurrentTab();
    void setTabBarHidden(bool hidden = true);
    void setCurrentView(View* pView);
    void setCurrentTab(int index);

    ClusterView* getCluster();
    PlatterView* getFirstPlatter();
    PlatterView* getSecondPlatter();

private:
//    ClusterView* addCluster();
    ClusterView* addCluster(bool horizontalLayout);

    ClusterView*                            _pCluster;
    PlatterView*                            _pFirstPlatter;
    PlatterView*                            _pSecondPlatter;
//    std::vector<ClusterView*>               _clusters;
};


class Platter : public Widget<PlatterView, Controller, Model>
{
public:
    Platter(View* pParent = 0) : Widget<PlatterView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
