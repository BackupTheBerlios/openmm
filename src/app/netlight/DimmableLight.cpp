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

/***************************************************************************|
|                               WARNING                                     |
|        This file is generated by the omm stub generator ommgen.           |
|       Don't edit, it will be overriden at the next run of ommgen.         |
****************************************************************************/

#include <Omm/Upnp.h>
#include <Omm/UpnpInternal.h>

#include "DimmableLight.h"
#include "DimmableLightDescriptions.h"


void
DimmableLight::actionHandler(Action* pAction)
{
    // the great action dispatcher
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:Dimming:1") {
        _pDimmingImpl->_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetLoadLevelTarget") {
            Omm::ui1 newLoadlevelTarget = pAction->getArgument<Omm::ui1>("newLoadlevelTarget");
            _pDimmingImpl->SetLoadLevelTarget(newLoadlevelTarget);
        }
        else if (actionName == "GetLoadLevelTarget") {
            Omm::ui1 retLoadlevelTarget;
            _pDimmingImpl->GetLoadLevelTarget(retLoadlevelTarget);
            pAction->setArgument<Omm::ui1>("retLoadlevelTarget", retLoadlevelTarget);
        }
        else if (actionName == "GetLoadLevelStatus") {
            Omm::ui1 retLoadlevelStatus;
            _pDimmingImpl->GetLoadLevelStatus(retLoadlevelStatus);
            pAction->setArgument<Omm::ui1>("retLoadlevelStatus", retLoadlevelStatus);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:SwitchPower:1") {
        _pSwitchPowerImpl->_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetTarget") {
            bool NewTargetValue = pAction->getArgument<bool>("NewTargetValue");
            _pSwitchPowerImpl->SetTarget(NewTargetValue);
        }
        else if (actionName == "GetTarget") {
            bool RetTargetValue;
            _pSwitchPowerImpl->GetTarget(RetTargetValue);
            pAction->setArgument<bool>("RetTargetValue", RetTargetValue);
        }
        else if (actionName == "GetStatus") {
            bool ResultStatus;
            _pSwitchPowerImpl->GetStatus(ResultStatus);
            pAction->setArgument<bool>("ResultStatus", ResultStatus);
        }
    }
}


void
DimmableLight::initStateVars(Service* pThis)
{
    std::string serviceType = pThis->getServiceType();
    if (serviceType == "urn:schemas-upnp-org:service:Dimming:1") {
        _pDimmingImpl->_pService = pThis;
        _pDimmingImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:SwitchPower:1") {
        _pSwitchPowerImpl->_pService = pThis;
        _pSwitchPowerImpl->initStateVars();
    }
}


DimmableLight::DimmableLight(SwitchPower* pSwitchPowerImpl, Dimming* pDimmingImpl) :
DevDeviceCode(),
_pSwitchPowerImpl(pSwitchPowerImpl), 
_pDimmingImpl(pDimmingImpl)
{
    // TODO: rewrite to new Device / DeviceContainer stuff

//    _descriptions["/network-light-desc.xml"] = &DimmableLight::_deviceDescription;
//    _descriptions["/SwitchPower-scpd.xml"] = &SwitchPower::_description;
//    _descriptions["/Dimming-scpd.xml"] = &Dimming::_description;
//
//    Omm::StringDescriptionReader descriptionReader(_descriptions);
//    descriptionReader.getDeviceDescription("/network-light-desc.xml");
//    _pDeviceContainer = descriptionReader.deviceContainer();
//    _pDeviceContainer->setImplAdapter(this);
}


void
Dimming::_setLoadLevelTarget(const Omm::ui1& val)
{
    _pService->setStateVar<Omm::ui1>("LoadLevelTarget", val);
}

Omm::ui1
Dimming::_getLoadLevelTarget()
{
    return _pService->getStateVar<Omm::ui1>("LoadLevelTarget");
}

void
Dimming::_setLoadLevelStatus(const Omm::ui1& val)
{
    _pService->setStateVar<Omm::ui1>("LoadLevelStatus", val);
}

Omm::ui1
Dimming::_getLoadLevelStatus()
{
    return _pService->getStateVar<Omm::ui1>("LoadLevelStatus");
}

void
SwitchPower::_setTarget(const bool& val)
{
    _pService->setStateVar<bool>("Target", val);
}

bool
SwitchPower::_getTarget()
{
    return _pService->getStateVar<bool>("Target");
}

void
SwitchPower::_setStatus(const bool& val)
{
    _pService->setStateVar<bool>("Status", val);
}

bool
SwitchPower::_getStatus()
{
    return _pService->getStateVar<bool>("Status");
}


