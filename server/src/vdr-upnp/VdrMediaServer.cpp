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

#include <vdr/channels.h>
#include <vdr/recording.h>

#include "VdrMediaServer.h"


VdrChannels::VdrChannels() :
MediaServerContainer("VDR Channels")
{
    for (cChannel* pChan = Channels.First(); pChan; pChan = Channels.Next(pChan)) {
        if (pChan->Vpid() == 0 || pChan->GetChannelID().ToString() == "0-0-0-0") {
            continue;
        }
        
        std::string objectId(pChan->GetChannelID().ToString());
        std::string title(pChan->Name());
        
        Omm::Av::MediaItem* pChanItem = new Omm::Av::MediaItem(objectId, title, "videoItem.movie");
        pChanItem->addResource(new Omm::Av::WebResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL",
            "http://localhost:3000/TS/" + objectId));
        appendChild(pChanItem);
    }
}


VdrRecordings::VdrRecordings() :
MediaServerContainer("VDR Recordings")
{
    for (cRecording* pRec = Recordings.First(); pRec; pRec = Recordings.Next(pRec)) {
        std::string objectId(pRec->Info()->ChannelID().ToString());
        objectId += "_" + Poco::NumberFormatter::format(pRec->start);
        std::string title(pRec->Name());
        
        Omm::Av::MediaItem* pRecItem = new Omm::Av::MediaItem(objectId, title, "videoItem.movie");
        pRecItem->addResource(new Omm::Av::FileResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL",
            0, std::string(pRec->FileName()) + "/001.vdr"));
        appendChild(pRecItem);
    }
}
