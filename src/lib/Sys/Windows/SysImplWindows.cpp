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

#include <shlobj.h>

#include "Poco/Environment.h"
#include "Poco/Path.h"

#include "SysImplWindows.h"

namespace Omm {
namespace Sys {

const std::string
SysImpl::getPath(SysPath::Location loc)
{
    TCHAR path[MAX_PATH];
    switch (loc) {
        case SysPath::Home:
            if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
                return std::string(path);
            }
            return "";
        case SysPath::Cache:
            if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) {
                return std::string(path);
            }
            return "";
        case SysPath::Tmp:
            return Poco::Path::temp();
        default:
            return "";
    }
}


}  // namespace Sys
} // namespace Omm
