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
#include "Omm/UpnpAv.h"

#include <vlc/vlc.h>
#ifdef LIBVLC_VERSION_HEADER_FOUND
#include <vlc/libvlc_version.h>
#endif

// EnginePlugin::VlcEngine(int argc, char **argv) :
VlcEngine::VlcEngine() :
_pVlcMedia(0),
_maxMediaConnect(5)
{
    _engineId = "VLC engine " + Omm::OMM_VERSION + ", vlc version " + libvlc_get_version();
}


VlcEngine::~VlcEngine()
{
    libvlc_release(_pVlcInstance);

    if (_pVisual) {
        _pVisual->hide();
        delete _pVisual;
    }
}


void
VlcEngine::createPlayer()
{
#if LIBVLC_VERSION_INT < 0x110
    _pException = new libvlc_exception_t;
    libvlc_exception_init(_pException);
#endif
//    if (_fullscreen) {
//        int argc = 3;
//        const char* argv[3] = {"ommrender", "--no-osd",  "--fullscreen"};
//#if LIBVLC_VERSION_INT < 0x110
//        _pVlcInstance = libvlc_new(argc, argv, _pException);
//#else
//        _pVlcInstance = libvlc_new(argc, argv);
//#endif
//    }
//    else {
        int argc = 2;
        const char* argv[2] = {"ommrender", "--no-osd"};
#if LIBVLC_VERSION_INT < 0x110
        _pVlcInstance = libvlc_new(argc, argv, _pException);
#else
        _pVlcInstance = libvlc_new(argc, argv);
#endif
//    }
//     int argc = 3;
//     char* argv[3] = {"ommrender", "--codec=avcodec",  "--vout fb"};
//     _pVlcInstance = libvlc_new(argc, argv, _pException);
    handleException();
#if LIBVLC_VERSION_INT < 0x110
    _pVlcPlayer = libvlc_media_player_new(_pVlcInstance, _pException);
#else
    _pVlcPlayer = libvlc_media_player_new(_pVlcInstance);
#endif
    handleException();

#if LIBVLC_VERSION_INT < 0x110
    _pEventManager = libvlc_media_player_event_manager(_pVlcPlayer, _pException);
#else
    _pEventManager = libvlc_media_player_event_manager(_pVlcPlayer);
#endif
    handleException();

    int ret;
#if LIBVLC_VERSION_INT < 0x110
    libvlc_event_attach(_pEventManager, libvlc_MediaPlayerStopped, &eventHandler, this, _pException);
    libvlc_event_attach(_pEventManager, libvlc_MediaPlayerPlaying, &eventHandler, this, _pException);
    libvlc_event_attach(_pEventManager, libvlc_MediaPlayerEndReached, &eventHandler, this, _pException);
#else
    ret = libvlc_event_attach(_pEventManager, libvlc_MediaPlayerStopped, &eventHandler, this);
    ret = libvlc_event_attach(_pEventManager, libvlc_MediaPlayerPlaying, &eventHandler, this);
    ret = libvlc_event_attach(_pEventManager, libvlc_MediaPlayerEndReached, &eventHandler, this);
#endif
    handleException();

    if (_pVisual) {
        Omm::Av::Log::instance()->upnpav().debug("vlc engine: set visual ...");
#ifdef __LINUX__
            Omm::Av::Log::instance()->upnpav().debug("vlc engine: set X11 visual ...");
#if LIBVLC_VERSION_INT < 0x100
            libvlc_media_player_set_drawable(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#elif LIBVLC_VERSION_INT < 0x110
            libvlc_media_player_set_xwindow(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#else
            libvlc_media_player_set_xwindow(_pVlcPlayer, _pVisual->getWindowId());
#endif
#endif
#ifdef __DARWIN__
            Omm::Av::Log::instance()->upnpav().debug("vlc engine: set OSX visual ...");
#if LIBVLC_VERSION_INT < 0x110
//            libvlc_media_player_set_nsobject(_pVlcPlayer, _pVisual->getWindow(), _pException);
            libvlc_media_player_set_agl(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#else
//            libvlc_media_player_set_nsobject(_pVlcPlayer, _pVisual->getWindow());
            libvlc_media_player_set_agl(_pVlcPlayer, _pVisual->getWindowId());
#endif
#endif
#ifdef __WINDOWS__
            Omm::Av::Log::instance()->upnpav().debug("vlc engine: set Windows visual ...");
#if LIBVLC_VERSION_INT < 0x110
            libvlc_media_player_set_hwnd(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#else
            libvlc_media_player_set_hwnd(_pVlcPlayer, _pVisual->getWindowId());
#endif
#endif
        handleException();
    }

/*    clearException();
    libvlc_event_attach(vlc_my_object_event_manager(), NULL, _pException);
    handleException();*/
}


void
VlcEngine::setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo)
{
//     _uri = uri.substr(0, 4) + "/ffmpeg" + uri.substr(4);
    _uri = uri;
    _mime = Omm::Av::Mime(protInfo.getMimeString());
}


// void
// VlcEngine::setUri(std::istream& istr, const Omm::Av::ProtocolInfo& protInfo)
// {
// //     _fd = istr.socket().sockfd();
// }


void
VlcEngine::play()
{
    if (_mime.isImage()) {
        Omm::Icon image(0, 0, 0, "", _uri);
        _pVisual->renderImage(image.getBuffer());
    }
    else {
    Omm::Av::Log::instance()->upnpav().debug("vlc engine: play ...");
//    int tryMediaConnect = _maxMediaConnect;
//    while (tryMediaConnect-- && !_pVlcMedia) {
//#if LIBVLC_VERSION_INT < 0x110
//        _pVlcMedia = libvlc_media_new(_pVlcInstance, _uri.c_str(), _pException);
//#else
//        _pVlcMedia = libvlc_media_new_location(_pVlcInstance, _uri.c_str());
//#endif
//        handleException();
//        if (!_pVlcMedia) {
//            Poco::Thread::sleep(100);
//        }
//    }

    Omm::Av::Log::instance()->upnpav().debug("vlc engine: create new media ...");
#if LIBVLC_VERSION_INT < 0x110
    _pVlcMedia = libvlc_media_new(_pVlcInstance, _uri.c_str(), _pException);
#else
    _pVlcMedia = libvlc_media_new_location(_pVlcInstance, _uri.c_str());
#endif
    handleException();

    Omm::Av::Log::instance()->upnpav().debug("vlc engine: player set media ...");
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_set_media(_pVlcPlayer, _pVlcMedia, _pException);
#else
    libvlc_media_player_set_media(_pVlcPlayer, _pVlcMedia);
#endif
    handleException();

    Omm::Av::Log::instance()->upnpav().debug("vlc engine: release media ...");
    libvlc_media_release(_pVlcMedia);

    Omm::Av::Log::instance()->upnpav().debug("vlc engine: play media ...");
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_play(_pVlcPlayer, _pException);
#else
    libvlc_media_player_play(_pVlcPlayer);
#endif
    handleException();
    }

    if (!_mime.isAudio()) {
        _pVisual->show();
    }

    Omm::Av::Log::instance()->upnpav().debug("vlc engine: play media finished.");
}


void
VlcEngine::analyzeStream()
{
    _startTime = 0;
    _length = 0.0;

    // settle to a defined state
    libvlc_state_t state;
    do {
        Poco::Thread::sleep(100); // limit the cpu-load while loading the media and sleep for 100ms
#if LIBVLC_VERSION_INT < 0x110
        state = libvlc_media_player_get_state(_pVlcPlayer, _pException);
#else
        state = libvlc_media_player_get_state(_pVlcPlayer);
#endif
        handleException();
    } while(state != libvlc_Playing && state != libvlc_Error /*&& state != libvlc_MediaPlayerEndReached*/ );

    int hasVideo = 0;
    int trackCount = 0;
    do {
        Poco::Thread::sleep(100); // limit the cpu-load while loading the media and sleep for 100ms
#if LIBVLC_VERSION_INT < 0x110
        hasVideo = libvlc_media_player_has_vout(_pVlcPlayer, _pException);
        handleException();
        trackCount = libvlc_audio_get_track_count(_pVlcPlayer, _pException);
        handleException();
#else
        hasVideo = libvlc_media_player_has_vout(_pVlcPlayer);
        handleException();
        trackCount = libvlc_audio_get_track_count(_pVlcPlayer);
        handleException();
#endif
    } while(state == libvlc_Playing && !hasVideo && !trackCount);
//     TRACE("VlcEngine::load() hasVideo: %i, trackCount: %i", hasVideo, trackCount);
#if LIBVLC_VERSION_INT < 0x110
    _length = (libvlc_media_player_get_length(_pVlcPlayer, _pException) - _startTime) / 1000.0;
    libvlc_time_t d = libvlc_media_get_duration(_pVlcMedia, _pException);
    //     TRACE("VlcEngine::load() _length: %f, duration: %lli", _length, d);

    _startTime = libvlc_media_player_get_time(_pVlcPlayer, _pException);
    handleException();
//     TRACE("VlcEngine::load() _startTime [ms]: %lli", _startTime);

    if(!libvlc_media_player_is_seekable(_pVlcPlayer, _pException)) {
//         TRACE("VlcEngine::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(_pVlcPlayer, _pException)) {
//         TRACE("VlcEngine::load() pause not possible on media");
    }
    handleException();

    // TODO: receive video size changed - events and adjust display size
//    int videoWidth = libvlc_video_get_width(_pVlcPlayer, _pException);
//    handleException();
//    int videoHeight = libvlc_video_get_height(_pVlcPlayer, _pException);
//    handleException();
//     TRACE("VlcEngine::load() videoWidth: %i, videoHeight: %i", videoWidth, videoHeight);
/*    libvlc_video_resize(_pVlcPlayer, videoWidth, videoHeight, _pException);
    handleException();*/

    // TODO: fullscreen could initially be set at start
//     libvlc_set_fullscreen(_pVlcPlayer, (_fullscreen ? 1 : 0), _pException);
//     handleException();
#else
    _length = (libvlc_media_player_get_length(_pVlcPlayer) - _startTime) / 1000.0;
    libvlc_time_t d = libvlc_media_get_duration(_pVlcMedia);
    //     TRACE("VlcEngine::load() _length: %f, duration: %lli", _length, d);

    _startTime = libvlc_media_player_get_time(_pVlcPlayer);
    handleException();
//     TRACE("VlcEngine::load() _startTime [ms]: %lli", _startTime);

    if(!libvlc_media_player_is_seekable(_pVlcPlayer)) {
//         TRACE("VlcEngine::load() media is not seekable");
    }
    handleException();
    if(!libvlc_media_player_can_pause(_pVlcPlayer)) {
//         TRACE("VlcEngine::load() pause not possible on media");
    }
    handleException();

    unsigned int videoWidth;
    unsigned int videoHeight;
    int success = libvlc_video_get_size(_pVlcPlayer, 0, &videoWidth, &videoHeight);
    handleException();
    // TODO: receive video size changed - events and adjust display size
/*    libvlc_video_resize(_pVlcPlayer, videoWidth, videoHeight);
    handleException();*/

    // TODO: fullscreen could initially be set at start
//     libvlc_set_fullscreen(_pVlcPlayer, (_fullscreen ? 1 : 0));
//     handleException();
#endif
}


void
VlcEngine::eventHandler(const struct libvlc_event_t* pEvent, void* pUserData)
{
    static_cast<VlcEngine*>(pUserData)->transportStateChanged();
}


void
VlcEngine::setSpeed(int nom, int denom)
{
}


void
VlcEngine::pause()
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_pause(_pVlcPlayer, _pException);
#else
    libvlc_media_player_pause(_pVlcPlayer);
#endif
    handleException();
}


void
VlcEngine::stop()
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_stop(_pVlcPlayer, _pException);
#else
    libvlc_media_player_stop(_pVlcPlayer);
#endif
    handleException();

    if (!_mime.isAudio()) {
        _pVisual->blank();
        _pVisual->hide();
    }
}


void
VlcEngine::seekByte(Poco::UInt64 byte)
{

}


void
VlcEngine::seekPercentage(float percentage)
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_set_position(_pVlcPlayer, percentage, _pException);
#else
    libvlc_media_player_set_position(_pVlcPlayer, percentage);
#endif
    handleException();
}


void
VlcEngine::seekSecond(float second)
{
#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_set_time(_pVlcPlayer, second * 1000, _pException);
#else
    libvlc_media_player_set_time(_pVlcPlayer, second * 1000);
#endif
    handleException();
}


Poco::UInt64
VlcEngine::getPositionByte()
{
#if LIBVLC_VERSION_INT < 0x110
    // TODO: implement getPositionByte() for vlc version 1.0
#else
    libvlc_media_stats_t stats;
    int success = libvlc_media_get_stats(_pVlcMedia, &stats);
    return stats.i_read_bytes;
#endif
    handleException();
}


float
VlcEngine::getPositionPercentage()
{
    // TODO: implement size based seeking (not time based).
    // try mediacontrol_get_media_position() and mediacontrol_set_media_position() for seek control
    // mediacontrol API seems to be deprecated, it is not present in vlc 1.1
    //
    // try libvlc_vlm API (present in all versions of vlc)
    //
    // use size in bytes from meta data or http get request answer
    float res;

    libvlc_state_t state;
#if LIBVLC_VERSION_INT < 0x110
    state = libvlc_media_player_get_state(_pVlcPlayer, _pException);
#else
    state = libvlc_media_player_get_state(_pVlcPlayer);
#endif
    handleException();
    // TODO: catch vlc event at end of track, don't poll it
    if (state == libvlc_Ended) {
//        endOfStream();
        res = 0.0;
        return res;
    }

#if LIBVLC_VERSION_INT < 0x110
    res = _length * libvlc_media_player_get_position(_pVlcPlayer, _pException);
#else
    res = _length * libvlc_media_player_get_position(_pVlcPlayer);
#endif
    handleException();
    return res;
//     Omm::Av::Log::instance()->upnpav().debug("engine position: " + Poco::NumberFormatter::format(seconds, 2));
}


float
VlcEngine::getPositionSecond()
{
    float res;

    libvlc_state_t state;
#if LIBVLC_VERSION_INT < 0x110
    state = libvlc_media_player_get_state(_pVlcPlayer, _pException);
#else
    state = libvlc_media_player_get_state(_pVlcPlayer);
#endif
    handleException();
    // TODO: catch vlc event at end of track, don't poll it
    if (state == libvlc_Ended) {
//        endOfStream();
        res = 0.0;
        return res;
    }

#if LIBVLC_VERSION_INT < 0x110
    res = (libvlc_media_player_get_time(_pVlcPlayer, _pException) - _startTime) / 1000.0;
#else
    res = (libvlc_media_player_get_time(_pVlcPlayer) - _startTime) / 1000.0;
#endif
    handleException();
    return res;
//     Omm::Av::Log::instance()->upnpav().debug("engine position: " + Poco::NumberFormatter::format(seconds, 2));
}


float
VlcEngine::getLengthSeconds()
{
    // libvlc_media_player_get_length() sometimes fetches the last position of the stream, and not the length
#if LIBVLC_VERSION_INT < 0x110
    _length = (libvlc_media_player_get_length(_pVlcPlayer, _pException) - _startTime) / 1000.0;
#else
    _length = (libvlc_media_player_get_length(_pVlcPlayer) - _startTime) / 1000.0;
#endif
    handleException();
    return _length;
//     TRACE("VlcEngine::getLength() seconds: %f", seconds);

    /* use instead?:
    libvlc_time_t
    libvlc_media_get_duration( libvlc_media_t * p_md,
                               libvlc_exception_t * p_e )
    */

/*    seconds = libvlc_media_player_get_length(_pVlcPlayer, _pException) / 1000.0;
    handleException();
    seconds = 100.0;
    TRACE("VlcEngine::getLength() seconds: %f", seconds);*/
}


VlcEngine::TransportState
VlcEngine::getTransportState()
{
    libvlc_state_t res;
#if LIBVLC_VERSION_INT < 0x110
    res = libvlc_media_player_get_state(_pVlcPlayer, _pException);
#else
    res = libvlc_media_player_get_state(_pVlcPlayer);
#endif
    handleException();

    Omm::Av::Log::instance()->upnpav().debug("vlc engine transport state: " + Poco::NumberFormatter::format(res));

    switch (res) {
        case libvlc_Ended:
        case libvlc_Stopped:
            return Stopped;
        case libvlc_Playing:
            return Playing;
        case libvlc_Opening:
        case libvlc_Buffering:
            return Transitioning;
        case libvlc_Paused:
            return PausedPlayback;
//        case libvlc_NothingSpecial:
        default:
            return Stopped;
    }
}


void
VlcEngine::setVolume(const std::string& channel, float vol)
{
#if LIBVLC_VERSION_INT < 0x110
    if (channel == Omm::Av::AvChannel::MASTER) {
        libvlc_audio_set_volume(_pVlcInstance, vol, _pException);
    }
#else
    if (channel == Omm::Av::AvChannel::MASTER) {
        libvlc_audio_set_volume(_pVlcPlayer, vol);
    }
#endif
    handleException();
}


float
VlcEngine::getVolume(const std::string& channel)
{
    float res;
#if LIBVLC_VERSION_INT < 0x110
    res = libvlc_audio_get_volume(_pVlcInstance, _pException);
#else
    res = libvlc_audio_get_volume(_pVlcPlayer);
#endif
    handleException();
    return res;
}


void
VlcEngine::handleException()
{
    // TODO: really handle exception (vlc may crash, if exception is ignored).
    // throw exception to signal renderer engine code that engine failed.
#if LIBVLC_VERSION_INT < 0x110
    if (libvlc_exception_raised(_pException)) {
        Omm::Av::Log::instance()->upnpav().error("vlc engine: " + std::string(libvlc_exception_get_message(_pException)));
    }
    libvlc_exception_init(_pException);
#else
    const char* errMsg = libvlc_errmsg();
    if (errMsg) {
        Omm::Av::Log::instance()->upnpav().error("vlc engine: " + std::string(errMsg));
    }
#endif
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(VlcEngine)
POCO_END_MANIFEST
#endif
