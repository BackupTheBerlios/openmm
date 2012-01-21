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


Engine::Engine() :
_instanceId(0),
_pAVTransportImpl(0),
_pRenderingControlImpl(0),
_pVisual(0),
_trackNumberInPlaylist(0)
{
}


std::string
Engine::getEngineId()
{
    return _engineId;
}


void
Engine::setInstancedId(Omm::ui4 instanceId)
{
    _instanceId = instanceId;
}


void
Engine::setVisual(Sys::Visual* pVisual)
{
    _pVisual = pVisual;
}


void
Engine::setOption(const std::string& key, const std::string& value)
{
}


void
Engine::setUriEngine(const std::string& uri, const ProtocolInfo& protInfo)
{
    Poco::URI uriParsed(uri);
    if (protInfo.getMimeString() == Mime::PLAYLIST) {
        Poco::Net::HTTPClientSession session(uriParsed.getHost(), uriParsed.getPort());

        Poco::Net::HTTPRequest request("GET", uriParsed.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        Omm::Av::Log::instance()->upnpav().debug("request header:\n" + requestHeader.str());

        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);

        Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream responseHeader;
        response.write(responseHeader);
        Omm::Av::Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());

        std::string line;
        while(std::getline(istr, line)) {
            _playlist.push_back(line);
        }
        setUri(_playlist[_trackNumberInPlaylist]);
    }
    else if (preferStdStream()) {
        _playlist.clear();
        Poco::Net::HTTPClientSession session(uriParsed.getHost(), uriParsed.getPort());
        Poco::Net::HTTPRequest request("GET", uriParsed.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        Omm::Av::Log::instance()->upnpav().debug("request header:\n" + requestHeader.str());

        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);

        Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
        std::stringstream responseHeader;
        response.write(responseHeader);
        Omm::Av::Log::instance()->upnpav().debug("response header:\n" + responseHeader.str());
        setUri(istr, protInfo);
    }
    else {
        _playlist.clear();
        setUri(uri, protInfo);
    }
}


void
Engine::seekTrack(ui4 trackNumber)
{
    Omm::Av::Log::instance()->upnpav().debug("engine seek to track number: " + Poco::NumberFormatter::format(trackNumber));
    _trackNumberInPlaylist = trackNumber;
    setUri(_playlist[_trackNumberInPlaylist]);
}


const std::string&
Engine::transportState()
{
    switch (getTransportState()) {
        case Stopped:
            return AvTransportArgument::TRANSPORT_STATE_STOPPED;
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
    Variant val;
    val.setValue(newTransportState);
//    if (_playlist.size() && _trackNumberInPlaylist < _playlist.size() && (getTransportState() == Stopped)) {
//        Omm::Av::Log::instance()->upnpav().debug("engine skips to next track in playlist");
//        _trackNumberInPlaylist++;
//        setUri(_playlist[_trackNumberInPlaylist]);
//        play();
//    }
//    else {
        Omm::Av::Log::instance()->upnpav().debug("engine sets new transport state: " + newTransportState);
        _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
//    }
}


//void
//Engine::endOfStream()
//{
//    // position state vars are not evented via LastChange state var (and not evented at all).
//    _pAVTransportImpl->_setAbsoluteTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    _pAVTransportImpl->_setRelativeTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
//    // transport state is evented via LastChange state var.
//    _pAVTransportImpl->_setTransportState(AvTransportArgument::TRANSPORT_STATE_STOPPED);
//    Variant val;
//    val.setValue(AvTransportArgument::TRANSPORT_STATE_STOPPED);
//    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
//}


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
              new DevConnectionManagerRendererImpl,
              _pAVTransportImpl));
}


MediaRenderer::~MediaRenderer()
{
}


void
MediaRenderer::addEngine(Engine* pEngine)
{
    Omm::ui4 instanceId = _pAVTransportImpl->_engines.size();
    pEngine->setInstancedId(instanceId);
    _pAVTransportImpl->addEngine(pEngine);
    pEngine->_pAVTransportImpl = _pAVTransportImpl;
    _pRenderingControlImpl->addEngine(pEngine);
    pEngine->_pRenderingControlImpl = _pRenderingControlImpl;

    Log::instance()->upnpav().information("add renderer engine: " + pEngine->getEngineId());
}

} // namespace Av
} // namespace Omm
