#include <jamm/upnp.h>
#include "DimmableLight.h"
#include "DimmableLightDescriptions.h"


void
DimmableLight::actionHandler(Action* pAction)
{
    // the great action dispatcher
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:Dimming:1") {
        m_pDimmingImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetLoadLevelTarget") {
            Jamm::ui1 newLoadlevelTarget = pAction->getArgument<Jamm::ui1>("newLoadlevelTarget");
            m_pDimmingImpl->SetLoadLevelTarget(newLoadlevelTarget);
        }
        else if (actionName == "GetLoadLevelTarget") {
            Jamm::ui1 retLoadlevelTarget;
            m_pDimmingImpl->GetLoadLevelTarget(retLoadlevelTarget);
            pAction->setArgument<Jamm::ui1>("retLoadlevelTarget", retLoadlevelTarget);
        }
        else if (actionName == "GetLoadLevelStatus") {
            Jamm::ui1 retLoadlevelStatus;
            m_pDimmingImpl->GetLoadLevelStatus(retLoadlevelStatus);
            pAction->setArgument<Jamm::ui1>("retLoadlevelStatus", retLoadlevelStatus);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:SwitchPower:1") {
        m_pSwitchPowerImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetTarget") {
            bool NewTargetValue = pAction->getArgument<bool>("NewTargetValue");
            m_pSwitchPowerImpl->SetTarget(NewTargetValue);
        }
        else if (actionName == "GetTarget") {
            bool RetTargetValue;
            m_pSwitchPowerImpl->GetTarget(RetTargetValue);
            pAction->setArgument<bool>("RetTargetValue", RetTargetValue);
        }
        else if (actionName == "GetStatus") {
            bool ResultStatus;
            m_pSwitchPowerImpl->GetStatus(ResultStatus);
            pAction->setArgument<bool>("ResultStatus", ResultStatus);
        }
    }
}


void
DimmableLight::initStateVars(const std::string& serviceType, Service* pThis)
{
    if (serviceType == "urn:schemas-upnp-org:service:Dimming:1") {
        m_pDimmingImpl->m_pService = pThis;
        m_pDimmingImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:SwitchPower:1") {
        m_pSwitchPowerImpl->m_pService = pThis;
        m_pSwitchPowerImpl->initStateVars();
    }
}


DimmableLight::DimmableLight(SwitchPower* pSwitchPowerImpl, Dimming* pDimmingImpl) :
DeviceRootImplAdapter(),
m_pSwitchPowerImpl(pSwitchPowerImpl), 
m_pDimmingImpl(pDimmingImpl)
{
    m_descriptions["/network-light-desc.xml"] = &DimmableLight::m_deviceDescription;
    m_descriptions["/SwitchPower-scpd.xml"] = &SwitchPower::m_description;
    m_descriptions["/Dimming-scpd.xml"] = &Dimming::m_description;

    Jamm::StringDescriptionReader descriptionReader(m_descriptions, "/network-light-desc.xml");
    m_pDeviceRoot = descriptionReader.deviceRoot();
    m_pDeviceRoot->setImplAdapter(this);
}


void
Dimming::_setLoadLevelTarget(const Jamm::ui1& val)
{
    m_pService->setStateVar<Jamm::ui1>("LoadLevelTarget", val);
}

Jamm::ui1
Dimming::_getLoadLevelTarget()
{
    return m_pService->getStateVar<Jamm::ui1>("LoadLevelTarget");
}

void
Dimming::_setLoadLevelStatus(const Jamm::ui1& val)
{
    m_pService->setStateVar<Jamm::ui1>("LoadLevelStatus", val);
}

Jamm::ui1
Dimming::_getLoadLevelStatus()
{
    return m_pService->getStateVar<Jamm::ui1>("LoadLevelStatus");
}

void
SwitchPower::_setTarget(const bool& val)
{
    m_pService->setStateVar<bool>("Target", val);
}

bool
SwitchPower::_getTarget()
{
    return m_pService->getStateVar<bool>("Target");
}

void
SwitchPower::_setStatus(const bool& val)
{
    m_pService->setStateVar<bool>("Status", val);
}

bool
SwitchPower::_getStatus()
{
    return m_pService->getStateVar<bool>("Status");
}


