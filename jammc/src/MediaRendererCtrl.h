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

/***************************************************************************|
|                               WARNING                                     |
|        This file is generated by the jamm stub generator jammgen.         |
|       Don't edit, it will be overriden at the next run of jammgen.        |
***************************************************************************/

#ifndef MEDIARENDERER_CTRL_H
#define MEDIARENDERER_CTRL_H

#include <jamm/upnp.h>

class AVTransportController
{
    friend class MediaRendererController;

public:
    void SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    void GetMediaInfo(/*const Jamm::ui4& InstanceID, Jamm::ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus*/);
    void GetTransportInfo(const Jamm::ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed);
    void GetPositionInfo(const Jamm::ui4& InstanceID, Jamm::ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, Jamm::i4& RelCount, Jamm::i4& AbsCount);
    void GetDeviceCapabilities(const Jamm::ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes);
    void GetTransportSettings(const Jamm::ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode);
    void Stop(const Jamm::ui4& InstanceID);
    void Play(const Jamm::ui4& InstanceID, const std::string& Speed);
    void Pause(const Jamm::ui4& InstanceID);
    void Seek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target);
    void Next(const Jamm::ui4& InstanceID);
    void Previous(const Jamm::ui4& InstanceID);

    void _reqSetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData);
    void _reqGetMediaInfo(const Jamm::ui4& InstanceID, );
    void _reqGetTransportInfo(const Jamm::ui4& InstanceID, );
    void _reqGetPositionInfo(const Jamm::ui4& InstanceID, );
    void _reqGetDeviceCapabilities(const Jamm::ui4& InstanceID, );
    void _reqGetTransportSettings(const Jamm::ui4& InstanceID, );
    void _reqStop(const Jamm::ui4& InstanceID);
    void _reqPlay(const Jamm::ui4& InstanceID, const std::string& Speed);
    void _reqPause(const Jamm::ui4& InstanceID);
    void _reqSeek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target);
    void _reqNext(const Jamm::ui4& InstanceID);
    void _reqPrevious(const Jamm::ui4& InstanceID);

    std::string _getLastChange();

protected:
    virtual void _ansSetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData) = 0;
    virtual void _ansGetMediaInfo(const Jamm::ui4& InstanceID, const Jamm::ui4& NrTracks, const std::string& MediaDuration, const std::string& CurrentURI, const std::string& CurrentURIMetaData, const std::string& NextURI, const std::string& NextURIMetaData, const std::string& PlayMedium, const std::string& RecordMedium, const std::string& WriteStatus) = 0;
    virtual void _ansGetTransportInfo(const Jamm::ui4& InstanceID, const std::string& CurrentTransportState, const std::string& CurrentTransportStatus, const std::string& CurrentSpeed) = 0;
    virtual void _ansGetPositionInfo(const Jamm::ui4& InstanceID, const Jamm::ui4& Track, const std::string& TrackDuration, const std::string& TrackMetaData, const std::string& TrackURI, const std::string& RelTime, const std::string& AbsTime, const Jamm::i4& RelCount, const Jamm::i4& AbsCount) = 0;
    virtual void _ansGetDeviceCapabilities(const Jamm::ui4& InstanceID, const std::string& PlayMedia, const std::string& RecMedia, const std::string& RecQualityModes) = 0;
    virtual void _ansGetTransportSettings(const Jamm::ui4& InstanceID, const std::string& PlayMode, const std::string& RecQualityMode) = 0;
    virtual void _ansStop(const Jamm::ui4& InstanceID) = 0;
    virtual void _ansPlay(const Jamm::ui4& InstanceID, const std::string& Speed) = 0;
    virtual void _ansPause(const Jamm::ui4& InstanceID) = 0;
    virtual void _ansSeek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target) = 0;
    virtual void _ansNext(const Jamm::ui4& InstanceID) = 0;
    virtual void _ansPrevious(const Jamm::ui4& InstanceID) = 0;

    virtual void _changedLastChange(const std::string& val) = 0;

private:
    void _threadSetAVTransportURI(Jamm::Action* pAction);
    void _threadGetMediaInfo(Jamm::Action* pAction);
    void _threadGetTransportInfo(Jamm::Action* pAction);
    void _threadGetPositionInfo(Jamm::Action* pAction);
    void _threadGetDeviceCapabilities(Jamm::Action* pAction);
    void _threadGetTransportSettings(Jamm::Action* pAction);
    void _threadStop(Jamm::Action* pAction);
    void _threadPlay(Jamm::Action* pAction);
    void _threadPause(Jamm::Action* pAction);
    void _threadSeek(Jamm::Action* pAction);
    void _threadNext(Jamm::Action* pAction);
    void _threadPrevious(Jamm::Action* pAction);

    Jamm::Service* m_pService;
};

class ConnectionManagerController
{
    friend class MediaRendererController;

public:
    void GetProtocolInfo(std::string& Source, std::string& Sink);
    void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    void GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status);

    void _reqGetProtocolInfo();
    void _reqGetCurrentConnectionIDs();
    void _reqGetCurrentConnectionInfo(const Jamm::i4& ConnectionID, );

    std::string _getSourceProtocolInfo();
    std::string _getSinkProtocolInfo();
    std::string _getCurrentConnectionIDs();

protected:
    virtual void _ansGetProtocolInfo(const std::string& Source, const std::string& Sink) = 0;
    virtual void _ansGetCurrentConnectionIDs(const std::string& ConnectionIDs) = 0;
    virtual void _ansGetCurrentConnectionInfo(const Jamm::i4& ConnectionID, const Jamm::i4& RcsID, const Jamm::i4& AVTransportID, const std::string& ProtocolInfo, const std::string& PeerConnectionManager, const Jamm::i4& PeerConnectionID, const std::string& Direction, const std::string& Status) = 0;

    virtual void _changedSourceProtocolInfo(const std::string& val) = 0;
    virtual void _changedSinkProtocolInfo(const std::string& val) = 0;
    virtual void _changedCurrentConnectionIDs(const std::string& val) = 0;

private:
    void _threadGetProtocolInfo(Jamm::Action* pAction);
    void _threadGetCurrentConnectionIDs(Jamm::Action* pAction);
    void _threadGetCurrentConnectionInfo(Jamm::Action* pAction);

    Jamm::Service* m_pService;
};

class RenderingControlController
{
    friend class MediaRendererController;

public:
    void ListPresets(const Jamm::ui4& InstanceID, std::string& CurrentPresetNameList);
    void SelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName);
    void GetBrightness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBrightness);
    void SetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness);
    void GetContrast(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentContrast);
    void SetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast);
    void GetSharpness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentSharpness);
    void SetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness);
    void GetRedVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoGain);
    void SetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain);
    void GetGreenVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoGain);
    void SetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain);
    void GetBlueVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoGain);
    void SetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain);
    void GetRedVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoBlackLevel);
    void SetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel);
    void GetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoBlackLevel);
    void SetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel);
    void GetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoBlackLevel);
    void SetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel);
    void GetColorTemperature (const Jamm::ui4& InstanceID, Jamm::ui2& CurrentColorTemperature);
    void SetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature);
    void GetHorizontalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentHorizontalKeystone);
    void SetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone);
    void GetVerticalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentVerticalKeystone);
    void SetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone);
    void GetMute(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentMute);
    void SetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    void GetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::ui2& CurrentVolume);
    void SetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume);
    void GetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& CurrentVolume);
    void SetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume);
    void GetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& MinValue, Jamm::i2& MaxValue);
    void GetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness);
    void SetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    void _reqListPresets(const Jamm::ui4& InstanceID, );
    void _reqSelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName);
    void _reqGetBrightness(const Jamm::ui4& InstanceID, );
    void _reqSetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness);
    void _reqGetContrast(const Jamm::ui4& InstanceID, );
    void _reqSetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast);
    void _reqGetSharpness(const Jamm::ui4& InstanceID, );
    void _reqSetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness);
    void _reqGetRedVideoGain(const Jamm::ui4& InstanceID, );
    void _reqSetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain);
    void _reqGetGreenVideoGain(const Jamm::ui4& InstanceID, );
    void _reqSetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain);
    void _reqGetBlueVideoGain(const Jamm::ui4& InstanceID, );
    void _reqSetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain);
    void _reqGetRedVideoBlackLevel(const Jamm::ui4& InstanceID, );
    void _reqSetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel);
    void _reqGetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, );
    void _reqSetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel);
    void _reqGetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, );
    void _reqSetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel);
    void _reqGetColorTemperature (const Jamm::ui4& InstanceID, );
    void _reqSetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature);
    void _reqGetHorizontalKeystone(const Jamm::ui4& InstanceID, );
    void _reqSetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone);
    void _reqGetVerticalKeystone(const Jamm::ui4& InstanceID, );
    void _reqSetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone);
    void _reqGetMute(const Jamm::ui4& InstanceID, const std::string& Channel, );
    void _reqSetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    void _reqGetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, );
    void _reqSetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume);
    void _reqGetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, );
    void _reqSetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume);
    void _reqGetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, );
    void _reqGetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, );
    void _reqSetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    std::string _getLastChange();

protected:
    virtual void _ansListPresets(const Jamm::ui4& InstanceID, const std::string& CurrentPresetNameList) = 0;
    virtual void _ansSelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName) = 0;
    virtual void _ansGetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBrightness) = 0;
    virtual void _ansSetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness) = 0;
    virtual void _ansGetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentContrast) = 0;
    virtual void _ansSetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast) = 0;
    virtual void _ansGetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentSharpness) = 0;
    virtual void _ansSetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness) = 0;
    virtual void _ansGetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentRedVideoGain) = 0;
    virtual void _ansSetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain) = 0;
    virtual void _ansGetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentGreenVideoGain) = 0;
    virtual void _ansSetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain) = 0;
    virtual void _ansGetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBlueVideoGain) = 0;
    virtual void _ansSetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain) = 0;
    virtual void _ansGetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentRedVideoBlackLevel) = 0;
    virtual void _ansSetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel) = 0;
    virtual void _ansGetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentGreenVideoBlackLevel) = 0;
    virtual void _ansSetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel) = 0;
    virtual void _ansGetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentBlueVideoBlackLevel) = 0;
    virtual void _ansSetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel) = 0;
    virtual void _ansGetColorTemperature (const Jamm::ui4& InstanceID, const Jamm::ui2& CurrentColorTemperature) = 0;
    virtual void _ansSetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature) = 0;
    virtual void _ansGetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& CurrentHorizontalKeystone) = 0;
    virtual void _ansSetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone) = 0;
    virtual void _ansGetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& CurrentVerticalKeystone) = 0;
    virtual void _ansSetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone) = 0;
    virtual void _ansGetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& CurrentMute) = 0;
    virtual void _ansSetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute) = 0;
    virtual void _ansGetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& CurrentVolume) = 0;
    virtual void _ansSetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume) = 0;
    virtual void _ansGetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& CurrentVolume) = 0;
    virtual void _ansSetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume) = 0;
    virtual void _ansGetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& MinValue, const Jamm::i2& MaxValue) = 0;
    virtual void _ansGetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& CurrentLoudness) = 0;
    virtual void _ansSetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness) = 0;

    virtual void _changedLastChange(const std::string& val) = 0;

private:
    void _threadListPresets(Jamm::Action* pAction);
    void _threadSelectPreset(Jamm::Action* pAction);
    void _threadGetBrightness(Jamm::Action* pAction);
    void _threadSetBrightness(Jamm::Action* pAction);
    void _threadGetContrast(Jamm::Action* pAction);
    void _threadSetContrast(Jamm::Action* pAction);
    void _threadGetSharpness(Jamm::Action* pAction);
    void _threadSetSharpness(Jamm::Action* pAction);
    void _threadGetRedVideoGain(Jamm::Action* pAction);
    void _threadSetRedVideoGain(Jamm::Action* pAction);
    void _threadGetGreenVideoGain(Jamm::Action* pAction);
    void _threadSetGreenVideoGain(Jamm::Action* pAction);
    void _threadGetBlueVideoGain(Jamm::Action* pAction);
    void _threadSetBlueVideoGain(Jamm::Action* pAction);
    void _threadGetRedVideoBlackLevel(Jamm::Action* pAction);
    void _threadSetRedVideoBlackLevel(Jamm::Action* pAction);
    void _threadGetGreenVideoBlackLevel(Jamm::Action* pAction);
    void _threadSetGreenVideoBlackLevel(Jamm::Action* pAction);
    void _threadGetBlueVideoBlackLevel(Jamm::Action* pAction);
    void _threadSetBlueVideoBlackLevel(Jamm::Action* pAction);
    void _threadGetColorTemperature (Jamm::Action* pAction);
    void _threadSetColorTemperature(Jamm::Action* pAction);
    void _threadGetHorizontalKeystone(Jamm::Action* pAction);
    void _threadSetHorizontalKeystone(Jamm::Action* pAction);
    void _threadGetVerticalKeystone(Jamm::Action* pAction);
    void _threadSetVerticalKeystone(Jamm::Action* pAction);
    void _threadGetMute(Jamm::Action* pAction);
    void _threadSetMute(Jamm::Action* pAction);
    void _threadGetVolume(Jamm::Action* pAction);
    void _threadSetVolume(Jamm::Action* pAction);
    void _threadGetVolumeDB(Jamm::Action* pAction);
    void _threadSetVolumeDB(Jamm::Action* pAction);
    void _threadGetVolumeDBRange(Jamm::Action* pAction);
    void _threadGetLoudness(Jamm::Action* pAction);
    void _threadSetLoudness(Jamm::Action* pAction);

    Jamm::Service* m_pService;
};


class MediaRendererController : public Jamm::ControllerImplAdapter
{
public:
    MediaRendererController(Jamm::Device* pDevice, RenderingControlController* pRenderingControlController, ConnectionManagerController* pConnectionManagerController, AVTransportController* pAVTransportController);

    RenderingControlController* RenderingControl() { return m_pRenderingControlController; }
    ConnectionManagerController* ConnectionManager() { return m_pConnectionManagerController; }
    AVTransportController* AVTransport() { return m_pAVTransportController; }

private:
    virtual void eventHandler(Jamm::StateVar* pStateVar);

    Jamm::Device* m_pDevice;
    RenderingControlController* m_pRenderingControlController;
    ConnectionManagerController* m_pConnectionManagerController;
    AVTransportController* m_pAVTransportController;
};

#endif

