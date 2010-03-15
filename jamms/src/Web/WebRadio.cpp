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
#include <Poco/ClassLibrary.h>

#include "WebRadio.h"

WebRadio::WebRadio()
{
    setTitle("Web Radio");
    m_properties.append("upnp:class", new Jamm::Variant(std::string("object.container.musicContainer")));
    
    std::string protInfoAny = "http-get:*:*:*";
    std::string protInfoMp3 = "http-get:*:audio/mpeg:*";
    std::string protInfoDlna = "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_FLAGS=8D100000000000000000000000000000";
    
    std::string subClass = "audioItem.audioBroadcast";
//     std::string subClass = "audioItem.musicTrack";
    
    appendChild("11", new Jamm::Av::MediaItem("SOMA FM - Groove Salad (any protocol)",
                                              "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoAny, 0, subClass));
    appendChild("12", new Jamm::Av::MediaItem("SOMA FM - Groove Salad (mp3)",
                                              "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoMp3, 0, subClass));
    
    appendChild("1", new Jamm::Av::MediaItem("SOMA FM - Groove Salad (dlna)",
                                             "http://streamer-dtc-aa04.somafm.com:80/stream/1018", protInfoDlna, 0, subClass));
    appendChild("2", new Jamm::Av::MediaItem("SOMA FM - Indie Pop Rocks (Lush)",
                                             "http://streamer-ntc-aa02.somafm.com:80/stream/1073", protInfoMp3, 0, subClass));
    appendChild("3", new Jamm::Av::MediaItem("SOMA FM - Drone Zone",
                                             "http://streamer-dtc-aa01.somafm.com:80/stream/1032", protInfoMp3, 0, subClass));
    appendChild("4", new Jamm::Av::MediaItem("Digitally Imported - Chillout",
                                             "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035", protInfoMp3, 0, subClass));
    appendChild("5", new Jamm::Av::MediaItem("SWR DASDING",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("6", new Jamm::Av::MediaItem("SWR DASDING Lautstark",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("7", new Jamm::Av::MediaItem("SWR3",
                                             "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3", protInfoMp3, 0, subClass));
    appendChild("8", new Jamm::Av::MediaItem("MotorFM",
                                             "http://www.motorfm.de/stream-berlin", protInfoMp3, 0, subClass));
    appendChild("9", new Jamm::Av::MediaItem("Freies Radio Stuttgart",
                                             "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    appendChild("10", new Jamm::Av::MediaItem("HoRadS",
                                              "http://realserver3.hdm-stuttgart.de:8080/horads"));
    
//     Jamm::Av::MediaContainer* pFavourites = new Jamm::Av::MediaContainer("Favourites");
//     appendChild("11", pFavourites);
//     pFavourites->appendChild("1", new Jamm::Av::MediaItem("SOMA FM - Groove Salad",
//                                              "http://streamer-dtc-aa04.somafm.com:80/stream/1018"));
//     pFavourites->appendChild("2", new Jamm::Av::MediaItem("SOMA FM - Indie Pop Rocks (Lush)",
//                                              "http://streamer-ntc-aa02.somafm.com:80/stream/1073"));
//     pFavourites->appendChild("3", new Jamm::Av::MediaItem("SOMA FM - Drone Zone",
//                                              "http://streamer-dtc-aa01.somafm.com:80/stream/1032"));
//     pFavourites->appendChild("4", new Jamm::Av::MediaItem("Digitally Imported - Chillout",
//                                              "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035"));
//     Jamm::Av::MediaContainer* pGoodies = new Jamm::Av::MediaContainer("Goodies");
//     appendChild("12", pGoodies);
};

POCO_BEGIN_MANIFEST(Jamm::Av::MediaContainer)
POCO_EXPORT_CLASS(WebRadio)
POCO_END_MANIFEST
