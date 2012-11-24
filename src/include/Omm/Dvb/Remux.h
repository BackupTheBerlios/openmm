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

#ifndef Remux_INCLUDED
#define Remux_INCLUDED

#include <sys/poll.h>

#include <Poco/Thread.h>
#include <Poco/Mutex.h>

#include "Service.h"
//#include "Stream.h"
//#include "../AvStream.h"


namespace Omm {
namespace Dvb {

//class ElementaryTransportStream;


class Remux
{
public:
    Remux(int multiplex);
    ~Remux();

    Service* addService(Service* pService);
    void delService(Service* pService);

    void startRemux();
    void stopRemux();
    void waitForStopRemux();
    void flush();

private:
    TransportStreamPacket* getTransportStreamPacket();
    void readThread();
    bool readThreadRunning();

    int                                                 _multiplex;
    std::vector<Service*>                               _services;
//    std::map<Poco::UInt16, ElementaryTransportStream*>  _pStreams;

    Poco::FastMutex                                     _remuxLock;
    Poco::Thread*                                       _pReadThread;
    Poco::RunnableAdapter<Remux>                        _readThreadRunnable;
    bool                                                _readThreadRunning;
    struct pollfd                                       _fileDescPoll[1];
    const int                                           _readTimeout;
};


//class ElementaryTransportStream
//{
//public:
//private:
//    void dispatchThread();
//    bool dispatchThreadRunning();
//
//    Poco::UInt16                                        _pid;
//    std::queue<TransportStreamPacket*>                  _packetQueue;
//    Poco::FastMutex                                     _streamLock;
//    Poco::Thread*                                       _pDispatchThread;
//    Poco::RunnableAdapter<ElementaryTransportStream>    _dispatchThreadRunnable;
//    bool                                                _dispatchThreadRunning;
//};


}  // namespace Omm
}  // namespace Dvb

#endif
