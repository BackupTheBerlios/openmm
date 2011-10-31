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

#ifndef Tab_INCLUDED
#define Tab_INCLUDED

#include "View.h"

namespace Omm {
namespace Gui {


class TabView : public View
{
    friend class TabViewImpl;

public:
    TabView(View* pParent = 0);
    virtual ~TabView();

    void addView(View* pView, const std::string& tabName = "");
    int getTabCount();
    int getCurrentTab();
    void setTabBarHidden(bool hidden = true);
    void setCurrentView(View* pView);
    void setCurrentTab(int index);

private:
    int         _tabCount;
};


class Tab : public Widget<TabView, Controller, Model>
{
public:
    Tab(View* pParent = 0) : Widget<TabView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
