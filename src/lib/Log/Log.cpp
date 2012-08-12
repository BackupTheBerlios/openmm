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

#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/LineEndingConverter.h>

#include "Log.h"

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

} // namespace Util
} // namespace Omm
