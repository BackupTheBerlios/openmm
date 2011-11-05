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
#include <Poco/ClassLibrary.h>
#include <X11/Xlib.h>

#include "X11Visual.h"


X11Visual::X11Visual()
{
    // TODO: check for DISPLAY environment variable and throw exception if not present.
    createWindow();
}


X11Visual::~X11Visual()
{
/*    if (xDisplay)
        XCloseDisplay(xDisplay);
    xDisplay = NULL;*/
}


X11Visual::WindowHandle
X11Visual::getWindowId()
{
    return _x11Window;
}


void
X11Visual::createWindow()
{
    Display*    xDisplay;
    int         xScreen;
    int xPos    = 0;
    int yPos    = 0;

    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    if(getFullscreen()) {
        setWidth(DisplayWidth(xDisplay, xScreen));
        setHeight(DisplayHeight(xDisplay, xScreen));
    }
    _x11Window = XCreateSimpleWindow(xDisplay, XDefaultRootWindow(xDisplay),
                                  xPos, yPos, getWidth(), getHeight(), 1, 0, 0);

    XMapRaised(xDisplay, _x11Window);
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
    bm_no = XCreateBitmapFromData(xDisplay, _x11Window, no_data, 8, 8);
    no_ptr = XCreatePixmapCursor(xDisplay, bm_no, bm_no, &black, &black, 0, 0);

    XDefineCursor(xDisplay, _x11Window, no_ptr);
    XFreeCursor(xDisplay, no_ptr);
}


void
X11Visual::show()
{
}


void
X11Visual::hide()
{
}


Omm::Sys::Visual::VisualType
X11Visual::getType()
{
    return Omm::Sys::Visual::VTX11;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Sys::Visual)
POCO_EXPORT_CLASS(X11Visual)
POCO_END_MANIFEST
#endif