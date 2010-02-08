#ifndef MEDIARENDERER_IMPLEMENTATION_H
#define MEDIARENDERER_IMPLEMENTATION_H

#include <jamm/upnp.h>
#include <jamm/upnpav.h>

#include "engine.h"
#include "MediaRenderer.h"

// TODO: LastChange

class MediaRendererImplementation;

class AVTransportImplementation : public AVTransport
{
    friend class MediaRendererImplementation;
    
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
    
    Engine* m_pEngine;
    std::string                     m_lastCurrentTrackUri;
};

class ConnectionManagerImplementation : public ConnectionManager
{
    friend class MediaRendererImplementation;
    
private:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink);
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    // FIXME: strange syntax error which is none, is it gcc???!!!
//     virtual void GetCurrentConnectionInfo(std::string& Status);
    virtual void GetCurrentConnectionInfo(/*const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status*/);
    
    virtual void initStateVars();
    
    Engine* m_pEngine;
};

class RenderingControlImplementation : public RenderingControl
{
    friend class MediaRendererImplementation;
    
private:
    virtual void ListPresets(const Jamm::ui4& InstanceID, std::string& CurrentPresetNameList);
    virtual void SelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName);
    virtual void GetBrightness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBrightness);
    virtual void SetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness);
    virtual void GetContrast(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentContrast);
    virtual void SetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast);
    virtual void GetSharpness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentSharpness);
    virtual void SetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness);
    virtual void GetRedVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoGain);
    virtual void SetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain);
    virtual void GetGreenVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoGain);
    virtual void SetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain);
    virtual void GetBlueVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoGain);
    virtual void SetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain);
    virtual void GetRedVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoBlackLevel);
    virtual void SetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel);
    virtual void GetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoBlackLevel);
    virtual void SetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel);
    virtual void GetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoBlackLevel);
    virtual void SetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel);
    virtual void GetColorTemperature (const Jamm::ui4& InstanceID, Jamm::ui2& CurrentColorTemperature);
    virtual void SetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature);
    virtual void GetHorizontalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentHorizontalKeystone);
    virtual void SetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone);
    virtual void GetVerticalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentVerticalKeystone);
    virtual void SetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone);
    virtual void GetMute(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentMute);
    virtual void SetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    virtual void GetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::ui2& CurrentVolume);
    virtual void SetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume);
    virtual void GetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& CurrentVolume);
    virtual void SetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume);
    virtual void GetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& MinValue, Jamm::i2& MaxValue);
    virtual void GetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness);
    virtual void SetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness);
    
    virtual void initStateVars();
    
    Engine* m_pEngine;
};


class MediaRendererImplementation : public MediaRenderer
{
public:
    MediaRendererImplementation(RenderingControlImplementation* pRenderingControlImpl,
                                ConnectionManagerImplementation* pConnectionManagerImpl,
                                AVTransportImplementation* pAVTransportImpl,
                                Engine* engine) :
        MediaRenderer(pRenderingControlImpl,
                      pConnectionManagerImpl,
                      pAVTransportImpl)
    {
        pRenderingControlImpl->m_pEngine = engine;
        pConnectionManagerImpl->m_pEngine = engine;
        pAVTransportImpl->m_pEngine = engine;
    }
};

#endif

