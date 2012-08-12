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
#include <Poco/Thread.h>
#include <Poco/Net/Socket.h>
#include <iostream>
#include <Poco/LineEndingConverter.h>

#include "Util.h"
#include "Sys.h"

namespace Omm {
namespace Util {


Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    _pChannel = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
#ifdef __IPHONE__
    Util::TCPChannel* pTCPChannel = new Util::TCPChannel;
    pSplitterChannel->addChannel(pTCPChannel);
#else
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
    pSplitterChannel->addChannel(pConsoleChannel);
#endif
    _pChannel->setChannel(pSplitterChannel);
    _pChannel->open();
#ifdef NDEBUG
    _pUtilLogger = &Poco::Logger::create("UTIL", _pChannel, 0);
    _pPluginLogger = &Poco::Logger::create("PLUGIN", _pChannel, 0);
#else
    _pUtilLogger = &Poco::Logger::create("UTIL", _pChannel, Poco::Message::PRIO_DEBUG);
    _pPluginLogger = &Poco::Logger::create("PLUGIN", _pChannel, Poco::Message::PRIO_DEBUG);
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


Poco::Channel*
Log::channel()
{
    return _pChannel;
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


TCPChannel::TCPChannel() :
_port(60706),
_connectionThreadRunnable(*this, &TCPChannel::connectionThread),
_bufferTime(1000000)  // hold messages in buffer over a time period of one second
{
    try {
        _pSocket = new Poco::Net::ServerSocket(_port);
    }
    catch (Poco::Exception& e) {
//        std::clog << e.displayText() << std::endl;
    }
    _connectionThread.start(_connectionThreadRunnable);
}


void
TCPChannel::open()
{
}


void
TCPChannel::close()
{
}


void
TCPChannel::log(const Poco::Message& message)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    sendMessage(&message);
    _buffer.push_back(new Poco::Message(message));
    Poco::Timestamp t = message.getTime();
    std::vector<Poco::Message*>::iterator pos = _buffer.begin();
    while (pos != _buffer.end() && t - (*pos)->getTime() > _bufferTime) {
        ++pos;
    }
    for (std::vector<Poco::Message*>::const_iterator it = _buffer.begin(); it != pos; ++it) {
        delete *it;
    }
    _buffer.erase(_buffer.begin(), pos);
}


TCPChannel::~TCPChannel()
{
    _pSocket->close();
    _connectionThread.join();
    delete _pSocket;

    for (std::vector<Poco::Message*>::const_iterator it = _buffer.begin(); it != _buffer.end(); ++it) {
        delete *it;
    }
    _buffer.clear();
}


void
TCPChannel::connectionThread()
{
//    std::clog << "connection thread start" << std::endl;
    for (;;) {
        _connection = _pSocket->acceptConnection();
        _lock.lock();
        for (std::vector<Poco::Message*>::const_iterator it = _buffer.begin(); it != _buffer.end(); ++it) {
            sendMessage(*it);
        }
        _lock.unlock();
    }
//    std::clog << "connection thread end" << std::endl;
}


void
TCPChannel::sendMessage(const Poco::Message* pMessage)
{
    try {
        _connection.sendBytes((pMessage->getText() + Poco::LineEnding::NEWLINE_LF).data(), pMessage->getText().length() + 1);
    }
    catch (Poco::Exception& e) {
//        std::clog << e.displayText() << std::endl;
    }
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
