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
#ifndef NETWORK_LIGHT_H
#define NETWORK_LIGHT_H

#include <jamm/upnp.h>

using Jamm::Service;
using Jamm::DeviceRoot;
using Jamm::Action;

// TODO: What if all (embedded) devices of NetworkLight have SwitchPower and Dimming Services?
//      -> same implementation
//      -> differ only in m_pService

// NetworkLight interface
class NetworkLight;

// Service SwitchPower
class SwitchPower
{
    friend class NetworkLight;

public:
    virtual void SetTarget(const bool& NewTargetValue) = 0;
    virtual void GetTarget(bool& RetTargetValue) = 0;
    virtual void GetStatus(bool& ResultStatus) = 0;
    
    // getter/setter methods for StatusVariables
    void _setTarget(const bool& target);
    bool _getTarget();
    void _setStatus(const bool& status);
    bool _getStatus();
    
private:
    Service*    m_pService;
};


// Service Dimming
class Dimming
{
    friend class NetworkLight;
    
public:
    virtual void SetLoadLevelTarget(const Jamm::i1& newLoadLevelTarget) = 0;
    virtual void GetLoadLevelTarget(Jamm::i1& retLoadLevelTarget) = 0;
    virtual void GetLoadLevelStatus(Jamm::i1& retLoadLevelStatus) = 0;
    
    // getter/setter methods for StatusVariables
    void _setLoadLevelTarget(const Jamm::i1& target);
    Jamm::i1 _getLoadLevelTarget();
    void _setLoadLevelStatus(const Jamm::i1& status);
    Jamm::i1 _getLoadLevelStatus();
    
private:
    Service*    m_pService;
};


class NetworkLight
{
public:
    NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl);
    ~NetworkLight();
    
    // TODO: implement this ...
    void setUuid(std::string uuid);
    void setFriendlyName(std::string friendlyName);
    
private:
    void actionHandler(Action* action);
    
    // m_deviceRoot is the link into the "dynamic-string-world".
    DeviceRoot*     m_pDeviceRoot;
    SwitchPower*    m_pSwitchPowerImpl;
    Dimming*        m_pDimmingImpl;
};

#endif
