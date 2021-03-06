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
#ifndef Path_INCLUDED
#define Path_INCLUDED

#include <string>

namespace Omm {
namespace Sys {


class SysPath
{
public:
    enum Location {Home, Cache, Tmp};
    /// Home: a place in the system, where the user can store his documents
    /// Cache: a place in the system, where all users can store cached files, that can be restored automatically
    /// Tmp: a place in the system, where all users can store temporary files

    static const std::string getPath(Location loc);
};

}  // namespace Sys
}  // namespace Omm

#endif
