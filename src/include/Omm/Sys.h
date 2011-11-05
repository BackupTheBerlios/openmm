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
#ifndef Sys_INCLUDED
#define Sys_INCLUDED

#include <Poco/Logger.h>
#include <Poco/NotificationCenter.h>
#include <vector>
#include <string>

namespace Omm {
namespace Sys {

class Log
{
public:
    static Log* instance();

    Poco::Logger& sys();

private:
    Log();

    static Log*     _pInstance;
    Poco::Logger*   _pSysLogger;
};


class Visual
    /// Visual is a basic window that can be supplied by the graphical user
    /// interface of the OS.
{
public:
    enum VisualType {VTNone, VTX11, VTFB, VTOSX, VTWin};

#ifdef __LINUX__
    typedef Poco::UInt32 WindowHandle;
#elif __DARWIN__
    typedef uint32_t WindowHandle;
#elif __WINDOWS__
    typedef void* WindowHandle;
#endif

    Visual();
    virtual ~Visual() {}

    virtual void show() {}
    virtual void hide() {}

    virtual void* getWindow() { return 0; }
    virtual WindowHandle getWindowId() { return 0; }
    virtual VisualType getType() { return VTNone; }
    virtual void renderImage(const std::string& imageData) {}
    virtual void blank() {}

    int getWidth();
    int getHeight();
    bool getFullscreen();
    void setWidth(int width);
    void setHeight(int height);
    void setFullscreen(bool fullscreen = true);

private:
    int             _width;
    int             _height;
    bool            _fullscreen;
};


}  // namespace Sys
}  // namespace Omm

#endif
