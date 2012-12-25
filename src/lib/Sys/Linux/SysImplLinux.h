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

#include <string>
#include <vector>

#include "Sys/Path.h"
#include "Sys/System.h"

struct udev;

namespace Omm {
namespace Sys {

class SysImpl
{
public:
    SysImpl();
    ~SysImpl();

    static const std::string getPath(SysPath::Location loc);

    void getDevicesForType(std::vector<Device*>& devices, const std::string& deviceType);

private:
    struct udev*    _pUdev;
};

}  // namespace Sys
}  // namespace Omm

#endif
