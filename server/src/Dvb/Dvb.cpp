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
    Omm::Av::MediaItem* pRtl = new Omm::Av::MediaItem("o1", "RTL", "videoItem.movie");
    DvbChannel* pRtlChannel = new DvbChannel(0, 12188000, 0, 27500000, 163, 104, 12003);
    pRtl->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pRtlChannel));
    appendChild(pRtl);
    
    Omm::Av::MediaItem* pRtl2 = new Omm::Av::MediaItem("o2", "RTL2", "videoItem.movie");
    DvbChannel* pRtl2Channel = new DvbChannel(0, 12188000, 0, 27500000, 166, 128, 12020);
    pRtl2->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pRtl2Channel));
    appendChild(pRtl2);
    
    Omm::Av::MediaItem* pVox = new Omm::Av::MediaItem("o3", "VOX", "videoItem.movie");
    DvbChannel* pVoxChannel = new DvbChannel(0, 12188000, 0, 27500000, 167, 136, 12060);
    pVox->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pVoxChannel));
    appendChild(pVox);
    
    Omm::Av::MediaItem* pPro7 = new Omm::Av::MediaItem("o4", "Pro7", "videoItem.movie");
    DvbChannel* pPro7Channel = new DvbChannel(0, 12544000, 0, 22000000, 511, 512, 17501);
    pPro7->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pPro7Channel));
    appendChild(pPro7);
    
    Omm::Av::MediaItem* pSat1 = new Omm::Av::MediaItem("o5", "Sat1", "videoItem.movie");
    DvbChannel* pSat1Channel = new DvbChannel(0, 12544000, 0, 22000000, 255, 256, 17500);
    pSat1->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pSat1Channel));
    appendChild(pSat1);
    
    Omm::Av::MediaItem* pKabel1 = new Omm::Av::MediaItem("o6", "Kabel1", "videoItem.movie");
    DvbChannel* pKabel1Channel = new DvbChannel(0, 12544000, 0, 22000000, 767, 768, 17502);
    pKabel1->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pKabel1Channel));
    appendChild(pKabel1);
    
    Omm::Av::MediaItem* pArd = new Omm::Av::MediaItem("o7", "ARD", "videoItem.movie");
    DvbChannel* pArdChannel = new DvbChannel(0, 11837000, 0, 27500000, 101, 102, 28106);
    pArd->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pArdChannel));
    appendChild(pArd);
    
    Omm::Av::MediaItem* pZdf = new Omm::Av::MediaItem("o8", "ZDF", "videoItem.movie");
    DvbChannel* pZdfChannel = new DvbChannel(0, 11954000, 0, 27500000, 110, 120, 28006);
    pZdf->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pZdfChannel));
    appendChild(pZdf);
    
    Omm::Av::MediaItem* p3Sat = new Omm::Av::MediaItem("o9", "3Sat", "videoItem.movie");
    DvbChannel* p3SatChannel = new DvbChannel(0, 11954000, 0, 27500000, 210, 220, 28007);
    p3Sat->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", p3SatChannel));
    appendChild(p3Sat);
    
    Omm::Av::MediaItem* pZdfInfo = new Omm::Av::MediaItem("o10", "ZDF Info", "videoItem.movie");
    DvbChannel* pZdfInfoChannel = new DvbChannel(0, 11954000, 0, 27500000, 610, 620, 28011);
    pZdfInfo->addResource(new DvbResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pZdfInfoChannel));
    appendChild(pZdfInfo);
    
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
//     std::istream& istr = DvbDevice::instance()->getTransportStream(_pChannel);
    std::clog << "start tuning ..." << std::endl;
    DvbDevice::instance()->tune(_pChannel);

    std::clog << "reading from dvr device ..." << std::endl;
    std::ifstream istr("/dev/dvb/adapter0/dvr0");
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
    DvbDevice::instance()->stopTune();
    std::clog << "stopped tuning." << std::endl;
    return bytes;
}


DvbChannel::DvbChannel(unsigned int sat_no, unsigned int freq, unsigned int pol, unsigned int symbol_rate, unsigned int vpid, unsigned int apid, int sid) :
_sat_no(sat_no),
_freq(freq),
_pol(pol),
_symbol_rate(symbol_rate),
_vpid(vpid),
_apid(apid),
_sid(sid)
{
}


DvbDevice* DvbDevice::_pInstance = 0;

DvbDevice::DvbDevice()
// _dvbAdapter("/dev/dvb/adapter0")
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
    _pt = new TuningThread(pChannel);
    _t.start(*_pt);
}


void
DvbDevice::stopTune()
{
    _pt->stop();
    delete _pt;
}


TuningThread::TuningThread(DvbChannel* pChannel) :
_pChannel(pChannel)
{
}

void
TuningThread::run()
{
    std::clog << "starting tuning thread ..." << std::endl;
//     Poco::ScopedLock<Poco::FastMutex> lock(DvbDevice::instance()->_tuneLock);
    
    set_exit_after_tuning(0);
    
    int dvr = 1;
    int rec_psi = 1; // 1 - add pat and pmt to TS recording (implies -r)
    int bypass = 0; // 1 - enable Audio Bypass (default no)
    int human_readable = 0; // 1 - human readable output
    
    zap_to(0, 0, 0, _pChannel->_sat_no, _pChannel->_freq,
           _pChannel->_pol, _pChannel->_symbol_rate, _pChannel->_vpid, _pChannel->_apid, _pChannel->_sid,
           dvr, rec_psi, bypass, human_readable);
    std::clog << "tuning thread finished." << std::endl;
}


void
TuningThread::stop()
{
    std::clog << "stopping tuning thread ..." << std::endl;
    set_exit_after_tuning(1);
}

// std::istream&
// DvbDevice::getTransportStream(DvbChannel* pChannel)
// {
//     tune(pChannel);
// //     return _dvbAdapter;
// }


POCO_BEGIN_MANIFEST(Omm::Av::MediaContainer)
POCO_EXPORT_CLASS(MediaContainerPlugin)
POCO_END_MANIFEST
