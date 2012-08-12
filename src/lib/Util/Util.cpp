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

#include <Poco/Environment.h>
#include <Poco/NumberFormatter.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Thread.h>
#include <iostream>

#include "Util.h"
#include "Sys.h"

namespace Omm {
namespace Util {


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
}


const std::string
Home::getHomeDirPath()
{
    Poco::Mutex::ScopedLock lock(_lock);

    if (!_homeDirPath.size()) {
        _homeDirPath = Poco::Environment::get("OMM_HOME", Omm::Sys::SysPath::getPath(Omm::Sys::SysPath::Home)) + "/.omm";
        Log::instance()->util().information("OMM HOME: " + _homeDirPath);
        try {
            Poco::File(_homeDirPath).createDirectories();
        }
        catch (Poco::Exception& e) {
            Log::instance()->util().error("can not create OMM HOME: " + _homeDirPath);
        }
    }

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
    Log::instance()->util().debug("OMM config path: " + fullPath);
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
