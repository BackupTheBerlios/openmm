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
#include "DvbUtil.h"
#include "Mux.h"
#include "Device.h"
#include "ElementaryStream.h"
#include "TransportStream.h"
#include "Dvb/TransportStream.h"


namespace Omm {
namespace Dvb {

// Mux is currently not needed (was attempt to mux elementary streams coming from demuxer)

   
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
_readTimeout(1000)
{
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


void
InStream::readThread()
{
    LOG(dvb, debug, "read thread started.");

    const int timeout = 1; // one sec read timeout for PES stream

    _pStream->skipToElementaryStreamPacketHeader(0, timeout);

//    const int bufferSize = 2048;
//    char buf[bufferSize];
//    Poco::UInt8* buf;

    TransportStreamPacket tsPacket;
    tsPacket.setTransportErrorIndicator(false);
    tsPacket.setTransportPriority(false);
    tsPacket.setPacketIdentifier(_pStream->getPid());
    // TODO: set TS packet scrambled mode according to stream scrambled mode
    tsPacket.setScramblingControl(TransportStreamPacket::ScrambledNone);

    Poco::UInt8 continuityCounter = 0;
    int payloadOffset = 0;
    while (readThreadRunning()) {
//        _pStream->getStream()->read(buf, bufferSize);
//        _pMux->_byteQueue.write(buf, bufferSize);
        ElementaryStreamPacket* pPesPacket = _pStream->getElementaryStreamPacket(timeout);
        if (!pPesPacket) {
            break;
        }

        if (pPesPacket->isAudio() || pPesPacket->isVideo()) {
            LOG(dvb, debug, "found PES audio/video packet");
            // stuff TS packet and write out
            tsPacket.setAdaptionFieldExists(TransportStreamPacket::AdaptionFieldAndPayload);
            tsPacket.stuffPayload(payloadOffset);
            payloadOffset = 0;
            tsPacket.setContinuityCounter(continuityCounter);
            // write TS packet to output of muxer
            _pMux->_byteQueue.write((char*)tsPacket.getData(), TransportStreamPacket::Size);
            continuityCounter++;
            continuityCounter %= 16;

            // begin TS packet sequence with new PES packet
            tsPacket.clearPayload();
            tsPacket.setPayloadUnitStartIndicator(true);
        }
        else {
            tsPacket.setPayloadUnitStartIndicator(false);
        }
        // write PES data to TS packet
//        tsPacket.writePayloadFromStream(_pStream, _readTimeout);
        int payloadAvailable = TransportStreamPacket::PayloadSize - payloadOffset;
        if (pPesPacket->getSize() < payloadAvailable) {
            tsPacket.setData(payloadOffset, pPesPacket->getSize(), pPesPacket->getData());
            payloadOffset += pPesPacket->getSize();
        }
        else {
            tsPacket.setData(payloadOffset, payloadAvailable, pPesPacket->getData());
            payloadOffset = 0;
            // tsPacket has no more payload available, write it to output stream
            // TODO: set adaption field when stuffing is needed
            tsPacket.setAdaptionFieldExists(TransportStreamPacket::AdaptionFieldPayloadOnly);
            tsPacket.setContinuityCounter(continuityCounter);
            // write TS packet to output of muxer
            _pMux->_byteQueue.write((char*)tsPacket.getData(), TransportStreamPacket::Size);
            continuityCounter++;
            continuityCounter %= 16;
        }
    }

    LOG(dvb, debug, "read thread finished.");
}


}  // namespace Omm
}  // namespace Dvb
