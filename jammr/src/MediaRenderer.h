#ifndef MEDIARENDERER_H
#define MEDIARENDERER_H

#include <jamm/upnp.h>

using Jamm::DeviceRootImplAdapter;
using Jamm::Service;
using Jamm::Action;

class MediaRenderer;

class AVTransport
{
friend class MediaRenderer;

public:
    virtual void SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData) = 0;
    virtual void GetMediaInfo(const Jamm::ui4& InstanceID, Jamm::ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus) = 0;
    virtual void GetTransportInfo(const Jamm::ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed) = 0;
    virtual void GetPositionInfo(const Jamm::ui4& InstanceID, Jamm::ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, Jamm::i4& RelCount, Jamm::i4& AbsCount) = 0;
    virtual void GetDeviceCapabilities(const Jamm::ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes) = 0;
    virtual void GetTransportSettings(const Jamm::ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode) = 0;
    virtual void Stop(const Jamm::ui4& InstanceID) = 0;
    virtual void Play(const Jamm::ui4& InstanceID, const std::string& Speed) = 0;
    virtual void Pause(const Jamm::ui4& InstanceID) = 0;
    virtual void Seek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target) = 0;
    virtual void Next(const Jamm::ui4& InstanceID) = 0;
    virtual void Previous(const Jamm::ui4& InstanceID) = 0;

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
    void _setNumberOfTracks(const Jamm::ui4& val);
    Jamm::ui4 _getNumberOfTracks();
    void _setCurrentTrack(const Jamm::ui4& val);
    Jamm::ui4 _getCurrentTrack();
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
    void _setRelativeCounterPosition(const Jamm::i4& val);
    Jamm::i4 _getRelativeCounterPosition();
    void _setAbsoluteCounterPosition(const Jamm::i4& val);
    Jamm::i4 _getAbsoluteCounterPosition();
    void _setCurrentTransportActions(const std::string& val);
    std::string _getCurrentTransportActions();
    void _setLastChange(const std::string& val);
    std::string _getLastChange();

protected:
    MediaRenderer* m_pMediaRenderer;

private:
    static std::string  m_description;
    Service*            m_pService;
};


class ConnectionManager
{
friend class MediaRenderer;

public:
    virtual void GetProtocolInfo(std::string& Source, std::string& Sink) = 0;
    virtual void GetCurrentConnectionIDs(std::string& ConnectionIDs) = 0;
    virtual void GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status) = 0;

    void _setSourceProtocolInfo(const std::string& val);
    std::string _getSourceProtocolInfo();
    void _setSinkProtocolInfo(const std::string& val);
    std::string _getSinkProtocolInfo();
    void _setCurrentConnectionIDs(const std::string& val);
    std::string _getCurrentConnectionIDs();

protected:
    MediaRenderer* m_pMediaRenderer;

private:
    static std::string  m_description;
    Service*            m_pService;
};


class RenderingControl
{
friend class MediaRenderer;

public:
    virtual void ListPresets(const Jamm::ui4& InstanceID, std::string& CurrentPresetNameList) = 0;
    virtual void SelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName) = 0;
    virtual void GetMute(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentMute) = 0;
    virtual void SetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute) = 0;
    virtual void GetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::ui2& CurrentVolume) = 0;
    virtual void SetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume) = 0;

    void _setPresetNameList(const std::string& val);
    std::string _getPresetNameList();
    void _setLastChange(const std::string& val);
    std::string _getLastChange();
    void _setBrightness(const Jamm::ui2& val);
    Jamm::ui2 _getBrightness();
    void _setContrast(const Jamm::ui2& val);
    Jamm::ui2 _getContrast();
    void _setSharpness(const Jamm::ui2& val);
    Jamm::ui2 _getSharpness();
    void _setRedVideoGain(const Jamm::ui2& val);
    Jamm::ui2 _getRedVideoGain();
    void _setGreenVideoGain(const Jamm::ui2& val);
    Jamm::ui2 _getGreenVideoGain();
    void _setBlueVideoGain(const Jamm::ui2& val);
    Jamm::ui2 _getBlueVideoGain();
    void _setRedVideoBlackLevel(const Jamm::ui2& val);
    Jamm::ui2 _getRedVideoBlackLevel();
    void _setGreenVideoBlackLevel(const Jamm::ui2& val);
    Jamm::ui2 _getGreenVideoBlackLevel();
    void _setBlueVideoBlackLevel(const Jamm::ui2& val);
    Jamm::ui2 _getBlueVideoBlackLevel();
    void _setColorTemperature(const Jamm::ui2& val);
    Jamm::ui2 _getColorTemperature();
    void _setHorizontalKeystone(const Jamm::i2& val);
    Jamm::i2 _getHorizontalKeystone();
    void _setVerticalKeystone(const Jamm::i2& val);
    Jamm::i2 _getVerticalKeystone();
    void _setMute(const bool& val);
    bool _getMute();
    void _setVolume(const Jamm::ui2& val);
    Jamm::ui2 _getVolume();
    void _setVolumeDB(const Jamm::i2& val);
    Jamm::i2 _getVolumeDB();
    void _setLoudness(const bool& val);
    bool _getLoudness();

protected:
    MediaRenderer* m_pMediaRenderer;

private:
    static std::string  m_description;
    Service*            m_pService;
};


class MediaRenderer : public DeviceRootImplAdapter
{
public:
    MediaRenderer(RenderingControl* pRenderingControlImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl);

private:
    virtual void actionHandler(Action* action);

    static std::string m_deviceDescription;
    RenderingControl* m_pRenderingControlImpl;
    ConnectionManager* m_pConnectionManagerImpl;
    AVTransport* m_pAVTransportImpl;
};

#endif

