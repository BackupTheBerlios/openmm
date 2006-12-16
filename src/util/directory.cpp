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
#include "directory.h"
#include "stringutil.h"
#include "debug.h"

#include <cstring>

Directory::Directory(string path)
{
    m_path = path;
    m_dir = opendir(m_path.c_str());
}


Directory::~Directory()
{
    closedir(m_dir);
}


vector<string>
Directory::entryList(string pattern, bool ignoreNonAscii)
{
//     TRACE("Directory::entryList() listing pattern: %s", pattern.c_str());
    dirent *dirEntry;
    vector<string> res;

    while(dirEntry = readdir(m_dir)) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
        }
        else if (StringUtil::s_wildcmp(pattern.c_str(), dirEntry->d_name, ignoreNonAscii)) {
//             TRACE("Directory::entryList() found match.");
            res.push_back(dirEntry->d_name);
        }
    }
    rewinddir(m_dir);
//     TRACE("Directory::entryList() finished");
    return res;
}
