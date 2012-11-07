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
#include "ElementaryStream.h"


namespace Omm {
namespace Dvb {


const int ElementaryStreamPacket::_maxSize = 0x10000;

ElementaryStreamPacket::ElementaryStreamPacket() :
//_maxSize(0x10000),
_size(0)
{
    _data = new Poco::UInt8[_maxSize];
    setBytes<Poco::UInt16>(0, 0);
    setBytes<Poco::UInt8>(0, 1);
}


Poco::UInt8
ElementaryStreamPacket::getStreamId()
{
    return getBytes<Poco::UInt8>(3);
}


Poco::UInt16
ElementaryStreamPacket::getSize()
{
    return getBytes<Poco::UInt16>(4);
}


const int
ElementaryStreamPacket::getMaxSize()
{
    return _maxSize;
}


void*
ElementaryStreamPacket::getDataAfterStartcodePrefix()
{
    return (Poco::UInt8*)(_data) + 3;
}


void*
ElementaryStreamPacket::getDataStart()
{
    return (Poco::UInt8*)(_data) + 6;
}


bool
ElementaryStreamPacket::isAudio()
{
    return (getStreamId() >= 0xC0 && getStreamId() <= 0xDF);
}


bool
ElementaryStreamPacket::isVideo()
{
    return (getStreamId() >= 0xE0 && getStreamId() <= 0xEF);
}


}  // namespace Omm
}  // namespace Dvb
