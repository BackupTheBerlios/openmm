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

#include "upnpmediacontroller.h"

UpnpMediaController::UpnpMediaController(PLT_CtrlPointReference&      ctrl_point,
                                         PLT_MediaControllerListener* listener) :
PLT_MediaController(ctrl_point, listener)
{
}


NPT_Result
UpnpMediaController::OnDeviceAdded(PLT_DeviceDataReference& device)
{
    NPT_Result res;
    
    res = PLT_MediaController::OnDeviceAdded(device);
    
    PLT_Service* serviceAVRC;
    NPT_String type;
    
    type = "urn:schemas-upnp-org:service:RenderingControl:1";
    if (NPT_FAILED(device->FindServiceByType(type, serviceAVRC))) {
        NPT_LOG_FINE_1("Service %s not found", (const char*)type);
        return NPT_FAILURE;
    }
    
    return res;
}
