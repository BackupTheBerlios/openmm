#ifndef DIMMABLELIGHT_H
#define DIMMABLELIGHT_H

#include <jamm/upnp.h>

using Jamm::DeviceRootImplAdapter;
using Jamm::Service;
using Jamm::Action;

class DimmableLight;

class Dimming
{
    friend class DimmableLight;

protected:
    virtual void SetLoadLevelTarget(const Jamm::ui1& newLoadlevelTarget) = 0;
    virtual void GetLoadLevelTarget(Jamm::ui1& retLoadlevelTarget) = 0;
    virtual void GetLoadLevelStatus(Jamm::ui1& retLoadlevelStatus) = 0;

    virtual void initStateVars() = 0;

    void _setLoadLevelTarget(const Jamm::ui1& val);
    Jamm::ui1 _getLoadLevelTarget();
    void _setLoadLevelStatus(const Jamm::ui1& val);
    Jamm::ui1 _getLoadLevelStatus();

private:
    static std::string  m_description;
    Service* m_pService;
};


class SwitchPower
{
    friend class DimmableLight;

protected:
    virtual void SetTarget(const bool& NewTargetValue) = 0;
    virtual void GetTarget(bool& RetTargetValue) = 0;
    virtual void GetStatus(bool& ResultStatus) = 0;

    virtual void initStateVars() = 0;

    void _setTarget(const bool& val);
    bool _getTarget();
    void _setStatus(const bool& val);
    bool _getStatus();

private:
    static std::string  m_description;
    Service* m_pService;
};


class DimmableLight : public DeviceRootImplAdapter
{
public:
    DimmableLight(SwitchPower* pSwitchPowerImpl, Dimming* pDimmingImpl);


private:
    virtual void actionHandler(Action* action);
    virtual void initStateVars(const std::string& serviceType, Service* pThis);

    static std::string m_deviceDescription;
    SwitchPower* m_pSwitchPowerImpl;
    Dimming* m_pDimmingImpl;
};

#endif

