/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "enginevlc.h"

#include <jamm/debug.h>


EngineVlc::EngineVlc(int argc, char **argv)
{
    libvlc_exception_init(&m_exception);
    m_vlcInstance = libvlc_new(argc, argv, &m_exception);
    handleException();
    m_vlcPlayer = libvlc_media_player_new(m_vlcInstance, &m_exception);
    handleException();
        
    int xWindow = openXWindow();
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
    libvlc_media_t* media = libvlc_media_new(m_vlcInstance, m_uri.c_str(), &m_exception);
    handleException();
    libvlc_media_player_set_media(m_vlcPlayer, media, &m_exception);
    handleException();
    libvlc_media_release(media);    
    libvlc_media_player_play(m_vlcPlayer, &m_exception);
    handleException();

    // settle to a defined state
    libvlc_state_t state;
    do {
        state = libvlc_media_player_get_state(m_vlcPlayer, &m_exception);
        handleException();
    } while(state != libvlc_Playing && state != libvlc_Error && state != libvlc_MediaPlayerEndReached );

    // wait for position information
    do {
        m_startTime = libvlc_media_player_get_time(m_vlcPlayer, &m_exception);
        handleException();
    } while(state == libvlc_Playing && m_startTime == 0);
    
    TRACE("EngineVlc::load() m_length: %f, m_startTime: %lli", m_length, m_startTime);

    if(!libvlc_media_player_is_seekable(m_vlcPlayer, &m_exception)) {
        TRACE("EngineVlc::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(m_vlcPlayer, &m_exception)) {
        TRACE("EngineVlc::load() pause not possible on media");
    }
    handleException();
}


void
EngineVlc::setSpeed(int nom, int denom)
{
}


void
EngineVlc::pause()
{
    libvlc_media_player_pause(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::stop()
{
    libvlc_media_player_stop(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::seek(int seconds)
{
//     libvlc_media_player_set_time(m_vlcPlayer, 78232 * 1000, &m_exception);
    if (m_length > 0.0) {
        libvlc_media_player_set_position(m_vlcPlayer, seconds / m_length, &m_exception);
    }
    else {
        libvlc_media_player_set_time(m_vlcPlayer, seconds * 1000, &m_exception);
    }
//     libvlc_media_player_set_time(m_vlcPlayer, seconds * 1000, &m_exception);
    handleException();
//     libvlc_media_player_set_position(m_vlcPlayer, seconds * 1000.0, &m_exception);
//     handleException();
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
    // TODO: emit a real signal at end of track, don't poll it
    libvlc_state_t state = libvlc_media_player_get_state(m_vlcPlayer, &m_exception);
    handleException();
    if (state == libvlc_Ended) {
        endOfTrack.emitSignal();
        seconds = 0.0;
        return;
    }
    
    if (m_length > 0.0) {
        seconds = m_length * libvlc_media_player_get_position(m_vlcPlayer, &m_exception);
    }
    else {
        seconds = (libvlc_media_player_get_time(m_vlcPlayer, &m_exception) - m_startTime) / 1000.0;
    }
    handleException();
    TRACE("EngineVlc::getPosition() seconds: %f", seconds);
}


void
EngineVlc::getLength(float &seconds)
{
    // libvlc_media_player_get_length() sometimes fetches the last position of the stream, and not the length
    m_length = (libvlc_media_player_get_length(m_vlcPlayer, &m_exception) - m_startTime) / 1000.0;
    handleException();
    seconds = m_length;
    TRACE("EngineVlc::getLength() seconds: %f", seconds);
/*    seconds = libvlc_media_player_get_length(m_vlcPlayer, &m_exception) / 1000.0;
    handleException();
    seconds = 100.0;
    TRACE("EngineVlc::getLength() seconds: %f", seconds);*/
}


void
EngineVlc::setVolume(int channel, float vol)
{
    libvlc_audio_set_volume(m_vlcInstance, vol, &m_exception);
    handleException();
}


void
EngineVlc::getVolume(int channel, float &vol)
{
    vol = libvlc_audio_get_volume(m_vlcInstance, &m_exception);
    handleException();
}


// void
// EngineVlc::clearException()
// {
//     libvlc_exception_init(&m_exception);
// }


void
EngineVlc::handleException()
{
    if (libvlc_exception_raised(&m_exception)) {
        TRACE(libvlc_exception_get_message(&m_exception));
    }
    libvlc_exception_init(&m_exception);
}