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

#ifndef DEBUG_H
#define DEBUG_H

// uncomment this to turn debugging on (or provide it on the compiler command line)
//#define __DEBUG__

// //#ifdef __DEBUG__
extern void TRACE( const char *, ... );

//#define TRACE(args) printf args
//#define TRACE( const char *, ... ) __attribute__ ((format (printf, 1, 2)))
//void TRACE( const char *, ... ) __attribute__ ((format (printf, 1, 2)));
//extern void trace( const char *, ... ) __attribute__ ((format (printf, 1, 2)));
//#define TRACE(args) trace args
// // #else
// // #define TRACE(args) 
// // #endif  // __DEBUG__


// extern void traceprint( const char *, ... );
// #define TRACE(args) traceprint( #args )




#endif  // DEBUG_H
