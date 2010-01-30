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

using Jamm::DeviceRootImplAdapter;
using Jamm::Service;
using Jamm::Action;

// NetworkLight interface
class NetworkLight;

// Service SwitchPower (ServiceType, like a class)
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
    static std::string   m_description;
    Service*             m_pService;  // like pointer this, that points to the object
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
    static std::string  m_description;
    Service*            m_pService;
};


class NetworkLight : public DeviceRootImplAdapter
{
public:
    NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl);
    
private:
    virtual void actionHandler(Action* action);
    virtual void initStateVars(const std::string& serviceType, Service* pThis) {}
    
    static std::string                  m_deviceDescription;
    SwitchPower*                        m_pSwitchPowerImpl;
    Dimming*                            m_pDimmingImpl;
};

#endif
