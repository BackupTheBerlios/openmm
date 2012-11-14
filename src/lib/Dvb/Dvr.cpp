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

#include <fcntl.h>
#include <sys/ioctl.h>

#include "DvbLogger.h"
#include "DvbUtil.h"
#include "Dvr.h"
#include "Device.h"
#include "Remux.h"


namespace Omm {
namespace Dvb {

Dvr::Dvr(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num),
_pDvrStream(0),
_pRemux(0),
//_useByteQueue(true),
_useByteQueue(false),
_fileDescDvr(-1),
_byteQueue(100*1024),
_bufferSize(50*1024),
_pollTimeout(1),
_pReadThread(0),
_readThreadRunnable(*this, &Dvr::readThread),
_readThreadRunning(false)
{
    _deviceName = _pAdapter->_deviceName + "/dvr" + Poco::NumberFormatter::format(_num);
}


Dvr::~Dvr()
{
}


void
Dvr::openDvr(bool blocking)
{
    LOG(dvb, debug, "opening dvb rec device.");

    if (_pDvrStream) {
        LOG(dvb, debug, "dvb rec device already open.");
        return;
    }
    else {
        int flags = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if ((_fileDescDvr = open(_deviceName.c_str(), flags)) < 0) {
            LOG(dvb, error, "failed to open dvb rec device \"" + _deviceName + "\": " + strerror(errno));
            return;
        }
        if (Device::instance()->getMode() == Device::ModeDvrMultiplex || Device::instance()->getMode() == Device::ModeDvr) {
            _pRemux = new Remux(_fileDescDvr);
            _pDvrStream = new UnixFileIStream(_fileDescDvr, _bufferSize);
//            _pDvrStream = _pRemux->getMux();
//            _pRemux->start();
        }
        else if (_useByteQueue) {
            _fileDescPoll[0].fd = _fileDescDvr;
            _fileDescPoll[0].events = POLLIN;
            _pDvrStream = new ByteQueueIStream(_byteQueue);
        }
        else {
            _pDvrStream = new UnixFileIStream(_fileDescDvr, _bufferSize);
        }
        if (!_pDvrStream) {
            LOG(dvb, error, "failed to open dvb rec stream.");
            return;
        }
        if (_useByteQueue) {
            startReadThread();
        }
    }
}


void
Dvr::closeDvr()
{
    LOG(dvb, debug, "closing dvb rec device.");

    if (_useByteQueue) {
        stopReadThread();
    }
    if (_pRemux) {
        delete _pRemux;
        _pRemux = 0;
    }
    if (_pDvrStream) {
        delete _pDvrStream;
        _pDvrStream = 0;
        if (close(_fileDescDvr)) {
            LOG(dvb, error, "failed to close dvb rec device \"" + _deviceName + "\": " + strerror(errno));
        }
        _fileDescDvr = -1;
    }
    else {
        LOG(dvb, debug, "dvb rec device already closed.");
    }
}


void
Dvr::clearBuffer()
{
    if (_pDvrStream) {
        const int bufsize(_bufferSize);
        char buf[bufsize];
        while (int bytes = _pDvrStream->readsome(buf, bufsize)) {
            LOG(dvb, debug, "clear buffer: " + Poco::NumberFormatter::format(bytes) + " bytes");
        }
    }
}


void
Dvr::prefillBuffer()
{
    if (_pReadThread) {
        while (_byteQueue.level() < _byteQueue.size() * 0.5) {
            Poco::Thread::sleep(1);
        }
    }
}


void
Dvr::setBlocking(bool blocking)
{
    LOG(dvb, debug, "set dvb rec device to " + std::string(blocking ? "blocking" : "non-blocking"));

    if (_pDvrStream) {
        long fcntlFlag = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if (fcntl(_fileDescDvr, F_SETFL, fcntlFlag) < 0) {
            LOG(dvb, error, "failed to set dvb rec device \"" + _deviceName + "\" to " + (blocking ? "blocking" : "non-blocking") + ":" + strerror(errno));
        }
    }
}


void
Dvr::startReadThread()
{
    LOG(dvb, debug, "start dvr read thread ...");

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
Dvr::stopReadThread()
{
    LOG(dvb, debug, "stop dvr read thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        if (!_pReadThread->tryJoin(_pollTimeout)) {
            LOG(dvb, error, "failed to join read thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }

    LOG(dvb, debug, "dvr read thread stopped.");
}


bool
Dvr::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


std::istream*
Dvr::getStream()
{
    LOG(dvb, debug, "get dvb rec stream.");

    return _pDvrStream;
}


void
Dvr::readThread()
{
    LOG(dvb, debug, "dvr read thread started.");

    char buf[_bufferSize];
    while (readThreadRunning()) {
        if (poll(_fileDescPoll, 1, _pollTimeout)) {
            if (_fileDescPoll[0].revents & POLLIN){
                int len = read(_fileDescDvr, buf, _bufferSize);
                if (len > 0) {
                    _byteQueue.write(buf, len);
                }
                else if (len < 0) {
                    LOG(dvb, error, std::string("dvr read thread failed to read from device: ") + strerror(errno));
                    break;
                }
            }
        }
    }

    LOG(dvb, debug, "dvr read thread finished.");
}

}  // namespace Omm
}  // namespace Dvb
