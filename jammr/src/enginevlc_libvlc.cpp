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

#include "enginevlc.h"

#include <jamm/debug.h>


EngineVlc::EngineVlc(int argc, char **argv)
{
    clearException();
    m_vlcInstance = libvlc_new(argc, argv, &m_exception);
    handleException();
    clearException();
    m_vlcPlayer = libvlc_media_player_new(m_vlcInstance, &m_exception);
    handleException();
        
    int xWindow = openXWindow();
    clearException();
    libvlc_media_player_set_drawable(m_vlcPlayer, xWindow, &m_exception);
    handleException();
    
/*    clearException();
    libvlc_event_attach(vlc_my_object_event_manager(), NULL, &m_exception);
    handleException();*/
}


EngineVlc::~EngineVlc()
{
    libvlc_release(m_vlcInstance);
    closeXWindow();
}

int
EngineVlc::openXWindow()
{
    Display*    xDisplay;
    int         xScreen;
    Window      xWindow;
    int xPos    = 0;
    int yPos    = 0;
    int width   = 320;
    int height  = 200;
    
    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    xWindow = XCreateSimpleWindow(xDisplay, XDefaultRootWindow(xDisplay),
                                  xPos, yPos, width, height, 1, 0, 0);
    XMapRaised(xDisplay, xWindow);
//     res_h = (DisplayWidth(xDisplay, xScreen) * 1000 / DisplayWidthMM(xDisplay, xScreen));
//     res_v = (DisplayHeight(xDisplay, xScreen) * 1000 / DisplayHeightMM(xDisplay, xScreen));
    XSync(xDisplay, False);
    XUnlockDisplay(xDisplay);
    return xWindow;
}


void
EngineVlc::closeXWindow()
{
/*    if (xDisplay)
        XCloseDisplay(xDisplay);
    xDisplay = NULL;*/
}


void
EngineVlc::load()
{
    clearException();
    libvlc_media_t* media = libvlc_media_new(m_vlcInstance, m_uri.c_str(), &m_exception);
    handleException();
    clearException();
    libvlc_media_player_set_media(m_vlcPlayer, media, &m_exception);
    handleException();
    libvlc_media_release(media);
    clearException();
    if(!libvlc_media_player_is_seekable(m_vlcPlayer, &m_exception)) {
        TRACE("EngineVlc::load(): media is not seekable");
    }
    handleException();
    clearException();
    libvlc_media_player_play(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::setSpeed(int nom, int denom)
{
}


void
EngineVlc::pause()
{
    clearException();
    libvlc_media_player_pause(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::stop()
{
    clearException();
    libvlc_media_player_stop(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::seek(int seconds)
{
    clearException();
    libvlc_media_player_set_time(m_vlcPlayer, seconds * 1000, &m_exception);
    handleException();
}


void
EngineVlc::next()
{
}


void
EngineVlc::previous()
{
}


void
EngineVlc::getPosition(float &seconds)
{
/*    clearException();
    libvlc_media_player_t* media = libvlc_media_player_get_media(m_vlcPlayer, &m_exception);
    handleException();*/
    
    clearException();
//     libvlc_state_t state = libvlc_media_get_state(media, &m_exception);
    libvlc_state_t state = libvlc_media_player_get_state(m_vlcPlayer, &m_exception);
    handleException();
    if (state == libvlc_Ended) {
        endOfTrack.emitSignal();
        seconds = 0.0;
        return;
    }
    
    clearException();
    seconds = libvlc_media_player_get_time(m_vlcPlayer, &m_exception) / 1000.0;
    handleException();
}


void
EngineVlc::getLength(float &seconds)
{
    clearException();
    seconds = libvlc_media_player_get_length(m_vlcPlayer, &m_exception) / 1000.0;
    handleException();
}


void
EngineVlc::setVolume(int channel, float vol)
{
    clearException();
    libvlc_audio_set_volume(m_vlcInstance, vol, &m_exception);
    handleException();
}


void
EngineVlc::getVolume(int channel, float &vol)
{
    clearException();
    vol = libvlc_audio_get_volume(m_vlcInstance, &m_exception);
    handleException();
}


void
EngineVlc::clearException()
{
    libvlc_exception_init(&m_exception);
}


void
EngineVlc::handleException()
{
    if (libvlc_exception_raised(&m_exception)) {
        TRACE(libvlc_exception_get_message(&m_exception));
    }
}