/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>

#include "Util.h"

namespace Omm {
namespace Util {


Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::FormattingChannel* pFormatLogger = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
//     Poco::FileChannel* pFileChannel = new Poco::FileChannel("omm.log");
    pSplitterChannel->addChannel(pConsoleChannel);
//     pSplitterChannel->addChannel(pFileChannel);
    pFormatLogger->setChannel(pSplitterChannel);
    pFormatLogger->open();
#ifdef NDEBUG
    _pUtilLogger = &Poco::Logger::create("UTIL", pFormatLogger, 0);
#else
    _pUtilLogger = &Poco::Logger::create("UTIL", pFormatLogger, Poco::Message::PRIO_DEBUG);
#endif
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::util()
{
    return *_pUtilLogger;
}


} // namespace Util
} // namespace Omm
