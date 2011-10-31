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

#include "View.h"

namespace Omm {
namespace Gui {


class NavigatorView : public View
{
public:
    NavigatorView(View* pParent = 0);
    virtual ~NavigatorView();

    void push(View* pView, const std::string& name = "");
    View* getVisibleView();

private:
    View*       _pVisibleView;
};


class Navigator : public Widget<NavigatorView, Controller, Model>
{
public:
    Navigator(View* pParent = 0) : Widget<NavigatorView, Controller, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
