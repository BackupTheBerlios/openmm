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

#include "Gui/Tab.h"
#include "Gui/GuiLogger.h"
#include "TabImpl.h"


namespace Omm {
namespace Gui {


TabView::TabView(View* pParent) :
View(pParent, false)
{
    Omm::Gui::Log::instance()->gui().debug("tab view ctor.");
    _pImpl = new TabViewImpl(this);
}


TabView::~TabView()
{
}


void
TabView::addView(View* pView, const std::string& tabName)
{
    static_cast<TabViewImpl*>(_pImpl)->addView(pView, tabName);
}


void
TabView::setTabBarHidden(bool hidden)
{
    static_cast<TabViewImpl*>(_pImpl)->setTabBarHidden(hidden);
}


} // namespace Gui
} // namespace Omm
