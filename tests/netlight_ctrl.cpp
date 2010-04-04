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

#include "netlight_ctrl.h"

void
SwitchPowerController::GetStatus(bool& ResultStatus)
{
    // TOP
    // clone action
    Omm::Action* pAction = m_pService->getAction("GetStatus")->clone();
    // set in arguments of action from method args
    // ...
    // BOTTOM
    // post action message via http-session
    // sendAction() blocks until answer is received and result is put into *pAction
    m_pService->sendAction(pAction);
    // get out arguments and assign to method args
    ResultStatus = pAction->getArgument<bool>("ResultStatus");
    // ...
}


void
SwitchPowerController::_reqGetStatus()
{
    // TOP
    // clone action
    Omm::Action* pAction = m_pService->getAction("GetStatus")->clone();
    // set in arguments of action from method args
    // ...
    // BOTTOM
    // post action message in a new thread
    Omm::ActionThread<SwitchPowerController> t(this, &SwitchPowerController::_threadGetStatus, pAction);
    t.start();
}


void
SwitchPowerController::_threadGetStatus(Omm::Action* pAction)
{
    m_pService->sendAction(pAction);
    // get out arguments and assign to allocated variables
    bool ResultStatus = pAction->getArgument<bool>("ResultStatus");
    // ...
    // return all args in answer callback
    _ansGetStatus(ResultStatus);
}


void
DimmableLightController::eventHandler(Omm::StateVar* pStateVar)
{
    // react on event messages, dispatch to corresponding _changeStateVar() method
    // (*i) should point to the correct StateVar in the DeviceRoot object
    // call _changeStateVar() method
    if (pStateVar->getName() == "Status") {
        bool val;
        pStateVar->getValue(val);
        m_pSwitchPowerController->_changedStatus(val);
    }
    else {
    }
}


DimmableLightController::DimmableLightController(Omm::Device* pDevice, SwitchPowerController* pSwitchPowerController) :
ControllerImplAdapter(pDevice),
m_pDevice(pDevice),
m_pSwitchPowerController(pSwitchPowerController)
{
    m_pSwitchPowerController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:SwitchPower:1");
    
    init();
}


bool
SwitchPowerController::_getStatus()
{
    return m_pService->getStateVar<bool>("Status");
}
