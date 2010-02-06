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
    // post action message to be send via http-session
    // sendAction() blocks until answer is received and result is put into *pAction
    m_pService->sendAction(pAction);
    // get action out arguments and set arguments out1
    ResultStatus = pAction->getArgument<bool>("ResultStatus");
}

void
SwitchPowerController::_reqGetStatus()
{
    bool ResultStatus;
    GetStatus(ResultStatus);
    _ansGetStatus(ResultStatus);
}


void
DimmableLightController::eventHandler(Jamm::Container<Jamm::StateVar>& stateVars)
{
    // react on event messages, dispatch to corresponding _changeStateVar() method
    for (Jamm::Container<Jamm::StateVar>::Iterator i = stateVars.begin(); i != stateVars.end(); ++i) {
        // (*i) should point to the correct StateVar in the DeviceRoot object
        // call _changeStateVar() method
        if ((*i)->getName() == "Status") {
            bool Status;
            (*i)->getValue(Status);
            m_pSwitchPowerController->_changedStatus(Status);
        }
    }
}


DimmableLightController::DimmableLightController(Jamm::Device* pDevice, SwitchPowerController* pSwitchPowerController) :
m_pDevice(pDevice),
m_pSwitchPowerController(pSwitchPowerController)
{
    m_pSwitchPowerController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:SwitchPower:1");
    
    for (Jamm::Device::ServiceIterator i = m_pDevice->beginService(); i != m_pDevice->endService(); ++i) {
        (*i)->initClient();
    }
}


bool
SwitchPowerController::_getStatus()
{
    return m_pService->getStateVar<bool>("Status");
}
