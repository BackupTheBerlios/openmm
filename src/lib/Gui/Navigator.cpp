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
NavigatorView::push(View* pView, const std::string& name)
{
    LOG(gui, debug, "navigator push: " + name + " ...");
    static_cast<NavigatorViewImpl*>(_pImpl)->pushView(pView, name);
    _pViewStack.push(pView);
    pView->resize(width(), height());
    LOG(gui, debug, "navigator push: " + name + " finished.");
}


void
NavigatorView::pop()
{
    if (!_pViewStack.empty()) {
        // NOTE: NavigatorView popView() needs to be callable from non-gui thread
        static_cast<NavigatorViewImpl*>(_pImpl)->popView();
        _pViewStack.pop();
    }
}


void
NavigatorView::popToRoot()
{
    LOG(gui, debug, "navigator pop to root, view stack size: " + Poco::NumberFormatter::format(_pViewStack.size()));
    while (_pViewStack.size() > 1) {
        pop();
    }
}


View*
NavigatorView::getVisibleView()
{
    return _pViewStack.top();
}


void
NavigatorView::showSearchBox(bool show)
{
    static_cast<NavigatorViewImpl*>(_pImpl)->showSearchBox(show);
}


} // namespace Gui
} // namespace Omm
