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


namespace Omm {
namespace Gui {


NavigatorView::NavigatorView(View* pParent) :
View(pParent, false),
_pVisibleView(0)
{
    Omm::Gui::Log::instance()->gui().debug("navigator view ctor.");
    _pImpl = new NavigatorViewImpl(this);
}


NavigatorView::~NavigatorView()
{
}


void
NavigatorView::push(View* pView, const std::string& name)
{
    Log::instance()->gui().debug("navigator push: " + name + " ...");
    static_cast<NavigatorViewImpl*>(_pImpl)->pushView(pView, name);
    _pVisibleView = pView;
//    pView->resize(width(), height());
//    pView->show();
    Log::instance()->gui().debug("navigator push: " + name + " finished.");
}


View*
NavigatorView::getVisibleView()
{
    return _pVisibleView;
}


} // namespace Gui
} // namespace Omm
