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

//#ifdef __GUI_QT_PLATFORM__
//#include "Qt/QtNavigator.h"
//#endif


namespace Omm {
namespace Gui {


Navigable::Navigable() :
_pNavigator(0)
{

}


Navigator*
Navigable::getNavigator() const
{
    return _pNavigator;
}


Navigator::Navigator()
{
}


Navigator::~Navigator()
{
}


void
Navigator::push(Navigable* pNavigable)
{
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " ...");
//    Omm::Util::Log::instance()->plugin().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString());

    pNavigable->_pNavigator = this;
    if (pNavigable->getWidget()) {
//        Omm::Av::Log::instance()->upnpav().debug("Qt navigator add widget: " + Poco::NumberFormatter::format(pNavigable->getWidget()));
        pushImpl(pNavigable);
    }
    _navigableStack.push(pNavigable);
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator showing widget ...");
    pNavigable->show();
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " finished.");
}


} // namespace Gui
} // namespace Omm
