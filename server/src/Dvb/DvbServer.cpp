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

#include "DvbServer.h"

DvbServer::DvbServer() :
MediaServerContainer("Digital TV")
{
    Omm::Dvb::DvbAdapter* pAdapter = new Omm::Dvb::DvbAdapter(0);
    Omm::Dvb::DvbDevice::instance()->addAdapter(pAdapter);
    
    Omm::Av::MediaItem* pRtl = new Omm::Av::MediaItem("o1", "RTL", "videoItem.movie");
    Omm::Dvb::DvbChannel* pRtlChannel = new Omm::Dvb::DvbChannel(0, 12188000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 163, 104, 12003);
    pRtl->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pRtlChannel));
    appendChild(pRtl);
    
    Omm::Av::MediaItem* pRtl2 = new Omm::Av::MediaItem("o2", "RTL2", "videoItem.movie");
    Omm::Dvb::DvbChannel* pRtl2Channel = new Omm::Dvb::DvbChannel(0, 12188000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 166, 128, 12020);
    pRtl2->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pRtl2Channel));
    appendChild(pRtl2);
    
    Omm::Av::MediaItem* pVox = new Omm::Av::MediaItem("o3", "VOX", "videoItem.movie");
    Omm::Dvb::DvbChannel* pVoxChannel = new Omm::Dvb::DvbChannel(0, 12188000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 167, 136, 12060);
    pVox->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pVoxChannel));
    appendChild(pVox);
    
    Omm::Av::MediaItem* pPro7 = new Omm::Av::MediaItem("o4", "Pro7", "videoItem.movie");
    Omm::Dvb::DvbChannel* pPro7Channel = new Omm::Dvb::DvbChannel(0, 12544000, Omm::Dvb::DvbChannel::HORIZ, 22000000, 511, 512, 17501);
    pPro7->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pPro7Channel));
    appendChild(pPro7);
    
    Omm::Av::MediaItem* pSat1 = new Omm::Av::MediaItem("o5", "Sat1", "videoItem.movie");
    Omm::Dvb::DvbChannel* pSat1Channel = new Omm::Dvb::DvbChannel(0, 12544000, Omm::Dvb::DvbChannel::HORIZ, 22000000, 255, 256, 17500);
    pSat1->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pSat1Channel));
    appendChild(pSat1);
    
    Omm::Av::MediaItem* pKabel1 = new Omm::Av::MediaItem("o6", "Kabel1", "videoItem.movie");
    Omm::Dvb::DvbChannel* pKabel1Channel = new Omm::Dvb::DvbChannel(0, 12544000, Omm::Dvb::DvbChannel::HORIZ, 22000000, 767, 768, 17502);
    pKabel1->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pKabel1Channel));
    appendChild(pKabel1);
    
    Omm::Av::MediaItem* pArd = new Omm::Av::MediaItem("o7", "ARD", "videoItem.movie");
    Omm::Dvb::DvbChannel* pArdChannel = new Omm::Dvb::DvbChannel(0, 11837000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 101, 102, 28106);
    pArd->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pArdChannel));
    appendChild(pArd);
    
    Omm::Av::MediaItem* pZdf = new Omm::Av::MediaItem("o8", "ZDF", "videoItem.movie");
    Omm::Dvb::DvbChannel* pZdfChannel = new Omm::Dvb::DvbChannel(0, 11954000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 110, 120, 28006);
    pZdf->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pZdfChannel));
    appendChild(pZdf);
    
    Omm::Av::MediaItem* p3Sat = new Omm::Av::MediaItem("o9", "3Sat", "videoItem.movie");
    Omm::Dvb::DvbChannel* p3SatChannel = new Omm::Dvb::DvbChannel(0, 11954000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 210, 220, 28007);
    p3Sat->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", p3SatChannel));
    appendChild(p3Sat);
    
    Omm::Av::MediaItem* pZdfInfo = new Omm::Av::MediaItem("o10", "ZDF Info", "videoItem.movie");
    Omm::Dvb::DvbChannel* pZdfInfoChannel = new Omm::Dvb::DvbChannel(0, 11954000, Omm::Dvb::DvbChannel::HORIZ, 27500000, 610, 620, 28011);
    pZdfInfo->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pZdfInfoChannel));
    appendChild(pZdfInfo);
};


DvbResource::DvbResource(const std::string& resourceId, const std::string& protInfo, Omm::Dvb::DvbChannel* pChannel) :
ServerResource(resourceId, protInfo, 0),
_pChannel(pChannel)
{
}


std::streamsize
DvbResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    Omm::Dvb::DvbDevice::instance()->tune(_pChannel);
    
    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
    std::ifstream istr("/dev/dvb/adapter0/dvr0");
//     std::istream& istr = DvbDevice::instance()->getTransportStream(_pChannel);
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
    Omm::Dvb::DvbDevice::instance()->stopTune();
    return bytes;
}


POCO_BEGIN_MANIFEST(Omm::Av::MediaContainer)
POCO_EXPORT_CLASS(DvbServer)
POCO_END_MANIFEST
