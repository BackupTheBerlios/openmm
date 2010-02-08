#include "MediaRendererImpl.h"


// AVTransportImplementation::AVTransportImplementation()
// {
//     m_pRenderer = static_cast<MediaRendererImplementation*>(m_pMediaRenderer);
// }


void
AVTransportImplementation::initStateVars()
{
    _setTransportState("STOPPED");
    _setTransportStatus("OK");
    _setPlaybackStorageMedium("NOT_IMPLEMENTED");
    _setRecordStorageMedium("NOT_IMPLEMENTED");
    _setPossiblePlaybackStorageMedia("NOT_IMPLEMENTED");
    _setPossibleRecordStorageMedia("NOT_IMPLEMENTED");
    _setCurrentPlayMode("NORMAL");
    _setTransportPlaySpeed("1");
    _setRecordMediumWriteStatus("NOT_IMPLEMENTED");
    _setCurrentRecordQualityMode("NOT_IMPLEMENTED");
    _setPossibleRecordQualityModes("NOT_IMPLEMENTED");
    _setNumberOfTracks(0);
    _setCurrentTrack(0);
    _setCurrentTrackDuration("00:00:00");
    _setCurrentMediaDuration("00:00:00");
    _setCurrentTrackMetaData("NOT_IMPLEMENTED");
    _setCurrentTrackURI("");
    _setAVTransportURI("");
    _setAVTransportURIMetaData("NOT_IMPLEMENTED");
    _setNextAVTransportURI("");
    _setNextAVTransportURIMetaData("NOT_IMPLEMENTED");
    _setRelativeTimePosition("NOT_IMPLEMENTED");
    _setAbsoluteTimePosition("NOT_IMPLEMENTED");
    _setRelativeCounterPosition(2147483647);
    _setAbsoluteCounterPosition(2147483647);
    _setCurrentTransportActions("NOT_IMPLEMENTED");
    _setLastChange("");
}


void
AVTransportImplementation::SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::SetAVTransportURI() enters in state: " << transportState << std::endl;
    
    if (transportState == "NO_MEDIA_PRESENT") {
        _setTransportState("STOPPED");
    }

    std::cerr << "m_engine->setUri() uri: " << CurrentURI << std::endl;
//     std::cerr << "renderer: " << m_pRenderer << std::endl;
    std::cerr << "engine: " << m_pEngine << std::endl;
    std::cerr << "id: " << m_pEngine->getEngineId() << std::endl;
    m_pEngine->setUri(CurrentURI);
    std::cerr << "m_engine->setUri() finished" << std::endl;
    _setCurrentTrackURI(CurrentURI);
/*
2.2.16.CurrentTrackMetaData
Metadata, in the form of a valid DIDL-Lite XML fragment (defined in the ContentDirectory service
template), associated with the resource pointed to by state variable CurrentTrackURI. The meta data may
have been extracted from state variable AVTransportURIMetaData, or extracted from the resource binary
itself (e.g., embedded ID3 tags for MP3 audio). This is implementation dependent. If the service
implementation doesn’t support this feature then this state variable must be set to value
“NOT_IMPLEMENTED”.
*/
    _setCurrentTrackMetaData(CurrentURIMetaData);
    
//     PLT_MediaItem* item;
//     PLT_MediaObjectListReference didl;
//     PLT_Didl::FromDidl(currentUriMetaData, didl);
//     item = (PLT_MediaItem*)(*didl->GetFirstItem());
//     int meta_duration = item->m_Resources.GetFirstItem()->m_Duration;
//     int duration = (meta_duration>0)?meta_duration:0;
// //     Jamm::MediaObject obj(CurrentURIMetaData);
//     Jamm::r8 duration;
//     std::string timestamp = Jamm::AvTypeConverter::writeDuration(duration);
// //     std::string duration = obj.getProperty("res@duration");
// //     _setCurrentMediaDuration(duration);
// //     _setCurrentTrackDuration(duration);
    std::cerr << "AVTransportImplementation::SetAVTransportURI() leaves in state: " << transportState << std::endl;
}


void
AVTransportImplementation::GetMediaInfo(const Jamm::ui4& InstanceID, Jamm::ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportInfo(const Jamm::ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetPositionInfo(const Jamm::ui4& InstanceID, Jamm::ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, Jamm::i4& RelCount, Jamm::i4& AbsCount)
{
    std::cerr << "AVTransportImplementation::GetPositionInfo()" << std::endl;
    std::cerr << "_getCurrentTrack()" << std::endl;
    Track = _getCurrentTrack();
    std::cerr << "_getCurrentTrackDuration()" << std::endl;
    TrackDuration = _getCurrentTrackDuration();
    TrackMetaData = _getCurrentTrackMetaData();
    TrackURI = _getCurrentTrackURI();
    RelTime = _getRelativeTimePosition();
    AbsTime = _getAbsoluteTimePosition();
    RelCount = _getRelativeCounterPosition();
    AbsCount = _getAbsoluteCounterPosition();
}


void
AVTransportImplementation::GetDeviceCapabilities(const Jamm::ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportSettings(const Jamm::ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Stop(const Jamm::ui4& InstanceID)
{
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::Stop() enters in state: " << transportState << std::endl;
    
    if (transportState != "NO_MEDIA_PRESENT") {
        // be nice to the engine and don't stop when already in stopped or paused state
        // TODO: when in PAUSED_PLAYBACK we should actually stop (according to AVTransport 1.0 specs
        //       -> unfortunately mplayer has it's problems with stopping ...
        if (transportState != "STOPPED" && transportState != "PAUSED_PLAYBACK") {
            m_pEngine->stop();
        }
        // TODO: check if engine really stopped (by return value)
        _setTransportState("STOPPED");
        
        if (transportState == "PLAYING" || transportState == "PAUSED_PLAYBACK") {
        // TODO: reset positions and speed (this is not mentioned in the AVTransport 1.0 specs ...)?
        //       what does Stop() mean when in TRANSITIONING state? 
        //       -> stop transitioning and start playback at current position?
            std::string pos;
//             PLT_Didl::FormatTimeStamp(pos, 0);
            _setAbsoluteTimePosition(pos);
            _setRelativeTimePosition(pos);
            _setTransportPlaySpeed("0");
        }
    }
}


void
AVTransportImplementation::Play(const Jamm::ui4& InstanceID, const std::string& Speed)
{
    std::string speed = Speed;
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::Play() enters in state: " << transportState << std::endl;
    if (transportState == "STOPPED"
        || transportState == "PLAYING"
        || transportState == "PAUSED_PLAYBACK"
        || transportState == "TRANSITIONING")
    {
        if (transportState == "STOPPED") {
            // TODO: does it mean: play from beginning now?
            //       if yes: after STOPPED -> TRANSITIONING -> STOPPED we would play from beginning
            //               and not from the position we were seeking to.
            //       if no:  Action Stop behaves like Pause
            //       -> maybe it should be: STOPPED -> TRANSITIONING -> PAUSED_PLAYBACK
            //          contradicting AVTransport, 1.0, 2.4.12.3.
            m_pEngine->load();
        }
        else if (transportState == "PLAYING") {
            if (_getCurrentTrackURI() != m_lastCurrentTrackUri) {
                std::string pos;
//                 PLT_Didl::FormatTimeStamp(pos, 0);
                _setAbsoluteTimePosition(pos);
                _setRelativeTimePosition(pos);
                m_pEngine->load();
            }
        }
        else if (transportState == "PAUSED_PLAYBACK") {
            m_pEngine->pause();
        }
        else if (transportState == "TRANSITIONING") {
            // TODO: can we set another speed after transitioning, than normal speed?
            m_pEngine->setSpeed(1, 1);
            speed = "1";
        }
        // TODO: handle changes in speed
        // TODO: handle fractions of speed ('1/2', ...)
        // TODO: check state of engine (return value of load()).
        //       If start playing takes considerable amount of time
        //       set state to "TRANSITIONING" (-> 2.4.9.3. Effect on State, AVTransport spec)
        
        _setTransportState("PLAYING");
        _setTransportPlaySpeed(speed);
        
        m_lastCurrentTrackUri = _getCurrentTrackURI();
    }
}


void
AVTransportImplementation::Pause(const Jamm::ui4& InstanceID)
{
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::Pause() enters in state: " << transportState << std::endl;
    
    if (transportState == "PLAYING" || transportState == "RECORDING") {
        m_pEngine->pause();
        
        if (transportState == "PLAYING") {
            _setTransportState("PAUSED_PLAYBACK");
        }
        else if (transportState == "RECORDING") {
            _setTransportState("PAUSED_RECORDING");
        }
    }
}


void
AVTransportImplementation::Seek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::Pause() enters in state: " << transportState << std::endl;
    
    if (transportState == "STOPPED" || transportState == "PLAYING") {
        // TODO: does it make sense to handle "PAUSED_PLAYBACK", too?
        std::cerr << "AVTransportImplementation::Seek() seek mode: " << Unit << ", seek target: " << Target << std::endl;
        
        Jamm::ui4 position;
        if (Unit == "ABS_TIME") {
//             PLT_Didl::ParseTimeStamp(seekTarget, position);
        }
        m_pEngine->seek(position);
        // TODO: according to the specs AVTransport 1.0, 2.4.12.3.Effect on State
        //       TransportState should be set to TRANSITIONING, but only while seeking.
        //       OnSeek() should return immediately! So we are not conform here.
        // _setTransportState("TRANSITIONING");
        //       For now, we just keep the transportState to make life easier.
        _setRelativeTimePosition(Target);
        _setAbsoluteTimePosition(Target);
    }
}


void
AVTransportImplementation::Next(const Jamm::ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Previous(const Jamm::ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


// ConnectionManagerImplementation::ConnectionManagerImplementation()
// {
//     m_pRenderer = static_cast<MediaRendererImplementation*>(m_pMediaRenderer);
// }


void
ConnectionManagerImplementation::initStateVars()
{
    _setSourceProtocolInfo("http-get:*:*:*");
    _setSinkProtocolInfo("http-get:*:*:*");
    _setCurrentConnectionIDs("0");
}


void
ConnectionManagerImplementation::GetProtocolInfo(std::string& Source, std::string& Sink)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::GetCurrentConnectionInfo(/*const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status*/)
{
// begin of your own code

// end of your own code
}


// RenderingControlImplementation::RenderingControlImplementation()
// {
//     m_pRenderer = static_cast<MediaRendererImplementation*>(m_pMediaRenderer);
// }


void
RenderingControlImplementation::initStateVars()
{
    _setPresetNameList("");
    _setLastChange("");
    _setBrightness(0);
    _setContrast(0);
    _setSharpness(0);
    _setRedVideoGain(0);
    _setGreenVideoGain(0);
    _setBlueVideoGain(0);
    _setRedVideoBlackLevel(0);
    _setGreenVideoBlackLevel(0);
    _setBlueVideoBlackLevel(0);
    _setColorTemperature(0);
    _setHorizontalKeystone(0);
    _setVerticalKeystone(0);
    _setMute(false);
    _setVolume(0);
    _setVolumeDB(0);
    _setLoudness(false);
}


void
RenderingControlImplementation::ListPresets(const Jamm::ui4& InstanceID, std::string& CurrentPresetNameList)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SelectPreset(const Jamm::ui4& InstanceID, const std::string& PresetName)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetBrightness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBrightness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetBrightness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBrightness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetContrast(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentContrast)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetContrast(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredContrast)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetSharpness(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentSharpness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetSharpness(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredSharpness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetRedVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetRedVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetGreenVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetGreenVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetBlueVideoGain(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetBlueVideoGain(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetRedVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetRedVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetGreenVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, Jamm::ui2& CurrentBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetBlueVideoBlackLevel(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetColorTemperature (const Jamm::ui4& InstanceID, Jamm::ui2& CurrentColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetColorTemperature(const Jamm::ui4& InstanceID, const Jamm::ui2& DesiredColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetHorizontalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetHorizontalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetVerticalKeystone(const Jamm::ui4& InstanceID, Jamm::i2& CurrentVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetVerticalKeystone(const Jamm::ui4& InstanceID, const Jamm::i2& DesiredVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetMute(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentMute)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetMute(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::ui2& CurrentVolume)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetVolume(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::ui2& DesiredVolume)
{
    std::cerr << "RenderingControlImplementation::SetVolume() channel: " << Channel << ", volume: " << DesiredVolume << std::endl;
    
    Jamm::i4 chan;
    Jamm::Variant(Channel).getValue(chan);
    m_pEngine->setVolume(chan, DesiredVolume);
    
    _setVolume(DesiredVolume);
}


void
RenderingControlImplementation::GetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& CurrentVolume)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetVolumeDB(const Jamm::ui4& InstanceID, const std::string& Channel, const Jamm::i2& DesiredVolume)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetVolumeDBRange(const Jamm::ui4& InstanceID, const std::string& Channel, Jamm::i2& MinValue, Jamm::i2& MaxValue)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::GetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlImplementation::SetLoudness(const Jamm::ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
// begin of your own code
    
// end of your own code
}

