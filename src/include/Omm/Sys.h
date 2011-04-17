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


//class VisualImpl;

class Visual
    /// Visual is a basic window that can be supplied by the graphical user
    /// interface of the OS.
{
public:
    enum VisualType {VTNone, VTQt, VTX11, VTFB, VTMacOSX, VTWin};

    Visual() {}
    virtual ~Visual() {}

    virtual void show() {}
    virtual void hide() {}

    virtual void* getWindow() { return 0; }
    virtual int getWidth() { return 0; }
    virtual int getHeight() { return 0; }

    virtual VisualType getType() { return VTNone; }

//private:
//    VisualImpl*     _pImpl;
};


}  // namespace Sys
}  // namespace Omm

#endif
