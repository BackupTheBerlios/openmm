/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#ifndef OmmDvb_INCLUDED
#define OmmDvb_INCLUDED

#include "UpnpAvTypes.h"

namespace Dvb {

// NOTE: This API is linux specific

class Channel : public Omm::Av::MediaObject
{
};


class DvbDevice
{
public:
    static DvbDevice* instance();
    
    
    
    std::ostream& getTransportStream(Channel* pChannel);
//     std::ostream& getPacketizedElementaryStream(Channel* pChannel);
//     std::ostream& getProgramStream(Channel* pChannel);
    
private:
    DvbDevice();
    
    static DvbDevice*     _pInstance;
};

}  // namespace Dvb

#endif
