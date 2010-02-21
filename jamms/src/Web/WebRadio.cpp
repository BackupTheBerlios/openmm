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

#include "WebRadio.h"

WebRadio::WebRadio()
{
    setTitle("Web Radio");
    
    appendChild("1", new Jamm::Av::MediaItem("SOMA FM - Groove Salad",
                "http://streamer-dtc-aa04.somafm.com:80/stream/1018"));
    appendChild("2", new Jamm::Av::MediaItem("SOMA FM - Indie Pop Rocks (Lush)",
                "http://streamer-ntc-aa02.somafm.com:80/stream/1073"));
    appendChild("3", new Jamm::Av::MediaItem("SOMA FM - Drone Zone",
                "http://streamer-dtc-aa01.somafm.com:80/stream/1032"));
    appendChild("4", new Jamm::Av::MediaItem("Digitally Imported - Chillout",
                "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035"));
    appendChild("5", new Jamm::Av::MediaItem("SWR DASDING",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3"));
    appendChild("6", new Jamm::Av::MediaItem("SWR DASDING Lautstark",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3"));
    appendChild("7", new Jamm::Av::MediaItem("SWR3",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3"));
    appendChild("8", new Jamm::Av::MediaItem("MotorFM",
                "http://www.motorfm.de/stream-berlin"));
    appendChild("9", new Jamm::Av::MediaItem("Freies Radio Stuttgart",
                "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    appendChild("10", new Jamm::Av::MediaItem("HoRadS",
                "http://realserver3.hdm-stuttgart.de:8080/horads"));
};