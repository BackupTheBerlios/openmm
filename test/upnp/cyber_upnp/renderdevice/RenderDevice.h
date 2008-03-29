/******************************************************************
*
*	File : RenderDevice.h
*
******************************************************************/

#ifndef _RENDERDEVICE_H_
#define _RENDERDEVICE_H_

#include <cybergarage/upnp/CyberLink.h>

#include "device_desc.h"
#include "service_scpd.h"

using namespace CyberLink;
using namespace CyberHTTP;

// const char RENDER_DESCRIPTION_FILE_NAME[] = "/description/MediaRenderer1.xml";
const char RENDER_PRESENTATION_URI[] = "/presentation";

class RenderDevice : public Device, public ActionListener, public QueryListener
{	
public:

	RenderDevice();

	////////////////////////////////////////////////
	// ActionListener
	////////////////////////////////////////////////

	bool actionControlReceived(Action *action);

	////////////////////////////////////////////////
	// QueryListener
	////////////////////////////////////////////////

	bool queryControlReceived(StateVariable *stateVar);

	////////////////////////////////////////////////
	// HttpRequestListner
	////////////////////////////////////////////////

	void httpRequestRecieved(HTTPRequest *httpReq);
	
};

#endif

