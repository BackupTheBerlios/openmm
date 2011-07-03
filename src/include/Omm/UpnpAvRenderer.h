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

#ifndef UpnpAvRenderer_INCLUDED
#define UpnpAvRenderer_INCLUDED

#include "Upnp.h"
#include "UpnpAvObject.h"
#include "Util.h"
#include "Sys.h"

namespace Omm {
namespace Av {


class DevAVTransportRendererImpl;
class DevRenderingControlRendererImpl;
class Visual;
class ProtocolInfo;

class Engine : public Util::ConfigurablePlugin
{
    friend class AvRenderer;
    
public:
    Engine();
    
    std::string getEngineId();
    void setInstancedId(Omm::ui4 instanceId);
    void setVisual(Sys::Visual* pVisual);

    virtual void setOption(const std::string& key, const std::string& value);
    virtual void createPlayer()  = 0;

    // UPnP AV methods
    /*
      AVTransport
    */
    virtual bool preferStdStream() { return false; }
    virtual void setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo()) = 0;
    /// Set uri and protocol info of media item.
    virtual void setUri(std::istream& istr, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo()) {}
    /// Set stream and protocol info of media item.
    virtual void play() = 0;
    
    virtual void setSpeed(int nom, int denom) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void seekByte(Poco::UInt64 byte) = 0;
    virtual void seekPercentage(float percentage) = 0;
    virtual void seekSecond(float second) = 0;

    virtual Poco::UInt64 getPositionByte() = 0;
    virtual float getPositionPercentage() = 0;
    virtual float getPositionSecond() = 0;
    
    virtual float getLengthSeconds() = 0;
    
    /*
      Rendering Control
    */
    virtual void setVolume(const std::string& channel, float vol) = 0;
    virtual float getVolume(const std::string& channel) = 0;
    
protected:
    // TODO: implement endOfStream(), which is called by the engine implementation on end of track.
    void endOfStream();

    std::string                         _engineId;
    Omm::ui4                            _instanceId;
    Sys::Visual*                        _pVisual;
    DevAVTransportRendererImpl*         _pAVTransportImpl;
    DevRenderingControlRendererImpl*    _pRenderingControlImpl;
};


class AvRenderer : public Device
{
public:
    AvRenderer();
    ~AvRenderer();

    void addEngine(Engine* pEngine);
    
private:
    // these pointers to service implementations are only temporarily needed when setting them in the engine.
    DevAVTransportRendererImpl*         _pAVTransportImpl;
    DevRenderingControlRendererImpl*    _pRenderingControlImpl;
};

} // namespace Av
} // namespace Omm

#endif
