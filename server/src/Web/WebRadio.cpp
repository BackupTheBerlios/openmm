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
MediaContainer("Web Radio", "musicContainer")
{
    std::string protInfoAny = "http-get:*:*:*";
    std::string protInfoMp3 = "http-get:*:audio/mpeg:*";
    std::string protInfoDlna = "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_FLAGS=8D100000000000000000000000000000";
    
    std::string subClass = "audioItem.audioBroadcast";
    
    appendChild("11", new Omm::Av::MediaItem("SOMA FM - Groove Salad (any protocol)",
                                              "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoAny, 0, subClass));
    appendChild("12", new Omm::Av::MediaItem("SOMA FM - Groove Salad (mp3)",
                                              "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoMp3, 0, subClass));
    
    appendChild("1", new Omm::Av::MediaItem("SOMA FM - Groove Salad (dlna)",
                                             "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoDlna, 0, subClass));
    appendChild("2", new Omm::Av::MediaItem("SOMA FM - Indie Pop Rocks (Lush)",
                                             "http://streamer-ntc-aa02.somafm.com:80/stream/1073", protInfoMp3, 0, subClass));
    appendChild("3", new Omm::Av::MediaItem("SOMA FM - Drone Zone",
                                             "http://streamer-dtc-aa01.somafm.com:80/stream/1032", protInfoMp3, 0, subClass));
    appendChild("4", new Omm::Av::MediaItem("Digitally Imported - Chillout",
                                             "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035", protInfoMp3, 0, subClass));
    appendChild("5", new Omm::Av::MediaItem("SWR DASDING",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("6", new Omm::Av::MediaItem("SWR DASDING Lautstark",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("7", new Omm::Av::MediaItem("SWR3",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("8", new Omm::Av::MediaItem("MotorFM",
                                             "http://www.motorfm.de/stream-berlin", protInfoMp3, 0, subClass));
    appendChild("9", new Omm::Av::MediaItem("Freies Radio Stuttgart",
                                             "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    appendChild("10", new Omm::Av::MediaItem("HoRadS",
                                              "http://realserver3.hdm-stuttgart.de:8080/horads"));
};

POCO_BEGIN_MANIFEST(Omm::Av::MediaContainer)
POCO_EXPORT_CLASS(MediaContainerPlugin)
POCO_END_MANIFEST
