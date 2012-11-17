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

#include <vector>
#include <Poco/Types.h>
#include <string.h>

#include "DvbLogger.h"
#include "Remux.h"
#include "Device.h"
#include "TransportStream.h"


namespace Omm {
namespace Dvb {


Remux::Remux(int multiplex) :
_multiplex(multiplex),
_pReadThread(0),
_readThreadRunnable(*this, &Remux::readThread),
_readThreadRunning(false),
_readTimeout(1000)
{
    _fileDescPoll[0].fd = multiplex;
    _fileDescPoll[0].events = POLLIN;
}


Remux::~Remux()
{
}


void
Remux::addService(Service* pService)
{
    _pServices.insert(pService);
}


void
Remux::delService(Service* pService)
{
    _pServices.erase(pService);
}


void
Remux::start()
{
    LOG(dvb, debug, "start TS remux thread ...");

    if (!_pReadThread) {
        for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
            (*it)->startQueueThread();
        }
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
Remux::stop()
{
    LOG(dvb, debug, "stop TS remux thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
            (*it)->stopQueueThread();
        }
        // wait for threads to stop
        if (_pReadThread->isRunning() && !_pReadThread->tryJoin(_readTimeout)) {
            LOG(dvb, error, "failed to join TS remux thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
        for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
            (*it)->waitForStopQueueThread();
        }
    }

    LOG(dvb, debug, "TS remux thread stopped.");
}


void
Remux::flush()
{
    const int bufsize = 1000 * 188;
    char buf[bufsize];
    int bytes = 0;
    do {
        bytes = ::read(_multiplex, buf, bufsize);
        if (bytes >= 0) {
            LOG(dvb, debug, "flush remux input stream: " + Poco::NumberFormatter::format(bytes) + " bytes");
        }
        else if (bytes == -1) {
            LOG(dvb, warning, "flush remux input stream: " + std::string(strerror(errno)));
        }
    } while (bytes > 0);

    for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
        (*it)->flush();
    }
}


bool
Remux::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


TransportStreamPacket*
Remux::getTransportStreamPacket()
{
    TransportStreamPacket* pPacket = new TransportStreamPacket;
    int bytesRead = 0;
    int bytesToRead = pPacket->getSize();
    while (bytesToRead > 0) {
        int pollRes = poll(_fileDescPoll, 1, _readTimeout);
        if (pollRes > 0) {
            if (_fileDescPoll[0].revents & POLLIN){
                bytesRead += ::read(_multiplex, (Poco::UInt8*)pPacket->getData() + bytesRead, bytesToRead);
                if (bytesRead > 0) {
                    bytesToRead -= bytesRead;
                    if (pPacket->getBytes<Poco::UInt8>(0) != pPacket->getSyncByte()) {
                        LOG(dvb, error, "TS packet wrong sync byte: " + Poco::NumberFormatter::formatHex(pPacket->getBytes<Poco::UInt8>(0)));
                        return 0;
                    }
                }
                else if (bytesRead == -1) {
                    LOG(dvb, error, "remux read thread failed to read from device: " + std::string(strerror(errno)));
                    return 0;
                }
            }
            else {
                LOG(dvb, warning, "remux read thread wrong poll event");
            }
        }
        else if (pollRes == 0) {
            LOG(dvb, error, "remux read thread timed out reading TS packet");
            return 0;
        }
        else if (pollRes == -1) {
            LOG(dvb, error, "remux read thread failed to read TS packet: " + std::string(strerror(errno)));
            return 0;
        }
    }
    return pPacket;
}


void
Remux::readThread()
{
    LOG(dvb, debug, "TS remux thread started.");

    Poco::Timestamp t;
    long unsigned int tsPacketCounter = 0;
    Poco::UInt8 continuityCounter = 0;

    while (readThreadRunning()) {
        TransportStreamPacket* pTsPacket = getTransportStreamPacket();
        if (!pTsPacket) {
            LOG(dvb, error, "TS remux thread could not read packet.");
            break;
        }
        tsPacketCounter++;
        Poco::UInt16 pid = pTsPacket->getPacketIdentifier();

//        LOG(dvb, information, "remux received packet no: " + Poco::NumberFormatter::format(tsPacketCounter) + ", pid: " + Poco::NumberFormatter::format(pid));
        for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
            if (pid == 0) {
                (*it)->_pPatTsPacket->setContinuityCounter(continuityCounter);
                continuityCounter++;
                continuityCounter %= 16;
                (*it)->queueTsPacket((*it)->_pPatTsPacket);
            }
            else if ((*it)->hasPacketIdentifier(pid)) {
                (*it)->queueTsPacket(pTsPacket);
            }
        }
    }

    LOG(dvb, information, "remux received " + Poco::NumberFormatter::format(tsPacketCounter) + " TS packets in "
            + Poco::NumberFormatter::format(t.elapsed() / 1000) + " msec ("
            + Poco::NumberFormatter::format((float)tsPacketCounter * 1000 / t.elapsed(), 2) + " packets/msec)");
    LOG(dvb, debug, "TS remux thread finished.");
}


}  // namespace Omm
}  // namespace Dvb
