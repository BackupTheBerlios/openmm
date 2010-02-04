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

#include "jammlight_ctrl_impl.h"

void
SwitchPowerControllerImpl::_ansGetStatus(const bool& ResultStatus)
{
    // begin of your own code
    std::cout << "async request answer _ansGetStatus() returns: " << ResultStatus << std::endl;
    // end of your own code
}


void
SwitchPowerControllerImpl::_changedStatus(const bool& Status)
{
    // begin of your own code
    std::cout << " value of evented state variable Status changed to: " << Status << std::endl;
    // end of your own code
}


JammlightControllerImpl::JammlightControllerImpl(SwitchPowerControllerImpl* pSwitchPowerControllerImpl) :
m_pSwitchPowerCtrlImpl(pSwitchPowerControllerImpl)
{
}

// NOTE: this should be device type independend code -> move it to Controller
// Controller should assign an implementation based on device and service types
// Controller has a map: service type -> implementation
// set m_pService on every action / stateVar query call
void JammlightControllerImpl::deviceAdded(Jamm::DeviceRoot* device)
{
    // begin of your own code
    std::cout << "JammlightControllerImpl::deviceAdded()" << std::endl;
    std::cout << "uuid: " << device->getRootDevice()->getUuid() << std::endl;
    std::cout << "type: " << device->getRootDevice()->getDeviceType() << std::endl;
    // TODO: set a pointer to the implementation of each service in device
    // TODO: and put this code in JammlightController::deviceAdded
    // TODO: make another DeviceRootImplAdapter (or extend the existing one)
    //       derive JammlightControllerImpl from DeviceRootImplAdapter
    
    
    bool status;
    std::cout << "starting sync call of GetStatus()" << std::endl;
    // TODO: set SwitchPowerCtrlImpl::m_pService before starting the action
    //       or better:
    // device->getImpl()->GetStatus(status)
    // or when JammlightControllerImpl is derived from DeviceRootImplAdapter:
    // m_pSwitchPower->GetStatus(status); // aaaaahh ... what about m_pService ...
    m_pSwitchPowerCtrlImpl->GetStatus(status);
    std::cout << "sync call of GetStatus() returns: " << status << std::endl;
    std::cout << "starting async request _reqGetStatus()" << std::endl;
    m_pSwitchPowerCtrlImpl->_reqGetStatus();
    status = m_pSwitchPowerCtrlImpl->_getStatus();
    std::cout << "state variable Status value: " << status << std::endl;
    // end of your own code
}


void JammlightControllerImpl::deviceRemoved(Jamm::DeviceRoot* device)
{
    // begin of your own code
    std::cout << "JammlightControllerImpl::deviceRemoved() device uuid: " << device->getRootDevice()->getUuid() << std::endl;
    // end of your own code
}
