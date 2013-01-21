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

#include <Omm/UpnpAvLogger.h>
#include <Omm/UpnpAvCtlServer.h>

#include "QtMediaContainer.h"
#include "QtNavigator.h"
#include "QtMediaServer.h"


QtMediaContainer::QtMediaContainer()
{
}


QtMediaContainer::~QtMediaContainer()
{

}


QString
QtMediaContainer::getBrowserTitle()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt standard media object get widget browser title");

    if (_pObject) {
        return QString::fromStdString(_pObject->getTitle());
    }
    else {
        LOGNS(Omm::Av, upnpav, error, "Qt standard media object failed to get object title (ignoring)");
    }
}


void
QtMediaContainer::show()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt standard media object show");

    _pServerWidget->setRootIndex(_modelIndex);
}
