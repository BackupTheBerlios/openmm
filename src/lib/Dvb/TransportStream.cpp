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
#include "Stream.h"
#include "TransportStream.h"

namespace Omm {
namespace Dvb {


const int TransportStreamPacketBlock::SizeInPackets = 1;
const int TransportStreamPacketBlock::Size = SizeInPackets * TransportStreamPacket::Size;

TransportStreamPacketBlock::TransportStreamPacketBlock() :
_packetIndex(0),
_refCounter(1)
{
    _pPacketData = new Poco::UInt8[TransportStreamPacketBlock::Size];
    for (int i = 0; i < SizeInPackets; ++i) {
        TransportStreamPacket* pPacket = new TransportStreamPacket(false);
        pPacket->_pPacketBlock = this;
        pPacket->setData(_pPacketData + i * TransportStreamPacket::Size);
        _packetBlock.push_back(pPacket);
    }
}


TransportStreamPacketBlock::~TransportStreamPacketBlock()
{
    delete _pPacketData;
}


TransportStreamPacket*
TransportStreamPacketBlock::getPacket()
{
    if (_packetIndex < SizeInPackets) {
//        LOG(dvb, debug, "get packet number: " + Poco::NumberFormatter::format(_packetIndex));
        return _packetBlock[_packetIndex++];
    }
    else {
        return 0;
    }
}


const Poco::UInt8 TransportStreamPacket::SyncByte = 0x47;
const int TransportStreamPacket::Size = 188;
const int TransportStreamPacket::HeaderSize = 4;
const int TransportStreamPacket::PayloadSize = 188 - 4;

TransportStreamPacket::TransportStreamPacket(bool allocateData) :
_adaptionFieldSize(0),
_adaptionFieldPcrSet(false),
_adaptionFieldSplicingPointSet(false),
_refCounter(1),
_pPacketBlock(0)
{
    if (allocateData) {
        _data = new Poco::UInt8[Size];
        setBytes<Poco::UInt8>(0, SyncByte);
    }
}


TransportStreamPacket::~TransportStreamPacket()
{
    if (_data) {
        delete (Poco::UInt8*)_data;
    }
}


//void
//TransportStreamPacket::writePayloadFromStream(Stream* pStream, int timeout)
//{
//    pStream->read((Poco::UInt8*)_data + HeaderSize, PayloadSize, timeout);
//}


void
TransportStreamPacket::clearPayload()
{
    ::memset((Poco::UInt8*)(_data) + HeaderSize, 0xff, PayloadSize);
}


void
TransportStreamPacket::stuffPayload(int actualPayloadSize)
{
    ::memmove((Poco::UInt8*)(_data) + HeaderSize, (Poco::UInt8*)(_data) + HeaderSize + _adaptionFieldSize, actualPayloadSize);
    setStuffingBytes(PayloadSize - _adaptionFieldSize - actualPayloadSize);
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


Poco::UInt16
TransportStreamPacket::getPacketIdentifier()
{
    return getValue<Poco::UInt16>(11, 13);
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


void
TransportStreamPacket::setPointerField(Poco::UInt8 pointer)
{
    setBytes<Poco::UInt8>(HeaderSize, pointer);
}


void
TransportStreamPacket::setAdaptionFieldLength(Poco::UInt8 length)
{
    setValue<Poco::UInt8>(32, 8, length);
    _adaptionFieldSize = length + 1;
}


void
TransportStreamPacket::clearAllAdaptionFieldFlags()
{
    setBytes<Poco::UInt8>(5, 0);
    _adaptionFieldPcrSet = false;
    _adaptionFieldSplicingPointSet = false;
}


void
TransportStreamPacket::setDiscontinuityIndicator(bool discontinuity)
{
    setValue<Poco::UInt8>(40, 1, discontinuity ? 1 : 0);
}


void
TransportStreamPacket::setRandomAccessIndicator(bool randomAccess)
{
    setValue<Poco::UInt8>(41, 1, randomAccess ? 1 : 0);
}


void
TransportStreamPacket::setElementaryStreamPriorityIndicator(bool high)
{
    setValue<Poco::UInt8>(42, 1, high ? 1 : 0);
}


void
TransportStreamPacket::setPcrFlag(bool containsPcr)
{
    setValue<Poco::UInt8>(43, 1, containsPcr ? 1 : 0);
    _adaptionFieldPcrSet = true;
}


void
TransportStreamPacket::setOPcrFlag(bool containsOPcr)
{
    setValue<Poco::UInt8>(44, 1, containsOPcr ? 1 : 0);
    _adaptionFieldPcrSet = true;
}


void
TransportStreamPacket::setSplicingPointFlag(bool spliceCountdownPresent)
{
    setValue<Poco::UInt8>(45, 1, spliceCountdownPresent ? 1 : 0);
    _adaptionFieldSplicingPointSet = true;
}


void
TransportStreamPacket::setTransportPrivateDataFlag(bool privateDataPresent)
{
    setValue<Poco::UInt8>(46, 1, privateDataPresent ? 1 : 0);
}


void
TransportStreamPacket::setExtensionFlag(bool extensionPresent)
{
    setValue<Poco::UInt8>(47, 1, extensionPresent ? 1 : 0);
}


void
TransportStreamPacket::setPcr(Poco::UInt64 base, Poco::UInt8 padding, Poco::UInt16 extension)
{
    setValue<Poco::UInt64>(48, 33, base);
    setValue<Poco::UInt8>(81, 6, padding);
    setValue<Poco::UInt16>(87, 9, extension);
}


void
TransportStreamPacket::setSpliceCountdown(Poco::UInt8 countdown)
{
    if (_adaptionFieldPcrSet) {
        setValue<Poco::UInt8>(96, 8, countdown);
    }
    else {
        setValue<Poco::UInt8>(48, 8, countdown);
    }
}


void
TransportStreamPacket::setStuffingBytes(int count)
{
    if (_adaptionFieldPcrSet && _adaptionFieldSplicingPointSet) {
        ::memset((Poco::UInt8*)(_data) + 13, 0xff, count);
    }
    else if (_adaptionFieldPcrSet) {
        ::memset((Poco::UInt8*)(_data) + 12, 0xff, count);
    }
    else {
        ::memset((Poco::UInt8*)(_data) + 6, 0xff, count);
    }
}


}  // namespace Omm
}  // namespace Dvb
