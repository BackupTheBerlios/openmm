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

#ifndef ElementaryStream_INCLUDED
#define ElementaryStream_INCLUDED

#include "DvbUtil.h"

namespace Omm {
namespace Dvb {


class ElementaryStreamPacket : public BitField
{
public:
    ElementaryStreamPacket();

    Poco::UInt8 getStreamId();
    Poco::UInt16 getSize();
    static const int getMaxSize();
    void* getDataAfterStartcodePrefix();
    void* getDataStart();

    bool isAudio();
    bool isVideo();

private:
    static const int    _maxSize;
    Poco::UInt16        _size;
};


}  // namespace Omm
}  // namespace Dvb

#endif
