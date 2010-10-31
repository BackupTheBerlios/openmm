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

#ifndef ENGINEVLC_H
#define ENGINEVLC_H

#include <Omm/UpnpAvRenderer.h>

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <vlc/vlc.h>


class VlcEngine : public Omm::Av::Engine
{
public:
//     VlcEngine(int argc, char **argv);
    VlcEngine();
    ~VlcEngine();
    
    virtual void createPlayer();
    virtual void setFullscreen(bool on = true);
    
    /*
      AVTransport
    */
    virtual void setUri(std::string uri);
    virtual void load();
    
    /**
        void setSpeed(int nom, int denom)
        set speed to nom/denom
    */
    virtual void setSpeed(int nom, int denom);
    /**
        void pause()
        toggle pause
    */
    virtual void pause();
    virtual void stop();
    virtual void seek(int seconds);
    virtual void next();
    virtual void previous();
    virtual void getPosition(float &seconds);
    virtual void getLength(float &seconds);
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);
    
private:
    void handleException();
    int openXWindow();
    void closeXWindow();
    
    libvlc_exception_t      _exception;
    libvlc_instance_t*      _vlcInstance;
    libvlc_media_player_t*  _vlcPlayer;
    std::string             _uri;
    long long               _startTime;
    float                   _length; // length of media in seconds
};


#endif
