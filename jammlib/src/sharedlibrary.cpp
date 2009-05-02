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

#include "sharedlibrary.h"
#include "debug.h"

#include <dlfcn.h>

using namespace Jamm;

JSharedLibrary::JSharedLibrary(string filename)
{
    m_libHandle = dlopen(filename.c_str(), RTLD_NOW);
    if (!m_libHandle) {
        TRACE("JSharedLibrary::JSharedLibrary() loading failed: %s", dlerror());
        return;
    }
    TRACE("JSharedLibrary::JSharedLibrary() opened library: %s", filename.c_str());
    dlerror();  // clear any pending error.
}


JSharedLibrary::~JSharedLibrary()
{
    dlclose(m_libHandle);
}


void*
JSharedLibrary::resolve(const char * symb)
{
    void* res = dlsym(m_libHandle, symb);
    char* error;
    if ((error = dlerror()) != NULL)  {
        TRACE("JSharedLibrary::JSharedLibrary() could not find symbol %s, error was: %s",
                 symb, error);
    }
    else {
        TRACE("JSharedLibrary::JSharedLibrary() found symbol %s", symb);
    }
    return res;
}
