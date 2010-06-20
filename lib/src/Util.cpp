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

using namespace Omm;
using namespace Omm::Util;


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
    _pUtilLogger = &Poco::Logger::create("UTIL", pFormatLogger, Poco::Message::PRIO_DEBUG);
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


std::string
Omm::Util::format(const std::string& fmt, const std::string& str1)
{
    std::string::size_type pos = fmt.find("%s");
    if (pos != std::string::npos) {
        return fmt.substr(0, pos) + str1 + fmt.substr(pos + 2);
    }
    return fmt;
}


std::string
Omm::Util::format(const std::string& fmt, const std::string& str1, const std::string& str2)
{
    std::string res;
    std::string::size_type pos = fmt.find("%s");
    if (pos != std::string::npos) {
        res = fmt.substr(0, pos) + str1;
    }
    else {
        return fmt;
    }
    std::string::size_type lastPos = pos + 2;
    pos = fmt.find("%s", lastPos);
    if (pos != std::string::npos) {
        return res + fmt.substr(lastPos, pos - lastPos) + str2 + fmt.substr(pos + 2);
    }
    return res;
}


std::string
Omm::Util::format(const std::string& fmt, const std::string& str1, const std::string& str2, const std::string& str3)
{
    std::string res;
    std::string::size_type pos = fmt.find("%s");
    if (pos != std::string::npos) {
        res = fmt.substr(0, pos) + str1;
    }
    else {
        return fmt;
    }
    std::string::size_type lastPos = pos + 2;
    pos = fmt.find("%s", lastPos);
    if (pos != std::string::npos) {
        res += fmt.substr(lastPos, pos - lastPos) + str2 + fmt.substr(pos + 2);
    }
    else {
        return res;
    }
    lastPos = pos + 2;
    pos = fmt.find("%s", lastPos);
    if (pos != std::string::npos) {
        return res + fmt.substr(lastPos, pos - lastPos) + str3 + fmt.substr(pos + 2);
    }
    return res;
}


std::string
Omm::Util::format(const std::string& fmt, const std::string& str1, const std::string& str2, const std::string& str3, const std::string& str4)
{
    return "not implemented";
}


std::string
Omm::Util::format(const std::string& fmt, const std::string& str1, const std::string& str2, const std::string& str3, const std::string& str4, const std::string& str5)
{
    return "not implemented";
}
