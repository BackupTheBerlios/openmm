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

#include "UpnpAvLogger.h"
#include "Log.h"


#ifndef NDEBUG
namespace Omm {
namespace Av {


Av::Log* Av::Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Av::Log::Log()
{
    Poco::Channel* pChannel = Util::Log::instance()->channel();
    _pUpnpAvLogger = &Poco::Logger::create("UPNP.AV", pChannel, Poco::Message::PRIO_DEBUG);
}


Av::Log*
Av::Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Av::Log::upnpav()
{
    return *_pUpnpAvLogger;
}


}  // namespace Omm
}  // namespace Av
#endif // NDEBUG

