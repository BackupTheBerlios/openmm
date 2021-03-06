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

#ifndef MEDIASERVER_IMPLEMENTATION_H
#define MEDIASERVER_IMPLEMENTATION_H

#include "Upnp.h"
#include "UpnpAvPrivate.h"
#include "UpnpAvObject.h"
#include "UpnpAvServer.h"
#include "UpnpAvDevices.h"


namespace Omm {
namespace Av {


class DevAVTransportServerImpl : public DevAVTransport
{
private:
    virtual void SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    virtual void GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus);
    virtual void GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed);
    virtual void GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount);
    virtual void GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes);
    virtual void GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode);
    virtual void Stop(const ui4& InstanceID);
    virtual void Play(const ui4& InstanceID, const std::string& Speed);
    virtual void Pause(const ui4& InstanceID);
    virtual void Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target);
    virtual void Next(const ui4& InstanceID);
    virtual void Previous(const ui4& InstanceID);

    virtual void initStateVars();
};


class DevConnectionManagerServerImpl : public DevConnectionManager, public ConnectionManager
{
    friend class MediaServer;

public:
    DevConnectionManagerServerImpl(Device* pThisDevice) : ConnectionManager(pThisDevice), _pThisDevice(pThisDevice) {}

private:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink);
    virtual void PrepareForConnection(const std::string& RemoteProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, i4& ConnectionID, i4& AVTransportID, i4& RcsID);
    virtual void ConnectionComplete(const i4& ConnectionID);
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    virtual void GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status);

    virtual void initStateVars();

    Device*                             _pThisDevice;
//    CsvList                             _connectionIds;
};


class DevContentDirectoryServerImpl : public DevContentDirectory
{
    friend class MediaServer;
    friend class DevMediaServer;

private:
    virtual void GetSearchCapabilities(std::string& SearchCaps);
    virtual void GetSortCapabilities(std::string& SortCaps);
    virtual void GetSystemUpdateID(ui4& Id);
    virtual void Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID);
    virtual void Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID);
    virtual void CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result);
    virtual void DestroyObject(const std::string& ObjectID);
    virtual void UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue);
    virtual void ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID);
    virtual void GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal);
    virtual void DeleteResource(const uri& ResourceURI);
    virtual void CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID);

    virtual void initStateVars();

    ServerContainer* _pRoot;
};

} // namespace Av
} // namespace Omm

#endif

