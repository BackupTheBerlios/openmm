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

#include <Poco/NumberFormatter.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>

#include "Sys.h"

namespace Omm {
namespace Sys {

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
    _pSysLogger = &Poco::Logger::create("SYS", pFormatLogger, 0);
#else
    _pSysLogger = &Poco::Logger::create("SYS", pFormatLogger, Poco::Message::PRIO_DEBUG);
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
Log::sys()
{
    return *_pSysLogger;
}


Visual::Visual() :
_width(1020),
_height(576),
_fullscreen(false)
{
}


int
Visual::getWidth()
{
    return _width;
}


int
Visual::getHeight()
{
    return _height;
}


bool
Visual::getFullscreen()
{
    return _fullscreen;
}


void
Visual::setWidth(int width)
{
    _width = width;
}


void
Visual::setHeight(int height)
{
    _height = height;
}


void
Visual::setFullscreen(bool fullscreen)
{
    _fullscreen = fullscreen;
}


SignalHandler::SignalHandler() :
_signalHandler(*this, &SignalHandler::signalHandlerListener)
{
#ifdef __LINUX__
    sigemptyset(&_sset);
    sigaddset(&_sset, SIGINT);
    sigaddset(&_sset, SIGQUIT);
    sigaddset(&_sset, SIGTERM);
    sigprocmask(SIG_BLOCK, &_sset, NULL);
#endif
    _signalListenerThread.start(_signalHandler);
}


void
SignalHandler::signalHandlerListener()
{
#ifdef __LINUX__
    int sig;
    sigwait(&_sset, &sig);
    switch (sig) {
        case SIGINT:
            receivedSignal(SigInt);
            break;
        case SIGQUIT:
            receivedSignal(SigQuit);
            break;
        case SIGTERM:
            receivedSignal(SigTerm);
            break;
    }
#endif
}


}  // namespace Sys
}  // namespace Omm
