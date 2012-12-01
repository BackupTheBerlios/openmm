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

#include <Foundation.h>

#include "SysImplDarwin.h"

namespace Omm {
namespace Sys {


const std::string Sys::System::DeviceTypeOther("");
const std::string Sys::System::DeviceTypeDvb("dvb");
const std::string Sys::System::DeviceTypeDisk("block");


const std::string
SysImpl::getPath(SysPath::Location loc)
{
    switch (loc) {
        case SysPath::Home:
            return std::string([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] UTF8String]);
        case SysPath::Cache:
            return std::string([[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject] UTF8String]);
        case SysPath::Tmp:
            return std::string([NSTemporaryDirectory() UTF8String]);
        default:
            return "";
    }
}


void
SysImpl::getDevicesForType(std::vector<Device*>& devices, const std::string& deviceType)
{
    // TODO: implement SysImpl::getDevicesForType() for Darwin
}

}  // namespace Sys
} // namespace Omm
