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

#include "EngineVlc.h"


EngineVlc::EngineVlc(int argc, char **argv) :
_fullscreen(true)
{
    libvlc_exception_init(&_exception);
    _vlcInstance = libvlc_new(argc, argv, &_exception);
    handleException();
    _vlcPlayer = libvlc_media_player_new(_vlcInstance, &_exception);
    handleException();
        
    int xWindow = openXWindow();
    libvlc_media_player_set_drawable(_vlcPlayer, xWindow, &_exception);
    handleException();
    
/*    clearException();
    libvlc_event_attach(vlc_my_object_event_manager(), NULL, &_exception);
    handleException();*/
    
    _engineId = "OmmR VLC engine 0.0.3";
}


EngineVlc::~EngineVlc()
{
    libvlc_release(_vlcInstance);
    closeXWindow();
}


void
EngineVlc::setFullscreen(bool on)
{
    _fullscreen = on;
}


int
EngineVlc::openXWindow()
{
    Display*    xDisplay;
    int         xScreen;
    Window      xWindow;
    int xPos    = 0;
    int yPos    = 0;
/*    int width   = 720;
    int height  = 576;*/
/*    int width   = 800;
    int height  = 600;*/
    /*    int width   = 1280;
    int height  = 800;*/
    int width   = 1360;
    int height  = 768;
    
    
    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    if(_fullscreen) {
        width   = DisplayWidth(xDisplay, xScreen);
        height  = DisplayHeight(xDisplay, xScreen);
    }
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
    _startTime = 0;
    _length = 0.0;

    libvlc_media_t* media = libvlc_media_new(_vlcInstance, _uri.c_str(), &_exception);
    handleException();
    libvlc_media_player_set_media(_vlcPlayer, media, &_exception);
    handleException();
    libvlc_media_release(media);    
    libvlc_media_player_play(_vlcPlayer, &_exception);
    handleException();

    // settle to a defined state
    libvlc_state_t state;
    do {
        usleep(100000); // limit the cpu-load while loading the media
        state = libvlc_media_player_get_state(_vlcPlayer, &_exception);
        handleException();
    } while(state != libvlc_Playing && state != libvlc_Error && state != libvlc_MediaPlayerEndReached );

    int hasVideo = 0;
    int trackCount = 0;
    do {
        usleep(100000); // limit the cpu-load while waiting for stream demux
        hasVideo = libvlc_media_player_has_vout(_vlcPlayer, &_exception);
        handleException();
        trackCount = libvlc_audio_get_track_count(_vlcPlayer, &_exception);
        handleException();
    } while(state == libvlc_Playing && !hasVideo && !trackCount);
//     TRACE("EngineVlc::load() hasVideo: %i, trackCount: %i", hasVideo, trackCount);

    _length = (libvlc_media_player_get_length(_vlcPlayer, &_exception) - _startTime) / 1000.0;
    libvlc_time_t d = libvlc_media_get_duration(media, &_exception);
//     TRACE("EngineVlc::load() _length: %f, duration: %lli", _length, d);
    
    _startTime = libvlc_media_player_get_time(_vlcPlayer, &_exception);
    handleException();
//     TRACE("EngineVlc::load() _startTime [ms]: %lli", _startTime);

    if(!libvlc_media_player_is_seekable(_vlcPlayer, &_exception)) {
//         TRACE("EngineVlc::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(_vlcPlayer, &_exception)) {
//         TRACE("EngineVlc::load() pause not possible on media");
    }
    handleException();
    
    // TODO: receive video size changed - events and adjust display size
    int videoWidth = libvlc_video_get_width(_vlcPlayer, &_exception);
    handleException();
    int videoHeight = libvlc_video_get_height(_vlcPlayer, &_exception);
    handleException();
//     TRACE("EngineVlc::load() videoWidth: %i, videoHeight: %i", videoWidth, videoHeight);
/*    libvlc_video_resize(_vlcPlayer, videoWidth, videoHeight, &_exception);
    handleException();*/
    
    // TODO: fullscreen could initially be set at start
//     libvlc_set_fullscreen(_vlcPlayer, (_fullscreen ? 1 : 0), &_exception);
//     handleException();
}


void
EngineVlc::setSpeed(int nom, int denom)
{
}


void
EngineVlc::pause()
{
    libvlc_media_player_pause(_vlcPlayer, &_exception);
    handleException();
}


void
EngineVlc::stop()
{
    libvlc_media_player_stop(_vlcPlayer, &_exception);
    handleException();
}


void
EngineVlc::seek(int seconds)
{
//     libvlc_media_player_set_time(_vlcPlayer, 78232 * 1000, &_exception);
    if (_length > 0.0) {
        libvlc_media_player_set_position(_vlcPlayer, seconds / _length, &_exception);
    }
    else {
        libvlc_media_player_set_time(_vlcPlayer, seconds * 1000, &_exception);
    }
//     libvlc_media_player_set_time(_vlcPlayer, seconds * 1000, &_exception);
    handleException();
//     libvlc_media_player_set_position(_vlcPlayer, seconds * 1000.0, &_exception);
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
    libvlc_state_t state = libvlc_media_player_get_state(_vlcPlayer, &_exception);
    handleException();
    if (state == libvlc_Ended) {
//         endOfTrack.emitSignal();
        seconds = 0.0;
        return;
    }
    
    if (_length > 0.0) {
        seconds = _length * libvlc_media_player_get_position(_vlcPlayer, &_exception);
    }
    else {
        seconds = (libvlc_media_player_get_time(_vlcPlayer, &_exception) - _startTime) / 1000.0;
    }
    handleException();
//     TRACE("EngineVlc::getPosition() seconds: %f", seconds);
}


void
EngineVlc::getLength(float &seconds)
{
    // libvlc_media_player_get_length() sometimes fetches the last position of the stream, and not the length
    _length = (libvlc_media_player_get_length(_vlcPlayer, &_exception) - _startTime) / 1000.0;
    handleException();
    seconds = _length;
//     TRACE("EngineVlc::getLength() seconds: %f", seconds);
    
    /* use instead?:
    libvlc_time_t
    libvlc_media_get_duration( libvlc_media_t * p_md,
                               libvlc_exception_t * p_e )
    */
    
/*    seconds = libvlc_media_player_get_length(_vlcPlayer, &_exception) / 1000.0;
    handleException();
    seconds = 100.0;
    TRACE("EngineVlc::getLength() seconds: %f", seconds);*/
}


void
EngineVlc::setVolume(int channel, float vol)
{
    libvlc_audio_set_volume(_vlcInstance, vol, &_exception);
    handleException();
}


void
EngineVlc::getVolume(int channel, float &vol)
{
    vol = libvlc_audio_get_volume(_vlcInstance, &_exception);
    handleException();
}


void
EngineVlc::handleException()
{
    if (libvlc_exception_raised(&_exception)) {
        std::cerr << "Error in EngineVlc: " << libvlc_exception_get_message(&_exception) << std::endl;
    }
    libvlc_exception_init(&_exception);
}