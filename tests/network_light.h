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

#include <fstream>

#include "Poco/Types.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include <sstream>

#include "jamm/upnp.h"

using Jamm::SsdpSocket;
using Jamm::SsdpMessage;
using Jamm::Device;
using Jamm::Service;
using Jamm::DeviceRoot;
using Jamm::Action;
using Poco::UInt8;
using Poco::StreamCopier;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using std::stringstream;

// NetworkLight interface
// TODO: should be created by a stub generator
// TODO: declare all in arguments as const references
// TODO: what if different embedded devices have the same Service?
// TODO: solve name conflict, when different Services have Action of same name

// Service SwitchPower

class SwitchPower/* : public Service*/
{
        // Interface to be implemented.
public:
//     SwitchPower(Service* service/*, SwitchPowerInterface* serviceImpl*/);
        // Actions received from a Controller
//     virtual void SetTarget(bool NewTargetValue) {/* empty implementation */}
//     virtual void GetTarget(bool& RetTargetValue) {/* empty implementation */}
//     virtual void ResultStatus(bool& ResultStatus) {/* empty implementation */}
    
    virtual void SetTarget(bool NewTargetValue) = 0;
    virtual void GetTarget(bool& RetTargetValue) = 0;
    virtual void ResultStatus(bool& ResultStatus) = 0;
    
    Service*                m_service;
    
protected:
        // getter/setter methods for StatusVariables
    // TODO: implement the StatusVariable stuff (could be some common code with the Action argument stuff).
    void _setTarget(bool target) { m_service->setStateVarBool("Target", target); }
    bool _getTarget() { return m_service->getStateVarBool("Target"); }
    void _setStatus(bool status) { m_service->setStateVarBool("Status", status); }
    bool _getStatus() { return m_service->getStateVarBool("Status"); }
    

private:
//     SwitchPowerInterface*   m_serviceImpl;
};




// Service Dimming
class Dimming/* : public Service*/
{
        // Interface to be implemented.
public:
//     Dimming(Service* service);
        // Actions received from a Controller
//     virtual void SetLoadLevelTarget(UInt8 newLoadlevelTarget) {}
//     virtual void GetLoadLevelTarget(UInt8& retLoadlevelTarget) {}
//     virtual void GetLoadLevelStatus(UInt8& retLoadlevelStatus) {}
    
    virtual void SetLoadLevelTarget(UInt8 newLoadlevelTarget) = 0;
    virtual void GetLoadLevelTarget(UInt8& retLoadlevelTarget) = 0;
    virtual void GetLoadLevelStatus(UInt8& retLoadlevelStatus) = 0;
    
    Service*         m_service;
    
protected:
        // getter/setter methods for StatusVariables
    void _setLoadLevelTarget(bool target) {/* setStatusVariableBool("Target", target); */}
    bool _getLoadLevelTarget() {}
    void _setLoadLevelStatus(bool status) {}
    bool _getLoadLevelStatus() {}
    

private:
};




// class NetworkLightDevice // : public Device
// {
// public:
//     SwitchPower     m_switchPowerService;
//     Dimming         m_dimmingService;
//     
// private:
//     Device          m_device;
// };


class NetworkLight /*: public DeviceRoot*/
{
    // code for dispatching events to methods etc. ...
public:
    NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl);
    ~NetworkLight();
    
private:
    void actionHandler(Action* action);
    
    // for now, m_deviceRoot is the link into the "dynamic-string-world".
    // get rid of the ctor DeviceRoot(std::string description) and we can inherit DeviceRoot
    // and set the description later
    DeviceRoot*     m_deviceRoot;
    
    SwitchPower*    m_switchPowerImpl;
    Dimming*        m_dimmingImpl;
    
    
//     NetworkLight(std::string description);
    
//     NetworkLightDevice  m_rootDevice;
};

#endif
