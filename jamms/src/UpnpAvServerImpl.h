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
#ifndef MEDIASERVER_IMPLEMENTATION_H
#define MEDIASERVER_IMPLEMENTATION_H

#include <Jamm/Upnp.h>
#include <Jamm/UpnpAvTypes.h>
#include <Jamm/UpnpAvDevices.h>


class AVTransportImplementation : public Jamm::Av::AVTransport
{
private:
    virtual void SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    virtual void GetMediaInfo(const Jamm::ui4& InstanceID, Jamm::ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus);
    virtual void GetTransportInfo(const Jamm::ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed);
    virtual void GetPositionInfo(const Jamm::ui4& InstanceID, Jamm::ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, Jamm::i4& RelCount, Jamm::i4& AbsCount);
    virtual void GetDeviceCapabilities(const Jamm::ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes);
    virtual void GetTransportSettings(const Jamm::ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode);
    virtual void Stop(const Jamm::ui4& InstanceID);
    virtual void Play(const Jamm::ui4& InstanceID, const std::string& Speed);
    virtual void Pause(const Jamm::ui4& InstanceID);
    virtual void Seek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target);
    virtual void Next(const Jamm::ui4& InstanceID);
    virtual void Previous(const Jamm::ui4& InstanceID);

    virtual void initStateVars();
};

class ConnectionManagerImplementation : public Jamm::Av::ConnectionManager
{
private:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink);
    virtual void ConnectionComplete(const Jamm::i4& ConnectionID);
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    virtual void GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status);

    virtual void initStateVars();
};

class ContentDirectoryImplementation : public Jamm::Av::ContentDirectory
{
private:
    virtual void GetSearchCapabilities(std::string& SearchCaps);
    virtual void GetSortCapabilities(std::string& SortCaps);
    virtual void GetSystemUpdateID(Jamm::ui4& Id);
    virtual void Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const Jamm::ui4& StartingIndex, const Jamm::ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, Jamm::ui4& NumberReturned, Jamm::ui4& TotalMatches, Jamm::ui4& UpdateID);
    virtual void Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const Jamm::ui4& StartingIndex, const Jamm::ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, Jamm::ui4& NumberReturned, Jamm::ui4& TotalMatches, Jamm::ui4& UpdateID);
    virtual void CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result);
    virtual void DestroyObject(const std::string& ObjectID);
    virtual void UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue);
    virtual void ImportResource(const Jamm::uri& SourceURI, const Jamm::uri& DestinationURI, Jamm::ui4& TransferID);
    virtual void GetTransferProgress(const Jamm::ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal);
    virtual void DeleteResource(const Jamm::uri& ResourceURI);
    virtual void CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID);

    virtual void initStateVars();
};

#endif

