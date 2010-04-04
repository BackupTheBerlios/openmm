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

#include "enginexine.h"
#include "debug.h"

#include <sys/stat.h>
#include <cerrno>
#include <string>
using namespace std;

#define MWM_HINTS_DECORATIONS   (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS 5
#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
ButtonMotionMask | StructureNotifyMask |        \
PropertyChangeMask | PointerMotionMask)

#define PROP_MWM_HINTS_ELEMENTS 5

int EngineXine::_globX = 0;
int EngineXine::_globY = 0;
int EngineXine::videoFrameWidth = 320;
int EngineXine::videoFrameHeight = 200;
double EngineXine::_pixel_aspect = 0.0;


EngineXine::EngineXine()
 : _pause(false),
   _currentZoom(100),
   _audioDeviceNum(0),
   _seekOff(50),
   _posTime(0),
   _lengthStream(0)
{
    TRACE("EngineXine::EngineXine()");
    init();
}


EngineXine::~EngineXine()
{
    close();
}


void
EngineXine::init()
{
    TRACE("EngineXine::init()");

    _xineEngine = xine_new();
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
    xine_init(_xineEngine);

    _pixel_aspect = 1.0;
#ifdef __FRAMEBUFFER___
//    char* videoDriverName = "fb";
//    char* videoDriverName = "vidixfb";
    char* videoDriverName = "directfb";
//    int visualType = XINE_VISUAL_TYPE_FB;
    int visualType = XINE_VISUAL_TYPE_DFB;
    fb_visual_t visual;
#elif __X11__
    XInitThreads ();
    x11Display = XOpenDisplay(NULL);
    x11Screen = DefaultScreen(x11Display);
//     x11Window = _parent->windowId();
    initWindow();

    char* videoDriverName = "xv";
    int visualType = XINE_VISUAL_TYPE_X11;
    x11_visual_t visual;
    visual.display = x11Display;
    visual.screen = x11Screen;
    visual.d = x11Window;
    //visual.dest_size_cb = DestSizeCallback;
#endif

    visual.frame_output_cb = FrameOutputCallback;
//     visual.user_data = (void*)_parent;

    _videoDriver = xine_open_video_driver(_xineEngine,
        videoDriverName,  visualType,
        (void *) &(visual));

    if (!_videoDriver)
    {
        TRACE("EngineXine::init() can't init Video Driver! (%s)", videoDriverName);
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
    _audioDriverName = "alsa";
    _audioDriver = xine_open_audio_driver(_xineEngine, _audioDriverName, NULL);

    TRACE("EngineXine::initStream() creating new xine stream.");
    _xineStream = xine_strea_new(_xineEngine, _audioDriver, _videoDriver);
}


void
EngineXine::close()
{
    TRACE("EngineXine::close()");
    xine_close(_xineStream);
    xine_dispose(_xineStream);
    xine_close_audio_driver(_xineEngine, _audioDriver);
    xine_close_video_driver(_xineEngine, _videoDriver);
    xine_exit(_xineEngine);
#ifndef QWS
    if (x11Display)
        XCloseDisplay(x11Display);
    x11Display = NULL;
#endif
}


void
EngineXine::initWindow()
{
    int xpos    = 0;
    int ypos    = 0;
    int width   = 320;
    int height  = 200;
    
      /* some initalization for the X11 Window we will be showing video in */
    XLockDisplay(x11Display);
    fullscreen = 0;
    x11Window = XCreateSimpleWindow(x11Display, XDefaultRootWindow(x11Display),
                                    _globX, _globY, videoFrameWidth, videoFrameHeight, 1, 0, 0);
    
    XSelectInput(x11Display, x11Window, INPUT_MOTION);
    
    XMapRaised(x11Display, x11Window);
    
    res_h = (DisplayWidth(x11Display, x11Screen) * 1000 / DisplayWidthMM(x11Display, x11Screen));
    res_v = (DisplayHeight(x11Display, x11Screen) * 1000 / DisplayHeightMM(x11Display, x11Screen));
    XSync(x11Display, False);
    XUnlockDisplay(x11Display);
}


void
EngineXine::FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
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
EngineXine::setMrl(string mrl)
{
    TRACE("EngineXine::setMrl() to: %s", mrl.c_str());
    _mrl = mrl;
}


void
EngineXine::play()
{
    TRACE("EngineXine::play() mrl: %s", _mrl.c_str());
    xine_open(_xineStream, _mrl.c_str());
    if (!isSeekable()) {
        TRACE("EngineXine::play() WARNING: stream is not seekable!");
    }
    xine_play(_xineStream, 0, 0);
}


void
EngineXine::stop()
{
    TRACE("EngineXine::stop()");
    xine_stop(_xineStream);
    xine_close(_xineStream);
}


void
EngineXine::next()
{
    TRACE("EngineXine::next()");
}


void
EngineXine::previous()
{
    TRACE("EngineXine::previous()");
}


void
EngineXine::pause()
{
    if (_pause) {
        TRACE("EngineXine::pauseStream() setting speed to normal");
        xine_set_param(_xineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
    }
    else {
        TRACE("EngineXine::pauseStream() setting speed to pause");
        xine_set_param(_xineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    }
    _pause = !_pause;
}


void
EngineXine::seek(long seekval)
{
    TRACE("EngineXine::seek() to position in millisec: %i", seekval);
    xine_play(_xineStream, seekval, 0);
}


bool
EngineXine::isSeekable()
{
    return (bool)xine_get_strea_info(_xineStream, XINE_STREAM_INFO_SEEKABLE);
}


void
EngineXine::savePosition()
{
    if (xine_get_pos_length(_xineStream, &_posStream, &_posTime, &_lengthStream) == 0) {
        TRACE("EngineXine::savePosition() could not get position");
    }
    TRACE("EngineXine::savePosition() at _posStream: %i, _posTime: %i, _lengthStream: %i", 
            _posStream, _posTime, _lengthStream);
}
