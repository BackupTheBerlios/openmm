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

#include "DvbLogger.h"
#include "DvbUtil.h"
#include "Mux.h"
#include "Device.h"


namespace Omm {
namespace Dvb {


Mux::Mux() :
_byteQueue(100*1024)
{
    _pOutStream = new ByteQueueIStream(_byteQueue);
}


Mux::~Mux()
{
    delete _pOutStream;
}


void
Mux::addStream(Stream* pStream)
{
    _inStreams.push_back(new InStream(pStream, this));
}


std::istream*
Mux::getMux()
{
    return _pOutStream;
}


void
Mux::start()
{
    for (std::vector<InStream*>::iterator it = _inStreams.begin(); it != _inStreams.end(); ++it) {
        (*it)->startReadThread();
    }
}


void
Mux::stop()
{
    for (std::vector<InStream*>::iterator it = _inStreams.begin(); it != _inStreams.end(); ++it) {
        (*it)->stopReadThread();
    }
}


InStream::InStream(Stream* pStream, Mux* pMux) :
_pStream(pStream),
_pMux(pMux),
_pReadThread(0),
_readThreadRunnable(*this, &InStream::readThread),
_readThreadRunning(false),
_readTimeout(1)
{
}


void
InStream::readThread()
{
    LOG(dvb, debug, "read thread started.");

    const int bufferSize = 2048;
    char buf[bufferSize];
    Poco::UInt8 continuityCounter = 0;
    while (readThreadRunning()) {
//        _pStream->getStream()->read(buf, bufferSize);
//        _pMux->_byteQueue.write(buf, bufferSize);

        TransportStreamPacket tsPacket;
        tsPacket.setTransportErrorIndicator(false);
        tsPacket.setPayloadUnitStartIndicator(false);
        tsPacket.setTransportPriority(false);
        tsPacket.setPacketIdentifier(_pStream->getPid());
        tsPacket.setScramblingControl(TransportStreamPacket::ScrambledNone);
        tsPacket.setAdaptionFieldExists(TransportStreamPacket::AdaptionFieldPayloadOnly);
        tsPacket.setContinuityCounter(continuityCounter);
        tsPacket.writePayloadFromStream(_pStream, _readTimeout);
        _pMux->_byteQueue.write((char*)tsPacket.getData(), tsPacket.getSize());
        continuityCounter++;
        continuityCounter %= 16;
    }

    LOG(dvb, debug, "read thread finished.");
}


void
InStream::startReadThread()
{
    LOG(dvb, debug, "start read thread ...");

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
InStream::stopReadThread()
{
    LOG(dvb, debug, "stop read thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        if (!_pReadThread->tryJoin(_readTimeout)) {
            LOG(dvb, error, "failed to join read thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }

    LOG(dvb, debug, "read thread stopped.");
}


bool
InStream::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


TransportStreamPacket::TransportStreamPacket() :
_syncByte(0x47),
_size(188),
_headerSize(4),
_payloadSize(188 - 4)
{
    _data = new Poco::UInt8[_size];
    setBytes<Poco::UInt8>(0, _syncByte);
}


TransportStreamPacket::~TransportStreamPacket()
{
    delete (Poco::UInt8*)_data;
}


void
TransportStreamPacket::writePayloadFromStream(Stream* pStream, int timeout)
{
    pStream->read((Poco::UInt8*)_data + _headerSize, _payloadSize, timeout);
}


int
TransportStreamPacket::getSize()
{
    return _size;
}


void
TransportStreamPacket::setTransportErrorIndicator(bool uncorrectableError)
{
    setValue<Poco::UInt8>(8, 1, uncorrectableError ? 1 : 0);
}


void
TransportStreamPacket::setPayloadUnitStartIndicator(bool PesOrPsi)
{
    setValue<Poco::UInt8>(9, 1, PesOrPsi ? 1 : 0);
}


void
TransportStreamPacket::setTransportPriority(bool high)
{
    setValue<Poco::UInt8>(10, 1, high ? 1 : 0);
}


void
TransportStreamPacket::setPacketIdentifier(Poco::UInt16 pid)
{
    setValue<Poco::UInt16>(11, 13, pid);
}


void
TransportStreamPacket::setScramblingControl(Poco::UInt8 scramble)
{
    setValue<Poco::UInt8>(24, 2, scramble);
}


void
TransportStreamPacket::setAdaptionFieldExists(Poco::UInt8 exists)
{
    setValue<Poco::UInt8>(26, 2, exists);
}


void
TransportStreamPacket::setContinuityCounter(Poco::UInt8 counter)
{
    setValue<Poco::UInt8>(28, 4, counter);
}


}  // namespace Omm
}  // namespace Dvb
