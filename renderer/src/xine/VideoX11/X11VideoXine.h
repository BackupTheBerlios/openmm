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

#ifndef ENGINEXINE_X11_H
#define ENGINEXINE_X11_H

#include <X11/Xlib.h>

#include "../EngineXine.h"


class X11XineVideo : public XineVideo
{
public:
    X11XineVideo();
    
    virtual int displayWidth();
    virtual int displayHeight();
    virtual std::string driverName();
    virtual int visualType();
    virtual void* visual();

    
private:
    virtual void initVisual(int width, int height);
    virtual void closeVisual();

    Display*             x11Display;
    int                  x11Screen;
    Window               x11Window;
    
    x11_visual_t* 	 _pVisual;
};

#endif
