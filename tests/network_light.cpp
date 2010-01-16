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

#include <jamm/upnp.h>
#include "network_light.h"

void
SwitchPower::_setTarget(const bool& target)
{
    m_pService->setStateVar<bool>("Target", target);
}


bool
SwitchPower::_getTarget()
{
    return m_pService->getStateVar<bool>("Target");
}


void
SwitchPower::_setStatus(const bool& status)
{
    m_pService->setStateVar<bool>("Status", status);
}


bool
SwitchPower::_getStatus()
{
    return m_pService->getStateVar<bool>("Status");
}


void
Dimming::_setLoadLevelTarget(const Jamm::i1& target)
{
    m_pService->setStateVar<Jamm::i1>("LoadLevelTarget", target);
}


Jamm::i1
Dimming::_getLoadLevelTarget()
{
    return m_pService->getStateVar<Jamm::i1>("LoadLevelTarget");
}


void Dimming::_setLoadLevelStatus(const Jamm::i1& status)
{
    m_pService->setStateVar<Jamm::i1>("LoadLevelStatus", status);
}


Jamm::i1
Dimming::_getLoadLevelStatus()
{
    return m_pService->getStateVar<Jamm::i1>("LoadLevelStatus");
}


NetworkLight::NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl)
{
    // TODO: stub generator should transform device description into a string.
    //       and add code to set Uuid, friendlyName etc. from application config.
    //       Don't read the description from a file in this place.
    Jamm::DescriptionReader descriptionReader(URI("file:/home/jb/devel/cc/jamm/tests/"), "xml/network-light-desc.xml");
    m_pDeviceRoot = descriptionReader.deviceRoot();
    m_pSwitchPowerImpl = switchPowerImpl;
    // Service implementation needs a pointer to Service to access StateVariables
    m_pSwitchPowerImpl->m_pService = m_pDeviceRoot->getDevice("23b0189c-549f-11dc-a7c7-001641597c49")->getService("urn:schemas-upnp-org:service:SwitchPower:1");
    
    m_pDimmingImpl = dimmingImpl;
    m_pDimmingImpl->m_pService = m_pDeviceRoot->getDevice("23b0189c-549f-11dc-a7c7-001641597c49")->getService("urn:schemas-upnp-org:service:Dimming:1");
    
    m_pDeviceRoot->registerActionHandler(Observer<NetworkLight, Action>(*this, &NetworkLight::actionHandler));
    
    m_pDeviceRoot->init();
    m_pDeviceRoot->startHttp();
    m_pDeviceRoot->startSsdp();
}


NetworkLight::~NetworkLight()
{
    delete m_pDeviceRoot;
}


void
NetworkLight::actionHandler(Action* pAction)
{
    std::cerr << "NetworkLight::actionHandler() receives Action: " << pAction->getName() 
        << " (Service: " <<  pAction->getService()->getServiceType() << ")"  << std::endl;
    
    // dispatch all SwitchPower Actions
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:SwitchPower:1") {
        std::string actionName = pAction->getName();
        if (actionName == "SetTarget") {
            bool inArg1;
            inArg1 = pAction->getArgument<bool>("NewTargetValue");
            m_pSwitchPowerImpl->SetTarget(inArg1);
        }
        else if (actionName == "GetTarget") {
            bool outArg1;
            m_pSwitchPowerImpl->GetTarget(outArg1);
            pAction->setArgument<bool>("RetTargetValue", outArg1);
        }
        else if (actionName == "GetStatus") {
            bool outArg1;
            m_pSwitchPowerImpl->GetStatus(outArg1);
            pAction->setArgument<bool>("ResultStatus", outArg1);
        }
    }
    
    // dispatch all Dimming Actions
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:Dimming:1") {
        std::string actionName = pAction->getName();
        if (actionName == "SetLoadLevelTarget") {
            Jamm::i1 inArg1;
            inArg1 = pAction->getArgument<Jamm::i1>("newLoadlevelTarget");
            m_pDimmingImpl->SetLoadLevelTarget(inArg1);
        }
        else if (actionName == "GetLoadLevelTarget") {
            Jamm::i1 outArg1;
            m_pDimmingImpl->GetLoadLevelTarget(outArg1);
            pAction->setArgument<Jamm::i1>("retLoadlevelTarget", outArg1);
        }
        else if (actionName == "GetLoadLevelStatus") {
            Jamm::i1 outArg1;
            m_pDimmingImpl->GetLoadLevelStatus(outArg1);
            pAction->setArgument<Jamm::i1>("retLoadlevelStatus", outArg1);
        }
    }
}
