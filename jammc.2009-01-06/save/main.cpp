/*****************************************************************
|
|   Platinum - main
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

// #include "upnpcontroller.h"

// #include <cstdio>
// #include <cstdlib>
// #include <string>

#include <Qt/qapplication.h>

#include "controllergui.h"


/*----------------------------------------------------------------------
|   main
+---------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    //PLT_SetLogLevel(3);

    // Create upnp engine
//     PLT_UPnP upnp(1900, true);

    // Create control point
//     PLT_CtrlPointReference ctrlPoint(new PLT_CtrlPoint());

    // Create controller
//     UpnpController controller(ctrlPoint);

    // add control point to upnp engine and start it
//     upnp.AddCtrlPoint(ctrlPoint);

//     upnp.Start();

    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
//     ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
//     ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);

    // start to process commands 
//     controller.ProcessCommandLoop();
    QApplication app(argc, argv);
    ControllerGui window;
//     ControllerGui window(&controller);
    window.show();
//     window.startControllerThread();
    
//     int ret = app.exec();
    
//     upnp.Stop();
//     return ret;
    return app.exec();
}
