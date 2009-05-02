/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef SHAREDLIBRARY_H
#define SHAREDLIBRARY_H

#include <string>
using namespace std;

namespace Jamm {

/**
Utility class for handling shared libraries.

	@author Jörg Bakker <joerg@hakker.de>
*/

// TODO: add code to automatically load all exported symbols from the shared library.

class JSharedLibrary{
public:
    JSharedLibrary(string filename);
    ~JSharedLibrary();

    void* resolve(const char * symb);

private:
    void *m_libHandle;
};

} // namespace Jamm

#endif
