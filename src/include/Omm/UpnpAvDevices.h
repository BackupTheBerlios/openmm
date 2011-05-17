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

#ifndef UPNPAV_DEV_H
#define UPNPAV_DEV_H

#include "Upnp.h"

// Somewhere, there's an evil macro that scrambles argument Status in
// ConnectionManager::GetCurrentConnectionInfo()
#undef Status

namespace Omm {
namespace Av {

// Forward declaration of all UPnP AV services
class AVTransport;
class ConnectionManager;
class RenderingControl;
class ContentDirectory;


class MediaRenderer : public DevDevice
{
public:
    MediaRenderer(RenderingControl* pRenderingControlImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl);

protected:
    RenderingControl* _pRenderingControlImpl;
    ConnectionManager* _pConnectionManagerImpl;
    AVTransport* _pAVTransportImpl;

private:
    virtual void actionHandler(Action* pAction);
    virtual void initStateVars(Service* pService);

    static std::string _deviceDescription;
};


class MediaServer : public DevDevice
{
public:
    MediaServer(ContentDirectory* pContentDirectoryImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl);

protected:
    ContentDirectory* _pContentDirectoryImpl;
    ConnectionManager* _pConnectionManagerImpl;
    AVTransport* _pAVTransportImpl;

private:
    virtual void actionHandler(Action* pAction);
    virtual void initStateVars(Service* pService);

    static std::string _deviceDescription;
};


class AVTransport
{
    friend class MediaServer;
    friend class MediaRenderer;

protected:
    virtual void SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData) = 0;
    virtual void GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus) = 0;
    virtual void GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed) = 0;
    virtual void GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount) = 0;
    virtual void GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes) = 0;
    virtual void GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode) = 0;
    virtual void Stop(const ui4& InstanceID) = 0;
    virtual void Play(const ui4& InstanceID, const std::string& Speed) = 0;
    virtual void Pause(const ui4& InstanceID) = 0;
    virtual void Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target) = 0;
    virtual void Next(const ui4& InstanceID) = 0;
    virtual void Previous(const ui4& InstanceID) = 0;

    virtual void initStateVars() = 0;
    void actionHandler(Action* pAction);

    void _setTransportState(const std::string& val);
    std::string _getTransportState();
    void _setTransportStatus(const std::string& val);
    std::string _getTransportStatus();
    void _setPlaybackStorageMedium(const std::string& val);
    std::string _getPlaybackStorageMedium();
    void _setRecordStorageMedium(const std::string& val);
    std::string _getRecordStorageMedium();
    void _setPossiblePlaybackStorageMedia(const std::string& val);
    std::string _getPossiblePlaybackStorageMedia();
    void _setPossibleRecordStorageMedia(const std::string& val);
    std::string _getPossibleRecordStorageMedia();
    void _setCurrentPlayMode(const std::string& val);
    std::string _getCurrentPlayMode();
    void _setTransportPlaySpeed(const std::string& val);
    std::string _getTransportPlaySpeed();
    void _setRecordMediumWriteStatus(const std::string& val);
    std::string _getRecordMediumWriteStatus();
    void _setCurrentRecordQualityMode(const std::string& val);
    std::string _getCurrentRecordQualityMode();
    void _setPossibleRecordQualityModes(const std::string& val);
    std::string _getPossibleRecordQualityModes();
    void _setNumberOfTracks(const ui4& val);
    ui4 _getNumberOfTracks();
    void _setCurrentTrack(const ui4& val);
    ui4 _getCurrentTrack();
    void _setCurrentTrackDuration(const std::string& val);
    std::string _getCurrentTrackDuration();
    void _setCurrentMediaDuration(const std::string& val);
    std::string _getCurrentMediaDuration();
    void _setCurrentTrackMetaData(const std::string& val);
    std::string _getCurrentTrackMetaData();
    void _setCurrentTrackURI(const std::string& val);
    std::string _getCurrentTrackURI();
    void _setAVTransportURI(const std::string& val);
    std::string _getAVTransportURI();
    void _setAVTransportURIMetaData(const std::string& val);
    std::string _getAVTransportURIMetaData();
    void _setNextAVTransportURI(const std::string& val);
    std::string _getNextAVTransportURI();
    void _setNextAVTransportURIMetaData(const std::string& val);
    std::string _getNextAVTransportURIMetaData();
    void _setRelativeTimePosition(const std::string& val);
    std::string _getRelativeTimePosition();
    void _setAbsoluteTimePosition(const std::string& val);
    std::string _getAbsoluteTimePosition();
    void _setRelativeCounterPosition(const i4& val);
    i4 _getRelativeCounterPosition();
    void _setAbsoluteCounterPosition(const i4& val);
    i4 _getAbsoluteCounterPosition();
    void _setCurrentTransportActions(const std::string& val);
    std::string _getCurrentTransportActions();
    void _setLastChange(const std::string& val);
    std::string _getLastChange();

private:
    static std::string  _description;
    Service* _pService;
};


class ConnectionManager
{
    friend class MediaServer;
    friend class MediaRenderer;

protected:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink) = 0;
    virtual void ConnectionComplete(const i4& ConnectionID) = 0;
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs) = 0;
    virtual void GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status) = 0;

    virtual void initStateVars() = 0;
    void actionHandler(Action* pAction);

    void _setSourceProtocolInfo(const std::string& val);
    std::string _getSourceProtocolInfo();
    void _setSinkProtocolInfo(const std::string& val);
    std::string _getSinkProtocolInfo();
    void _setCurrentConnectionIDs(const std::string& val);
    std::string _getCurrentConnectionIDs();

private:
    static std::string  _description;
    Service* _pService;
};


class ContentDirectory
{
    friend class MediaServer;

protected:
    virtual void GetSearchCapabilities(std::string& SearchCaps) = 0;
    virtual void GetSortCapabilities(std::string& SortCaps) = 0;
    virtual void GetSystemUpdateID(ui4& Id) = 0;
    virtual void Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID) = 0;
    virtual void Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID) = 0;
    virtual void CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result) = 0;
    virtual void DestroyObject(const std::string& ObjectID) = 0;
    virtual void UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue) = 0;
    virtual void ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID) = 0;
    virtual void GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal) = 0;
    virtual void DeleteResource(const uri& ResourceURI) = 0;
    virtual void CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID) = 0;

    virtual void initStateVars() = 0;
    void actionHandler(Action* pAction);

    void _setTransferIDs(const std::string& val);
    std::string _getTransferIDs();
    void _setSearchCapabilities(const std::string& val);
    std::string _getSearchCapabilities();
    void _setSortCapabilities(const std::string& val);
    std::string _getSortCapabilities();
    void _setSystemUpdateID(const ui4& val);
    ui4 _getSystemUpdateID();
    void _setContainerUpdateIDs(const std::string& val);
    std::string _getContainerUpdateIDs();

private:
    static std::string  _description;
    Service* _pService;
};


class RenderingControl
{
    friend class MediaRenderer;

protected:
    virtual void ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList) = 0;
    virtual void SelectPreset(const ui4& InstanceID, const std::string& PresetName) = 0;
    virtual void GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness) = 0;
    virtual void SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness) = 0;
    virtual void GetContrast(const ui4& InstanceID, ui2& CurrentContrast) = 0;
    virtual void SetContrast(const ui4& InstanceID, const ui2& DesiredContrast) = 0;
    virtual void GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness) = 0;
    virtual void SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness) = 0;
    virtual void GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain) = 0;
    virtual void SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain) = 0;
    virtual void GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain) = 0;
    virtual void SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain) = 0;
    virtual void GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain) = 0;
    virtual void SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain) = 0;
    virtual void GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel) = 0;
    virtual void SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel) = 0;
    virtual void GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel) = 0;
    virtual void SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel) = 0;
    virtual void GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel) = 0;
    virtual void SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel) = 0;
    virtual void GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature) = 0;
    virtual void SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature) = 0;
    virtual void GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone) = 0;
    virtual void SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone) = 0;
    virtual void GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone) = 0;
    virtual void SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone) = 0;
    virtual void GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute) = 0;
    virtual void SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute) = 0;
    virtual void GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume) = 0;
    virtual void SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume) = 0;
    virtual void GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume) = 0;
    virtual void SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume) = 0;
    virtual void GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue) = 0;
    virtual void GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness) = 0;
    virtual void SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness) = 0;

    virtual void initStateVars() = 0;
    void actionHandler(Action* pAction);

    void _setPresetNameList(const std::string& val);
    std::string _getPresetNameList();
    void _setLastChange(const std::string& val);
    std::string _getLastChange();
    void _setBrightness(const ui2& val);
    ui2 _getBrightness();
    void _setContrast(const ui2& val);
    ui2 _getContrast();
    void _setSharpness(const ui2& val);
    ui2 _getSharpness();
    void _setRedVideoGain(const ui2& val);
    ui2 _getRedVideoGain();
    void _setGreenVideoGain(const ui2& val);
    ui2 _getGreenVideoGain();
    void _setBlueVideoGain(const ui2& val);
    ui2 _getBlueVideoGain();
    void _setRedVideoBlackLevel(const ui2& val);
    ui2 _getRedVideoBlackLevel();
    void _setGreenVideoBlackLevel(const ui2& val);
    ui2 _getGreenVideoBlackLevel();
    void _setBlueVideoBlackLevel(const ui2& val);
    ui2 _getBlueVideoBlackLevel();
    void _setColorTemperature(const ui2& val);
    ui2 _getColorTemperature();
    void _setHorizontalKeystone(const i2& val);
    i2 _getHorizontalKeystone();
    void _setVerticalKeystone(const i2& val);
    i2 _getVerticalKeystone();
    void _setMute(const bool& val);
    bool _getMute();
    void _setVolume(const ui2& val);
    ui2 _getVolume();
    void _setVolumeDB(const i2& val);
    i2 _getVolumeDB();
    void _setLoudness(const bool& val);
    bool _getLoudness();

private:
    static std::string  _description;
    Service* _pService;
};


} // namespace Av
} // namespace Omm

#endif

