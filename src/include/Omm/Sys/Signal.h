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
#ifndef Signal_INCLUDED
#define Signal_INCLUDED

#ifdef __LINUX__
#include <signal.h>
#endif

#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>

namespace Omm {
namespace Sys {


class SignalHandler
{
public:
    enum SignalType {SigInt, SigQuit, SigTerm};

    SignalHandler();

    virtual void receivedSignal(SignalType signal) {}

private:
    void signalHandlerListener();

    Poco::RunnableAdapter<SignalHandler>        _signalHandler;
    Poco::Thread                                _signalListenerThread;

#ifdef __LINUX__
    sigset_t _sset;
#endif
};



}  // namespace Sys
}  // namespace Omm

#endif
