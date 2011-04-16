/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <X11/Xlib.h>

#include "Sys.h"
#include "SysImplLinux.h"


namespace Omm {
namespace Sys {


VisualImpl::VisualImpl() :
_width(640),
_height(480),
_fullscreen(false)
{
}


VisualImpl::~VisualImpl()
{

}


void*
VisualImpl::getWindow()
{
    return _pX11Window;
}


void
VisualImpl::show()
{
    Display*    xDisplay;
    int         xScreen;
    int xPos    = 0;
    int yPos    = 0;

    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    if(_fullscreen) {
        _width   = DisplayWidth(xDisplay, xScreen);
        _height  = DisplayHeight(xDisplay, xScreen);
    }
    _pX11Window = new Poco::UInt32;
    *_pX11Window = XCreateSimpleWindow(xDisplay, XDefaultRootWindow(xDisplay),
                                  xPos, yPos, _width, _height, 1, 0, 0);
    
    XMapRaised(xDisplay, *_pX11Window);
//     res_h = (DisplayWidth(xDisplay, xScreen) * 1000 / DisplayWidthMM(xDisplay, xScreen));
//     res_v = (DisplayHeight(xDisplay, xScreen) * 1000 / DisplayHeightMM(xDisplay, xScreen));
    XSync(xDisplay, False);
    XUnlockDisplay(xDisplay);

    // hide X cursor
    Cursor no_ptr;
    Pixmap bm_no;
    XColor black, dummy;
    Colormap colormap;
    static char no_data[] = { 0,0,0,0,0,0,0,0 };

    colormap = DefaultColormap(xDisplay, xScreen);
    XAllocNamedColor(xDisplay, colormap, "black", &black, &dummy);
    bm_no = XCreateBitmapFromData(xDisplay, *_pX11Window, no_data, 8, 8);
    no_ptr = XCreatePixmapCursor(xDisplay, bm_no, bm_no, &black, &black, 0, 0);

    XDefineCursor(xDisplay, *_pX11Window, no_ptr);
    XFreeCursor(xDisplay, no_ptr);
}


void
VisualImpl::hide()
{
/*    if (xDisplay)
        XCloseDisplay(xDisplay);
    xDisplay = NULL;*/
    if (_pX11Window) {
        delete _pX11Window;
    }
}


int
VisualImpl::getWidth()
{
    return _width;
}


int
VisualImpl::getHeight()
{
    return _height;
}


Visual::VisualType
VisualImpl::getType()
{
    return Visual::VTX11;
}

}  // namespace Sys
} // namespace Omm
