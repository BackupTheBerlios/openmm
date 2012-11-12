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

#include "Service.h"
//#include "Stream.h"
//#include "../AvStream.h"


namespace Omm {
namespace Dvb {


class Remux
{
public:
    Remux(int multiplex);
    ~Remux();

    void addStream(Stream* pStream);
//    std::istream* getMux();

    void start();
    void stop();

    TransportStreamPacket* getTransportStreamPacket(int timeout = 0);

    void addService(Service* pService);
    void delService(Service* pService);

private:
    void readThread();
    bool readThreadRunning();

    int                                 _multiplex;
    std::set<Service*>                  _pServices;
//    std::istream*                       _pOutStream;
//    AvStream::ByteQueue                 _byteQueue;

    Poco::Thread*                       _pReadThread;
    Poco::RunnableAdapter<Remux>        _readThreadRunnable;
    bool                                _readThreadRunning;
    Poco::FastMutex                     _readThreadLock;
    struct pollfd                       _fileDescPoll[1];
    const int                           _readTimeout;
};


}  // namespace Omm
}  // namespace Dvb

#endif
