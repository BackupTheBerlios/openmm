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
Engine::setVisual(Sys::Visual* pVisual)
{
    _pVisual = pVisual;
}


void
Engine::setOption(const std::string& key, const std::string& value)
{
}


void
Engine::endOfStream()
{
    _pAVTransportImpl->_setAbsoluteTimePosition(Omm::Av::AvTransportArgument::CURRENT_TRACK_DURATION_0);
    _pAVTransportImpl->_setRelativeTimePosition(Omm::Av::AvTransportArgument::CURRENT_TRACK_DURATION_0);
    _pAVTransportImpl->_setTransportState(Omm::Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
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
    _pAVTransportImpl->_engines.push_back(pEngine);
    pEngine->_pAVTransportImpl = _pAVTransportImpl;
    _pRenderingControlImpl->_engines.push_back(pEngine);
    pEngine->_pRenderingControlImpl = _pRenderingControlImpl;
    
    Log::instance()->upnpav().information("add renderer engine: " + pEngine->getEngineId());
}

} // namespace Av
} // namespace Omm
