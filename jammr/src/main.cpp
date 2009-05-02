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
