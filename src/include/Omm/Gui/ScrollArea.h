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

#ifndef ScrollArea_INCLUDED
#define ScrollArea_INCLUDED

#include "View.h"

namespace Omm {
namespace Gui {


class ScrollAreaController : public Controller
{
    friend class ScrollAreaViewImpl;
    friend class ScrollAreaSignalProxy;

protected:
    virtual void scrolled(int xOffset, int yOffset) {}
};


class ScrollAreaView : public View
{
public:
    ScrollAreaView(View* pParent = 0);

    View* getAreaView();
    int getViewportWidth();
    int getViewportHeight();
    int getXOffset();
    int getYOffset();
    void scrollContentsTo(int x, int y);
    void showScrollBars(bool show = true);
    void setAreaResizable(bool resize = true);
};


class ScrollArea : public Widget<ScrollAreaView, ScrollAreaController, Model>
{
public:
    ScrollArea(View* pParent = 0) : Widget<ScrollAreaView, ScrollAreaController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif

