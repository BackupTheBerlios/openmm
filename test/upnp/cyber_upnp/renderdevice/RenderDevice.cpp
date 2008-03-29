/******************************************************************
*
*	File : RenderDevice.cpp
*
******************************************************************/

#include "RenderDevice.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace CyberLink;

// RenderDevice::RenderDevice() : Device(RENDER_DESCRIPTION_FILE_NAME)
RenderDevice::RenderDevice() : Device()
{
    cout << "loading device description" << endl;
    loadDescription(RENDER_DEVICE_DESCRIPTION);
    
    Service* avtransportService = getService("urn:schemas-upnp-org:service:AVTransport:1");
    cout << "loading avtransport service description" << endl;
//     cout << AVTRANSPORT_SCPD << endl;
    avtransportService->loadSCPD(AVTRANSPORT_SCPD);
    
    Service* connectionManagerService = getService("urn:schemas-upnp-org:service:ConnectionManager:1");
    cout << "loading connectionmanager service description" << endl;
    connectionManagerService->loadSCPD(CONNECTION_MANAGER_SCPD);

    Service* renderingControlService = getService("urn:schemas-upnp-org:service:RenderingControl:1");
    cout << "loading renderingcontrol service description" << endl;
    renderingControlService->loadSCPD(RENDERING_CONTROL_SCPD);

    // define a listener for each action of each service ...
    Action *SetAVTransportURIAction = getAction("SetAVTransportURI");
    SetAVTransportURIAction->setActionListener(this);

    ServiceList *serviceList = getServiceList();
    Service *service = serviceList->getService(0);
    service->setQueryListener(this);

//	setLeaseTime(60);
}

////////////////////////////////////////////////
// ActionListener
////////////////////////////////////////////////

bool RenderDevice::actionControlReceived(Action *action)
{
	const char *actionName = action->getName();
	if (strcmp("GetTime", actionName) == 0) {
		Argument *timeArg = action->getArgument("CurrentTime");
		timeArg->setValue("");
		return true;
	}
	if (strcmp(actionName, "SetTime") == 0) {
		Argument *timeArg = action->getArgument("NewTime");
		const char *newTime = timeArg->getValue();
		Argument *resultArg = action->getArgument("Result");
		std::ostringstream valbuf;
		resultArg->setValue("");
		return true;
	}
	return false;
}

////////////////////////////////////////////////
// QueryListener
////////////////////////////////////////////////

bool RenderDevice::queryControlReceived(StateVariable *stateVar)
{
	const char *varName = stateVar->getName();
	stateVar->setValue("");
	return true;
}

////////////////////////////////////////////////
// HttpRequestListner
////////////////////////////////////////////////
	
void RenderDevice::httpRequestRecieved(HTTPRequest *httpReq)
{
	ParameterList paramList;
	httpReq->getParameterList(paramList);
	for (int n=0; n<paramList.size(); n++) {
		Parameter *param = paramList.getParameter(n);
		cout << "[" << n << "] : " << param->getName() << " = " << param->getValue() << endl;
	}

	string uri;
	httpReq->getURI(uri);
	if (uri.find(RENDER_PRESENTATION_URI) == string::npos)  {
		Device::httpRequestRecieved(httpReq);
		return;
	}
			 
	string contents;
	contents = "<HTML><BODY><H1>";
	contents += "";
	contents += "</H1></BODY></HTML>";
		
	HTTPResponse httpRes;
	httpRes.setStatusCode(HTTP::OK_REQUEST);
	httpRes.setContent(contents);
	httpReq->post(&httpRes);
}
	

