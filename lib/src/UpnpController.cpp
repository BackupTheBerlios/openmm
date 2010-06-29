/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#include "UpnpController.h"

using namespace Omm;
using namespace Omm::Av;


UpnpController::~UpnpController()
{
//     std::clog << "UpnpController::~UpnpController()";
}


// void
// UpnpController::start()
// {
//     init();
// //     JSignal::connectNodes(&_pollPositionInfoTimer.fire, this);
//     
//     // poll for current position every second
// //     _pollPositionInfoTimer.startTimer(1000);
// }



void
UpnpController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnp().information("device added, friendly name: " + pDevice->getFriendlyName() + " ,uuid: " + pDevice->getUuid());
//     std::clog << "UpnpController::deviceAdded()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    _pUserInterface->beginAddDevice(_devices.position(pDevice->getUuid()));
    _devices.append(pDevice->getUuid(), pDevice);
    _pUserInterface->endAddDevice(_devices.position(pDevice->getUuid()));
}


void
UpnpController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnp().information("device removed, friendly name: " + pDevice->getFriendlyName() + " ,uuid: " + pDevice->getUuid());
//     std::clog << "UpnpController::deviceRemoved()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    _pUserInterface->beginRemoveDevice(_devices.position(pDevice->getUuid()));
    _devices.remove(pDevice);
    _pUserInterface->endRemoveDevice(_devices.position(pDevice->getUuid()));
}



