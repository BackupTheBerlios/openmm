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
#include <Poco/ClassLibrary.h>
#include "EngineVlc.h"

#include <Omm/Upnp.h>
#include <Omm/UpnpAvTypes.h>

// EnginePlugin::VlcEngine(int argc, char **argv) :
VlcEngine::VlcEngine()
{
    _engineId = "VLC engine " + Omm::OMM_VERSION + ", vlc version " + libvlc_get_version();
}


VlcEngine::~VlcEngine()
{
    libvlc_release(_vlcInstance);
    closeXWindow();
}


void
VlcEngine::createPlayer()
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_exception_init(&_exception);
#endif
    if (_fullscreen) {
        int argc = 3;
        const char* argv[3] = {"ommrender", "--no-osd",  "--fullscreen"};
#if LIBVLC_VERSION_INT < 0x110
        _vlcInstance = libvlc_new(argc, argv, &_exception);
#else
        _vlcInstance = libvlc_new(argc, argv);
#endif
    }
    else {
        int argc = 2;
        const char* argv[2] = {"ommrender", "--no-osd"};
#if LIBVLC_VERSION_INT < 0x110
        _vlcInstance = libvlc_new(argc, argv, &_exception);
#else
        _vlcInstance = libvlc_new(argc, argv);
#endif
    }
//     int argc = 3;
//     char* argv[3] = {"ommrender", "--codec=avcodec",  "--vout fb"};
//     _vlcInstance = libvlc_new(argc, argv, &_exception);
    handleException();
#if LIBVLC_VERSION_INT < 0x110
    _vlcPlayer = libvlc_media_player_new(_vlcInstance, &_exception);
#else
    _vlcPlayer = libvlc_media_player_new(_vlcInstance);
#endif
    handleException();
        
    int xWindow = openXWindow();
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_set_xwindow(_vlcPlayer, xWindow, &_exception);
#else
    libvlc_media_player_set_xwindow(_vlcPlayer, xWindow);
#endif
    handleException();
    
/*    clearException();
    libvlc_event_attach(vlc_my_object_event_manager(), NULL, &_exception);
    handleException();*/
}


void
VlcEngine::setUri(std::string uri)
{
//     _uri = uri.substr(0, 4) + "/ffmpeg" + uri.substr(4);
    _uri = uri;
}


void
VlcEngine::setFullscreen(bool on)
{
}


int
VlcEngine::openXWindow()
{
    Display*    xDisplay;
    int         xScreen;
    Window      xWindow;
    int xPos    = 0;
    int yPos    = 0;
    
    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    if(_fullscreen) {
        _width   = DisplayWidth(xDisplay, xScreen);
        _height  = DisplayHeight(xDisplay, xScreen);
    }
    xWindow = XCreateSimpleWindow(xDisplay, XDefaultRootWindow(xDisplay),
                                  xPos, yPos, _width, _height, 1, 0, 0);
    XMapRaised(xDisplay, xWindow);
//     res_h = (DisplayWidth(xDisplay, xScreen) * 1000 / DisplayWidthMM(xDisplay, xScreen));
//     res_v = (DisplayHeight(xDisplay, xScreen) * 1000 / DisplayHeightMM(xDisplay, xScreen));
    XSync(xDisplay, False);
    XUnlockDisplay(xDisplay);
    
    // hide X cursor
    Cursor no_ptr;
    Pixmap bm_no;
    XColor black, dummy;
    Colormap colormap;
    static char no_data[] = { 0,0,0,0,0,0,0,0 };
    
    colormap = DefaultColormap(xDisplay, xScreen);
    XAllocNamedColor(xDisplay, colormap, "black", &black, &dummy);
    bm_no = XCreateBitmapFromData(xDisplay, xWindow, no_data, 8, 8);
    no_ptr = XCreatePixmapCursor(xDisplay, bm_no, bm_no, &black, &black, 0, 0);
    
    XDefineCursor(xDisplay, xWindow, no_ptr);
    XFreeCursor(xDisplay, no_ptr);
    
    return xWindow;
}


void
VlcEngine::closeXWindow()
{
/*    if (xDisplay)
        XCloseDisplay(xDisplay);
    xDisplay = NULL;*/
}


void
VlcEngine::load()
{
    _startTime = 0;
    _length = 0.0;
    libvlc_media_t* media = 0;
#if LIBVLC_VERSION_INT < 0x110
    media = libvlc_media_new(_vlcInstance, _uri.c_str(), &_exception);
#else
    media = libvlc_media_new_location(_vlcInstance, _uri.c_str());
#endif
    handleException();
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_set_media(_vlcPlayer, media, &_exception);
#else
    libvlc_media_player_set_media(_vlcPlayer, media);
#endif
    handleException();
    libvlc_media_release(media);
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_play(_vlcPlayer, &_exception);
#else
    libvlc_media_player_play(_vlcPlayer);
#endif
    handleException();

    // settle to a defined state
    libvlc_state_t state;
    do {
        usleep(100000); // limit the cpu-load while loading the media
#if LIBVLC_VERSION_INT < 0x110
        state = libvlc_media_player_get_state(_vlcPlayer, &_exception);
#else
        state = libvlc_media_player_get_state(_vlcPlayer);
#endif
        handleException();
    } while(state != libvlc_Playing && state != libvlc_Error /*&& state != libvlc_MediaPlayerEndReached*/ );

    int hasVideo = 0;
    int trackCount = 0;
    do {
        usleep(100000); // limit the cpu-load while waiting for stream demux
#if LIBVLC_VERSION_INT < 0x110
        hasVideo = libvlc_media_player_has_vout(_vlcPlayer, &_exception);
        handleException();
        trackCount = libvlc_audio_get_track_count(_vlcPlayer, &_exception);
        handleException();
#else
        hasVideo = libvlc_media_player_has_vout(_vlcPlayer);
        handleException();
        trackCount = libvlc_audio_get_track_count(_vlcPlayer);
        handleException();
#endif   
    } while(state == libvlc_Playing && !hasVideo && !trackCount);
//     TRACE("VlcEngine::load() hasVideo: %i, trackCount: %i", hasVideo, trackCount);
#if LIBVLC_VERSION_INT < 0x110
    _length = (libvlc_media_player_get_length(_vlcPlayer, &_exception) - _startTime) / 1000.0;
    libvlc_time_t d = libvlc_media_get_duration(media, &_exception);
    //     TRACE("VlcEngine::load() _length: %f, duration: %lli", _length, d);
    
    _startTime = libvlc_media_player_get_time(_vlcPlayer, &_exception);
    handleException();
//     TRACE("VlcEngine::load() _startTime [ms]: %lli", _startTime);

    if(!libvlc_media_player_is_seekable(_vlcPlayer, &_exception)) {
//         TRACE("VlcEngine::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(_vlcPlayer, &_exception)) {
//         TRACE("VlcEngine::load() pause not possible on media");
    }
    handleException();
    
    // TODO: receive video size changed - events and adjust display size
    int videoWidth = libvlc_video_get_width(_vlcPlayer, &_exception);
    handleException();
    int videoHeight = libvlc_video_get_height(_vlcPlayer, &_exception);
    handleException();
//     TRACE("VlcEngine::load() videoWidth: %i, videoHeight: %i", videoWidth, videoHeight);
/*    libvlc_video_resize(_vlcPlayer, videoWidth, videoHeight, &_exception);
    handleException();*/
    
    // TODO: fullscreen could initially be set at start
//     libvlc_set_fullscreen(_vlcPlayer, (_fullscreen ? 1 : 0), &_exception);
//     handleException();
#else
    _length = (libvlc_media_player_get_length(_vlcPlayer) - _startTime) / 1000.0;
    libvlc_time_t d = libvlc_media_get_duration(media);
    //     TRACE("VlcEngine::load() _length: %f, duration: %lli", _length, d);
    
    _startTime = libvlc_media_player_get_time(_vlcPlayer);
    handleException();
//     TRACE("VlcEngine::load() _startTime [ms]: %lli", _startTime);

    if(!libvlc_media_player_is_seekable(_vlcPlayer)) {
//         TRACE("VlcEngine::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(_vlcPlayer)) {
//         TRACE("VlcEngine::load() pause not possible on media");
    }
    handleException();
    
    // TODO: receive video size changed - events and adjust display size
    int videoWidth = libvlc_video_get_width(_vlcPlayer);
    handleException();
    int videoHeight = libvlc_video_get_height(_vlcPlayer);
    handleException();
//     TRACE("VlcEngine::load() videoWidth: %i, videoHeight: %i", videoWidth, videoHeight);
/*    libvlc_video_resize(_vlcPlayer, videoWidth, videoHeight);
    handleException();*/
    
    // TODO: fullscreen could initially be set at start
//     libvlc_set_fullscreen(_vlcPlayer, (_fullscreen ? 1 : 0));
//     handleException();
#endif
}


void
VlcEngine::setSpeed(int nom, int denom)
{
}


void
VlcEngine::pause()
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_pause(_vlcPlayer, &_exception);
#else
    libvlc_media_player_pause(_vlcPlayer);
#endif
    handleException();
}


void
VlcEngine::stop()
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_stop(_vlcPlayer, &_exception);
#else
    libvlc_media_player_stop(_vlcPlayer);
#endif
    handleException();
}


void
VlcEngine::seek(int seconds)
{
//     libvlc_media_player_set_time(_vlcPlayer, 78232 * 1000, &_exception);
    if (_length > 0.0) {
#if LIBVLC_VERSION_INT < 0x110
        libvlc_media_player_set_position(_vlcPlayer, seconds / _length, &_exception);
#else
        libvlc_media_player_set_position(_vlcPlayer, seconds / _length);
#endif
    }
    else {
#if LIBVLC_VERSION_INT < 0x110
        libvlc_media_player_set_time(_vlcPlayer, seconds * 1000, &_exception);
#else
        libvlc_media_player_set_time(_vlcPlayer, seconds * 1000);
#endif
    }
//     libvlc_media_player_set_time(_vlcPlayer, seconds * 1000, &_exception);
    handleException();
//     libvlc_media_player_set_position(_vlcPlayer, seconds * 1000.0, &_exception);
//     handleException();
}


void
VlcEngine::next()
{
}


void
VlcEngine::previous()
{
}


void
VlcEngine::getPosition(float &seconds)
{
    libvlc_state_t state;
    // TODO: emit a real signal at end of track, don't poll it
#if LIBVLC_VERSION_INT < 0x110
    state = libvlc_media_player_get_state(_vlcPlayer, &_exception);
#else
    state = libvlc_media_player_get_state(_vlcPlayer);
#endif
    handleException();
    if (state == libvlc_Ended) {
//         endOfTrack.emitSignal();
        seconds = 0.0;
        return;
    }
    
    if (_length > 0.0) {
#if LIBVLC_VERSION_INT < 0x110
        seconds = _length * libvlc_media_player_get_position(_vlcPlayer, &_exception);
#else
        seconds = _length * libvlc_media_player_get_position(_vlcPlayer);
#endif
    }
    else {
#if LIBVLC_VERSION_INT < 0x110
        seconds = (libvlc_media_player_get_time(_vlcPlayer, &_exception) - _startTime) / 1000.0;
#else
        seconds = (libvlc_media_player_get_time(_vlcPlayer) - _startTime) / 1000.0;
#endif
    }
    handleException();
//     Omm::Av::Log::instance()->upnpav().debug("engine position: " + Poco::NumberFormatter::format(seconds, 2));
}


void
VlcEngine::getLength(float &seconds)
{
    // libvlc_media_player_get_length() sometimes fetches the last position of the stream, and not the length
#if LIBVLC_VERSION_INT < 0x110
    _length = (libvlc_media_player_get_length(_vlcPlayer, &_exception) - _startTime) / 1000.0;
#else
    _length = (libvlc_media_player_get_length(_vlcPlayer) - _startTime) / 1000.0;
#endif
    handleException();
    seconds = _length;
//     TRACE("VlcEngine::getLength() seconds: %f", seconds);
    
    /* use instead?:
    libvlc_time_t
    libvlc_media_get_duration( libvlc_media_t * p_md,
                               libvlc_exception_t * p_e )
    */
    
/*    seconds = libvlc_media_player_get_length(_vlcPlayer, &_exception) / 1000.0;
    handleException();
    seconds = 100.0;
    TRACE("VlcEngine::getLength() seconds: %f", seconds);*/
}


void
VlcEngine::setVolume(int channel, float vol)
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_audio_set_volume(_vlcInstance, vol, &_exception);
#else
    libvlc_audio_set_volume(_vlcPlayer, vol);
#endif
    handleException();
}


void
VlcEngine::getVolume(int channel, float &vol)
{
#if LIBVLC_VERSION_INT < 0x110
    vol = libvlc_audio_get_volume(_vlcInstance, &_exception);
#else
    vol = libvlc_audio_get_volume(_vlcPlayer);
#endif
    handleException();
}


void
VlcEngine::handleException()
{
#if LIBVLC_VERSION_INT < 0x110
    if (libvlc_exception_raised(&_exception)) {
        std::cerr << "Error in VlcEngine: " << libvlc_exception_get_message(&_exception) << std::endl;
    }
    libvlc_exception_init(&_exception);
#else
    const char* errMsg = libvlc_errmsg();
    if (errMsg) {
        std::cerr << "Error in VlcEngine: " <<  errMsg << std::endl;
    }
#endif
}

POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(VlcEngine)
POCO_END_MANIFEST