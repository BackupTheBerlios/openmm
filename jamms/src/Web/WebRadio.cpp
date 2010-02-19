/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
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
    setObjectId("1");
    setIsContainer(true);
    setTitle("Web Radio Stations");
    
    appendChild("2", new RadioStation("SOMA FM - Groove Salad",
                "http://streamer-dtc-aa04.somafm.com:80/stream/1018"));
    appendChild("3", new RadioStation("SOMA FM - Indie Pop Rocks (Lush)",
                "http://streamer-ntc-aa02.somafm.com:80/stream/1073"));
    appendChild("4", new RadioStation("SOMA FM - Drone Zone",
                "http://streamer-dtc-aa01.somafm.com:80/stream/1032"));
    appendChild("5", new RadioStation("Digitally Imported - Chillout",
                "http://scfire-ntc-aa01.stream.aol.com:80/stream/1035"));
    appendChild("6", new RadioStation("SWR DASDING",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdinglive/livestream.mp3"));
    appendChild("7", new RadioStation("SWR DASDING Lautstark",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net:80/swrdasdingraka01/livestream.mp3"));
    appendChild("8", new RadioStation("SWR3",
                "http://edge.live.mp3.mdn.newmedia.nacamar.net/swr3live/livestream.mp3"));
    appendChild("9", new RadioStation("MotorFM",
                "http://www.motorfm.de/stream-berlin"));
    appendChild("10", new RadioStation("Freies Radio Stuttgart",
                "http://frs.kumbi.org:8000/frs_stereo.ogg"));
    appendChild("11", new RadioStation("HoRadS",
                "http://realserver3.hdm-stuttgart.de:8080/horads"));
    
    m_childCount = m_children.size();
}



RadioStation::RadioStation(std::string name, std::string mrl)
{
    setTitle(name);
    setResource(mrl);
}
