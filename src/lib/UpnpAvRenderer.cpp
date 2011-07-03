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
_pVisual(0)
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
    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
    Omm::Av::Log::instance()->upnpav().debug("new transport state: " + newTransportState);
}


void
Engine::endOfStream()
{
    // position state vars are not evented via LastChange state var (and not evented at all).
    _pAVTransportImpl->_setAbsoluteTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
    _pAVTransportImpl->_setRelativeTimePosition(AvTransportArgument::CURRENT_TRACK_DURATION_0);
    // transport state is evented via LastChange state var.
    _pAVTransportImpl->_setTransportState(AvTransportArgument::TRANSPORT_STATE_STOPPED);
    Variant val;
    val.setValue(AvTransportArgument::TRANSPORT_STATE_STOPPED);
    _pAVTransportImpl->_pLastChange->setStateVar(_instanceId, AvTransportEventedStateVar::TRANSPORT_STATE, val);
}


AvRenderer::AvRenderer() 
{
    MediaRendererDescriptions mediaRendererDescriptions;
    MemoryDescriptionReader descriptionReader(mediaRendererDescriptions);
    descriptionReader.getDeviceDescription();
    setDeviceData(descriptionReader.rootDeviceData(this));

    _pAVTransportImpl = new DevAVTransportRendererImpl;
    _pRenderingControlImpl = new DevRenderingControlRendererImpl;
    setDevDeviceCode(new DevMediaRenderer(
              _pRenderingControlImpl,
              new DevConnectionManagerRendererImpl,
              _pAVTransportImpl));
}


AvRenderer::~AvRenderer()
{
}


void
AvRenderer::addEngine(Engine* pEngine)
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
