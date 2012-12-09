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

#include "View.h"

namespace Omm {
namespace Gui {


class Button;

class PlatterView : public View
{
    friend class PlatterViewImpl;
    friend class PlatterStackedLayout;

public:
    PlatterView(View* pParent = 0);
    virtual ~PlatterView();

    void addView(View* pView, const std::string& name = "", bool show = true);
    int getTabCount();
    int getCurrentTab();
    void setTabBarHidden(bool hidden = true);
    void setCurrentView(View* pView);
    void setCurrentTab(int index);

    virtual SubviewIterator beginSubview();
    virtual SubviewIterator endSubview();

private:
    int                         _barHeight;
    int                         _handleWidth;
    std::map<View*, Button*>    _handles;
    std::vector<View*>          _views;
    int                         _currentView;
    bool                        _tabBarHidden;
};


class Platter : public Widget<PlatterView, Controller, Model>
{
public:
    Platter(View* pParent = 0) : Widget<PlatterView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
