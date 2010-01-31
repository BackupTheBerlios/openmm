#ifndef DIMMABLELIGHT_IMPLEMENTATION_H
#define DIMMABLELIGHT_IMPLEMENTATION_H

#include <jamm/upnp.h>
#include "DimmableLight.h"

class DimmingImplementation : public Dimming
{
private:
    virtual void SetLoadLevelTarget(const Jamm::ui1& newLoadlevelTarget);
    virtual void GetLoadLevelTarget(Jamm::ui1& retLoadlevelTarget);
    virtual void GetLoadLevelStatus(Jamm::ui1& retLoadlevelStatus);

    virtual void initStateVars();
};

class SwitchPowerImplementation : public SwitchPower
{
private:
    virtual void SetTarget(const bool& NewTargetValue);
    virtual void GetTarget(bool& RetTargetValue);
    virtual void GetStatus(bool& ResultStatus);

    virtual void initStateVars();
};

#endif

