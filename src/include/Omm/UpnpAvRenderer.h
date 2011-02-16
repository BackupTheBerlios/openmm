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

#include "UpnpAvDevices.h"
#include "Util.h"

namespace Omm {
namespace Av {


class Engine : public Util::ConfigurablePlugin
{
public:
    Engine();
    
    std::string getEngineId();
    
    virtual void setOption(const std::string& key, const std::string& value);
    virtual void createPlayer()  = 0;
    // Engine handling
    // virtual void setVideoDriver(std::string) = 0;
    // virtual void setAudioDriver(std::string) = 0;
    virtual void setFullscreen(bool on = true) = 0;
    // virtual vector<std::string> getAudioAdapters() = 0;
    // virtual void setAudioAdapter(std::string) = 0;

    // UPnP methods
    /*
      AVTransport
    */
    virtual bool preferStdStream() { return false; }
    virtual void setUri(const std::string& uri, const std::string& mime = "") = 0;
    virtual void setUri(std::istream& istr) {}
    virtual void load() = 0;
    
    /**
        void setSpeed(int nom, int denom)
        set speed to nom/denom
    */
    virtual void setSpeed(int nom, int denom) = 0;
    /**
        void pause()
        toggle pause
    */
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void next() = 0;
    virtual void previous() = 0;
    
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
    virtual void setVolume(int channel, float vol) = 0;
    virtual float getVolume(int channel) = 0;
    
protected:
    // TODO: implement endOfStream(), which is called by the engine implementation on end of track
    virtual void endOfStream() {}

    std::string                 _engineId;
    bool                        _fullscreen;
    int                         _width;
    int                         _height;
};


class AvRenderer : public MediaRenderer
{
public:
    AvRenderer(Engine* engine);
    
    void setFullscreen(bool on = true);
    
private:
    Engine*     _pEngine;
};

} // namespace Av
} // namespace Omm

#endif
