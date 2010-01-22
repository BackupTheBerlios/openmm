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
#include "network_light_descriptions.h"


void
NetworkLight::start()
{
    // TODO: write modified device description to m_description
    // TODO: maybe, write newly assigned uuids as keys to DeviceRoot::m_devices
    //       -> Container needs an append(const Entity&)
    m_pDeviceRoot->init();
    m_pDeviceRoot->startHttp();
    m_pDeviceRoot->startSsdp();
}


void
NetworkLight::stop()
{
    m_pDeviceRoot->stopSsdp();
    m_pDeviceRoot->stopHttp();
}


NetworkLight::~NetworkLight()
{
    delete m_pDeviceRoot;
}


void
NetworkLight::setFriendlyName(const std::string& friendlyName)
{
    // set Property friendlyName of **Device**
}


// getter/setter for each StateVar (which is not an A_ARG ?!)
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


NetworkLight::NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl) :
m_pSwitchPowerImpl(switchPowerImpl),
m_pDimmingImpl(dimmingImpl)
{
    // TODO: stub generator should add code to set Uuid, friendlyName etc. from application config.
    
    // register device description
    m_descriptions["network-light-desc.xml"] = &NetworkLight::m_description;
    // for each service type register description
    m_descriptions["/SwitchPower-scpd.xml"] = &SwitchPower::m_description;
    m_descriptions["/Dimming-scpd.xml"] = &Dimming::m_description;
    
    // read in descriptions and build DeviceRoot
    Jamm::StringDescriptionReader descriptionReader(m_descriptions, "network-light-desc.xml");
    m_pDeviceRoot = descriptionReader.deviceRoot();
    
    // register the great action dispatcher
    m_pDeviceRoot->registerActionHandler(Observer<NetworkLight, Action>(*this, &NetworkLight::actionHandler));
}


void
NetworkLight::actionHandler(Action* pAction)
{
    std::cerr << "NetworkLight::actionHandler() receives Action: " << pAction->getName() 
        << " (Service: " <<  pAction->getService()->getServiceType() << ")"  << std::endl;
    
    // dispatch all SwitchPower Actions
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:SwitchPower:1") {
        // TODO: may do some locking here (all SwitchPower Actions set m_pService)
        m_pSwitchPowerImpl->m_pService = pAction->getService();
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
        m_pDimmingImpl->m_pService = pAction->getService();
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

