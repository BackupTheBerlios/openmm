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
    
    virtual void setOption(const std::string& key, const std::string& value);
    // Engine handling
    // virtual void setVideoDriver(std::string) = 0;
    // virtual void setAudioDriver(std::string) = 0;
    virtual void setFullscreen(bool on = true) = 0;
    // virtual vector<std::string> getAudioAdapters() = 0;
    // virtual void setAudioAdapter(std::string) = 0;

    virtual std::string getEngineId() = 0;
    
    // UPnP methods
    /*
      AVTransport
    */
    
    virtual void setUri(std::string uri) = 0;
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
    // what type of seek should be supported by the engine?
    virtual void seek(int seconds) = 0;
    // handle playlists in the engine (gapless playback possible with HTTP GET?) 
    // or in UpnpMediaRenderer (same code for all)?
    virtual void next() = 0;
    virtual void previous() = 0;
    
    virtual void getPosition(float &seconds) = 0;
    virtual void getLength(float &seconds) = 0;
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol) = 0;
    virtual void getVolume(int channel, float &vol) = 0;
    
    /*
      Events
    */
//     JSignal endOfTrack;
protected:
    bool                        _fullscreen;
};


class UpnpAvRenderer : public MediaRenderer
{
public:
    UpnpAvRenderer(Engine* engine);
    
    void setFullscreen(bool on = true);
    
private:
    Engine*     _pEngine;
};

} // namespace Av
} // namespace Omm

#endif
