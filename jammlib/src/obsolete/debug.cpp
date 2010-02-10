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

#include "debug.h"
#include <cstdio>
#include <cstdarg>
#include <stdlib.h>


// basically we use the standard printf functions, with two changes:
// 1. we name it TRACE(), so we have to go through the hassle with varargs.
// 2. we append a newline.


void
TRACE(const char *msg, ...) {
#ifdef __DEBUG__
    static int debug_env = getenv("JAMM_DEBUG") ? 1 : 0;

    if(debug_env)
    {
        va_list ap;
        va_start(ap, msg);                    // use variable arg list
        vfprintf(stderr, msg, ap);
        fprintf(stderr, "\n");
        va_end(ap);
    }

#endif  // __DEBUG__
}
