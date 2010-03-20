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

Filesystem::Filesystem()
{
    setTitle("Collection");
    m_properties.append("upnp:class", new Jamm::Variant(std::string("object.container")));
    
    m_pFileServer = new Jamm::Av::MediaItemServer;
    m_pFileServer->start();
    m_serverPort = m_pFileServer->getPort();
    m_serverAddress =  Jamm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    
    Jamm::Av::MediaItem* pKyuss = new Jamm::Av::MediaItem("Hurricane", 
        "http://" + m_serverAddress + ":" + Poco::NumberFormatter::format(m_serverPort) + "/1",
        "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01",
        3895296, "audioItem.musicTrack");
    appendChild("1", pKyuss);
    m_pFileServer->registerMediaItem("1", pKyuss, "file:/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");
    
    
    Jamm::Av::MediaItem* pRtl = new Jamm::Av::MediaItem("RTL", 
        "http://" + m_serverAddress + ":" + Poco::NumberFormatter::format(m_serverPort) + "/2",
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL",
        0, "videoItem.movie");
    appendChild("2", pRtl);
    m_pFileServer->registerMediaItem("2", pRtl, "http://192.168.178.23:3000/TS/1");
};

POCO_BEGIN_MANIFEST(Jamm::Av::MediaContainer)
POCO_EXPORT_CLASS(Filesystem)
POCO_END_MANIFEST
