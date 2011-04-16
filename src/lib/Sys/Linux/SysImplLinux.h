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
#ifndef SysImplLinux_INCLUDED
#define SysImplLinux_INCLUDED

#include <Poco/Runnable.h>
#include <Poco/Thread.h>


namespace Omm {
namespace Sys {

#ifdef __SYS_NETMAN_PLATFORM__
class NetworkInterfaceManagerImpl : Poco::Runnable
{
public:
    NetworkInterfaceManagerImpl();
    ~NetworkInterfaceManagerImpl();

    void start();
    void stop();

private:
    class Private;

    virtual void run();

    Private*                _p;
    Poco::Thread            _monitorThread;
};
#endif


#ifdef __SYS_VISUAL_PLATFORM__
class VisualImpl
{
public:
    VisualImpl();
    ~VisualImpl();
    
    void* getWindow();
    void show();
    void hide();
    int getWidth();
    int getHeight();
    Visual::VisualType getType();

private:
    int             _width;
    int             _height;
    bool            _fullscreen;
    Poco::UInt32*   _pX11Window;
};
#endif


}  // namespace Sys
}  // namespace Omm

#endif
