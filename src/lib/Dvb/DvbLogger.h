/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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

#ifndef DvbLogger_INCLUDED
#define DvbLogger_INCLUDED

#include <Poco/Logger.h>
#include <Poco/Format.h>
#include <Poco/StringTokenizer.h>
#include <Poco/TextConverter.h>
#include <Poco/TextEncoding.h>
#include <Poco/UTF8Encoding.h>

#include "Log.h"

namespace Omm {
namespace Dvb {


#ifndef NDEBUG
class Log
{
public:
    static Log* instance();

    Poco::Logger& dvb();

private:
    Log();

    static Log*     _pInstance;
    Poco::Logger*   _pDvbLogger;
};
#endif // NDEBUG


}  // namespace Omm
}  // namespace Dvb

#endif
