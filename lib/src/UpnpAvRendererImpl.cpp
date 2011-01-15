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
#include "UpnpAvRendererImpl.h"

namespace Omm {
namespace Av {

void
AVTransportRendererImpl::initStateVars()
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
    
    _pSession = 0;
}


void
AVTransportRendererImpl::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    std::string transportState = _getTransportState();
    Omm::Av::Log::instance()->upnpav().debug("SetAVTransporURI enters in state: " + transportState);
    
    if (transportState == "NO_MEDIA_PRESENT") {
        _setTransportState("STOPPED");
    }

    Omm::Av::Log::instance()->upnpav().debug("engine: " + _pEngine->getEngineId() + " set uri: " + CurrentURI);
    if (_pEngine->preferStdStream() && (_getCurrentTrackURI() != CurrentURI || _getTransportState() == "STOPPED")) {
        Poco::URI uri(CurrentURI);
        _pEngine->stop();
        if (_pSession) {
            delete _pSession;
        }
        _pSession = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request("GET", uri.getPath());
        _pSession->sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        Omm::Av::Log::instance()->upnpav().debug("request header:\n" + requestHeader.str());
        
        Poco::Net::HTTPResponse response;
        std::istream& istr = _pSession->receiveResponse(response);
        
        Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream responseHeader;
        response.write(responseHeader);
        Omm::Av::Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());
        _pEngine->setUri(istr);
    }
    else {
        _pEngine->setUri(CurrentURI);
    }
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
    _setCurrentTrackDuration("00:00:00");

    try {
        MemoryMediaObject obj;
        MediaObjectReader objReader(&obj);
        objReader.read(CurrentURIMetaData);
        
        std::string duration = obj.getResource()->getAttributeValue("duration");
        if (duration != "") {
            Omm::Av::Log::instance()->upnpav().debug("set duration from CurrentURIMetaData to: " + duration);
            _setCurrentTrackDuration(duration);
        }
    }
    catch (...) {
        Omm::Av::Log::instance()->upnpav().error("could not parse uri meta data");
    }
    Omm::Av::Log::instance()->upnpav().debug("SetAVTransporURI leaves in state: " + transportState);
}


void
AVTransportRendererImpl::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
    NrTracks = _getNumberOfTracks();
    MediaDuration = _getCurrentMediaDuration();
    CurrentURI = _getCurrentTrackURI();
    CurrentURIMetaData = _getCurrentTrackMetaData();
    NextURI = _getNextAVTransportURI();
    NextURIMetaData = _getNextAVTransportURIMetaData();
    PlayMedium = _getPlaybackStorageMedium();
    RecordMedium = _getRecordStorageMedium();
    WriteStatus = _getRecordMediumWriteStatus();
}


void
AVTransportRendererImpl::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
    CurrentTransportState = _getTransportState();
    CurrentTransportStatus = _getTransportStatus();
    CurrentSpeed = _getTransportPlaySpeed();
}


void
AVTransportRendererImpl::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
    Omm::Av::Log::instance()->upnpav().debug("GetPositionInfo() ...");
    Track = _getCurrentTrack();
    
    float engineTrackDuration = _pEngine->getLengthSeconds();
    Omm::Av::Log::instance()->upnpav().debug("engine track duration (sec): " + Poco::NumberFormatter::format(engineTrackDuration, 2));
    if (engineTrackDuration > 0.0) {
        _setCurrentTrackDuration(AvTypeConverter::writeDuration(engineTrackDuration));
        Omm::Av::Log::instance()->upnpav().debug("set TrackDuration to: " + _getCurrentTrackDuration());
        TrackDuration = _getCurrentTrackDuration();
    }

    TrackMetaData = _getCurrentTrackMetaData();
    TrackURI = _getCurrentTrackURI();
    
    float enginePosition = _pEngine->getPositionPercentage();
    Omm::Av::Log::instance()->upnpav().debug("engine position: " + Poco::NumberFormatter::format(enginePosition, 2));
    float engineTimePosition = _pEngine->getPositionSecond();
    Omm::Av::Log::instance()->upnpav().debug("engine position (sec): " + Poco::NumberFormatter::format(engineTimePosition, 2));
    
    std::string timePosition = AvTypeConverter::writeDuration(engineTimePosition);
    Omm::Av::Log::instance()->upnpav().debug("set RelativePosition to: " + timePosition);
    _setRelativeTimePosition(timePosition);
    _setAbsoluteTimePosition(timePosition);
    RelTime = _getRelativeTimePosition();
    AbsTime = _getAbsoluteTimePosition();
    
    RelCount = _getRelativeCounterPosition();
    AbsCount = _getAbsoluteCounterPosition();
}


void
AVTransportRendererImpl::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
    PlayMedia = _getPossiblePlaybackStorageMedia();
    RecMedia = _getPossibleRecordStorageMedia();
    RecQualityModes = _getPossibleRecordQualityModes();
}


void
AVTransportRendererImpl::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
    PlayMode = _getCurrentPlayMode();
    RecQualityMode = _getCurrentRecordQualityMode();
}


void
AVTransportRendererImpl::Stop(const ui4& InstanceID)
{
    std::string transportState = _getTransportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Stop() enters in state: " + transportState);
    
    if (transportState != "NO_MEDIA_PRESENT") {
        // be nice to the engine and don't stop when already in stopped or paused state
        // TODO: when in PAUSED_PLAYBACK we should actually stop (according to AVTransport 1.0 specs
        //       -> unfortunately mplayer has it's problems with stopping ...
        if (transportState != "STOPPED" && transportState != "PAUSED_PLAYBACK") {
            _pEngine->stop();
            if (_pEngine->preferStdStream()) {
                delete _pSession;
                _pSession = 0;
            }
        }
        // TODO: check if engine really stopped (by return value)
        _setTransportState("STOPPED");
        
        if (transportState == "PLAYING" || transportState == "PAUSED_PLAYBACK") {
        // TODO: reset positions and speed (this is not mentioned in the AVTransport 1.0 specs ...)?
        //       what does Stop() mean when in TRANSITIONING state? 
        //       -> stop transitioning and start playback at current position?
            _setAbsoluteTimePosition("00:00:00");
            _setRelativeTimePosition("00:00:00");
            _setTransportPlaySpeed("0");
        }
    }
}


void
AVTransportRendererImpl::Play(const ui4& InstanceID, const std::string& Speed)
{
    std::string speed = Speed;
    std::string transportState = _getTransportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Play() enters in state: " + transportState);
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
            _pEngine->load();
        }
        else if (transportState == "PLAYING") {
            if (_getCurrentTrackURI() != _lastCurrentTrackUri) {
                std::string pos;
//                 PLT_Didl::FormatTimeStamp(pos, 0);
                _setAbsoluteTimePosition(pos);
                _setRelativeTimePosition(pos);
                _pEngine->load();
            }
        }
        else if (transportState == "PAUSED_PLAYBACK") {
            _pEngine->pause();
        }
        else if (transportState == "TRANSITIONING") {
            // TODO: can we set another speed after transitioning, than normal speed?
            _pEngine->setSpeed(1, 1);
            speed = "1";
        }
        // TODO: handle changes in speed
        // TODO: handle fractions of speed ('1/2', ...)
        // TODO: check state of engine (return value of load()).
        //       If start playing takes considerable amount of time
        //       set state to "TRANSITIONING" (-> 2.4.9.3. Effect on State, AVTransport spec)
        
        _setTransportState("PLAYING");
        _setTransportPlaySpeed(speed);
        
        _lastCurrentTrackUri = _getCurrentTrackURI();
    }
}


void
AVTransportRendererImpl::Pause(const ui4& InstanceID)
{
    std::string transportState = _getTransportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Pause() enters in state: " + transportState);
    
    if (transportState == "PLAYING" || transportState == "RECORDING") {
        _pEngine->pause();
        
        if (transportState == "PLAYING") {
            _setTransportState("PAUSED_PLAYBACK");
        }
        else if (transportState == "RECORDING") {
            _setTransportState("PAUSED_RECORDING");
        }
    }
}


void
AVTransportRendererImpl::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    std::string transportState = _getTransportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Seek() enters in state: " + transportState);
    
    if (transportState == "STOPPED" || transportState == "PLAYING") {
        // TODO: does it make sense to handle "PAUSED_PLAYBACK", too?
        Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Seek() seek mode: " + Unit + ", seek target: " + Target);
        
        ui4 position;
        if (Unit == "ABS_TIME") {
            position = AvTypeConverter::readTime(Target).epochMicroseconds() / 1000000;
        }
        _pEngine->seekSecond(position);
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
AVTransportRendererImpl::Next(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportRendererImpl::Previous(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


// ConnectionManagerRendererImpl::ConnectionManagerRendererImpl()
// {
//     _pRenderer = static_cast<MediaRendererImplementation*>(_pMediaRenderer);
// }


void
ConnectionManagerRendererImpl::initStateVars()
{
//     _setSourceProtocolInfo("http-get:*:*:*");
    _setSourceProtocolInfo("");
    _setSinkProtocolInfo("http-get:*:*:*");
    _setCurrentConnectionIDs("0");
}


void
ConnectionManagerRendererImpl::GetProtocolInfo(std::string& Source, std::string& Sink)
{
// begin of your own code
    Source = "";
    // TODO: this should ask the rendering engine about the formats it can render
    Sink = "http-get:*:*:*";
// end of your own code
}


void
ConnectionManagerRendererImpl::ConnectionComplete(const i4& ConnectionID)
{
// begin of your own code
    
// end of your own code
}


void
ConnectionManagerRendererImpl::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
// begin of your own code
    ConnectionIDs = "0";
// end of your own code
}


void
ConnectionManagerRendererImpl::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
// begin of your own code

// end of your own code
}


// RenderingControlRendererImpl::RenderingControlRendererImpl()
// {
//     _pRenderer = static_cast<MediaRendererImplementation*>(_pMediaRenderer);
// }


void
RenderingControlRendererImpl::initStateVars()
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
RenderingControlRendererImpl::ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetContrast(const ui4& InstanceID, ui2& CurrentContrast)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume)
{
    CurrentVolume = _getVolume();
}


void
RenderingControlRendererImpl::SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Omm::Av::Log::instance()->upnpav().debug("RenderingControlRendererImpl::SetVolume() channel: " + Channel + ", volume: " + Poco::NumberFormatter::format(DesiredVolume));
    
    _pEngine->setVolume(0, DesiredVolume);
    
    _setVolume(DesiredVolume);
//     std::cerr << "RenderingControlRendererImpl::SetVolume() finished" << std::endl;
}


void
RenderingControlRendererImpl::GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume)
{
    CurrentVolume = _getVolumeDB();
}


void
RenderingControlRendererImpl::SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness)
{
// begin of your own code
    
// end of your own code
}


void
RenderingControlRendererImpl::SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
// begin of your own code
    
// end of your own code
}

} // namespace Av
} // namespace Omm
