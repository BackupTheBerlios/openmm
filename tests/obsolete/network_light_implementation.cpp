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
#include "network_light_implementation.h"


void
SwitchPowerImplementation::SetTarget(const bool& NewTargetValue)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::SetTarget()" << std::endl;
    _setTarget(NewTargetValue);
    _setStatus(NewTargetValue);
    // end of your own code
}


void
SwitchPowerImplementation::GetTarget(bool& RetTargetValue)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::GetTarget()" << std::endl;
    RetTargetValue = _getTarget();
    // end of your own code
}


void
SwitchPowerImplementation::GetStatus(bool& ResultStatus)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::GetStatus()" << std::endl;
    ResultStatus = _getStatus();
    // end of your own code
}


void
DimmingImplementation::SetLoadLevelTarget(const Jamm::i1& newLoadLevelTarget)
{
    // begin of your own code
    std::cerr << "DimmingImplementation::SetLoadLevelTarget()" << std::endl;
    _setLoadLevelTarget(newLoadLevelTarget);
    _setLoadLevelStatus(newLoadLevelTarget);
    // end of your own code
}


void
DimmingImplementation::GetLoadLevelTarget(Jamm::i1& retLoadLevelTarget)
{
    // begin of your own code
    std::cerr << "DimmingImplementation::GetLoadLevelTarget()" << std::endl;
    retLoadLevelTarget = _getLoadLevelTarget();
    // end of your own code
}


void
DimmingImplementation::GetLoadLevelStatus(Jamm::i1& retLoadLevelStatus)
{
    // begin of your own code
    std::cerr << "DimmingImplementation::GetLoadLevelStatus()" << std::endl;
    retLoadLevelStatus = _getLoadLevelStatus();
    // end of your own code
}
