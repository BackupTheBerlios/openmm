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

#ifndef UPNPAV_CTRL_H
#define UPNPAV_CTRL_H

#include "Upnp.h"

#undef Status

namespace Jamm {
namespace Av {


class AVTransportController
{
    friend class MediaRendererController;
    friend class MediaServerController;

public:
    void SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    void GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus);
    void GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed);
    void GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount);
    void GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes);
    void GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode);
    void Stop(const ui4& InstanceID);
    void Play(const ui4& InstanceID, const std::string& Speed);
    void Pause(const ui4& InstanceID);
    void Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target);
    void Next(const ui4& InstanceID);
    void Previous(const ui4& InstanceID);

    void _reqSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    void _reqGetMediaInfo(const ui4& InstanceID);
    void _reqGetTransportInfo(const ui4& InstanceID);
    void _reqGetPositionInfo(const ui4& InstanceID);
    void _reqGetDeviceCapabilities(const ui4& InstanceID);
    void _reqGetTransportSettings(const ui4& InstanceID);
    void _reqStop(const ui4& InstanceID);
    void _reqPlay(const ui4& InstanceID, const std::string& Speed);
    void _reqPause(const ui4& InstanceID);
    void _reqSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target);
    void _reqNext(const ui4& InstanceID);
    void _reqPrevious(const ui4& InstanceID);

    std::string _getLastChange();

protected:
    virtual void _ansSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData) = 0;
    virtual void _ansGetMediaInfo(const ui4& InstanceID, const ui4& NrTracks, const std::string& MediaDuration, const std::string& CurrentURI, const std::string& CurrentURIMetaData, const std::string& NextURI, const std::string& NextURIMetaData, const std::string& PlayMedium, const std::string& RecordMedium, const std::string& WriteStatus) = 0;
    virtual void _ansGetTransportInfo(const ui4& InstanceID, const std::string& CurrentTransportState, const std::string& CurrentTransportStatus, const std::string& CurrentSpeed) = 0;
    virtual void _ansGetPositionInfo(const ui4& InstanceID, const ui4& Track, const std::string& TrackDuration, const std::string& TrackMetaData, const std::string& TrackURI, const std::string& RelTime, const std::string& AbsTime, const i4& RelCount, const i4& AbsCount) = 0;
    virtual void _ansGetDeviceCapabilities(const ui4& InstanceID, const std::string& PlayMedia, const std::string& RecMedia, const std::string& RecQualityModes) = 0;
    virtual void _ansGetTransportSettings(const ui4& InstanceID, const std::string& PlayMode, const std::string& RecQualityMode) = 0;
    virtual void _ansStop(const ui4& InstanceID) = 0;
    virtual void _ansPlay(const ui4& InstanceID, const std::string& Speed) = 0;
    virtual void _ansPause(const ui4& InstanceID) = 0;
    virtual void _ansSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target) = 0;
    virtual void _ansNext(const ui4& InstanceID) = 0;
    virtual void _ansPrevious(const ui4& InstanceID) = 0;

    virtual void _changedLastChange(const std::string& val) = 0;

private:
    void _threadSetAVTransportURI(Action* pAction);
    void _threadGetMediaInfo(Action* pAction);
    void _threadGetTransportInfo(Action* pAction);
    void _threadGetPositionInfo(Action* pAction);
    void _threadGetDeviceCapabilities(Action* pAction);
    void _threadGetTransportSettings(Action* pAction);
    void _threadStop(Action* pAction);
    void _threadPlay(Action* pAction);
    void _threadPause(Action* pAction);
    void _threadSeek(Action* pAction);
    void _threadNext(Action* pAction);
    void _threadPrevious(Action* pAction);

    Service* m_pService;
};


class ConnectionManagerController
{
    friend class MediaRendererController;
    friend class MediaServerController;

public:
    void GetProtocolInfo(std::string& Source, std::string& Sink);
    void ConnectionComplete(const i4& ConnectionID);
    void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    void GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status);

    void _reqGetProtocolInfo();
    void _reqConnectionComplete(const i4& ConnectionID);
    void _reqGetCurrentConnectionIDs();
    void _reqGetCurrentConnectionInfo(const i4& ConnectionID);

    std::string _getSourceProtocolInfo();
    std::string _getSinkProtocolInfo();
    std::string _getCurrentConnectionIDs();

protected:
    virtual void _ansGetProtocolInfo(const std::string& Source, const std::string& Sink) = 0;
    virtual void _ansConnectionComplete(const i4& ConnectionID) = 0;
    virtual void _ansGetCurrentConnectionIDs(const std::string& ConnectionIDs) = 0;
    virtual void _ansGetCurrentConnectionInfo(const i4& ConnectionID, const i4& RcsID, const i4& AVTransportID, const std::string& ProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, const std::string& Status) = 0;

    virtual void _changedSourceProtocolInfo(const std::string& val) = 0;
    virtual void _changedSinkProtocolInfo(const std::string& val) = 0;
    virtual void _changedCurrentConnectionIDs(const std::string& val) = 0;

private:
    void _threadGetProtocolInfo(Action* pAction);
    void _threadConnectionComplete(Action* pAction);
    void _threadGetCurrentConnectionIDs(Action* pAction);
    void _threadGetCurrentConnectionInfo(Action* pAction);

    Service* m_pService;
};


class ContentDirectoryController
{
    friend class MediaServerController;

public:
    void GetSearchCapabilities(std::string& SearchCaps);
    void GetSortCapabilities(std::string& SortCaps);
    void GetSystemUpdateID(ui4& Id);
    void Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID);
    void Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID);
    void CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result);
    void DestroyObject(const std::string& ObjectID);
    void UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue);
    void ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID);
    void GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal);
    void DeleteResource(const uri& ResourceURI);
    void CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID);

    void _reqGetSearchCapabilities();
    void _reqGetSortCapabilities();
    void _reqGetSystemUpdateID();
    void _reqBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria);
    void _reqSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria);
    void _reqCreateObject(const std::string& ContainerID, const std::string& Elements);
    void _reqDestroyObject(const std::string& ObjectID);
    void _reqUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue);
    void _reqImportResource(const uri& SourceURI, const uri& DestinationURI);
    void _reqGetTransferProgress(const ui4& TransferID);
    void _reqDeleteResource(const uri& ResourceURI);
    void _reqCreateReference(const std::string& ContainerID, const std::string& ObjectID);

    std::string _getTransferIDs();
    ui4 _getSystemUpdateID();
    std::string _getContainerUpdateIDs();

protected:
    virtual void _ansGetSearchCapabilities(const std::string& SearchCaps) = 0;
    virtual void _ansGetSortCapabilities(const std::string& SortCaps) = 0;
    virtual void _ansGetSystemUpdateID(const ui4& Id) = 0;
    virtual void _ansBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID) = 0;
    virtual void _ansSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID) = 0;
    virtual void _ansCreateObject(const std::string& ContainerID, const std::string& Elements, const std::string& ObjectID, const std::string& Result) = 0;
    virtual void _ansDestroyObject(const std::string& ObjectID) = 0;
    virtual void _ansUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue) = 0;
    virtual void _ansImportResource(const uri& SourceURI, const uri& DestinationURI, const ui4& TransferID) = 0;
    virtual void _ansGetTransferProgress(const ui4& TransferID, const std::string& TransferStatus, const std::string& TransferLength, const std::string& TransferTotal) = 0;
    virtual void _ansDeleteResource(const uri& ResourceURI) = 0;
    virtual void _ansCreateReference(const std::string& ContainerID, const std::string& ObjectID, const std::string& NewID) = 0;

    virtual void _changedTransferIDs(const std::string& val) = 0;
    virtual void _changedSystemUpdateID(const ui4& val) = 0;
    virtual void _changedContainerUpdateIDs(const std::string& val) = 0;

private:
    void _threadGetSearchCapabilities(Action* pAction);
    void _threadGetSortCapabilities(Action* pAction);
    void _threadGetSystemUpdateID(Action* pAction);
    void _threadBrowse(Action* pAction);
    void _threadSearch(Action* pAction);
    void _threadCreateObject(Action* pAction);
    void _threadDestroyObject(Action* pAction);
    void _threadUpdateObject(Action* pAction);
    void _threadImportResource(Action* pAction);
    void _threadGetTransferProgress(Action* pAction);
    void _threadDeleteResource(Action* pAction);
    void _threadCreateReference(Action* pAction);

    Service* m_pService;
};


class RenderingControlController
{
    friend class MediaRendererController;

public:
    void ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList);
    void SelectPreset(const ui4& InstanceID, const std::string& PresetName);
    void GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness);
    void SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness);
    void GetContrast(const ui4& InstanceID, ui2& CurrentContrast);
    void SetContrast(const ui4& InstanceID, const ui2& DesiredContrast);
    void GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness);
    void SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness);
    void GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain);
    void SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain);
    void GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain);
    void SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain);
    void GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain);
    void SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain);
    void GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel);
    void SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel);
    void GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel);
    void SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel);
    void GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel);
    void SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel);
    void GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature);
    void SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature);
    void GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone);
    void SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone);
    void GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone);
    void SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone);
    void GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute);
    void SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    void GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume);
    void SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume);
    void GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume);
    void SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume);
    void GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue);
    void GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness);
    void SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    void _reqListPresets(const ui4& InstanceID);
    void _reqSelectPreset(const ui4& InstanceID, const std::string& PresetName);
    void _reqGetBrightness(const ui4& InstanceID);
    void _reqSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness);
    void _reqGetContrast(const ui4& InstanceID);
    void _reqSetContrast(const ui4& InstanceID, const ui2& DesiredContrast);
    void _reqGetSharpness(const ui4& InstanceID);
    void _reqSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness);
    void _reqGetRedVideoGain(const ui4& InstanceID);
    void _reqSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain);
    void _reqGetGreenVideoGain(const ui4& InstanceID);
    void _reqSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain);
    void _reqGetBlueVideoGain(const ui4& InstanceID);
    void _reqSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain);
    void _reqGetRedVideoBlackLevel(const ui4& InstanceID);
    void _reqSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel);
    void _reqGetGreenVideoBlackLevel(const ui4& InstanceID);
    void _reqSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel);
    void _reqGetBlueVideoBlackLevel(const ui4& InstanceID);
    void _reqSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel);
    void _reqGetColorTemperature (const ui4& InstanceID);
    void _reqSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature);
    void _reqGetHorizontalKeystone(const ui4& InstanceID);
    void _reqSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone);
    void _reqGetVerticalKeystone(const ui4& InstanceID);
    void _reqSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone);
    void _reqGetMute(const ui4& InstanceID, const std::string& Channel);
    void _reqSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    void _reqGetVolume(const ui4& InstanceID, const std::string& Channel);
    void _reqSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume);
    void _reqGetVolumeDB(const ui4& InstanceID, const std::string& Channel);
    void _reqSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume);
    void _reqGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel);
    void _reqGetLoudness(const ui4& InstanceID, const std::string& Channel);
    void _reqSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    std::string _getLastChange();

protected:
    virtual void _ansListPresets(const ui4& InstanceID, const std::string& CurrentPresetNameList) = 0;
    virtual void _ansSelectPreset(const ui4& InstanceID, const std::string& PresetName) = 0;
    virtual void _ansGetBrightness(const ui4& InstanceID, const ui2& CurrentBrightness) = 0;
    virtual void _ansSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness) = 0;
    virtual void _ansGetContrast(const ui4& InstanceID, const ui2& CurrentContrast) = 0;
    virtual void _ansSetContrast(const ui4& InstanceID, const ui2& DesiredContrast) = 0;
    virtual void _ansGetSharpness(const ui4& InstanceID, const ui2& CurrentSharpness) = 0;
    virtual void _ansSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness) = 0;
    virtual void _ansGetRedVideoGain(const ui4& InstanceID, const ui2& CurrentRedVideoGain) = 0;
    virtual void _ansSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain) = 0;
    virtual void _ansGetGreenVideoGain(const ui4& InstanceID, const ui2& CurrentGreenVideoGain) = 0;
    virtual void _ansSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain) = 0;
    virtual void _ansGetBlueVideoGain(const ui4& InstanceID, const ui2& CurrentBlueVideoGain) = 0;
    virtual void _ansSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain) = 0;
    virtual void _ansGetRedVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentRedVideoBlackLevel) = 0;
    virtual void _ansSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel) = 0;
    virtual void _ansGetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentGreenVideoBlackLevel) = 0;
    virtual void _ansSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel) = 0;
    virtual void _ansGetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentBlueVideoBlackLevel) = 0;
    virtual void _ansSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel) = 0;
    virtual void _ansGetColorTemperature (const ui4& InstanceID, const ui2& CurrentColorTemperature) = 0;
    virtual void _ansSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature) = 0;
    virtual void _ansGetHorizontalKeystone(const ui4& InstanceID, const i2& CurrentHorizontalKeystone) = 0;
    virtual void _ansSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone) = 0;
    virtual void _ansGetVerticalKeystone(const ui4& InstanceID, const i2& CurrentVerticalKeystone) = 0;
    virtual void _ansSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone) = 0;
    virtual void _ansGetMute(const ui4& InstanceID, const std::string& Channel, const bool& CurrentMute) = 0;
    virtual void _ansSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute) = 0;
    virtual void _ansGetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& CurrentVolume) = 0;
    virtual void _ansSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume) = 0;
    virtual void _ansGetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& CurrentVolume) = 0;
    virtual void _ansSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume) = 0;
    virtual void _ansGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, const i2& MinValue, const i2& MaxValue) = 0;
    virtual void _ansGetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& CurrentLoudness) = 0;
    virtual void _ansSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness) = 0;

    virtual void _changedLastChange(const std::string& val) = 0;

private:
    void _threadListPresets(Action* pAction);
    void _threadSelectPreset(Action* pAction);
    void _threadGetBrightness(Action* pAction);
    void _threadSetBrightness(Action* pAction);
    void _threadGetContrast(Action* pAction);
    void _threadSetContrast(Action* pAction);
    void _threadGetSharpness(Action* pAction);
    void _threadSetSharpness(Action* pAction);
    void _threadGetRedVideoGain(Action* pAction);
    void _threadSetRedVideoGain(Action* pAction);
    void _threadGetGreenVideoGain(Action* pAction);
    void _threadSetGreenVideoGain(Action* pAction);
    void _threadGetBlueVideoGain(Action* pAction);
    void _threadSetBlueVideoGain(Action* pAction);
    void _threadGetRedVideoBlackLevel(Action* pAction);
    void _threadSetRedVideoBlackLevel(Action* pAction);
    void _threadGetGreenVideoBlackLevel(Action* pAction);
    void _threadSetGreenVideoBlackLevel(Action* pAction);
    void _threadGetBlueVideoBlackLevel(Action* pAction);
    void _threadSetBlueVideoBlackLevel(Action* pAction);
    void _threadGetColorTemperature (Action* pAction);
    void _threadSetColorTemperature(Action* pAction);
    void _threadGetHorizontalKeystone(Action* pAction);
    void _threadSetHorizontalKeystone(Action* pAction);
    void _threadGetVerticalKeystone(Action* pAction);
    void _threadSetVerticalKeystone(Action* pAction);
    void _threadGetMute(Action* pAction);
    void _threadSetMute(Action* pAction);
    void _threadGetVolume(Action* pAction);
    void _threadSetVolume(Action* pAction);
    void _threadGetVolumeDB(Action* pAction);
    void _threadSetVolumeDB(Action* pAction);
    void _threadGetVolumeDBRange(Action* pAction);
    void _threadGetLoudness(Action* pAction);
    void _threadSetLoudness(Action* pAction);

    Service* m_pService;
};


class MediaRendererController : public ControllerImplAdapter
{
public:
    MediaRendererController(Device* pDevice, RenderingControlController* pRenderingControlController, ConnectionManagerController* pConnectionManagerController, AVTransportController* pAVTransportController);

    RenderingControlController* RenderingControl() { return m_pRenderingControlController; }
    ConnectionManagerController* ConnectionManager() { return m_pConnectionManagerController; }
    AVTransportController* AVTransport() { return m_pAVTransportController; }

private:
    virtual void eventHandler(StateVar* pStateVar);

//     Device* m_pDevice;
    RenderingControlController* m_pRenderingControlController;
    ConnectionManagerController* m_pConnectionManagerController;
    AVTransportController* m_pAVTransportController;
};


class MediaServerController : public ControllerImplAdapter
{
public:
    MediaServerController(Device* pDevice, ContentDirectoryController* pContentDirectoryController, ConnectionManagerController* pConnectionManagerController, AVTransportController* pAVTransportController);

    ContentDirectoryController* ContentDirectory() { return m_pContentDirectoryController; }
    ConnectionManagerController* ConnectionManager() { return m_pConnectionManagerController; }
    AVTransportController* AVTransport() { return m_pAVTransportController; }

private:
    virtual void eventHandler(StateVar* pStateVar);

//     Device* m_pDevice;
    ContentDirectoryController* m_pContentDirectoryController;
    ConnectionManagerController* m_pConnectionManagerController;
    AVTransportController* m_pAVTransportController;
};

} // namespace Av
} // namespace Jamm

#endif

