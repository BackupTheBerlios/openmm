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

#include "UpnpAvRenderer.h"
#include "UpnpAvRendererImpl.h"

namespace Omm {
namespace Av {


Engine::Engine() :
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


AvRenderer::AvRenderer(Engine* engine) :
//DevMediaRenderer(new DevRenderingControlRendererImpl,
//              new DevConnectionManagerRendererImpl,
//              new DevAVTransportRendererImpl),
_pEngine(engine)
{
    setDevDevice(new DevMediaRenderer(
              new DevRenderingControlRendererImpl,
              new DevConnectionManagerRendererImpl,
              new DevAVTransportRendererImpl));

    static_cast<DevRenderingControlRendererImpl*>(static_cast<DevMediaRenderer*>(getDevDevice())->_pDevRenderingControlImpl)->_pEngine = engine;
    static_cast<DevConnectionManagerRendererImpl*>(static_cast<DevMediaRenderer*>(getDevDevice())->_pDevConnectionManagerImpl)->_pEngine = engine;
    static_cast<DevAVTransportRendererImpl*>(static_cast<DevMediaRenderer*>(getDevDevice())->_pDevAVTransportImpl)->_pEngine = engine;

//    static_cast<DevRenderingControlRendererImpl*>(_pDevRenderingControlImpl)->_pEngine = engine;
//    static_cast<DevConnectionManagerRendererImpl*>(_pDevConnectionManagerImpl)->_pEngine = engine;
//    static_cast<DevAVTransportRendererImpl*>(_pDevAVTransportImpl)->_pEngine = engine;
    Omm::Av::Log::instance()->upnpav().information("renderer engine: " + engine->getEngineId());
}


} // namespace Av
} // namespace Omm
