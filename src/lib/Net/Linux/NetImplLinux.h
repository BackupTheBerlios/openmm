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
#ifndef NetImplLinux_INCLUDED
#define NetImplLinux_INCLUDED

#include <Poco/Runnable.h>
#include <Poco/Thread.h>


namespace Omm {
namespace Net {

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


}  // namespace Net
}  // namespace Omm

#endif
