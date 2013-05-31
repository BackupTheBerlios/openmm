/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <Poco/NumberParser.h>

#include "UpnpAv.h"
#include "UpnpAvPrivate.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlObject.h"

namespace Omm {
namespace Av {

void
CtlAVTransportImpl::_ansSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansGetMediaInfo(const ui4& InstanceID, const ui4& NrTracks, const std::string& MediaDuration, const std::string& CurrentURI, const std::string& CurrentURIMetaData, const std::string& NextURI, const std::string& NextURIMetaData, const std::string& PlayMedium, const std::string& RecordMedium, const std::string& WriteStatus)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansGetTransportInfo(const ui4& InstanceID, const std::string& CurrentTransportState, const std::string& CurrentTransportStatus, const std::string& CurrentSpeed)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansGetPositionInfo(const ui4& InstanceID, const ui4& Track, const std::string& TrackDuration, const std::string& TrackMetaData, const std::string& TrackURI, const std::string& RelTime, const std::string& AbsTime, const i4& RelCount, const i4& AbsCount)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansGetDeviceCapabilities(const ui4& InstanceID, const std::string& PlayMedia, const std::string& RecMedia, const std::string& RecQualityModes)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansGetTransportSettings(const ui4& InstanceID, const std::string& PlayMode, const std::string& RecQualityMode)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansStop(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansPlay(const ui4& InstanceID, const std::string& Speed)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansPause(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansNext(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_ansPrevious(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
CtlAVTransportImpl::_changedLastChange(const std::string& val)
{
    LOG(upnpav, debug, "controller avtransport got last change: " + val);

    LastChangeReader lastChangeReader;
    lastChangeReader.read(val);

    // dispatcher code for last change state var
    for (LastChangeReader::ChangeSetIterator instanceIdIt = lastChangeReader.beginChangeSet(); instanceIdIt != lastChangeReader.endChangeSet(); ++instanceIdIt) {
        // instance id of change set can be handled here
        for (LastChangeSet::StateVarIterator it = (*instanceIdIt).beginStateVar(); it != (*instanceIdIt).endStateVar(); ++it) {
            std::string stateVarName = (*it).first;
            std::string val = (*it).second["val"];
            if (stateVarName == AvTransportEventedStateVar::TRANSPORT_STATE) {
                LOG(upnpav, debug, "controller, transport state changed to \"" + val + "\" on device: " + _pService->getDevice()->getUuid());
                if (_pMediaRenderer) {
                    _pMediaRenderer->newTransportState(val);
                    if (val == AvTransportArgument::TRANSPORT_STATE_STOPPED || val == AvTransportArgument::TRANSPORT_STATE_NO_MEDIA_PRESENT) {
                        _pMediaRenderer->newPosition(0.0, 0.0);
                    }
                }
            }
            else if (stateVarName == AvTransportEventedStateVar::CURRENT_TRACK_URI && val != "") {
                LOG(upnpav, debug, "controller, current track uri changed to \"" + val + "\" on device: " + _pService->getDevice()->getUuid());
                if (_pMediaRenderer) {
                    _pMediaRenderer->newUri(val);
                }
            }
            else if (stateVarName == AvTransportEventedStateVar::CURRENT_TRACK_META_DATA && val != AvTransportArgument::CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED) {
                CtlMediaObject object;
                MediaObjectReader reader;
                try {
                    reader.read(&object, val);
                }
                catch (Poco::Exception& e) {
                    LOG(upnpav, debug, "controller failed to parse current track meta data: " + e.displayText());
                    continue;
                }
                if (_pMediaRenderer) {
                    AbstractProperty* pArtist = object.getProperty(AvProperty::ARTIST);
                    AbstractProperty* pAlbum = object.getProperty(AvProperty::ALBUM);
                    std::string objectClass = object.getClass();
                    std::string uri = (object.getResource() ? object.getResource()->getUri() : "");
                    // FIXME: getServer() doesn't work with track metadata
                    std::string serverName = (object.getServer() ? object.getServer()->getFriendlyName() : "");
                    _pMediaRenderer->newTrack(object.getTitle(), pArtist ? pArtist->getValue() : "", pAlbum ? pAlbum->getValue() : "", objectClass, serverName, uri);
//                    if (object.getResource()) {
//                        std::string protInfoString = object.getResource()->getAttributeValue(AvProperty::PROTOCOL_INFO);
//                        std::string duration = object.getResource()->getAttributeValue(AvProperty::DURATION);
//                        if (duration != "") {
//                            LOG(upnpav, debug, "controller, current track meta data, duration: " + duration);
//                            try {
//                                r8 durationVal = AvTypeConverter::readDuration(duration);
//                                _pMediaRenderer->newPosition(durationVal, 0);
//                            }
//                            catch (Poco::Exception& e) {
//                                LOG(upnpav, error, "controller, could not parse track size: " + e.displayText());
//                            }
//                        }
//                        std::string size = object.getResource()->getAttributeValue(AvProperty::SIZE);
//                        if (size != "") {
//                            LOG(upnpav, debug, "controller, current track meta data, size: " + size);
//                            try {
//                                ui4 sizeVal = Poco::NumberParser::parse(size);
//                                _pMediaRenderer->newPosition(sizeVal, 0);
//                            }
//                            catch (Poco::Exception& e) {
//                                LOG(upnpav, error, "controller, could not parse track size: " + e.displayText());
//                            }
//                        }
//                    }
                }
            }
        }
    }
}


void
CtlConnectionManagerImpl::addConnection(Connection* pConnection, const std::string& protInfo)
{
    LOG(upnpav, debug, "controller, add connection");

    ConnectionPeer& thisPeer = pConnection->getThisPeer(_pThisDevice->getDeviceType());
    ConnectionPeer& remotePeer = pConnection->getRemotePeer(_pThisDevice->getDeviceType());

    i4 connectionId;
    i4 avTransportId;
    i4 rcId;
    try {
        PrepareForConnection(protInfo, remotePeer.getConnectionManagerId().toString(), -1, "Output", connectionId, avTransportId, rcId);
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "action \"PrepareForConnection\" not available on device");
    }

//    thisPeer._connectionId = connectionId;
//    thisPeer._AVTId = avTransportId;
//    thisPeer._RCId = rcId;
//
//    ConnectionManager::addConnection(pConnection, protInfo);
//
//    std::string connectionIds;
//    GetCurrentConnectionIDs(connectionIds);
//    LOG(upnpav, debug, "connection ids: " + connectionIds);
//    i4 RcsID;
//    i4 AVTransportID;
//    std::string ProtocolInfo;
//    std::string PeerConnectionManager;
//    i4 PeerConnectionID;
//    std::string Direction;
//    std::string Status;
//    GetCurrentConnectionInfo(connectionId, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
}


Connection*
CtlConnectionManagerImpl::getConnection(ui4 connectionId)
{
    LOG(upnpav, debug, "controller, get connection with id: " + Poco::NumberFormatter::format(connectionId));

    i4 RcsID;
    i4 AVTransportID;
    std::string ProtocolInfo;
    std::string PeerConnectionManager;
    i4 PeerConnectionID;
    std::string Direction;
    std::string Status;
    GetCurrentConnectionInfo(connectionId, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);

    ConnectionManagerId peerConnectionManager;
    peerConnectionManager.parseManagerIdString(PeerConnectionManager);
    Connection* pConnection = 0;
    if (_pDevice->getDeviceType() == DeviceType::MEDIA_RENDERER_1) {
//    if (peerConnectionManager.getServiceId() == ServiceType::CD_1) {
        pConnection = new Connection(peerConnectionManager.getUuid(), _pDevice->getUuid());
    }
    else if (_pDevice->getDeviceType() == DeviceType::MEDIA_SERVER_1) {
//    else if (peerConnectionManager.getServiceId() == ServiceType::AVT_1) {
        pConnection = new Connection(_pDevice->getUuid(), peerConnectionManager.getUuid());
    }
    return pConnection;
}


CsvList
CtlConnectionManagerImpl::getConnectionIds()
{
    std::string connectionIdString;
    GetCurrentConnectionIDs(connectionIdString);
    LOG(upnpav, debug, "controller connection ids: " + connectionIdString);

    return CsvList(connectionIdString);
}


int
CtlConnectionManagerImpl::getConnectionCount()
{
    return getConnectionIds().getSize();
}


void
CtlConnectionManagerImpl::_ansGetProtocolInfo(const std::string& Source, const std::string& Sink)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_ansPrepareForConnection(const std::string& RemoteProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, const i4& ConnectionID, const i4& AVTransportID, const i4& RcsID)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_ansConnectionComplete(const i4& ConnectionID)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_ansGetCurrentConnectionIDs(const std::string& ConnectionIDs)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_ansGetCurrentConnectionInfo(const i4& ConnectionID, const i4& RcsID, const i4& AVTransportID, const std::string& ProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, const std::string& Status)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_changedSourceProtocolInfo(const std::string& val)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_changedSinkProtocolInfo(const std::string& val)
{
// begin of your own code

// end of your own code
}


void
CtlConnectionManagerImpl::_changedCurrentConnectionIDs(const std::string& val)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansListPresets(const ui4& InstanceID, const std::string& CurrentPresetNameList)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetBrightness(const ui4& InstanceID, const ui2& CurrentBrightness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetContrast(const ui4& InstanceID, const ui2& CurrentContrast)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetSharpness(const ui4& InstanceID, const ui2& CurrentSharpness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetRedVideoGain(const ui4& InstanceID, const ui2& CurrentRedVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetGreenVideoGain(const ui4& InstanceID, const ui2& CurrentGreenVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetBlueVideoGain(const ui4& InstanceID, const ui2& CurrentBlueVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetRedVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentRedVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentGreenVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentBlueVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetColorTemperature (const ui4& InstanceID, const ui2& CurrentColorTemperature)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetHorizontalKeystone(const ui4& InstanceID, const i2& CurrentHorizontalKeystone)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetVerticalKeystone(const ui4& InstanceID, const i2& CurrentVerticalKeystone)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetMute(const ui4& InstanceID, const std::string& Channel, const bool& CurrentMute)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& CurrentVolume)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& CurrentVolume)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, const i2& MinValue, const i2& MaxValue)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansGetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& CurrentLoudness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_ansSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
// begin of your own code

// end of your own code
}


void
CtlRenderingControlImpl::_changedLastChange(const std::string& val)
{
    LOG(upnpav, debug, "controller rendering control got last change: " + val);

    LastChangeReader lastChangeReader;
    lastChangeReader.read(val);

    // dispatcher code for last change state var
    for (LastChangeReader::ChangeSetIterator instanceIdIt = lastChangeReader.beginChangeSet(); instanceIdIt != lastChangeReader.endChangeSet(); ++instanceIdIt) {
        for (LastChangeSet::StateVarIterator it = (*instanceIdIt).beginStateVar(); it != (*instanceIdIt).endStateVar(); ++it) {
            std::string stateVarName = (*it).first;
            std::string val = (*it).second["val"];
            // FIXME: InstanceID and Channel are not honored when storing the new evented value, device tree has only one instance (and one channel).
//            _pService->setStateVar<std::string>(stateVarName, val);
            if (stateVarName == RenderingControlEventedStateVar::VOLUME) {
                // set volume only when this renderer is selected.
                LOG(upnpav, debug, "controller, volume changed on device: " + _pService->getDevice()->getUuid());
                if (_pMediaRenderer) {
                    _pMediaRenderer->newVolume(Poco::NumberParser::parse(val));
                }
            }
        }
    }
}


void
CtlContentDirectoryImpl::_ansGetSearchCapabilities(const std::string& SearchCaps)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansGetSortCapabilities(const std::string& SortCaps)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansGetSystemUpdateID(const ui4& Id)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansCreateObject(const std::string& ContainerID, const std::string& Elements, const std::string& ObjectID, const std::string& Result)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansDestroyObject(const std::string& ObjectID)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansImportResource(const uri& SourceURI, const uri& DestinationURI, const ui4& TransferID)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansGetTransferProgress(const ui4& TransferID, const std::string& TransferStatus, const std::string& TransferLength, const std::string& TransferTotal)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansDeleteResource(const uri& ResourceURI)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_ansCreateReference(const std::string& ContainerID, const std::string& ObjectID, const std::string& NewID)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_changedTransferIDs(const std::string& val)
{
// begin of your own code

// end of your own code
}


void
CtlContentDirectoryImpl::_changedSystemUpdateID(const ui4& val)
{
    LOG(upnpav, debug, "event handler for ContentDirectory::SystemUpdateID gets: " + Poco::NumberFormatter::format(val));
    _pMediaServer->newSystemUpdateId(val);
}


void
CtlContentDirectoryImpl::_changedContainerUpdateIDs(const std::string& val)
{
// begin of your own code

// end of your own code
}

} // namespace Av
} // namespace Omm

