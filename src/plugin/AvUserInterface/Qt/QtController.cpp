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

#include <Omm/UpnpAv.h>
#include <Omm/UpnpAvController.h>

#include "QtController.h"
#include "QtMediaServerGroup.h"
#include "QtMediaRendererGroup.h"
#include "QtApplication.h"
#include "QtWidget.h"


QtController::QtController(QtApplication* pQtApplication) :
_pQtApplication(pQtApplication)
{
//    setMovable(true);
    
    registerDeviceGroup(new QtMediaServerGroup);
    registerDeviceGroup(new QtMediaRendererGroup);
}


void
QtController::showDeviceGroup(Omm::DeviceGroup* pDeviceGroup)
{
    Omm::Log::instance()->upnp().debug("Qt controller show device group: " + pDeviceGroup->getDeviceType());
    
    QtWidget* pDeviceGroupWidget = static_cast<QtWidget*>(pDeviceGroup->getDeviceGroupWidget());
    if (pDeviceGroupWidget) {
        addTab(pDeviceGroupWidget, pDeviceGroup->shortName().c_str());
    }
    else {
        Omm::Log::instance()->upnp().error("Qt controller failed to show device group, no widget available: " + pDeviceGroup->getDeviceType());
    }
}


void
QtController::addPanel(QToolBar* pPanel)
{
    _pQtApplication->addToolBar(pPanel);
}
