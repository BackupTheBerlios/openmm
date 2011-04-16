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
_fullscreen(false),
// _width(720),
_width(1020),
_height(576),
_pVisual(0)
{
}


std::string
Engine::getEngineId()
{
    return _engineId;
}


void
Engine::setOption(const std::string& key, const std::string& value)
{
    if (key == "fullscreen") {
        _fullscreen = true;
        setFullscreen(true);
    }
    else if (key == "width") {
        _width = Poco::NumberParser::parse(value);
    }
    else if (key == "height") {
        _height = Poco::NumberParser::parse(value);
    }
}

    
AvRenderer::AvRenderer(Engine* engine) :
MediaRenderer(new RenderingControlRendererImpl,
              new ConnectionManagerRendererImpl,
              new AVTransportRendererImpl),
_pEngine(engine)
{
    static_cast<RenderingControlRendererImpl*>(_pRenderingControlImpl)->_pEngine = engine;
    static_cast<ConnectionManagerRendererImpl*>(_pConnectionManagerImpl)->_pEngine = engine;
    static_cast<AVTransportRendererImpl*>(_pAVTransportImpl)->_pEngine = engine;
    Omm::Av::Log::instance()->upnpav().information("renderer engine: " + engine->getEngineId());
}


void
AvRenderer::setFullscreen(bool on)
{
    // FIXME: need to set Engine::_fullscreen.
    _pEngine->setFullscreen(on);
}

} // namespace Av
} // namespace Omm
