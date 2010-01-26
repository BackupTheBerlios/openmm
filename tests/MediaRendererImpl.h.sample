#ifndef MEDIARENDERER_IMPLEMENTATION_H
#define MEDIARENDERER_IMPLEMENTATION_H

#include <jamm/upnp.h>
#include "MediaRenderer.h"

class AVTransportImplementation : public AVTransport
{
public:
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
};

class ConnectionManagerImplementation : public ConnectionManager
{
public:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink);
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs);
    virtual void GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status);
};

class RenderingControlImplementation : public RenderingControl
{
public:
    virtual void ListPresets(const Jamm::ui4& InstanceID, std::string& CurrentPresetNameList);
    virtual void SelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName);
    virtual void GetMute(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentMute);
    virtual void SetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute);
    virtual void GetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::ui2& CurrentVolume);
    virtual void SetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume);
};

#endif

