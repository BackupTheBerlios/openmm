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
    m_properties.append("upnp:class", new Jamm::Variant(std::string("object.container.musicContainer")));
    
    m_pFileServer = new Jamm::HttpFileServer;
    m_pFileServer->start();
    m_serverPort = m_pFileServer->getPort();
    m_serverAddress =  Jamm::NetworkInterfaceManager::instance()->getValidInterfaceAddress().toString();
    
    std::string objectId("1");
    std::string title("Hurricane");
    std::string fileName("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");
    std::string resource = "http://" +
        m_serverAddress + ":" +
        Poco::NumberFormatter::format(m_serverPort) + "/" + objectId;
    m_pFileServer->registerFile(objectId, fileName);
    
    Poco::File file(fileName);
    Jamm::ui4 size = file.getSize();
    std::string protInfoDlna =
        "http-get:*:audio/mpeg:DLNA.ORG_PS=1;DLNA.ORG_CI=0;DLNA.ORG_OP=01;DLNA.ORG_PN=MP3;DLNA.ORG_FLAGS=01700000000000000000000000000000";
    std::string protInfoMiniDlna =
        "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01";
//     std::string protInfoDlna = "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_FLAGS=8D100000000000000000000000000000";
//     std::string subClass = "audioItem.audioBroadcast";
    std::string subClass = "audioItem.musicTrack";
    
    Jamm::Av::MediaItem* pKyuss = new Jamm::Av::MediaItem(title, resource, protInfoMiniDlna, size, subClass);
    pKyuss->setProperty("dc:creator", "Kyuss");
    pKyuss->setProperty("dc:date", "2005-01-01");
    pKyuss->setProperty("upnp:artist", "Kyuss");
    appendChild(objectId, pKyuss);
    

};

POCO_BEGIN_MANIFEST(Jamm::Av::MediaContainer)
POCO_EXPORT_CLASS(Filesystem)
POCO_END_MANIFEST
