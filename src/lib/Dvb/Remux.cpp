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
#include <string.h>

#include <Poco/Types.h>

#include "DvbLogger.h"
#include "Remux.h"
#include "Device.h"
#include "TransportStream.h"


namespace Omm {
namespace Dvb {


void
TsPacketBlock::free()
{
    _pRemux->putFreePacketBlock(this);
}


Remux::Remux(int multiplex) :
_multiplex(multiplex),
_readTimeout(1000),
_pReadThread(0),
_readThreadRunnable(*this, &Remux::readThread),
_readThreadRunning(false),
_pQueueThread(0),
_queueThreadRunnable(*this, &Remux::queueThread),
_queueThreadRunning(false),
_packetBlockQueueSize(100)
{
    _fileDescPoll[0].fd = multiplex;
    _fileDescPoll[0].events = POLLIN;

//    _packetPool.push(new TsPacketBlock(this));
}


Remux::~Remux()
{
}


Service*
Remux::addService(Service* pService)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);
    std::vector<Service*>::iterator it = std::find(_services.begin(), _services.end(), pService);
    if (it != _services.end()) {
//        // service already added to remux, need a clone
        LOG(dvb, debug, "clone service " + pService->getName());
        pService = new Service(*pService);
    }
    pService->startQueueThread();
    _services.push_back(pService);
    return pService;
}


void
Remux::delService(Service* pService)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);
    std::vector<Service*>::iterator it = std::find(_services.begin(), _services.end(), pService);
    if (it != _services.end()) {
        _services.erase(it);
    }
    pService->stopQueueThread();
    pService->waitForStopQueueThread();
    pService->flush();
    if (pService->_clone) {
        delete pService;
    }
}


void
Remux::startRemux()
{
    LOG(dvb, debug, "TS remux thread start ...");

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
//    if (!_pQueueThread) {
//        _queueThreadRunning = true;
//        _pQueueThread = new Poco::Thread;
//        _pQueueThread->start(_queueThreadRunnable);
//    }
}


void
Remux::stopRemux()
{
    LOG(dvb, debug, "remux thread stop ...");

//    if (_pQueueThread) {
//        _remuxLock.lock();
//        _queueThreadRunning = false;
//        _packetBlockQueue.push(0);
//        _packetBlockQueueReadCondition.broadcast();
//        _remuxLock.unlock();
//    }
    if (_pReadThread) {
        _remuxLock.lock();
        _readThreadRunning = false;
        _remuxLock.unlock();
    }
}


void
Remux::waitForStopRemux()
{
    LOG(dvb, debug, "remux wait for stop ...");

//    if (_pQueueThread) {
//        if (_pQueueThread->isRunning() && !_pQueueThread->tryJoin(_readTimeout)) {
//            LOG(dvb, error, "failed to join TS remux queue thread");
//        }
//        delete _pQueueThread;
//        _pQueueThread = 0;
//    }
    if (_pReadThread) {
        if (_pReadThread->isRunning() && !_pReadThread->tryJoin(_readTimeout)) {
            LOG(dvb, error, "failed to join TS remux read thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }
}


void
Remux::flush()
{
    LOG(dvb, debug, "remux flush ...");

    const int bufsize = 1000 * TransportStreamPacket::Size;
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

//    LOG(dvb, debug, "flush remux packet block queue: " + Poco::NumberFormatter::format(_packetBlockQueue.size()) + " packet blocks");
//    while (_packetBlockQueue.size()) {
//        putFreePacketBlock(_packetBlockQueue.front());
//        _packetBlockQueue.pop();
//    }
}


bool
Remux::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);
    return _readThreadRunning;
}


bool
Remux::queueThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);
    return _queueThreadRunning;
}


TransportStreamPacket*
Remux::getTransportStreamPacket()
{
    TransportStreamPacket* pPacket = new TransportStreamPacket;
    int bytesToRead = TransportStreamPacket::Size;
    int bytesRead = 0;

    while (bytesToRead > 0) {
        int pollRes = poll(_fileDescPoll, 1, _readTimeout);
        if (pollRes > 0) {
            if (_fileDescPoll[0].revents & POLLIN){
                bytesRead += ::read(_multiplex, (Poco::UInt8*)pPacket->getData() + bytesRead, bytesToRead);
                if (bytesRead > 0) {
                    bytesToRead -= bytesRead;
                    if (pPacket->getBytes<Poco::UInt8>(0) != TransportStreamPacket::SyncByte) {
                        LOG(dvb, error, "TS packet wrong sync byte: " + Poco::NumberFormatter::formatHex(pPacket->getBytes<Poco::UInt8>(0)));
                        delete pPacket;
                        return 0;
                    }
                }
                else if (bytesRead == -1) {
                    LOG(dvb, error, "remux read thread failed to read from device: " + std::string(strerror(errno)));
                    delete pPacket;
                    return 0;
                }
            }
            else {
                LOG(dvb, warning, "remux read thread uncatched poll event");
            }
        }
        else if (pollRes == 0) {
            LOG(dvb, trace, "remux read thread timeout");
            delete pPacket;
            return 0;
        }
        else if (pollRes == -1) {
            LOG(dvb, error, "remux read thread failed to read TS packet: " + std::string(strerror(errno)));
            delete pPacket;
            return 0;
        }
    }
    return pPacket;
}


TsPacketBlock*
Remux::getFreePacketBlock()
{
//    LOG(dvb, debug, "remux get free packet block, pool size: " + Poco::NumberFormatter::format(_packetPool.size()));

    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);

    if (_packetPool.size()) {
        TsPacketBlock* pRes = _packetPool.top();
        _packetPool.pop();
        return pRes;
    }
    else {
//        LOG(dvb, debug, "remux new packet block");
        return new TsPacketBlock(this);
    }
}


void
Remux::putFreePacketBlock(TsPacketBlock* pPacketBlock)
{
//    LOG(dvb, debug, "remux put free packet block, pool size: " + Poco::NumberFormatter::format(_packetPool.size()));

    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);

    _packetPool.push(pPacketBlock);
}


void
Remux::queuePacketBlock(TsPacketBlock* pPacketBlock)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_remuxLock);

    if (_packetBlockQueue.size() < _packetBlockQueueSize) {
        _packetBlockQueue.push(pPacketBlock);
        _packetBlockQueueReadCondition.broadcast();
    }
    else {
        LOG(dvb, error, "packet block queue full, discard packet block.");
        putFreePacketBlock(pPacketBlock);
    }
}


TsPacketBlock*
Remux::readPacketBlock()
{
    TsPacketBlock* pPacketBlock = getFreePacketBlock();
    Poco::UInt8* pPacketBlockData = pPacketBlock->getPacketData();

    int bytesToRead = TsPacketBlock::Size;
    int bytesRead = 0;

    while (bytesToRead > 0) {
        int pollRes = poll(_fileDescPoll, 1, _readTimeout);
        if (pollRes > 0) {
            if (_fileDescPoll[0].revents & POLLIN){
                bytesRead += ::read(_multiplex, pPacketBlockData + bytesRead, bytesToRead);
                if (bytesRead > 0) {
                    bytesToRead -= bytesRead;
                    if (*pPacketBlockData != TransportStreamPacket::SyncByte) {
                        LOG(dvb, error, "TS packet wrong sync byte: " + Poco::NumberFormatter::formatHex(*pPacketBlockData));
                        putFreePacketBlock(pPacketBlock);
                        return 0;
                    }
                }
                else if (bytesRead == -1) {
                    LOG(dvb, error, "remux read thread failed to read from device: " + std::string(strerror(errno)));
                    putFreePacketBlock(pPacketBlock);
                    return 0;
                }
            }
            else {
                LOG(dvb, warning, "remux read thread uncatched poll event");
            }
        }
        else if (pollRes == 0) {
            LOG(dvb, trace, "remux read thread timeout");
            putFreePacketBlock(pPacketBlock);
            return 0;
        }
        else if (pollRes == -1) {
            LOG(dvb, error, "remux read thread failed to read TS packet block: " + std::string(strerror(errno)));
            putFreePacketBlock(pPacketBlock);
            return 0;
        }
    }
    return pPacketBlock;
}


//void
//Remux::readThread()
//{
//    // NOTE: the remuxer loop is very performance critical
//    LOG(dvb, debug, "remux read thread started.");
//
//    while (readThreadRunning()) {
//        TsPacketBlock* pPacketBlock = readPacketBlock();
//        if (!pPacketBlock) {
////            LOG(dvb, warning, "remux thread could not read packet.");
//            continue;
//        }
//        queuePacketBlock(pPacketBlock);
//    }
//
//    LOG(dvb, debug, "remux read thread finished.");
//}


void
Remux::queueThread()
{
    LOG(dvb, debug, "remux queue thread started.");

    while (queueThreadRunning()) {
        _remuxLock.lock();
        if (_packetBlockQueue.size() == 0) {
//            LOG(dvb, trace, "service queue thread wait for packet");
            _packetBlockQueueReadCondition.wait<Poco::FastMutex>(_remuxLock);
        }
//        LOG(dvb, debug, "remux queue thread get packet block, queue size: " + Poco::NumberFormatter::format(_packetBlockQueue.size()));
        TsPacketBlock* pPacketBlock = _packetBlockQueue.front();
        _packetBlockQueue.pop();
        _remuxLock.unlock();
        if (!pPacketBlock) {
            continue;
        }

        while (TransportStreamPacket* pTsPacket = pPacketBlock->getPacket()) {
            Poco::UInt16 pid = pTsPacket->getPacketIdentifier();
            for (std::vector<Service*>::const_iterator it = _services.begin(); it != _services.end(); ++it) {
                if ((*it)->hasPacketIdentifier(pid)) {
                    (*it)->queueTsPacket(pTsPacket);
                }
            }
        }
        // NOTE: enabling queue thread reduces cpu load but introduces interrupts in stream
        // no interrupts when:
        // 1. TransportStreamPacketBlock::SizeInPackets = 1 (but then no cpu load decrease, either)
        // 2. next line is commented. Then packet blocks are not reused, but newly allocated
        pPacketBlock->decRefCounter();
    }

    LOG(dvb, debug, "remux queue thread finished.");
}


void
Remux::readThread()
{
    // NOTE: the remuxer loop is very performance critical (do more optimizing?)
    LOG(dvb, debug, "remux thread started.");

    while (readThreadRunning()) {
        TransportStreamPacket* pTsPacket = getTransportStreamPacket();
        if (!pTsPacket) {
//            LOG(dvb, warning, "remux thread could not read packet.");
            continue;
        }
        Poco::UInt16 pid = pTsPacket->getPacketIdentifier();
        for (std::vector<Service*>::const_iterator it = _services.begin(); it != _services.end(); ++it) {
            if ((*it)->hasPacketIdentifier(pid)) {
                (*it)->queueTsPacket(pTsPacket);
            }
        }
        pTsPacket->decRefCounter();
    }

    LOG(dvb, debug, "remux thread finished.");
}


}  // namespace Omm
}  // namespace Dvb
