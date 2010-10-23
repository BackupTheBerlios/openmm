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
#ifndef MEDIARENDERER_IMPLEMENTATION_H
#define MEDIARENDERER_IMPLEMENTATION_H

#include "UpnpAvTypes.h"
#include "UpnpAvRenderer.h"

namespace Omm {
namespace Av {


// TODO: LastChange

class AVTransportRendererImpl : public AVTransport
{
    friend class UpnpAvRenderer;
    
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

    Engine*                             _pEngine;
    Poco::Net::HTTPClientSession*       _pSession;
    Poco::Net::HTTPRequest*             _pRequest;
    std::string                         _lastCurrentTrackUri;
};

class ConnectionManagerRendererImpl : public ConnectionManager
{
    friend class UpnpAvRenderer;
    
private:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink);
    virtual void ConnectionComplete(const i4& ConnectionID);
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    virtual void GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status);

    virtual void initStateVars();

    Engine* _pEngine;
};

class RenderingControlRendererImpl : public RenderingControl
{
    friend class UpnpAvRenderer;
    
private:
    virtual void ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList);
    virtual void SelectPreset(const ui4& InstanceID, const std::string& PresetName);
    virtual void GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness);
    virtual void SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness);
    virtual void GetContrast(const ui4& InstanceID, ui2& CurrentContrast);
    virtual void SetContrast(const ui4& InstanceID, const ui2& DesiredContrast);
    virtual void GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness);
    virtual void SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness);
    virtual void GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain);
    virtual void SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain);
    virtual void GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain);
    virtual void SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain);
    virtual void GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain);
    virtual void SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain);
    virtual void GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel);
    virtual void SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel);
    virtual void GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel);
    virtual void SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel);
    virtual void GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel);
    virtual void SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel);
    virtual void GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature);
    virtual void SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature);
    virtual void GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone);
    virtual void SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone);
    virtual void GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone);
    virtual void SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone);
    virtual void GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute);
    virtual void SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    virtual void GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume);
    virtual void SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume);
    virtual void GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume);
    virtual void SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume);
    virtual void GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue);
    virtual void GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness);
    virtual void SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);

    virtual void initStateVars();

    Engine* _pEngine;
};


} // namespace Av
} // namespace Omm


#endif

