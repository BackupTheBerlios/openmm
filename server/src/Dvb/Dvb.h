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

#include <Omm/UpnpAvServer.h>

// DVB includes:
// #include <linux/dvb/dmx.h>
// #include <linux/dvb/frontend.h>

// #include "mumudvb/tune.h"

class DvbDevice;
class DvbChannel;

class MediaContainerPlugin : public Omm::Av::MediaServerContainer
{
public:
    MediaContainerPlugin();
};


class DvbResource : public Omm::Av::ServerResource
{
public:
    DvbResource(const std::string& resourceId, const std::string& protInfo, DvbChannel* pChannel);
    
    virtual bool isSeekable() { return false; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
private:
    DvbChannel*         _pChannel;
};


// NOTE: The following API is linux specific
class DvbChannel
{
public:
    DvbChannel(const std::string& mumuParams);
    
private:
//     tuning_parameters_t    _tuningParams;
};


class DvbDevice
{
public:
    static DvbDevice* instance();
    
    void tune(DvbChannel* pChannel);
    std::istream& getTransportStream(DvbChannel* pChannel);
//     std::ostream& getPacketizedElementaryStream(Channel* pChannel);
//     std::ostream& getProgramStream(Channel* pChannel);
    
    
private:
    DvbDevice();
    
    static DvbDevice*     _pInstance;
    std::ifstream         _dvbAdapter;
};

#endif
