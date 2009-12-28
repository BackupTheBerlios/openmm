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

#include <fstream>

#include "Poco/Types.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include <sstream>

#include "jamm/upnp.h"
#include "network_light.h"

using Jamm::SsdpSocket;
using Jamm::SsdpMessage;
using Jamm::Device;
using Jamm::Service;
using Jamm::DeviceRoot;
using Jamm::Action;
using Jamm::ControlRequestHandler;
using Poco::UInt8;
using Poco::StreamCopier;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using std::stringstream;


// SwitchPower::SwitchPower(Service* service) :
// m_service(service)
// {
// }
// 
// 
// SwitchPowerImplementation::SwitchPowerImplementation(Service* service) :
// SwitchPower(service)
// {
// }


// Dimming::Dimming(Service* service) :
// m_service(service)
// {
// }
// 
// 
// DimmingImplementation::DimmingImplementation(Service* service):
// Dimming(service)
// {
// }

// NetworkLight::NetworkLight(std::string description) :
// DeviceRoot(description)
// {
// }


NetworkLight::NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl) :
m_switchPowerImpl(switchPowerImpl),
m_dimmingImpl(dimmingImpl)
{
    // TODO: stub generator should transform device description into a static string.
    //       Don't read the description from a file in this place.
    std::stringstream ss;
    std::ifstream ifs("/home/jb/devel/cc/jamm/tests/xml/network-light-desc.xml");
    StreamCopier::copyStream(ifs, ss);
    std::string s = ss.str();
    
    m_deviceRoot = new DeviceRoot(s);
    m_switchPowerImpl->m_service = m_deviceRoot->m_rootDevice.m_services["SwitchPower:1"];
    m_dimmingImpl->m_service = m_deviceRoot->m_rootDevice.m_services["Dimming:1"];
    
    m_deviceRoot->m_httpSocket.m_notificationCenter.addObserver(Observer<NetworkLight, Action>(*this, &NetworkLight::actionHandler));
}


NetworkLight::~NetworkLight()
{
    delete m_deviceRoot;
}


void
NetworkLight::actionHandler(Action* action)
{
    // TODO: dispatch each received Action to the corresponding method of NetworkLight
    std::cerr << "NetworkLight::actionHandler() receives Action: " << action->m_actionName 
              << " (Service: " <<  action->m_serviceType << ")"  << std::endl;
    
    // dispatch all SwitchPower Actions
    if (action->m_serviceType == "SwitchPower:1") {
        if (action->m_actionName == "SetTarget") {
            bool in;
            in = action->getArgumentBool("NewTargetValue");
            m_switchPowerImpl->SetTarget(in);
        }
        else if (action->m_actionName == "GetTarget") {
            bool out;
            m_switchPowerImpl->GetTarget(out);
            action->setArgumentBool("RetTargetValue", out);
        }
        else if (action->m_actionName == "ResultStatus") {
            bool out;
            m_switchPowerImpl->ResultStatus(out);
            action->setArgumentBool("ResultStatus", out);
        }
    }
}
