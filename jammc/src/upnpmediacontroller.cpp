/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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
