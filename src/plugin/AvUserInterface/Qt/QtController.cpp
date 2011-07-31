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
#include "QtDeviceGroup.h"
#include "QtMediaRendererGroup.h"
#include "QtApplication.h"


QtController::QtController(QtApplication* pQtApplication) :
_pQtApplication(pQtApplication)
{
    createDeviceGroup(Omm::Av::DeviceType::MEDIA_SERVER_1);
    createDeviceGroup(Omm::Av::DeviceType::MEDIA_RENDERER_1);
}


Omm::DeviceGroup*
QtController::createDeviceGroup(const std::string deviceType)
{
    // TODO: device group can be loaded as a plugin and is shown when a device of that type pops up
    QtDeviceGroup* pQtDeviceGroup;

    if (deviceType == Omm::Av::DeviceType::MEDIA_SERVER_1) {
        pQtDeviceGroup = new QtDeviceGroup(deviceType, "Media");
    }
    else if (deviceType == Omm::Av::DeviceType::MEDIA_RENDERER_1) {
        pQtDeviceGroup = new QtMediaRendererGroup;
    }
    
    addDeviceGroup(pQtDeviceGroup);
    addTab(pQtDeviceGroup->getDeviceGroupWidget(), pQtDeviceGroup->shortName().c_str());
}


void
QtController::addPanel(QToolBar* pPanel)
{
    _pQtApplication->addToolBar(pPanel);
}
