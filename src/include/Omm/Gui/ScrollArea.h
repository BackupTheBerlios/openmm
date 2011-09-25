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

protected:
    virtual void scrolled(int xOffset, int yOffset) {}

    // TODO: if possible, put this in ViewController
    virtual void resized(int width, int height) {}
    virtual void presented() {}
};


class ScrollAreaView : public View
{
public:
    ScrollAreaView(View* pParent = 0);

    int getXOffset();
    int getYOffset();
    int getViewportWidth();
    int getViewportHeight();
    int getScrollAreaWidth();
    int getScrollAreaHeight();
    void resizeScrollArea(int width, int height);

    // TODO: put this in View
    virtual void addSubview(View* pView);
};


class ScrollArea : public Widget<ScrollAreaView, ScrollAreaController, Model>
{
public:
    ScrollArea(View* pParent = 0) : Widget<ScrollAreaView, ScrollAreaController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif

