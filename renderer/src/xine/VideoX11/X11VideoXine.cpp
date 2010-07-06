/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/
#include <Poco/ClassLibrary.h>
#include "X11VideoXine.h"

#define MWM_HINTS_DECORATIONS   (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS 5
#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
ButtonMotionMask | StructureNotifyMask |        \
PropertyChangeMask | PointerMotionMask)

#define PROP_MWM_HINTS_ELEMENTS 5


X11XineVideo::X11XineVideo() :
pVisual(new x11_visual_t)
{
    XInitThreads ();
    x11Display = XOpenDisplay(NULL);
    x11Screen = DefaultScreen(x11Display);
}


int
X11XineVideo::displayWidth()
{
    return DisplayWidth(x11Display, x11Screen);
}


int
X11XineVideo::displayHeight()
{
    return DisplayHeight(x11Display, x11Screen);
}


// xine_video_port_t*
// X11XineVideo::videoDriver()
// {
//     return _pVideoDriver;
// }


std::string
X11XineVideo::driverName()
{
    return std::string("xv");
}


int
X11XineVideo::visualType()
{
    return XINE_VISUAL_TYPE_X11;
}


void*
X11XineVideo::visual()
{
    return pVisual;
}


void
X11XineVideo::initVisual()
{

    
      /* some initalization for the X11 Window we will be showing video in */
    XLockDisplay(x11Display);
//     fullscreen = 0;
    x11Window = XCreateSimpleWindow(x11Display, XDefaultRootWindow(x11Display),
                                    0, 0, displayWidth(), displayHeight(), 1, 0, 0);
    
    XSelectInput(x11Display, x11Window, INPUT_MOTION);
    
    XMapRaised(x11Display, x11Window);
    
//     res_h = (displayWidth() * 1000 / DisplayWidthMM(x11Display, x11Screen));
//     res_v = (displayHeight() * 1000 / DisplayHeightMM(x11Display, x11Screen));
    XSync(x11Display, False);
    XUnlockDisplay(x11Display);

//     std::string videoDriverName("xv");
//     int visualType = XINE_VISUAL_TYPE_X11;
    
    pVisual->display = x11Display;
    pVisual->screen = x11Screen;
    pVisual->d = x11Window;
//     pVisual->frame_output_cb = FrameOutputCallback;
//     _pVideoDriver = xine_open_video_driver(_xineEngine,
// 	videoDriverName.c_str(),  visualType,
// 	(void *) &(visual));
    

}


void
X11XineVideo::closeVisual()
{
    if (x11Display)
        XCloseDisplay(x11Display);
    x11Display = NULL;
}


POCO_BEGIN_MANIFEST(XineVideo)
POCO_EXPORT_CLASS(X11XineVideo)
POCO_END_MANIFEST