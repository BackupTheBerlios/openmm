#include <jamm/upnp.h>
#include "MediaRendererImpl.h"


AVTransportImplementation::AVTransportImplementation()
{
    m_pRenderer = static_cast<MediaRendererImplementation*>(m_pMediaRenderer);
}


void
AVTransportImplementation::SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    std::string transportState = _getTransportState();
    std::cerr << "AVTransportImplementation::SetAVTransportURI() enters in state: " << transportState << std::endl;
    
    if (transportState == "NO_MEDIA_PRESENT") {
        _setTransportState("STOPPED");
    }

    m_pRenderer->m_engine->setUri(CurrentURI);
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
    std::string timestamp;
//     PLT_Didl::FormatTimeStamp(timestamp, duration);
    _setCurrentMediaDuration(timestamp);
    _setCurrentTrackDuration(timestamp);
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
    Track = _getCurrentTrack();
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
            m_pRenderer->m_engine->stop();
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
            m_pRenderer->m_engine->load();
        }
        else if (transportState == "PLAYING") {
            if (_getCurrentTrackURI() != m_lastCurrentTrackUri) {
                std::string pos;
//                 PLT_Didl::FormatTimeStamp(pos, 0);
                _setAbsoluteTimePosition(pos);
                _setRelativeTimePosition(pos);
                m_pRenderer->m_engine->load();
            }
        }
        else if (transportState == "PAUSED_PLAYBACK") {
            m_pRenderer->m_engine->pause();
        }
        else if (transportState == "TRANSITIONING") {
            // TODO: can we set another speed after transitioning, than normal speed?
            m_pRenderer->m_engine->setSpeed(1, 1);
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
        m_pRenderer->m_engine->pause();
        
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
        m_pRenderer->m_engine->seek(position);
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
ConnectionManagerImplementation::GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
// begin of your own code

// end of your own code
}


RenderingControlImplementation::RenderingControlImplementation()
{
    m_pRenderer = static_cast<MediaRendererImplementation*>(m_pMediaRenderer);
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
    m_pRenderer->m_engine->setVolume(chan, DesiredVolume);
    
    _setVolume(DesiredVolume);
}


