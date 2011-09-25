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

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ScrollAreaImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/ScrollAreaImpl.h"
#endif


namespace Omm {
namespace Gui {


ScrollAreaView::ScrollAreaView(View* pParent) :
View(pParent, false)
{
    _pImpl = new ScrollAreaViewImpl(this);
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
ScrollAreaView::getScrollAreaWidth()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getScrollAreaWidth();
}


int
ScrollAreaView::getScrollAreaHeight()
{
    return static_cast<ScrollAreaViewImpl*>(_pImpl)->getScrollAreaHeight();
}


void
ScrollAreaView::resizeScrollArea(int width, int height)
{
    static_cast<ScrollAreaViewImpl*>(_pImpl)->resizeScrollArea(width, height);
}


void
ScrollAreaView::addSubview(View* pView)
{
    static_cast<ScrollAreaViewImpl*>(_pImpl)->addSubview(pView);
}


} // namespace Gui
} // namespace Omm
