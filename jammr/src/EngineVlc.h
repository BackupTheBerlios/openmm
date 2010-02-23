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

#ifndef ENGINEVLC_H
#define ENGINEVLC_H

#include <Jamm/UpnpAvRenderer.h>

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <vlc/vlc.h>


class EngineVlc : public Jamm::Av::Engine
{
public:
    EngineVlc(int argc, char **argv);
    ~EngineVlc();
    
    std::string getEngineId() { return m_engineId; }
    
    /*
      AVTransport
    */
    virtual void setUri(std::string uri) { m_uri = uri; }
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
    
    libvlc_exception_t      m_exception;
    libvlc_instance_t*      m_vlcInstance;
    libvlc_media_player_t*  m_vlcPlayer;
    std::string             m_uri;
    long long               m_startTime;
    float                   m_length; // length of media in seconds
    
    std::string             m_engineId;
};


#endif
