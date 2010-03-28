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

#include <Poco/File.h>
#include "Filesystem.h"

MediaContainerPlugin::MediaContainerPlugin() :
MediaServerContainer("Collection")
{
    // FileObjectSource::addAllFilesInDirectory("/home/jb/mp3/current/");
    // FileObjectSource::setDirectory("/home/jb/mp3/current/");
    
    // let FileObjectSource add an object:
    // pKyuss = FileObjectSource::addFile("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");
    // und
    // pStream = FileObjectSource::getStream(objectId, resourceId, seek)
    
    Jamm::Av::MediaItem* pKyuss = new Jamm::Av::MediaItem("o1", "Hurricane", "audioItem.musicTrack");
    pKyuss->addResource("r1", "audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01", 3895296);
    appendChild(pKyuss);
    
//     Jamm::Av::MediaItem* pKyuss = new Jamm::Av::MediaItem("Hurricane", "audioItem.musicTrack");
//     pKyuss->addResource("file:/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3", "audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01",
//         3895296);
//     appendChild("1", pKyuss);
    
//     m_pFileServer = new Jamm::Av::MediaItemServer;
//     m_pFileServer->start();
//     int localPort = m_pFileServer->getPort();
//     std::string localAddress =  Jamm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();

//     Jamm::Av::MediaItem* pRtl = new Jamm::Av::MediaItem("RTL",
// //         "http://" + localAddress + ":" + Poco::NumberFormatter::format(localPort) + "/2",
//         "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL",
//         0, "videoItem.movie");
//     appendChild("2", pRtl, , "http://192.168.178.23:3000/TS/1");
// //     m_pFileServer->registerMediaItem("2", pRtl, "http://192.168.178.23:3000/TS/1");
};


// std::istream*
// MediaContainerPlugin::getStream(const std::string& objectId, const std::string& resourceId, std::iostream::pos_type seek)
// {
//     std::ofstream* pStream;
//     if (objectId == "o1" && resourceId == "r1") {
//         pStream = new std::ifstream("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");
//         if (seek) {
//             pStream->seekg(seek);
//         }
//     }
//     return pStream;
// }


POCO_BEGIN_MANIFEST(Jamm::Av::MediaContainer)
POCO_EXPORT_CLASS(MediaContainerPlugin)
POCO_END_MANIFEST
