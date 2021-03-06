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

#include "Gui/Navigator.h"
#include "Gui/GuiLogger.h"
#include "NavigatorImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


NavigatorView::NavigatorView(View* pParent) :
View(pParent, false)
{
    LOG(gui, debug, "navigator view ctor.");
    _pImpl = new NavigatorViewImpl(this);
}


NavigatorView::~NavigatorView()
{
}


void
NavigatorView::push(View* pView, const std::string& label)
{
    LOG(gui, debug, "navigator push: " + pView->getName() + " ...");
    static_cast<NavigatorViewImpl*>(_pImpl)->pushView(pView, label);
    // NOTE: is resize() needed or should navigator implementation care of that (also when "drag resize")?
    pView->resize(width(), height());
    LOG(gui, debug, "navigator push: " + pView->getName() + " finished.");
}


void
NavigatorView::pop(bool keepRootView)
{
    // NOTE: NavigatorView popView() needs to be callable from non-gui thread
    static_cast<NavigatorViewImpl*>(_pImpl)->popView(keepRootView);
}


void
NavigatorView::popToRoot()
{
    static_cast<NavigatorViewImpl*>(_pImpl)->popToRootView();
}


View*
NavigatorView::getVisibleView()
{
    return static_cast<NavigatorViewImpl*>(_pImpl)->getVisibleView();
}


int
NavigatorView::viewCount()
{
    return static_cast<NavigatorViewImpl*>(_pImpl)->viewCount();
}


void
NavigatorView::showNavigatorBar(bool show)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->showNavigatorBar(show);
}


void
NavigatorView::showSearchBox(bool show)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->showSearchBox(show);
}


void
NavigatorView::clearSearchText()
{
    static_cast<NavigatorViewImpl*>(_pImpl)->clearSearchText();
}


void
NavigatorView::showRightButton(bool show)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->showRightButton(show);
}


void
NavigatorView::setRightButtonLabel(const std::string& label)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->setRightButtonLabel(label);
}


void
NavigatorView::setRightButtonColor(const Color& color)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->setRightButtonColor(color);
}


} // namespace Gui
} // namespace Omm
