// #include "enginexine.h"
// #include "enginemplayer.h"
#include "enginevlc.h"
#include "upnpmediarenderer.h"

#include <cstdio>
#include <cstdlib>
#include <platinum/PltUPnP.h>
#include <platinum/PltDeviceHost.h>
#include <platinum/PltLog.h>


int main(int argc, char **argv)
{
    PLT_UPnP upnp;
    UpnpMediaRenderer *renderer;
    Engine *engine;
    
    engine = new EngineVlc(argc, argv);
    renderer = new UpnpMediaRenderer(engine, "Tristan JammR UPnP Media Renderer");

    PLT_SetLogLevel(PLT_LOG_LEVEL_MAX);

    upnp.Start();
    PLT_DeviceHostReference device(renderer);
    upnp.AddDevice(device);
    
    char keystroke;
    while ((keystroke = getchar())) {
        if (keystroke == 'q')
            break;
    }
    
    delete engine;
    upnp.Stop();
    
    return EXIT_SUCCESS;
}
