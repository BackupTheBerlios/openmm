#ifndef UPNPAV_CTRL_IMPL_H
#define UPNPAV_CTRL_IMPL_H

#include <jamm/upnp.h>
#include <jamm/UpnpAvControllers.h>

using namespace Jamm;
using namespace Jamm::Av;

class AVTransportControllerImpl : public AVTransportController
{
private:
    virtual void _ansSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    virtual void _ansGetMediaInfo(const ui4& InstanceID, const ui4& NrTracks, const std::string& MediaDuration, const std::string& CurrentURI, const std::string& CurrentURIMetaData, const std::string& NextURI, const std::string& NextURIMetaData, const std::string& PlayMedium, const std::string& RecordMedium, const std::string& WriteStatus);
    virtual void _ansGetTransportInfo(const ui4& InstanceID, const std::string& CurrentTransportState, const std::string& CurrentTransportStatus, const std::string& CurrentSpeed);
    virtual void _ansGetPositionInfo(const ui4& InstanceID, const ui4& Track, const std::string& TrackDuration, const std::string& TrackMetaData, const std::string& TrackURI, const std::string& RelTime, const std::string& AbsTime, const i4& RelCount, const i4& AbsCount);
    virtual void _ansGetDeviceCapabilities(const ui4& InstanceID, const std::string& PlayMedia, const std::string& RecMedia, const std::string& RecQualityModes);
    virtual void _ansGetTransportSettings(const ui4& InstanceID, const std::string& PlayMode, const std::string& RecQualityMode);
    virtual void _ansStop(const ui4& InstanceID);
    virtual void _ansPlay(const ui4& InstanceID, const std::string& Speed);
    virtual void _ansPause(const ui4& InstanceID);
    virtual void _ansSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target);
    virtual void _ansNext(const ui4& InstanceID);
    virtual void _ansPrevious(const ui4& InstanceID);

    virtual void _changedLastChange(const std::string& val);
};

class ConnectionManagerControllerImpl : public ConnectionManagerController
{
private:
    virtual void _ansGetProtocolInfo(const std::string& Source, const std::string& Sink);
    virtual void _ansConnectionComplete(const i4& ConnectionID);
    virtual void _ansGetCurrentConnectionIDs(const std::string& ConnectionIDs);
    virtual void _ansGetCurrentConnectionInfo(const i4& ConnectionID, const i4& RcsID, const i4& AVTransportID, const std::string& ProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, const std::string& Status);

    virtual void _changedSourceProtocolInfo(const std::string& val);
    virtual void _changedSinkProtocolInfo(const std::string& val);
    virtual void _changedCurrentConnectionIDs(const std::string& val);
};

class RenderingControlControllerImpl : public RenderingControlController
{
private:
    virtual void _ansListPresets(const ui4& InstanceID, const std::string& CurrentPresetNameList);
    virtual void _ansSelectPreset(const ui4& InstanceID, const std::string& PresetName);
    virtual void _ansGetBrightness(const ui4& InstanceID, const ui2& CurrentBrightness);
    virtual void _ansSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness);
    virtual void _ansGetContrast(const ui4& InstanceID, const ui2& CurrentContrast);
    virtual void _ansSetContrast(const ui4& InstanceID, const ui2& DesiredContrast);
    virtual void _ansGetSharpness(const ui4& InstanceID, const ui2& CurrentSharpness);
    virtual void _ansSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness);
    virtual void _ansGetRedVideoGain(const ui4& InstanceID, const ui2& CurrentRedVideoGain);
    virtual void _ansSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain);
    virtual void _ansGetGreenVideoGain(const ui4& InstanceID, const ui2& CurrentGreenVideoGain);
    virtual void _ansSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain);
    virtual void _ansGetBlueVideoGain(const ui4& InstanceID, const ui2& CurrentBlueVideoGain);
    virtual void _ansSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain);
    virtual void _ansGetRedVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentRedVideoBlackLevel);
    virtual void _ansSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel);
    virtual void _ansGetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentGreenVideoBlackLevel);
    virtual void _ansSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel);
    virtual void _ansGetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& CurrentBlueVideoBlackLevel);
    virtual void _ansSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel);
    virtual void _ansGetColorTemperature (const ui4& InstanceID, const ui2& CurrentColorTemperature);
    virtual void _ansSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature);
    virtual void _ansGetHorizontalKeystone(const ui4& InstanceID, const i2& CurrentHorizontalKeystone);
    virtual void _ansSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone);
    virtual void _ansGetVerticalKeystone(const ui4& InstanceID, const i2& CurrentVerticalKeystone);
    virtual void _ansSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone);
    virtual void _ansGetMute(const ui4& InstanceID, const std::string& Channel, const bool& CurrentMute);
    virtual void _ansSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    virtual void _ansGetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& CurrentVolume);
    virtual void _ansSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume);
    virtual void _ansGetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& CurrentVolume);
    virtual void _ansSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume);
    virtual void _ansGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, const i2& MinValue, const i2& MaxValue);
    virtual void _ansGetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& CurrentLoudness);
    virtual void _ansSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    virtual void _changedLastChange(const std::string& val);
};


class ContentDirectoryControllerImpl : public ContentDirectoryController
{
private:
    virtual void _ansGetSearchCapabilities(const std::string& SearchCaps);
    virtual void _ansGetSortCapabilities(const std::string& SortCaps);
    virtual void _ansGetSystemUpdateID(const ui4& Id);
    virtual void _ansBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID);
    virtual void _ansSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, const std::string& Result, const ui4& NumberReturned, const ui4& TotalMatches, const ui4& UpdateID);
    virtual void _ansCreateObject(const std::string& ContainerID, const std::string& Elements, const std::string& ObjectID, const std::string& Result);
    virtual void _ansDestroyObject(const std::string& ObjectID);
    virtual void _ansUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue);
    virtual void _ansImportResource(const uri& SourceURI, const uri& DestinationURI, const ui4& TransferID);
    virtual void _ansGetTransferProgress(const ui4& TransferID, const std::string& TransferStatus, const std::string& TransferLength, const std::string& TransferTotal);
    virtual void _ansDeleteResource(const uri& ResourceURI);
    virtual void _ansCreateReference(const std::string& ContainerID, const std::string& ObjectID, const std::string& NewID);

    virtual void _changedTransferIDs(const std::string& val);
    virtual void _changedSystemUpdateID(const ui4& val);
    virtual void _changedContainerUpdateIDs(const std::string& val);
};

#endif

