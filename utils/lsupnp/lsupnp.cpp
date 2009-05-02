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

#include <iostream>
using namespace std;

#include "lsupnp.h"

UpnpController::UpnpController()
{
    cout << "lsupnp 0.0.1 - display information on UPnP devices" << endl;
    cout << "hit the \"q\"-key and <enter> to quit" << endl << endl;

    m_upnp = new PLT_UPnP(1900, true);
    m_ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
    m_upnp->AddCtrlPoint(m_ctrlPoint);
    m_ctrlPoint->AddListener(this);
    m_upnp->Start();
    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
    m_ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
    m_ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);
}


UpnpController::~UpnpController()
{
    m_upnp->Stop();
}


NPT_Result
UpnpController::OnDeviceAdded(PLT_DeviceDataReference& device)
{
    NPT_String uuid = device->GetUUID();
    string name = (char*) device->GetFriendlyName();
    cout << "device added: " << name.c_str() << " (" << (char*) uuid << ")" << endl;
    NPT_Array<PLT_Service*> services = device->GetServices();
    NPT_Cardinal serviceCount = services.GetItemCount();
    cout << "found " << serviceCount << " services:" << endl;

    for (NPT_Cardinal i = 0; i < serviceCount; ++i) {
        PLT_Service* s = *(services.GetItem(i));
        cout << "    " << (char*) s->GetServiceType() << endl;
        if (s->IsSubscribable()) {
            m_ctrlPoint->Subscribe(s);
        }
    }
    cout << endl;

    return NPT_SUCCESS;
}


NPT_Result
UpnpController::OnDeviceRemoved(PLT_DeviceDataReference& device)
{
    NPT_String uuid = device->GetUUID();
    string name = (char*) device->GetFriendlyName();
    cout << "device removed: " << name.c_str() << " (" << (char*) uuid << ")" << endl << endl;
    
    return NPT_SUCCESS;
}


NPT_Result
UpnpController::OnActionResponse(NPT_Result res, PLT_ActionReference& action, void* userdata)
{
    cout << "action response" << endl << endl;
    return NPT_SUCCESS;
}


NPT_Result
UpnpController::OnEventNotify(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars)
{
    cout << "state variable(s) changed on device:" << 
        (char*)service->GetDevice()->GetFriendlyName() << " (" << (char*)service->GetDevice()->GetUUID() << ")" << endl;
    cout << "service:" << (char*)service->GetServiceType() << endl;
    for (unsigned int i = 0; i < vars->GetItemCount(); ++i) {
        cout << "    "
            << (char*)(*vars->GetItem(i))->GetName() << ": "
            << (char*)(*vars->GetItem(i))->GetValue() << " ("
            << (char*)(*vars->GetItem(i))->GetDataType() << ") "<< endl;
    }
    cout << endl;
    return NPT_SUCCESS;
}
