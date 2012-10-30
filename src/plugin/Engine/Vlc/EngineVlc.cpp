/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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
        LOGNS(Omm::Av, upnpav, debug, "vlc engine: set visual ...");
#ifdef __LINUX__
            LOGNS(Omm::Av, upnpav, debug, "vlc engine: set X11 visual ...");
#if LIBVLC_VERSION_INT < 0x100
            libvlc_media_player_set_drawable(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#elif LIBVLC_VERSION_INT < 0x110
            libvlc_media_player_set_xwindow(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#else
            libvlc_media_player_set_xwindow(_pVlcPlayer, _pVisual->getWindowId());
#endif
#endif
#ifdef __DARWIN__
            LOGNS(Omm::Av, upnpav, debug, "vlc engine: set OSX visual ...");
#if LIBVLC_VERSION_INT < 0x110
//            libvlc_media_player_set_nsobject(_pVlcPlayer, _pVisual->getWindow(), _pException);
            libvlc_media_player_set_agl(_pVlcPlayer, _pVisual->getWindowId(), _pException);
#else
//            libvlc_media_player_set_nsobject(_pVlcPlayer, _pVisual->getWindow());
            libvlc_media_player_set_agl(_pVlcPlayer, _pVisual->getWindowId());
#endif
#endif
#ifdef __WINDOWS__
            LOGNS(Omm::Av, upnpav, debug, "vlc engine: set Windows visual ...");
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
    LOGNS(Omm::Av, upnpav, debug, "vlc engine set uri: " + uri);
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
        LOGNS(Omm::Av, upnpav, debug, "vlc engine: play ...");

        LOGNS(Omm::Av, upnpav, debug, "vlc engine: create new media with uri: " + _uri + " ...");
#if LIBVLC_VERSION_INT < 0x110
        _pVlcMedia = libvlc_media_new(_pVlcInstance, _uri.c_str(), _pException);
#else
        _pVlcMedia = libvlc_media_new_location(_pVlcInstance, _uri.c_str());
#endif
        handleException();

        LOGNS(Omm::Av, upnpav, debug, "vlc engine: player set media ...");
#if LIBVLC_VERSION_INT < 0x110
        libvlc_media_player_set_media(_pVlcPlayer, _pVlcMedia, _pException);
#else
        libvlc_media_player_set_media(_pVlcPlayer, _pVlcMedia);
#endif
        handleException();

//        libvlc_media_add_option_flag(_pVlcMedia, "sout=#description:dummy", 0);
//        handleException();

        LOGNS(Omm::Av, upnpav, debug, "vlc engine: release media ...");
        libvlc_media_release(_pVlcMedia);

        LOGNS(Omm::Av, upnpav, debug, "vlc engine: play media ...");
#if LIBVLC_VERSION_INT < 0x110
        libvlc_media_player_play(_pVlcPlayer, _pException);
#else
        libvlc_media_player_play(_pVlcPlayer);
#endif
        handleException();
    }

    LOGNS(Omm::Av, upnpav, debug, "vlc engine: play media finished.");
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
    LOGNS(Omm::Av, upnpav, debug, "vlc engine: stop ...");

#if LIBVLC_VERSION_INT < 0x110
    libvlc_media_player_stop(_pVlcPlayer, _pException);
#else
    libvlc_media_player_stop(_pVlcPlayer);
#endif
    handleException();
}


void
VlcEngine::seekByte(Poco::UInt64 byte)
{
    // TODO: implement size based seeking (not time based).
    // try mediacontrol_get_media_position() and mediacontrol_set_media_position() for seek control
    // mediacontrol API seems to be deprecated, it is not present in vlc 1.1
    //
    // try libvlc_vlm API (present in all versions of vlc)
    //
    // use size in bytes from meta data or http get request answer
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
    Poco::UInt64 res = 0;

#if LIBVLC_VERSION_INT < 0x110
    // TODO: implement getPositionByte() for vlc version 1.0
#else
    libvlc_media_stats_t stats;
    int success = libvlc_media_get_stats(_pVlcMedia, &stats);
    res =  stats.i_read_bytes;
#endif
    handleException();
    LOGNS(Omm::Av, upnpav, debug, "vlc engine position [b]: " + Poco::NumberFormatter::format(res, 2));
    return res;
}


float
VlcEngine::getPositionPercentage()
{
    float res = 0.0;

#if LIBVLC_VERSION_INT < 0x110
    res = _length * libvlc_media_player_get_position(_pVlcPlayer, _pException);
#else
    res = _length * libvlc_media_player_get_position(_pVlcPlayer);
#endif
    handleException();
    LOGNS(Omm::Av, upnpav, debug, "vlc engine position [%]: " + Poco::NumberFormatter::format(res, 2));
    return res;
}


float
VlcEngine::getPositionSecond()
{
    float res = 0.0;

#if LIBVLC_VERSION_INT < 0x110
    res = libvlc_media_player_get_time(_pVlcPlayer, _pException) / 1000.0;
#else
    res = libvlc_media_player_get_time(_pVlcPlayer) / 1000.0;
#endif
    handleException();
    LOGNS(Omm::Av, upnpav, debug, "vlc engine position [s]: " + Poco::NumberFormatter::format(res, 2));
    return res;
}


float
VlcEngine::getLengthSeconds()
{
    // libvlc_media_player_get_length() sometimes fetches the last position of the stream, and not the length
#if LIBVLC_VERSION_INT < 0x110
    _length = libvlc_media_player_get_length(_pVlcPlayer, _pException) / 1000.0;
#else
    _length = libvlc_media_player_get_length(_pVlcPlayer) / 1000.0;
#endif
    handleException();
    LOGNS(Omm::Av, upnpav, debug, "vlc engine length [s]: " + Poco::NumberFormatter::format(_length, 2));
    return _length;

    /* use instead?:
    libvlc_time_t
    libvlc_media_get_duration( libvlc_media_t * p_md,
                               libvlc_exception_t * p_e )
    */
}


std::string
VlcEngine::getStreamType()
{
    std::string res = Omm::Av::Engine::StreamTypeOther;

    int videoTrackCount = libvlc_video_get_track_count(_pVlcPlayer);
    handleException();
    LOGNS(Omm::Av, upnpav, debug, "vlc engine number of elementary video streams: " + Poco::NumberFormatter::format(videoTrackCount));

    if (videoTrackCount) {
        return Omm::Av::Engine::StreamTypeVideo;
    }
    return res;

    // FIXME: libvlc_media_get_tracks_info() always returns 0 number of elementary streams
    //        even with libvlc_media_add_option_flag(_pVlcMedia, "sout=#description:dummy", 0);
//    libvlc_media_track_info_t** streams;
//    int countStreams = libvlc_media_get_tracks_info(_pVlcMedia, streams);
//    handleException();
//    LOGNS(Omm::Av, upnpav, debug, "vlc engine number of elementary streams: " + Poco::NumberFormatter::format(countStreams));
//    for (int i = 0; i < countStreams; i++) {
//        LOGNS(Omm::Av, upnpav, debug, "vlc engine elementary stream type: " + Poco::NumberFormatter::format(streams[i]->i_type));
//        if (streams[i]->i_type == libvlc_track_video) {
//            // if stream contains one video elementary stream we're done.
//            return Omm::Av::Engine::StreamTypeVideo;
//        }
//        else if (streams[i]->i_type == libvlc_track_audio) {
//            // stream contains at least one audio stream, so it can be audio or video
//            res = Omm::Av::Engine::StreamTypeAudio;
//        }
//        delete streams[i];
//    }
//    return res;
}


Omm::Av::Engine::TransportState
VlcEngine::getTransportState()
{
    libvlc_state_t res;
#if LIBVLC_VERSION_INT < 0x110
    res = libvlc_media_player_get_state(_pVlcPlayer, _pException);
#else
    res = libvlc_media_player_get_state(_pVlcPlayer);
#endif
    handleException();

    LOGNS(Omm::Av, upnpav, debug, "vlc engine transport state: " + Poco::NumberFormatter::format(res));

    switch (res) {
        case libvlc_Ended:
            return Ended;
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
        LOGNS(Omm::Av, upnpav, error, "vlc engine: " + std::string(libvlc_exception_get_message(_pException)));
    }
    libvlc_exception_init(_pException);
#else
    const char* errMsg = libvlc_errmsg();
    if (errMsg) {
        LOGNS(Omm::Av, upnpav, error, "vlc engine: " + std::string(errMsg));
    }
#endif
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(VlcEngine)
POCO_END_MANIFEST
#endif
