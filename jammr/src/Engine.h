/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef ENGINE_H
#define ENGINE_H

// #include <jamm/signode.h>
// using namespace Jamm;

#include <string>
using namespace std;

/**
@author Jörg Bakker
*/
class Engine
{
public:
    // Engine handling
    // virtual void setVideoDriver(string) = 0;
    // virtual void setAudioDriver(string) = 0;
    // virtual void setFullscreen(bool on) = 0;
    // virtual vector<string> getAudioAdapters() = 0;
    // virtual void setAudioAdapter(string) = 0;

    virtual string getEngineId() = 0;
    
    // UPnP methods
    /*
      AVTransport
    */
    
    virtual void setUri(string uri) = 0;
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
};

#endif
