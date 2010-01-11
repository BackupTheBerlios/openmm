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
using Jamm::DescriptionReader;
using Poco::UInt8;
using Poco::StreamCopier;
using Poco::Path;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using std::stringstream;


NetworkLight::NetworkLight(SwitchPower* switchPowerImpl, Dimming* dimmingImpl) :
m_switchPowerImpl(switchPowerImpl),
m_dimmingImpl(dimmingImpl)
{
    // TODO: stub generator should transform device description into a static string.
    //       Don't read the description from a file in this place.
    DescriptionReader descriptionReader(URI("file:/home/jb/devel/cc/jamm/tests/"), "xml/network-light-desc.xml");
    m_deviceRoot = descriptionReader.deviceRoot();
    
    // Service implementation needs a pointer to Service to access StateVariables
    // TODO: use UUID and not getRootDevice() here
    m_switchPowerImpl->m_service = &m_deviceRoot->getRootDevice()->getService("urn:schemas-upnp-org:service:SwitchPower:1");
    m_dimmingImpl->m_service = &m_deviceRoot->getRootDevice()->getService("urn:schemas-upnp-org:service:Dimming:1");
    
    m_deviceRoot->registerActionHandler(Observer<NetworkLight, Action>(*this, &NetworkLight::actionHandler));
    
    m_deviceRoot->startHttp();
    
    m_deviceRoot->init();
    m_deviceRoot->startSsdp();
}


NetworkLight::~NetworkLight()
{
    delete m_deviceRoot;
}


void
NetworkLight::actionHandler(Action* action)
{
    // TODO: dispatch each received Action to the corresponding method of NetworkLight
    std::cerr << "NetworkLight::actionHandler() receives Action: " << action->getName() 
        << " (Service: " <<  action->getService()->getServiceType() << ")"  << std::endl;
    
    // dispatch all SwitchPower Actions
    if (action->getService()->getServiceType() == "SwitchPower:1") {
        std::string actionName = action->getName();
        if (actionName == "SetTarget") {
            bool inArg1;
            inArg1 = action->getArgument<bool>("NewTargetValue");
            m_switchPowerImpl->SetTarget(inArg1);
        }
        else if (actionName == "GetTarget") {
            bool outArg1;
            m_switchPowerImpl->GetTarget(outArg1);
            action->setArgument<bool>("RetTargetValue", outArg1);
        }
        else if (actionName == "ResultStatus") {
            bool outArg1;
            m_switchPowerImpl->ResultStatus(outArg1);
            action->setArgument<bool>("ResultStatus", outArg1);
        }
    }
}
