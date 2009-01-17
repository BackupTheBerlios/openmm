/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "sharedlibrary.h"
#include "debug.h"

#include <dlfcn.h>

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
