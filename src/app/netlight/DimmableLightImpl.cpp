/***************************************************************************|
|                               WARNING                                     |
|        This file is generated by the omm stub generator ommgen.           |
|       Don't edit, it will be overriden at the next run of ommgen.         |
****************************************************************************/

#include <iostream>

#include <Omm/Upnp.h>
#include "DimmableLightImpl.h"

void
DimmingImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
DimmingImplementation::SetLoadLevelTarget(const Omm::ui1& newLoadlevelTarget)
{
// begin of your own code

// end of your own code
}


void
DimmingImplementation::GetLoadLevelTarget(Omm::ui1& retLoadlevelTarget)
{
// begin of your own code

// end of your own code
}


void
DimmingImplementation::GetLoadLevelStatus(Omm::ui1& retLoadlevelStatus)
{
// begin of your own code

// end of your own code
}


void
SwitchPowerImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
SwitchPowerImplementation::SetTarget(const bool& NewTargetValue)
{
// begin of your own code

// end of your own code
}


void
SwitchPowerImplementation::GetTarget(bool& RetTargetValue)
{
// begin of your own code

// end of your own code
}


void
SwitchPowerImplementation::GetStatus(bool& ResultStatus)
{
// begin of your own code
    std::cerr << "SwitchPowerImplementation::GetStatus() sleeping 1 sec" << std::endl;
    Poco::Thread::sleep(1000);
    std::cerr << "SwitchPowerImplementation::GetStatus() sleeping 1 sec" << std::endl;
    Poco::Thread::sleep(1000);
    std::cerr << "SwitchPowerImplementation::GetStatus() sleeping 1 sec" << std::endl;
    Poco::Thread::sleep(1000);
    std::cerr << "SwitchPowerImplementation::GetStatus() finished" << std::endl;
// end of your own code
}


