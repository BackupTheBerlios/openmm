/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
***************************************************************************/

#include "jammlight_ctrl.h"

void
SwitchPowerController::GetStatus(bool& ResultStatus)
{
    // clone action and set arguments
    Jamm::Action* pAction = m_pService->getAction("GetStatus")->clone();
    // action -> xml
    Jamm::ActionRequestWriter actionWriter;
    // post action message to be send via http-session
    // receive response
    // xml -> action
    // get action out arguments and set arguments out1
}

void
SwitchPowerController::_reqGetStatus()
{
    // TODO: call this method as active method in a seperate thread
    bool ResultStatus;
    GetStatus(ResultStatus);
    _ansGetStatus(ResultStatus);
}


bool
SwitchPowerController::_getStatus()
{
    // TODO: if evented, return StateVar::getValue()
    
    // TODO: else start a QueryStateVarRequest
}


DimmableLightController::DimmableLightController(Jamm::Device* pDevice, SwitchPowerController* pSwitchPowerController) :
m_pDevice(pDevice),
m_pSwitchPowerController(pSwitchPowerController)
{
    m_pSwitchPowerController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:SwitchPower:1");
    
    // TODO:
    // connect to controlURL and eventURL
    // subscribe to services
//     pDevice->startSessions();
}
