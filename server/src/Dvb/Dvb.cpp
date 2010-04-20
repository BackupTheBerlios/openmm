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
#include <Poco/ClassLibrary.h>
#include <Poco/File.h>
#include "Dvb.h"


MediaContainerPlugin::MediaContainerPlugin() :
MediaServerContainer("Digital TV")
{
    Omm::Av::MediaItem* pRtl = new Omm::Av::MediaItem("o2", "RTL TS", "videoItem.movie");
    DvbChannel rtl("");
    pRtl->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", &rtl));
    appendChild(pRtl);
    
//     Omm::Av::MediaItem* pRtl_TS = new Omm::Av::MediaItem("o2", "RTL TS", "videoItem.movie");
//     pRtl_TS->addResource(new Omm::Av::FileResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", 5221888, "/home/jb/devel/cc/ommtest/dvb_streams/dvb-s/vdr-1.6.0-live-ts-rtl.dvb"));
//     appendChild(pRtl_TS);
//     
//     Omm::Av::MediaItem* pRtl_PES = new Omm::Av::MediaItem("o3", "RTL PES", "videoItem.movie");
//     pRtl_PES->addResource(new Omm::Av::FileResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", 5620944, "/home/jb/devel/cc/ommtest/dvb_streams/dvb-s/vdr-1.6.0-live-pes-rtl.dvb"));
//     appendChild(pRtl_PES);
};


DvbResource::DvbResource(const std::string& resourceId, const std::string& protInfo, DvbChannel* pChannel) :
ServerResource(resourceId, protInfo, 0),
_pChannel(pChannel)
{
}


std::streamsize
DvbResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    std::istream& istr = DvbDevice::instance()->getTransportStream(_pChannel);
    return Poco::StreamCopier::copyStream(istr, ostr);
}


DvbChannel::DvbChannel(const std::string& mumuParams)
{
//     read_tuning_configuration(&_tuningParams, mumuParams.c_str());
}


DvbDevice* DvbDevice::_pInstance = 0;

DvbDevice::DvbDevice() :
_dvbAdapter("/dev/dvb/adapter0/")
{
}


DvbDevice*
DvbDevice::instance()
{
    if (!_pInstance) {
        _pInstance = new DvbDevice;
    }
    return _pInstance;
}



void
DvbDevice::tune(DvbChannel* pChannel)
{
}


std::istream&
DvbDevice::getTransportStream(DvbChannel* pChannel)
{
    tune(pChannel);
    return _dvbAdapter;
}


POCO_BEGIN_MANIFEST(Omm::Av::MediaContainer)
POCO_EXPORT_CLASS(MediaContainerPlugin)
POCO_END_MANIFEST
