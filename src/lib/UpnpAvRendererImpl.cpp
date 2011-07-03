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

#include <sstream>

#include "UpnpAv.h"

#include "UpnpAvRendererImpl.h"

namespace Omm {
namespace Av {


void
DevAVTransportRendererImpl::initStateVars()
{
    // FIXME: init all engine instances instead of the state vars in the service tree.
    
//    _setTransportState(AvTransportArgument::TRANSPORT_STATE_STOPPED);
//    _setTransportStatus(AvTransportArgument::TRANSPORT_STATUS_OK);
//    _setPlaybackStorageMedium(AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED);
//    _setRecordStorageMedium(AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED);
//    _setPossiblePlaybackStorageMedia(AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED);
//    _setPossibleRecordStorageMedia(AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED);
//    _setCurrentPlayMode(AvTransportArgument::CURRENT_PLAY_MODE_NORMAL);
//    _setTransportPlaySpeed(AvTransportArgument::TRANSPORT_PLAY_SPEED_1);
//    _setRecordMediumWriteStatus(AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_NOT_IMPLEMENTED);
//    _setCurrentRecordQualityMode(AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_NOT_IMPLEMENTED);
//    _setPossibleRecordQualityModes(AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_NOT_IMPLEMENTED);
//    _setNumberOfTracks(0);
//    _setCurrentTrack(0);
//    _setCurrentTrackDuration(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    _setCurrentMediaDuration(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    _setCurrentTrackMetaData(AvTransportArgument::CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED);
//    _setCurrentTrackURI("");
//    _setAVTransportURI("");
//    _setAVTransportURIMetaData(AvTransportArgument::CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED);
//    _setNextAVTransportURI("");
//    _setNextAVTransportURIMetaData(AvTransportArgument::CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED);
//    _setRelativeTimePosition(AvTransportArgument::RELATIVE_TIME_POSITION_NOT_IMPLEMENTED);
//    _setAbsoluteTimePosition(AvTransportArgument::RELATIVE_TIME_POSITION_NOT_IMPLEMENTED);
//    _setRelativeCounterPosition(AvTransportArgument::RELATIVE_COUNTER_POSITION_UNDEFINED);
//    _setAbsoluteCounterPosition(AvTransportArgument::RELATIVE_COUNTER_POSITION_UNDEFINED);
//    _setCurrentTransportActions(AvTransportArgument::CURRENT_TRANSPORT_ACTIONS_NOT_IMPLEMENTED);

    _pSession = 0;
}


void
DevAVTransportRendererImpl::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
//    std::string transportState = _getTransportState();
    std::string transportState = _engines[InstanceID]->transportState();
    Omm::Av::Log::instance()->upnpav().debug("SetAVTransporURI enters in state: " + transportState);
    
    if (transportState == AvTransportArgument::TRANSPORT_STATE_NO_MEDIA_PRESENT) {
        _setTransportState(AvTransportArgument::TRANSPORT_STATE_STOPPED);
    }
    if (_getCurrentTrackURI() == CurrentURI) {
        return;
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
    _setCurrentTrackDuration(AvTransportArgument::CURRENT_TRACK_DURATION_0);

    std::string protInfoString;
    try {
        MemoryMediaObject obj;
        MediaObjectReader objReader(&obj);
        objReader.read(CurrentURIMetaData);

        if (obj.getResource()) {
            protInfoString = obj.getResource()->getAttributeValue(AvProperty::PROTOCOL_INFO);
            std::string duration = obj.getResource()->getAttributeValue(AvProperty::DURATION);
            if (duration != "") {
                Omm::Av::Log::instance()->upnpav().debug("set duration from CurrentURIMetaData to: " + duration);
                _setCurrentTrackDuration(duration);
            }
        }
    }
    catch (Poco::Exception& e) {
        Omm::Av::Log::instance()->upnpav().error("could not parse uri meta data: " + e.message());
    }
    ProtocolInfo protInfo(protInfoString);
    
    Omm::Av::Log::instance()->upnpav().debug("engine: " + _engines[InstanceID]->getEngineId() + " set uri: " + CurrentURI);
    if (_engines[InstanceID]->preferStdStream()) {
        Poco::URI uri(CurrentURI);
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
        _engines[InstanceID]->setUri(istr, protInfo);
    }
    else {
        _engines[InstanceID]->setUri(CurrentURI, protInfo);
    }

    Omm::Av::Log::instance()->upnpav().debug("SetAVTransporURI leaves in state: " + _getTransportState());
}


void
DevAVTransportRendererImpl::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
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
DevAVTransportRendererImpl::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
//    CurrentTransportState = _getTransportState();
    CurrentTransportState = _engines[InstanceID]->transportState();
    CurrentTransportStatus = _getTransportStatus();
    CurrentSpeed = _getTransportPlaySpeed();
}


void
DevAVTransportRendererImpl::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
//    std::string transportState = _getTransportState();
    std::string transportState = _engines[InstanceID]->transportState();
//    Omm::Av::Log::instance()->upnpav().debug("GetPositionInfo enters in state: " + transportState);
    
    if (transportState != AvTransportArgument::TRANSPORT_STATE_PLAYING && transportState != AvTransportArgument::TRANSPORT_STATE_TRANSITIONING) {
        // TODO: return an UPnP error
        return;
    }
//    Omm::Av::Log::instance()->upnpav().debug("GetPositionInfo() ...");
    Track = _getCurrentTrack();
    
    float engineTrackDuration = _engines[InstanceID]->getLengthSeconds();
//    Omm::Av::Log::instance()->upnpav().debug("engine track duration (sec): " + Poco::NumberFormatter::format(engineTrackDuration, 2));
    if (engineTrackDuration > 0.0) {
        _setCurrentTrackDuration(AvTypeConverter::writeDuration(engineTrackDuration));
//        Omm::Av::Log::instance()->upnpav().debug("set TrackDuration to: " + _getCurrentTrackDuration());
        TrackDuration = _getCurrentTrackDuration();
    }

    TrackMetaData = _getCurrentTrackMetaData();
    TrackURI = _getCurrentTrackURI();
    
    Poco::UInt64 enginePositionByte = _engines[InstanceID]->getPositionByte();
//    Omm::Av::Log::instance()->upnpav().debug("engine position byte: " + Poco::NumberFormatter::format(enginePositionByte));
    float enginePosition = _engines[InstanceID]->getPositionPercentage();
//    Omm::Av::Log::instance()->upnpav().debug("engine position percentage: " + Poco::NumberFormatter::format(enginePosition, 2));
    float engineTimePosition = _engines[InstanceID]->getPositionSecond();
//    Omm::Av::Log::instance()->upnpav().debug("engine position second: " + Poco::NumberFormatter::format(engineTimePosition, 2));
    
    std::string timePosition = AvTypeConverter::writeDuration(engineTimePosition);
//    Omm::Av::Log::instance()->upnpav().debug("set RelativePosition to: " + timePosition);
    _setRelativeTimePosition(timePosition);
    _setAbsoluteTimePosition(timePosition);
    RelTime = _getRelativeTimePosition();
    AbsTime = _getAbsoluteTimePosition();
    
    RelCount = _getRelativeCounterPosition();
    AbsCount = _getAbsoluteCounterPosition();
}


void
DevAVTransportRendererImpl::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
    PlayMedia = _getPossiblePlaybackStorageMedia();
    RecMedia = _getPossibleRecordStorageMedia();
    RecQualityModes = _getPossibleRecordQualityModes();
}


void
DevAVTransportRendererImpl::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
    PlayMode = _getCurrentPlayMode();
    RecQualityMode = _getCurrentRecordQualityMode();
}


void
DevAVTransportRendererImpl::Stop(const ui4& InstanceID)
{
    std::string transportState = _engines[InstanceID]->transportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Stop() enters in state: " + transportState);
    
    if (transportState != AvTransportArgument::TRANSPORT_STATE_NO_MEDIA_PRESENT) {
        // be nice to the engine and don't stop when already in stopped or paused state
        // TODO: when in PAUSED_PLAYBACK we should actually stop (according to AVTransport 1.0 specs
        //       -> unfortunately mplayer has it's problems with stopping ...
        if (transportState != AvTransportArgument::TRANSPORT_STATE_STOPPED
            && transportState != AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK) {
            _engines[InstanceID]->stop();
            if (_engines[InstanceID]->preferStdStream()) {
                delete _pSession;
                _pSession = 0;
            }
        }
        
        if (transportState == "PLAYING" || transportState == AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK) {
        // TODO: reset positions and speed (this is not mentioned in the AVTransport 1.0 specs ...)?
        //       what does Stop() mean when in TRANSITIONING state? 
        //       -> stop transitioning and start playback at current position?
            _setAbsoluteTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
            _setRelativeTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//             _setTransportPlaySpeed("0");  // only allowed value is "1" ...?
        }
    }
}


void
DevAVTransportRendererImpl::Play(const ui4& InstanceID, const std::string& Speed)
{
    std::string speed = Speed;
    std::string transportState = _engines[InstanceID]->transportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Play() enters in state: " + transportState);
    if (transportState == AvTransportArgument::TRANSPORT_STATE_STOPPED
        || transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING
        || transportState == AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK
        || transportState == AvTransportArgument::TRANSPORT_STATE_TRANSITIONING)
    {
        if (transportState == AvTransportArgument::TRANSPORT_STATE_STOPPED) {
            // TODO: does it mean: play from beginning now?
            //       if yes: after AvTransportArgument::TRANSPORT_STATE_STOPPED -> TRANSITIONING -> AvTransportArgument::TRANSPORT_STATE_STOPPED we would play from beginning
            //               and not from the position we were seeking to.
            //       if no:  Action Stop behaves like Pause
            //       -> maybe it should be: AvTransportArgument::TRANSPORT_STATE_STOPPED -> TRANSITIONING -> PAUSED_PLAYBACK
            //          contradicting AVTransport, 1.0, 2.4.12.3.
            _engines[InstanceID]->play();
        }
        else if (transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
            if (_getCurrentTrackURI() != _lastCurrentTrackUri) {
                // NOTE: not shure if we should stop here, or in the engine (if necessary)
                _engines[InstanceID]->stop();
                std::string pos = AvTransportArgument::CURRENT_TRACK_DURATION_0;
                _setAbsoluteTimePosition(pos);
                _setRelativeTimePosition(pos);
                _engines[InstanceID]->play();
            }
        }
        else if (transportState == AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK) {
            _engines[InstanceID]->pause();
        }
        else if (transportState == AvTransportArgument::TRANSPORT_STATE_TRANSITIONING) {
            // TODO: can we set another speed after transitioning, than normal speed?
            _engines[InstanceID]->setSpeed(1, 1);
            speed = "1";
        }
        // TODO: handle changes in speed
        // TODO: handle fractions of speed ('1/2', ...)
        // TODO: If start playing takes considerable amount of time (do this in Engine)
        //       set state to "TRANSITIONING" (-> 2.4.9.3. Effect on State, AVTransport spec)
        
        _setTransportPlaySpeed(speed);
        
        _lastCurrentTrackUri = _getCurrentTrackURI();
    }
}


void
DevAVTransportRendererImpl::Pause(const ui4& InstanceID)
{
    std::string transportState = _engines[InstanceID]->transportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Pause() enters in state: " + transportState);
    
    if (transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING
        || transportState == AvTransportArgument::TRANSPORT_STATE_RECORDING) {
        _engines[InstanceID]->pause();

        // TODO: differ between "pause recording" and "pause playing" in Engine
//        if (oldTransportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
//            newTransportState = AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK;
//        }
//        else if (oldTransportState == AvTransportArgument::TRANSPORT_STATE_RECORDING) {
//            newTransportState = AvTransportArgument::TRANSPORT_STATE_PAUSED_RECORDING;
//        }
    }
}


void
DevAVTransportRendererImpl::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    std::string transportState = _engines[InstanceID]->transportState();
    Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Seek() enters in state: " + transportState);
    
    if (transportState == AvTransportArgument::TRANSPORT_STATE_STOPPED
        || transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
        // TODO: does it make sense to handle "PAUSED_PLAYBACK", too?
        Omm::Av::Log::instance()->upnpav().debug("AVTransportRendererImpl::Seek() seek mode: " + Unit + ", seek target: " + Target);
        
        ui4 position;
        if (Unit == AvTransportArgument::SEEK_MODE_ABS_TIME) {
            position = AvTypeConverter::readTime(Target).epochMicroseconds() / 1000000;
        }
        _engines[InstanceID]->seekSecond(position);
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
DevAVTransportRendererImpl::Next(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportRendererImpl::Previous(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportRendererImpl::addEngine(Engine* pEngine)
{
    _engines.push_back(pEngine);
    _pLastChange->addInstance();
}

// ConnectionManagerRendererImpl::ConnectionManagerRendererImpl()
// {
//     _pRenderer = static_cast<MediaRendererImplementation*>(_pMediaRenderer);
// }


void
DevConnectionManagerRendererImpl::initStateVars()
{
//     _setSourceProtocolInfo("http-get:*:*:*");
    _setSourceProtocolInfo("");
    _setSinkProtocolInfo("http-get:*:*:*");
    _setCurrentConnectionIDs("0");
}


void
DevConnectionManagerRendererImpl::GetProtocolInfo(std::string& Source, std::string& Sink)
{
// begin of your own code
    Source = "";
    // TODO: this should ask the rendering engine about the formats it can render
    Sink = "http-get:*:*:*";
// end of your own code
}


void
DevConnectionManagerRendererImpl::ConnectionComplete(const i4& ConnectionID)
{
// begin of your own code
    
// end of your own code
}


void
DevConnectionManagerRendererImpl::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
// begin of your own code
    ConnectionIDs = "0";
// end of your own code
}


void
DevConnectionManagerRendererImpl::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
// begin of your own code

// end of your own code
}


void
DevRenderingControlRendererImpl::initStateVars()
{
    // FIXME: init all engine instances instead of the state vars in the service tree.
    
//    _setPresetNameList("");
//    _setBrightness(0);
//    _setContrast(0);
//    _setSharpness(0);
//    _setRedVideoGain(0);
//    _setGreenVideoGain(0);
//    _setBlueVideoGain(0);
//    _setRedVideoBlackLevel(0);
//    _setGreenVideoBlackLevel(0);
//    _setBlueVideoBlackLevel(0);
//    _setColorTemperature(0);
//    _setHorizontalKeystone(0);
//    _setVerticalKeystone(0);
//    _setMute(false);
//    _setVolume(_engines[InstanceID]->getVolume(0));
//    _setVolumeDB(0);
//    _setLoudness(false);
}


void
DevRenderingControlRendererImpl::ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetContrast(const ui4& InstanceID, ui2& CurrentContrast)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume)
{
    // we don't cache values in the state vars but retreive them directly from the enginge.
    // reason: we would need instances of the Service tree for each instanceID and channel.
//    CurrentVolume = _getVolume();
    Log::instance()->upnpav().debug("get volume of engine instance: " + Poco::NumberFormatter::format(InstanceID));
    
    CurrentVolume = _engines[InstanceID]->getVolume(Channel);
}


void
DevRenderingControlRendererImpl::SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Omm::Av::Log::instance()->upnpav().debug("RenderingControlRendererImpl::SetVolume() instance: " + Poco::NumberFormatter::format(InstanceID) + ", channel: " + Channel + ", volume: " + Poco::NumberFormatter::format(DesiredVolume));
    
    _engines[InstanceID]->setVolume(Channel, DesiredVolume);
    
    // we don't cache values in the state vars but retreive them directly from the engine.
    // reason: we would need instances of the Service tree for each instanceID and channel.
//    _setVolume(DesiredVolume);

    Variant val;
    val.setValue(DesiredVolume);
    _pLastChange->setChannelStateVar(InstanceID, Channel, RenderingControlEventedStateVar::VOLUME, val);
}


void
DevRenderingControlRendererImpl::GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume)
{
    CurrentVolume = _getVolumeDB();
}


void
DevRenderingControlRendererImpl::SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
// begin of your own code
    
// end of your own code
}


void
DevRenderingControlRendererImpl::addEngine(Engine* pEngine)
{
    _engines.push_back(pEngine);
    _pLastChange->addInstance();
}

} // namespace Av
} // namespace Omm
