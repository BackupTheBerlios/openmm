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

#ifndef TransportStream_INCLUDED
#define TransportStream_INCLUDED

#include <Poco/Thread.h>
#include "Poco/AtomicCounter.h"

#include "DvbUtil.h"


namespace Omm {
namespace Dvb {

class Stream;


class TransportStreamPacket : public BitField
{
public:
    enum { ScrambledNone = 0x00, ScrambledReserved = 0x01, ScrambledEvenKey = 0x10, ScrambledOddKey = 0x11 };
    enum { AdaptionFieldPayloadOnly = 0x01, AdaptionFieldOnly = 0x10, AdaptionFieldAndPayload = 0x11 };

    static const Poco::UInt8   SyncByte;
    static const int           Size;
    static const int           HeaderSize;
    static const int           PayloadSize;

    TransportStreamPacket();
    ~TransportStreamPacket();

    void writePayloadFromStream(Stream* pStream, int timeout);
    void clearPayload();
    void stuffPayload(int actualPayloadSize);

    // header fields
    void setTransportErrorIndicator(bool uncorrectableError);
    void setPayloadUnitStartIndicator(bool PesOrPsi);
    void setTransportPriority(bool high);
    Poco::UInt16 getPacketIdentifier();
    void setPacketIdentifier(Poco::UInt16 pid);
    void setScramblingControl(Poco::UInt8 scramble);
    void setAdaptionFieldExists(Poco::UInt8 exists);
    void setContinuityCounter(Poco::UInt8 counter);
    void setPointerField(Poco::UInt8 pointer);

    // optional header adaption fields
    void setAdaptionFieldLength(Poco::UInt8 length);
    void clearAllAdaptionFieldFlags();
    void setDiscontinuityIndicator(bool discontinuity);
    void setRandomAccessIndicator(bool randomAccess);
    void setElementaryStreamPriorityIndicator(bool high);
    void setPcrFlag(bool containsPcr);
    void setOPcrFlag(bool containsOPcr);
    void setSplicingPointFlag(bool spliceCountdownPresent);
    void setTransportPrivateDataFlag(bool privateDataPresent);
    void setExtensionFlag(bool extensionPresent);
    void setPcr(Poco::UInt64 base, Poco::UInt8 padding, Poco::UInt16 extension);
    void setSpliceCountdown(Poco::UInt8 countdown);
    void setStuffingBytes(int count);

    void incRefCounter() const
    {
        ++_refCounter;
    }

    void decRefCounter() const
    {
        if (--_refCounter == 0) delete this;
    }

private:
    int                             _adaptionFieldSize;
    bool                            _adaptionFieldPcrSet;
    bool                            _adaptionFieldSplicingPointSet;
    mutable Poco::AtomicCounter     _refCounter;
};


}  // namespace Omm
}  // namespace Dvb

#endif
