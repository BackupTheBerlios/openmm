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

#ifndef Mux_INCLUDED
#define Mux_INCLUDED

#include <Poco/Thread.h>

#include "Stream.h"
#include "../AvStream.h"


namespace Omm {
namespace Dvb {


class InStream;

class Mux
{
    friend class InStream;

public:
    Mux();
    ~Mux();

    void addStream(Stream* pStream);
    std::istream* getMux();

    void start();
    void stop();

private:
    std::vector<InStream*>              _inStreams;
    std::istream*                       _pOutStream;
    AvStream::ByteQueue                 _byteQueue;
};


class InStream
{
public:
    InStream(Stream* pStream, Mux* pMux);

    void startReadThread();
    void stopReadThread();
    bool readThreadRunning();

private:
    void readThread();

    Stream*                             _pStream;
    Mux*                                _pMux;

    Poco::Thread*                       _pReadThread;
    Poco::RunnableAdapter<InStream>     _readThreadRunnable;
    bool                                _readThreadRunning;
    Poco::FastMutex                     _readThreadLock;
    const int                           _readTimeout;
};


class TransportStreamPacket : public BitField
{
public:
    enum { ScrambledNone = 0x00, ScrambledReserved = 0x01, ScrambledEvenKey = 0x10, ScrambledOddKey = 0x11 };
    enum { AdaptionFieldPayloadOnly = 0x01, AdaptionFieldOnly = 0x10, AdaptionFieldAndPayload = 0x11 };

    TransportStreamPacket();
    ~TransportStreamPacket();

    void writePayloadFromStream(Stream* pStream, int timeout);
    int getSize();

    void setTransportErrorIndicator(bool uncorrectableError);
    void setPayloadUnitStartIndicator(bool PesOrPsi);
    void setTransportPriority(bool high);
    void setPacketIdentifier(Poco::UInt16 pid);
    void setScramblingControl(Poco::UInt8 scramble);
    void setAdaptionFieldExists(Poco::UInt8 exists);
    void setContinuityCounter(Poco::UInt8 counter);

private:
    const Poco::UInt8   _syncByte;
    const int           _size;
    const int           _headerSize;
    const int           _payloadSize;
};

}  // namespace Omm
}  // namespace Dvb

#endif
