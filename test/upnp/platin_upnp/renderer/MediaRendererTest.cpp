/*****************************************************************
|
|   Platinum - Test UPnP A/V MediaRenderer
|
|   Copyright (c) 2004-2006 Sylvain Rebaud
|   Author: Sylvain Rebaud (sylvain@rebaud.com)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltUPnP.h"
#include "MediaRendererTest.h"

#include <stdlib.h>

// NPT_SET_LOCAL_LOGGER("platinum.core.upnp")

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
struct Options {
    const char* friendly_name;
} Options;

/*----------------------------------------------------------------------
|   PrintUsageAndExit
+---------------------------------------------------------------------*/
static void
PrintUsageAndExit(char** args)
{
    fprintf(stderr, "usage: %s [-f <friendly_name>]\n", args[0]);
    fprintf(stderr, "-f : optional upnp server friendly name\n");
    fprintf(stderr, "<path> : local path to serve\n");
    exit(1);
}

/*----------------------------------------------------------------------
|   ParseCommandLine
+---------------------------------------------------------------------*/
static void
ParseCommandLine(char** args)
{
    const char* arg;
    char**      tmp = args+1;

    /* default values */
    Options.friendly_name = NULL;

    while ((arg = *tmp++)) {
        if (!strcmp(arg, "-f")) {
            Options.friendly_name = *tmp++;
        } else {
            fprintf(stderr, "ERROR: too many arguments\n");
            PrintUsageAndExit(args);
        }
    }
}

JammRenderer::JammRenderer(PlaybackCmdListener* listener,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
 : PLT_MediaRenderer(listener, friendly_name, show_ip, uuid, port)
{
    fprintf(stderr, "JammRenderer::JammRenderer()\n");
//     NPT_LOG_INFO("JammRenderer::JammRenderer()\n");
}


NPT_Result
JammRenderer::OnPlay(PLT_ActionReference& action)

{
    fprintf(stderr, "JammRenderer::OnPlay()\n");
    return NPT_SUCCESS;
}


NPT_Result
JammRenderer::OnStop(PLT_ActionReference& action)

{
    fprintf(stderr, "JammRenderer::OnStop()\n");
    return NPT_SUCCESS;
}


NPT_Result
JammRenderer::OnSetAVTransportURI(PLT_ActionReference& action)

{
    fprintf(stderr, "JammRenderer::OnSetAVTransportURI()\n");
    return NPT_SUCCESS;
}



/*----------------------------------------------------------------------
|   main
+---------------------------------------------------------------------*/
int
main(int /* argc */, char** argv)
{   
    PLT_UPnP upnp;

    /* parse command line */
    ParseCommandLine(argv);

    PLT_DeviceHostReference device(
        new JammRenderer(
//             new PlaybackCmdListener(),
            NULL,
            Options.friendly_name?Options.friendly_name:"Jamm Media Renderer"));
    upnp.AddDevice(device);
    bool added = true;

    upnp.Start();

    char buf[256];
    while (gets(buf)) {
        if (*buf == 'q')
            break;

        if (*buf == 's') {
            if (added) {
                upnp.RemoveDevice(device);
            } else {
                upnp.AddDevice(device);
            }
            added = !added;
        }
    }

    upnp.Stop();
    return 0;
}
