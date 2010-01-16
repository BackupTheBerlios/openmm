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

#include "jamm/upnp.h"
#include "network_light.h"

// Implementation of Service SwitchPower
class SwitchPowerImplementation : public SwitchPower
{
public:
    virtual void SetTarget(const bool& NewTargetValue);
    virtual void GetTarget(bool& RetTargetValue);
    virtual void GetStatus(bool& ResultStatus);
};


// Implementation of Service Dimming
class DimmingImplementation : public Dimming
{
public:
    virtual void SetLoadLevelTarget(const Jamm::i1& newLoadLevelTarget);
    virtual void GetLoadLevelTarget(Jamm::i1& retLoadLevelTarget);
    virtual void GetLoadLevelStatus(Jamm::i1& retLoadLevelStatus);
};

#endif
