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

#include <Poco/NumberFormatter.h>

#include "Gui/ScrollArea.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"
#include "ScrollAreaImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


ScrollAreaView::ScrollAreaView(View* pParent) :
View(pParent, false)
{
    _pImpl = new ScrollAreaViewImpl(this);
}


View*
ScrollAreaView::getAreaView()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getAreaView();
}


int
ScrollAreaView::getViewportWidth()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getViewportWidth();
}


int
ScrollAreaView::getViewportHeight()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getViewportHeight();
}


int
ScrollAreaView::getXOffset()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getXOffset();
}


int
ScrollAreaView::getYOffset()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getYOffset();
}


void
ScrollAreaView::scrollContentsTo(int x, int y)
{
    LOG(gui, debug, "scroll area scroll content to: " + Poco::NumberFormatter::format(x) + ", " + Poco::NumberFormatter::format(y));

    static_cast<ScrollAreaViewImpl*>(_pImpl)->scrollContentsTo(x, y);
}


void
ScrollAreaView::showScrollBars(bool show)
{
    static_cast<ScrollAreaViewImpl*>(_pImpl)->showScrollBars(show);
}


void
ScrollAreaView::setAreaResizable(bool resize)
{
    static_cast<ScrollAreaViewImpl*>(_pImpl)->setAreaResizable(resize);
}

} // namespace Gui
} // namespace Omm
