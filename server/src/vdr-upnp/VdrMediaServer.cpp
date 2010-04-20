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
#include <vdr/device.h>
#include <vdr/receiver.h>

#include "VdrMediaServer.h"


class VdrChannelResource : public Omm::Av::ServerResource
{
public:
    VdrChannelResource(const std::string& resourceId, const std::string& protInfo, cChannel* pChannel);
    
    virtual bool isSeekable() { return false; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
private:
    cChannel*         _pChannel;
};


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
        pChanItem->addResource(new VdrChannelResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL", pChan));
        
//         pChanItem->addResource(new Omm::Av::WebResource("r1", "video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL",
//             "http://localhost:3000/TS/" + objectId));
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


class VdrChannelReceiver : public cReceiver
{
public:
    VdrChannelReceiver(std::ostream& ostr, tChannelID ChannelID, int Priority, int Pid, const int *Pids1 = NULL, const int *Pids2 = NULL, const int *Pids3 = NULL);
    
    virtual void Receive(uchar *Data, int Length);
    std::streamsize bytesReceived() { return _bytesReceived; }
    
private:
    std::ostream& _ostr;
    std::streamsize _bytesReceived;
};


void
VdrChannelReceiver::Receive(uchar *Data, int Length)
{
    _ostr.write((char*) Data, Length);
    _bytesReceived += Length;
}


VdrChannelReceiver::VdrChannelReceiver(std::ostream& ostr, tChannelID ChannelID, int Priority, int Pid, const int *Pids1, const int *Pids2, const int *Pids3) :
cReceiver(ChannelID, Priority, Pid, Pids1, Pids2, Pids3),
_ostr(ostr),
_bytesReceived(0)
{
}


VdrChannelResource::VdrChannelResource(const std::string& resourceId, const std::string& protInfo, cChannel* pChannel) :
ServerResource(resourceId, protInfo, 0),
_pChannel(pChannel)
{
}


std::streamsize
VdrChannelResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    isyslog("start streaming ...");
    cDevice* pDevice = cDevice::GetDevice(_pChannel, 0, false);
    pDevice->SwitchChannel(_pChannel, false);
    int pids[3] = {_pChannel->Vpid(), _pChannel->Apid(0), 0};
    VdrChannelReceiver receiver(ostr, _pChannel->GetChannelID(), 0, 0, pids);
    pDevice->AttachReceiver(&receiver);
    // FIXME: do a select or sth. similar and don't poll from end of stream
    while (ostr.good()) {
        isyslog("waiting for client to close ...");
        Poco::Thread::sleep(1000);
    }
    pDevice->Detach(&receiver);
    isyslog("sent %d bytes.", receiver.bytesReceived());
    return receiver.bytesReceived();
}



