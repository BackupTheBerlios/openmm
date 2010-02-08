/***************************************************************************|
|                               WARNING                                     |
|        This file is generated by the jamm stub generator jammgen.         |
|                   RENAME THIS FILE BEFORE EDITING!                        |
|           It will be overriden at the next run of jammgen.                |
***************************************************************************/

#ifndef MEDIARENDERER_CTRL_IMPL_H
#define MEDIARENDERER_CTRL_IMPL_H

#include <jamm/upnp.h>
#include "MediaRendererCtrl.h"

class AVTransportControllerImpl : public AVTransportController
{
private:
    virtual void _ansSetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    virtual void _ansGetMediaInfo(/*const Jamm::ui4& InstanceID, const Jamm::ui4& NrTracks, const std::string& MediaDuration, const std::string& CurrentURI, const std::string& CurrentURIMetaData, const std::string& NextURI, const std::string& NextURIMetaData, const std::string& PlayMedium, const std::string& RecordMedium, const std::string& WriteStatus*/);
    virtual void _ansGetTransportInfo(const Jamm::ui4& InstanceID, const std::string& CurrentTransportState, const std::string& CurrentTransportStatus, const std::string& CurrentSpeed);
    virtual void _ansGetPositionInfo(const Jamm::ui4& InstanceID, const Jamm::ui4& Track, const std::string& TrackDuration, const std::string& TrackMetaData, const std::string& TrackURI, const std::string& RelTime, const std::string& AbsTime, const Jamm::i4& RelCount, const Jamm::i4& AbsCount);
    virtual void _ansGetDeviceCapabilities(const Jamm::ui4& InstanceID, const std::string& PlayMedia, const std::string& RecMedia, const std::string& RecQualityModes);
    virtual void _ansGetTransportSettings(const Jamm::ui4& InstanceID, const std::string& PlayMode, const std::string& RecQualityMode);
    virtual void _ansStop(const Jamm::ui4& InstanceID);
    virtual void _ansPlay(const Jamm::ui4& InstanceID, const std::string& Speed);
    virtual void _ansPause(const Jamm::ui4& InstanceID);
    virtual void _ansSeek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target);
    virtual void _ansNext(const Jamm::ui4& InstanceID);
    virtual void _ansPrevious(const Jamm::ui4& InstanceID);

    virtual void _changedLastChange(const std::string& val);
};

class ConnectionManagerControllerImpl : public ConnectionManagerController
{
private:
    virtual void _ansGetProtocolInfo(const std::string& Source, const std::string& Sink);
    virtual void _ansGetCurrentConnectionIDs(const std::string& ConnectionIDs);
    virtual void _ansGetCurrentConnectionInfo(const Jamm::i4& ConnectionID, const Jamm::i4& RcsID, const Jamm::i4& AVTransportID, const std::string& ProtocolInfo, const std::string& PeerConnectionManager, const Jamm::i4& PeerConnectionID, const std::string& Direction, const std::string& Status);

    virtual void _changedSourceProtocolInfo(const std::string& val);
    virtual void _changedSinkProtocolInfo(const std::string& val);
    virtual void _changedCurrentConnectionIDs(const std::string& val);
};

class RenderingControlControllerImpl : public RenderingControlController
{
private:
    virtual void _ansListPresets(const Jamm::ui4& InstanceID, const std::string& CurrentPresetNameList);
    virtual void _ansSelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName);
    virtual void _ansGetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBrightness);
    virtual void _ansSetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness);
    virtual void _ansGetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentContrast);
    virtual void _ansSetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast);
    virtual void _ansGetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentSharpness);
    virtual void _ansSetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness);
    virtual void _ansGetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentRedVideoGain);
    virtual void _ansSetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain);
    virtual void _ansGetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentGreenVideoGain);
    virtual void _ansSetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain);
    virtual void _ansGetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBlueVideoGain);
    virtual void _ansSetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain);
    virtual void _ansGetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentRedVideoBlackLevel);
    virtual void _ansSetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel);
    virtual void _ansGetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentGreenVideoBlackLevel);
    virtual void _ansSetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel);
    virtual void _ansGetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBlueVideoBlackLevel);
    virtual void _ansSetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel);
    virtual void _ansGetColorTemperature (const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentColorTemperature);
    virtual void _ansSetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature);
    virtual void _ansGetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& CurrentHorizontalKeystone);
    virtual void _ansSetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone);
    virtual void _ansGetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& CurrentVerticalKeystone);
    virtual void _ansSetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone);
    virtual void _ansGetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& CurrentMute);
    virtual void _ansSetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    virtual void _ansGetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& CurrentVolume);
    virtual void _ansSetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume);
    virtual void _ansGetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& CurrentVolume);
    virtual void _ansSetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume);
    virtual void _ansGetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& MinValue, const Jamm::i2& MaxValue);
    virtual void _ansGetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& CurrentLoudness);
    virtual void _ansSetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    virtual void _changedLastChange(const std::string& val);
};

#endif

