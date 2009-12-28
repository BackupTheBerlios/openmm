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

#include "Poco/Types.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "jamm/upnp.h"
#include "network_light.h"
#include "network_light_implementation.h"

using Jamm::Device;
using Jamm::Service;
using Jamm::DeviceRoot;
using Poco::UInt8;
using Poco::StreamCopier;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


void
SwitchPowerImplementation::SetTarget(bool NewTargetValue)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::SetTarget() argument NewTargetValue: " << NewTargetValue << std::endl;
    _setTarget(NewTargetValue);
    // end of your own code
}


void
SwitchPowerImplementation::GetTarget(bool& RetTargetValue)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::GetTarget() argument RetTargetValue: " << RetTargetValue << std::endl;
    RetTargetValue = _getTarget();
    // end of your own code
}


void
SwitchPowerImplementation::ResultStatus(bool& ResultStatus)
{
    // begin of your own code
    std::cerr << "SwitchPowerImplementation::ResultStatus() argument ResultStatus: " << ResultStatus << std::endl;
    ResultStatus = _getStatus();
    // end of your own code
}



void
DimmingImplementation::SetLoadLevelTarget(UInt8 newLoadlevelTarget)
{
}


void
DimmingImplementation::GetLoadLevelTarget(UInt8& retLoadlevelTarget)
{
}


void
DimmingImplementation::GetLoadLevelStatus(UInt8& retLoadlevelStatus)
{
}
