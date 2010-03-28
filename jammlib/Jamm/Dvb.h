/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009 - 2010                                                |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/
#ifndef JammDvb_INCLUDED
#define JammDvb_INCLUDED

#include "UpnpAvTypes.h"

namespace Dvb {

// NOTE: This API is linux specific

class Channel : public Jamm::Av::MediaObject
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
    
    static DvbDevice*     m_pInstance;
};

}  // namespace Dvb

#endif
