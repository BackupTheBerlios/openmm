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

#include "WebRadio.h"

MediaContainerPlugin::MediaContainerPlugin() :
MediaServerContainer("Web Radio", "musicContainer")
{
    std::string protInfoDlna = "audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01";
    std::string subClass = "audioItem.audioBroadcast";
    
    Omm::Av::MediaItem* pStation01 = new Omm::Av::MediaItem("01", "SOMA FM - Groove Salad", subClass);
    pStation01->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://streamer-dtc-aa04.somafm.com:80/stream/1018"));
    appendChild(pStation01);
    
    Omm::Av::MediaItem* pStation02 = new Omm::Av::MediaItem("02", "SOMA FM - Indie Pop Rocks (Lush)", subClass);
    pStation02->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://streamer-ntc-aa02.somafm.com:80/stream/1073"));
    appendChild(pStation02);
    
    Omm::Av::MediaItem* pStation03 = new Omm::Av::MediaItem("03", "SOMA FM - Drone Zone", subClass);
    pStation03->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://streamer-dtc-aa01.somafm.com:80/stream/1032"));
    appendChild(pStation03);
    
    Omm::Av::MediaItem* pStation04 = new Omm::Av::MediaItem("04", "Digitally Imported - Chillout", subClass);
    pStation04->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035"));
    appendChild(pStation04);
    
    Omm::Av::MediaItem* pStation05 = new Omm::Av::MediaItem("05", "MotorFM", subClass);
    pStation05->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://dude.mp3streams.net/motorfm"));
    appendChild(pStation05);
    
    Omm::Av::MediaItem* pStation06 = new Omm::Av::MediaItem("06", "Freies Radio Stuttgart", subClass);
    pStation06->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    appendChild(pStation06);
    
    Omm::Av::MediaItem* pStation07 = new Omm::Av::MediaItem("07", "HoRadS", subClass);
    pStation07->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://realserver3.hdm-stuttgart.de:8080/horads"));
    appendChild(pStation07);
    
//     Omm::Av::MediaItem* pStation08 = new Omm::Av::MediaItem("08", "SWR DASDING", subClass);
//     pStation08->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3"));
//     appendChild(pStation08);
//     
//     Omm::Av::MediaItem* pStation09 = new Omm::Av::MediaItem("09", "SWR DASDING Lautstark", subClass);
//     pStation09->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3"));
//     appendChild(pStation09);
    
//     Omm::Av::MediaItem* pStation10 = new Omm::Av::MediaItem("10", "SWR3", subClass);
//     pStation10->addResource(new Omm::Av::WebResource("r1", protInfoDlna, "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3"));
//     appendChild(pStation10);
};

POCO_BEGIN_MANIFEST(Omm::Av::MediaContainer)
POCO_EXPORT_CLASS(MediaContainerPlugin)
POCO_END_MANIFEST
