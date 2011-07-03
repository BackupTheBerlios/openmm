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

#include <sys/stat.h>
#include <cerrno>
#include <string>

#include <Omm/Util.h>

#include "EngineXine.h"


using namespace std;

int XineEngine::_globX = 0;
int XineEngine::_globY = 0;
int XineEngine::videoFrameWidth = 320;
int XineEngine::videoFrameHeight = 200;
double XineEngine::_pixel_aspect = 0.0;


XineEngine::XineEngine() :
_pause(false),
_currentZoom(100),
_audioDeviceNum(0),
_seekOff(50),
_posTime(0),
_lengthStream(0)
{
    _engineId = "OMM xine engine " + Omm::OMM_VERSION;
}


XineEngine::~XineEngine()
{
    close();
//    delete _pVideo;
}


void
XineEngine::createPlayer()
{    
//    Omm::Util::PluginLoader<XineVideo> pluginLoader;
//    std::string videoPlugin("xinevideo-x11");
////     std::string videoPlugin("xinevideo-fb");
//
//    try {
//	_pVideo = pluginLoader.load(videoPlugin, "XineVideo");
//    }
//    catch(Poco::NotFoundException) {
//	std::cerr << "Error could not find xine video plugin: " << videoPlugin << std::endl;
//	return;
//    }
//    std::clog << "xine video plugin: " << videoPlugin << " loaded successfully" << std::endl;

    if (!_pVisual) {
        _pVisual = new Omm::Sys::Visual;
    }
    // open window on window system
    _pVisual->show();


    
    //xine_engine_set_param(xineEngine, XINE_ENGINE_PARAM_VERBOSITY, 99);
//     char* configFile = "/etc/omm/xineconfig";
/*    struct stat s;
    if (stat(configFile, &s) == 0)
    {
        TRACE("MediaPlayer::init() loading config file: %s", configFile);
        xine_config_load(_xineEngine, configFile);
    }
    else {
        TRACE("MediaPlayer::initStream() no config file loaded: %s", strerror(errno));
    }*/

    _xineEngine = xine_new();
    xine_init(_xineEngine);

    _pixel_aspect = 1.0;

//    if (getFullscreen()) {
//        setWidth(_pVideo->displayWidth());
//        setHeight(_pVideo->displayHeight());
//    }
    videoFrameWidth = _pVisual->getWidth();
    videoFrameHeight = _pVisual->getHeight();
//    _pVideo->initVisual(getWidth(), getHeight);

    std::string driverName;

    if (_pVisual->getType() == Omm::Sys::Visual::VTX11) {
        driverName = "xv";
        _x11Visual.frame_output_cb = FrameOutputCallback;
        _x11Visual.d = *(Poco::UInt32*)_pVisual->getWindow();
//        _x11Visual.display = XOpenDisplay(NULL);
//        _x11Visual.screen = x11Screen;
        _videoDriver = xine_open_video_driver(_xineEngine,
        driverName.c_str(), XINE_VISUAL_TYPE_X11,
        _pVisual);
    }

//    switch (_pVideo->visualType()) {
//	case XINE_VISUAL_TYPE_X11:
//	    x11_visual_t* v = (x11_visual_t*)(_pVideo->visual());
//	    v->frame_output_cb = FrameOutputCallback;
//	    break;
//    }

//    _videoDriver = xine_open_video_driver(_xineEngine,
//        _pVideo->driverName().c_str(),  _pVideo->visualType(),
//        _pVideo->visual());

    if (!_videoDriver)
    {
        Omm::Av::Log::instance()->upnpav().error("xine engine can't init video driver " + driverName);
//        std::cerr << "XineEngine::init() can't init video driver " << _pVideo->driverName() << std::endl;
    }

/*
    set audio device for ALSA through config option: audio.device.alsa_front_device  (for stereo device)
    set audio device for OSS through config option: audio.device.oss_device_name  (/dev/dsp  /dev/sound/dsp)
        or audio.device.oss_device_number
*/
/* 
    xine_cfg_entry_t entry;
    xine_config_lookup_entry(xineEngine, "audio.device.oss_device_name", &entry);
    TRACE("StreamPlayerXine::initStream() current audio device: %s", entry.str_value);
    entry.str_value = "/dev/dsp1";
    TRACE("StreamPlayerXine::initStream() setting audio device to: %s", "/dev/dsp1");
    xine_config_update_entry(xineEngine, &entry);*/
//     char* audioDriverName = "auto";
//     char* audioDriverName = "oss";
    std::string _audioDriverName("alsa");
    _audioDriver = xine_open_audio_driver(_xineEngine, _audioDriverName.c_str(), NULL);

    if (!_audioDriver)
    {
//        std::cerr << "XineEngine::init() can't init audio driver " << _audioDriverName << std::endl;
    }
    
    _xineStream = xine_stream_new(_xineEngine, _audioDriver, _videoDriver);
}


void
XineEngine::close()
{
    xine_close(_xineStream);
    xine_dispose(_xineStream);
    xine_close_audio_driver(_xineEngine, _audioDriver);
    xine_close_video_driver(_xineEngine, _videoDriver);
    xine_exit(_xineEngine);
    
//    _pVideo->closeVisual();
}


void
XineEngine::FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y)
{
    if (p == NULL) return;

    *dest_x = 0;
    *dest_y = 0 ;
    *dest_width = videoFrameWidth;
    *dest_height = videoFrameHeight;
    *dest_aspect = 1.0;
    //*dest_aspect = _pixel_aspect;
    //*win_x = _globX;
    //*win_y = _globY;
}


void
XineEngine::setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo)
{
    _mrl = uri;
}


void
XineEngine::play()
{
    xine_open(_xineStream, _mrl.c_str());
    if (!isSeekable()) {
//         TRACE("XineEngine::play() WARNING: stream is not seekable!");
    }
    xine_play(_xineStream, 0, 0);
}


void
XineEngine::stop()
{
//     TRACE("XineEngine::stop()");
    xine_stop(_xineStream);
    xine_close(_xineStream);
}


void
XineEngine::next()
{
//     TRACE("XineEngine::next()");
}


void
XineEngine::previous()
{
//     TRACE("XineEngine::previous()");
}


void
XineEngine::pause()
{
    if (_pause) {
//         TRACE("XineEngine::pauseStream() setting speed to normal");
        xine_set_param(_xineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
    }
    else {
//         TRACE("XineEngine::pauseStream() setting speed to pause");
        xine_set_param(_xineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    }
    _pause = !_pause;
}


void
XineEngine::seekByte(Poco::UInt64 byte)
{
}


void
XineEngine::seekPercentage(float percentage)
{
}


Poco::UInt64
XineEngine::getPositionByte()
{
}


float
XineEngine::getPositionPercentage()
{
}


float
XineEngine::getPositionSecond()
{
}


float
XineEngine::getLengthSeconds()
{
}


XineEngine::TransportState
XineEngine::getTransportState()
{
    // TODO: implement getTransportState() in XineEngine.
}


void
XineEngine::seekSecond(float second)
{
    xine_play(_xineStream, second, 0);
}


bool
XineEngine::isSeekable()
{
    return (bool)xine_get_stream_info(_xineStream, XINE_STREAM_INFO_SEEKABLE);
}


void
XineEngine::savePosition()
{
    if (xine_get_pos_length(_xineStream, &_posStream, &_posTime, &_lengthStream) == 0) {
//         TRACE("XineEngine::savePosition() could not get position");
    }
//     TRACE("XineEngine::savePosition() at _posStream: %i, _posTime: %i, _lengthStream: %i", 
//             _posStream, _posTime, _lengthStream);
}


void
XineEngine::setSpeed(int nom, int denom)
{
}


void
XineEngine::setVolume(const std::string& channel, float vol)
{
    xine_set_param(_xineStream, XINE_PARAM_AUDIO_VOLUME, vol);
}


float
XineEngine::getVolume(const std::string& channel)
{
    return xine_get_param(_xineStream, XINE_PARAM_AUDIO_VOLUME);
}

#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(XineEngine)
POCO_END_MANIFEST
#endif
