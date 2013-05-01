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

#include "UpnpAv.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvDescriptions.h"
#include "UpnpAvRendererImpl.h"

namespace Omm {
namespace Av {


const std::string Engine::StreamTypeOther("StreamTypeOther");
const std::string Engine::StreamTypeAudio("StreamTypeAudio");
const std::string Engine::StreamTypeVideo("StreamTypeVideo");
const std::string Engine::StreamTypePicture("StreamTypePicture");
const std::string Engine::StreamTypeText("StreamTypeText");

Engine::Engine() :
_instanceId(0),
_pAVTransportImpl(0),
_pRenderingControlImpl(0),
_pVisual(0),
_trackNumberInPlaylist(0),
_duration(0.0)
{
}


std::string
Engine::getEngineId()
{
    return _engineId;
}


void
Engine::setInstancedId(ui4 instanceId)
{
    _instanceId = instanceId;
}


void
Engine::setVisual(Sys::Visual* pVisual)
{
    _pVisual = pVisual;
}


//void
//Engine::setOption(const std::string& key, const std::string& value)
//{
//}


void
Engine::setUriEngine(const std::string& uri, const ProtocolInfo& protInfo)
{
    Poco::URI uriParsed(uri);
    _playlist.clear();
    _duration = 0.0;
    if (protInfo.getMimeString() == Mime::PLAYLIST) {
        LOG(upnpav, debug, "renderer engine got playlist");

        Poco::Net::HTTPClientSession session(uriParsed.getHost(), uriParsed.getPort());

        Poco::Net::HTTPRequest request("GET", uriParsed.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        LOG(upnpav, debug, "request header:\n" + requestHeader.str());

        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);

        LOG(upnpav, information, "HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream responseHeader;
        response.write(responseHeader);
        LOG(upnpav, debug, "response header:\n" + responseHeader.str());

        std::string line;
        while(std::getline(istr, line)) {
            _playlist.push_back(line);
        }
        setAtomicUriEngine(_playlist[_trackNumberInPlaylist]);
    }
    else if (preferStdStream()) {
        Poco::Net::HTTPClientSession session(uriParsed.getHost(), uriParsed.getPort());
        Poco::Net::HTTPRequest request("GET", uriParsed.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        LOG(upnpav, debug, "request header:\n" + requestHeader.str());

        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);

        LOG(upnpav, information, "HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream responseHeader;
        response.write(responseHeader);
        LOG(upnpav, debug, "response header:\n" + responseHeader.str());
        setUri(istr, protInfo);
    }
    else {
        setAtomicUriEngine(uri, protInfo);
    }
}


void
Engine::setAtomicUriEngine(const std::string& uri, const ProtocolInfo& protInfo)
{
    LOG(upnpav, debug, "engine sets new atomic uri: " + uri);

    setUri(uri, protInfo);

    Variant val;
    val.setValue(uri);
    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::CURRENT_TRACK_URI, val);
}


void
Engine::setDuration(r8 seconds)
{
    _duration = seconds;
}


r8
Engine::getDuration()
{
    r8 res = getLengthSeconds();
    if (res == 0.0) {
        res = _duration;
    }
    return res;
}


void
Engine::setSize(Poco::UInt64 bytes)
{
    _size = bytes;
}


Poco::UInt64
Engine::getSize()
{
    return _size;
}


void
Engine::seekTrack(ui4 trackNumber)
{
    LOG(upnpav, debug, "engine seek to track number: " + Poco::NumberFormatter::format(trackNumber));
    _trackNumberInPlaylist = trackNumber;
    if (_trackNumberInPlaylist >= 0 && _trackNumberInPlaylist < _playlist.size()) {
        setAtomicUriEngine(_playlist[_trackNumberInPlaylist]);
    }
    else {
        LOG(upnpav, error, "track number " + Poco::NumberFormatter::format(_trackNumberInPlaylist) + " not in range for playlist size " + Poco::NumberFormatter::format(_playlist.size()));
    }
}


void
Engine::nextTrack()
{
    if (_trackNumberInPlaylist < _playlist.size() - 1) {
        _trackNumberInPlaylist++;
        _duration = 0.0;
        LOG(upnpav, debug, "engine skip to next track number: " + Poco::NumberFormatter::format(_trackNumberInPlaylist));
        if (_trackNumberInPlaylist < _playlist.size()) {
            setAtomicUriEngine(_playlist[_trackNumberInPlaylist]);
        }
        else {
            LOG(upnpav, error, "track number " + Poco::NumberFormatter::format(_trackNumberInPlaylist) + " too large for playlist size " + Poco::NumberFormatter::format(_playlist.size()));
        }
    }
}


void
Engine::previousTrack()
{
    if (_trackNumberInPlaylist >= 1) {
        _trackNumberInPlaylist--;
        _duration = 0.0;
        LOG(upnpav, debug, "engine skip to previous track number: " + Poco::NumberFormatter::format(_trackNumberInPlaylist));
        if (_trackNumberInPlaylist) {
            setAtomicUriEngine(_playlist[_trackNumberInPlaylist]);
        }
        else {
            LOG(upnpav, error, "track number " + Poco::NumberFormatter::format(_trackNumberInPlaylist) + " must be positive.");
        }
    }
}


void
Engine::seekTime(r8 second)
{
    if (getLengthSeconds() == 0.0 && _duration > 0.0 && _size > 0) {
        seekByte(second / _duration * _size);
    }
    else {
        seekSecond(second);
    }
}


const std::string
Engine::transportState()
{
    switch (getTransportState()) {
        case Stopped:
            return AvTransportArgument::TRANSPORT_STATE_STOPPED;
        case Ended:
            // end of track must be distinguished within the engine from stopping a track.
            // there is no UPnP transport state ended, so we have to add it.
            // TODO: just pass playlists to the engine and don't handle them separately.
            return "Ended";
        case Playing:
            return AvTransportArgument::TRANSPORT_STATE_PLAYING;
        case Transitioning:
            return AvTransportArgument::TRANSPORT_STATE_TRANSITIONING;
        case PausedPlayback:
            return AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK;
        case PausedRecording:
            return AvTransportArgument::TRANSPORT_STATE_PAUSED_RECORDING;
        case Recording:
            return AvTransportArgument::TRANSPORT_STATE_RECORDING;
        case NoMediaPresent:
            return AvTransportArgument::TRANSPORT_STATE_NO_MEDIA_PRESENT;
    }
}


void
Engine::transportStateChanged()
{
    std::string newTransportState = transportState();

    if (newTransportState == "Ended") {
        newTransportState = AvTransportArgument::TRANSPORT_STATE_STOPPED;
        _pEndOfStreamTimer = new Poco::Timer(10);
        _pEndOfStreamTimer->start(Poco::TimerCallback<Engine> (*this, &Engine::endOfStream));
    }

    Variant val;
    val.setValue(newTransportState);
    LOG(upnpav, debug, "engine sets new transport state: " + newTransportState);
    // notify via upnp events over network of new transport state (usefull to update controllers view of remote renderers)
    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
    // also notify locally within process of new transport state and stream type (usefull for gui to e.g. bring video window to front)
    std::string streamType = StreamTypeOther;
    if (newTransportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
        streamType = getStreamType();
    }
    Poco::NotificationCenter::defaultCenter().postNotification(new StreamTypeNotification(_instanceId, newTransportState, streamType));
}


void
Engine::endOfStream(Poco::Timer& timer)
{
    if (_playlist.size() && _trackNumberInPlaylist < _playlist.size()) {
////    if (_playlist.size() && _trackNumberInPlaylist < _playlist.size() && (getTransportState() == Stopped)) {

        // TODO: disabled renderer playing next playlist item when current finishes, because
        //    stopping DVB services on a transponder leads to tuning to next service
        //    in playlist (when started from a playlist), which in turn may stop
        //    another service ... initiating a fast tuning sequence.
        LOG(upnpav, debug, "engine skips to next track in playlist");
        _trackNumberInPlaylist++;
        stop();
        setAtomicUriEngine(_playlist[_trackNumberInPlaylist]);
        play();
    }

//    // position state vars are not evented via LastChange state var (and not evented at all).
//    _pAVTransportImpl->_setAbsoluteTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    _pAVTransportImpl->_setRelativeTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    // transport state is evented via LastChange state var.
//    _pAVTransportImpl->_setTransportState(AvTransportArgument::TRANSPORT_STATE_STOPPED);
//    Variant val;
//    val.setValue(AvTransportArgument::TRANSPORT_STATE_STOPPED);
//    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
}


MediaRenderer::MediaRenderer()
{
    MediaRendererDescriptions mediaRendererDescriptions;
    MemoryDescriptionReader descriptionReader(mediaRendererDescriptions);
    descriptionReader.getDeviceDescription();
    setDeviceData(descriptionReader.rootDeviceData());

    _pAVTransportImpl = new DevAVTransportRendererImpl;
    _pRenderingControlImpl = new DevRenderingControlRendererImpl;
    setDevDeviceCode(new DevMediaRenderer(
              _pRenderingControlImpl,
              new DevConnectionManagerRendererImpl(this),
              _pAVTransportImpl));
}


MediaRenderer::~MediaRenderer()
{
}


void
MediaRenderer::addEngine(Engine* pEngine)
{
    ui4 instanceId = _pAVTransportImpl->_engines.size();
//    pEngine->_pRenderer = this;
    pEngine->setInstancedId(instanceId);
    _pAVTransportImpl->addEngine(pEngine);
    pEngine->_pAVTransportImpl = _pAVTransportImpl;
    _pRenderingControlImpl->addEngine(pEngine);
    pEngine->_pRenderingControlImpl = _pRenderingControlImpl;

    LOG(upnpav, information, "add renderer engine: " + pEngine->getEngineId());
}


Engine*
MediaRenderer::getEngine(ui4 instanceId)
{
    return _pAVTransportImpl->_engines[instanceId];
}


} // namespace Av
} // namespace Omm
