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

#ifndef Dvr_INCLUDED
#define Dvr_INCLUDED

#include <sys/poll.h>


namespace Omm {
namespace Dvb {

class Adapter;
class Remux;

class Dvr
{
    friend class Adapter;
    friend class Device;

public:
    Dvr(Adapter* pAdapter, int num);
    ~Dvr();

//    void openDvr(bool blocking = true);
    void openDvr();
    void closeDvr();
    void clearBuffer();
    void prefillBuffer();
    void startReadThread();
    void stopReadThread();
    bool readThreadRunning();
//    void setBlocking(bool blocking = true);

    std::istream* getStream();

private:
    void readThread();

    Adapter*                            _pAdapter;
    std::string                         _deviceName;
    int                                 _num;
    int                                 _fileDescDvr;
    AvStream::ByteQueue                 _byteQueue;
    const int                           _bufferSize;
    std::istream*                       _pDvrStream;
    Remux*                              _pRemux;

    bool                                _useByteQueue;
    struct pollfd                       _fileDescPoll[1];
    const int                           _pollTimeout;  // wait for _pollTimeout millisec for new data on dvr device
    Poco::Thread*                       _pReadThread;
    Poco::RunnableAdapter<Dvr>          _readThreadRunnable;
    bool                                _readThreadRunning;
    Poco::FastMutex                     _readThreadLock;
};

}  // namespace Omm
}  // namespace Dvb

#endif
