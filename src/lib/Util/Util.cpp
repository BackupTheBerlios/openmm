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
#include <Poco/Environment.h>
#include <Poco/NumberFormatter.h>
#include <Poco/File.h>
#include <Poco/Path.h>

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
    _pPluginLogger = &Poco::Logger::create("PLUGIN", pFormatLogger, 0);
#else
    _pUtilLogger = &Poco::Logger::create("UTIL", pFormatLogger, Poco::Message::PRIO_DEBUG);
    _pPluginLogger = &Poco::Logger::create("PLUGIN", pFormatLogger, Poco::Message::PRIO_DEBUG);
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


Poco::Logger&
Log::plugin()
{
    return *_pPluginLogger;
}


Home* Home::_pInstance = 0;
Poco::Mutex Home::_lock;

Home*
Home::instance()
{
    Poco::Mutex::ScopedLock lock(_lock);

    if (!_pInstance) {
        _pInstance = new Home;
    }
    return _pInstance;
}


Home::Home()
{
    _homeDirPath = Poco::Environment::get("OMM_HOME", Poco::Environment::get("HOME", "/var"));
    if (_homeDirPath == Poco::Environment::get("HOME")) {
        _homeDirPath += "/.omm";
    }
    else {
        _homeDirPath += "/omm";
    }
    Log::instance()->util().information("OMM HOME: " + _homeDirPath);
    try {
        Poco::File(_homeDirPath).createDirectories();
    }
    catch (Poco::Exception& e) {
        Log::instance()->util().error("can not create OMM HOME: " + _homeDirPath);
    }
}


const std::string
Home::getHomeDirPath()
{
    Poco::Mutex::ScopedLock lock(_lock);

    return _homeDirPath;
}



const std::string
Home::getCacheDirPath(const std::string& relPath)
{
    Poco::Mutex::ScopedLock lock(_lock);
//    Log::instance()->util().debug("get cache dir path: " + _homeDirPath);

    std::string fullPath = Poco::Environment::get("OMM_CACHE", getHomeDirPath() + "/cache/") + relPath;
    try {
        Poco::File(fullPath).createDirectories();
    }
    catch (Poco::Exception& e) {
        Log::instance()->util().error("can not create cache directory path: " + fullPath);
    }
    return fullPath;
}


const std::string
Home::getConfigDirPath(const std::string& relPath)
{
    Poco::Mutex::ScopedLock lock(_lock);

    std::string fullPath = Poco::Environment::get("OMM_CONFIG", getHomeDirPath() + "/config/") + relPath;
    try {
        Poco::File(fullPath).createDirectories();
    }
    catch (Poco::Exception& e) {
        Log::instance()->util().error("can not create config directory path: " + fullPath);
    }
    return fullPath;
}


const std::string
Home::getMetaDirPath(const std::string& relPath)
{
    Poco::Mutex::ScopedLock lock(_lock);

    std::string fullPath = Poco::Environment::get("OMM_META", getHomeDirPath() + "/meta/") + relPath;
    try {
        Poco::File(fullPath).createDirectories();
    }
    catch (Poco::Exception& e) {
        Log::instance()->util().error("can not create meta directory path: " + fullPath);
    }
    return fullPath;
}


void
Startable::startAsThread()
{
#if (POCO_VERSION & 0xFFFFFFFF) >= 0x01350000
    _thread.setOSPriority(Poco::Thread::getMinOSPriority());
#endif
    _thread.start(*this);
}


void
Startable::stopThread()
{
    stop();
    _thread.join();
}


void
Startable::run()
{
    start();
}


} // namespace Util
} // namespace Omm
