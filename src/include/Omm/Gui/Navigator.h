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

#ifndef Navigator_INCLUDED
#define Navigator_INCLUDED

#include <stack>
#include "View.h"

namespace Omm {
namespace Gui {


class NavigatorController : public Controller
{
    friend class NavigatorView;
    friend class NavigatorViewImpl;

protected:
    virtual void poppedToRoot() {}
    virtual void poppedToView(View* pView) {}
    virtual void changedSearchText(const std::string& searchText) {}
    virtual void rightButtonPushed() {}
};


class NavigatorView : public View
{
    friend class NavigatorViewImpl;

public:
    NavigatorView(View* pParent = 0);
    virtual ~NavigatorView();

    void push(View* pView, const std::string& label = "");
    void pop(bool keepRootView = true);
    void popToRoot();
    View* getVisibleView();
    int viewCount();
    void showNavigatorBar(bool show = true);
    void showSearchBox(bool show = true);
    void clearSearchText();
    void showRightButton(bool show = true);
    void setRightButtonLabel(const std::string& label);
    void setRightButtonColor(const Color& color);
};


class Navigator : public Widget<NavigatorView, Controller, Model>
{
public:
    Navigator(View* pParent = 0) : Widget<NavigatorView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
