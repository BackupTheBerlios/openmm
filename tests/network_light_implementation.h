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

#ifndef NETWORK_LIGHT_IMPLEMENTATION_H
#define NETWORK_LIGHT_IMPLEMENTATION_H

#include "Poco/Types.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

#include "jamm/upnp.h"
#include "network_light.h"

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

// Implementation of Service SwitchPower

class SwitchPowerImplementation : public SwitchPower
{
public:
//     SwitchPowerImplementation(/*Service* service*/);
    
    virtual void SetTarget(bool NewTargetValue);
    virtual void GetTarget(bool& RetTargetValue);
    virtual void ResultStatus(bool& ResultStatus);
};


class DimmingImplementation : public Dimming
{
public:
//     DimmingImplementation(/*Service* service*/);
    
    virtual void SetLoadLevelTarget(UInt8 newLoadlevelTarget);
    virtual void GetLoadLevelTarget(UInt8& retLoadlevelTarget);
    virtual void GetLoadLevelStatus(UInt8& retLoadlevelStatus);
};

#endif
