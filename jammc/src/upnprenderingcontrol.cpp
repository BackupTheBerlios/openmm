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

#include "upnprenderingcontrol.h"

#include <jamm/debug.h>

JRenderingController::JRenderingController(PLT_CtrlPointReference& ctrl_point)
: m_CtrlPoint(ctrl_point)
{
    m_CtrlPoint->AddListener(this);
}


JRenderingController::~JRenderingController()
{
    m_CtrlPoint->RemoveListener(this);
}


NPT_Result
JRenderingController::OnDeviceAdded(PLT_DeviceDataReference& device)
{
}


NPT_Result
JRenderingController::OnDeviceRemoved(PLT_DeviceDataReference& device)
{
}


NPT_Result
JRenderingController::OnActionResponse(NPT_Result res, PLT_ActionReference& action, void* userdata)
{
}


NPT_Result
JRenderingController::OnEventNotify(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars)
{
}


void
JRenderingController::setVolume(PLT_DeviceDataReference& device, int channel, int volume)
{
    TRACE("JRenderingController::setVolume() volume: %i", volume);
    const char* service_type = "urn:schemas-upnp-org:service:RenderingControl:1";
    const char* action_name = "SetVolume";
    
    // look for the service
    PLT_Service* service;
    if (NPT_FAILED(device->FindServiceByType(service_type, service))) {
        TRACE("Service %s not found", (const char*)service_type);
    }
    
    PLT_ActionDesc* action_desc;
    action_desc = service->FindActionDesc(action_name);
    if (action_desc == NULL) {
        TRACE("Action %s not found in service", action_name);
    }
    
    PLT_ActionReference action;
    action = new PLT_Action(action_desc);
    
    // set instance_id
    action->SetArgumentValue("InstanceID", "0");
    // set channel
    action->SetArgumentValue("Channel", "Master");
    // set volume
    action->SetArgumentValue("DesiredVolume", NPT_String::FromInteger(volume));
    
    m_CtrlPoint->InvokeAction(action, NULL);
}
