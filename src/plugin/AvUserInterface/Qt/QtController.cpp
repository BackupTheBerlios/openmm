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
#include "QtDeviceGroupModel.h"
#include "QtNavigator.h"


QtController::QtController()
{
    QtDeviceGroupModel* pQtMediaServerGroupModel = new QtDeviceGroupModel(Omm::Av::DeviceType::MEDIA_SERVER_1, "Media");
    QtDeviceGroup* pQtMediaServerGroup = new QtDeviceGroup(pQtMediaServerGroupModel);
    addDeviceGroup(pQtMediaServerGroupModel);
    QtNavigator* pQtMediaServerNavigator = new QtNavigator;
    pQtMediaServerNavigator->push(pQtMediaServerGroup);
    addTab(pQtMediaServerNavigator, pQtMediaServerGroupModel->shortName().c_str());

    QtDeviceGroupModel* pQtMediaRendererGroupModel = new QtDeviceGroupModel(Omm::Av::DeviceType::MEDIA_RENDERER_1, "Player");
    QtDeviceGroup* pQtMediaRendererGroup = new QtDeviceGroup(pQtMediaRendererGroupModel);
    addDeviceGroup(pQtMediaRendererGroupModel);
    addTab(pQtMediaRendererGroup, pQtMediaRendererGroupModel->shortName().c_str());
}
