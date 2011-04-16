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
#ifndef SysImpl_INCLUDED
#define SysImpl_INCLUDED

namespace Omm {
namespace Sys {

#ifndef __SYS_NETMAN_PLATFORM__
class NetworkInterfaceManagerImpl
{
public:
    void start() {}
    void stop() {}
};
#endif


#ifndef __SYS_VISUAL_PLATFORM__
class VisualImpl
{
public:
    void* getWindow() { return 0; }
    void show() {}
    void hide() {}
    int getWidth() { return 0; }
    int getHeight() { return 0; }

    Visual::VisualType getType() { return Visual::VTNone; }
};
#endif

}  // namespace Sys
}  // namespace Omm

#endif
