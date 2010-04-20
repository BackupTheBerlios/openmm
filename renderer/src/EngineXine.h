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

#ifndef ENGINEXINE_H
#define ENGINEXINE_H

#include <Omm/UpnpAvRenderer.h>

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <xine.h>
#include <xine/xineutils.h>

/**
@author Jörg Bakker
*/
class EnginePlugin : public Omm::Av::Engine
{
public:
    EnginePlugin();
    ~EnginePlugin();
    
    // virtual void setVideoDriver(string);
    // virtual void setAudioDriver(string);
    // virtual void setFullscreen(bool on);
    // virtual vector<string> getAudioAdapters();
    // virtual void setAudioAdapter(string);
    
    virtual void setFullscreen(bool on = true);
    std::string getEngineId() { return _engineId; }
    
    virtual void setUri(std::string mrl);
    virtual void load();
    virtual void pause();
    virtual void stop();
    virtual void seek(int seconds);
    virtual void next();
    virtual void previous();
    
    virtual void setSpeed(int nom, int denom);
    virtual void getPosition(float &seconds);
    virtual void getLength(float &seconds);
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);

private:
    void init();
    void close();
    void initWindow();
    bool isSeekable();
    void savePosition();

    static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

    void initOSD();

    xine_t              *_xineEngine;
    xine_audio_port_t   *_audioDriver;
    xine_video_port_t   *_videoDriver;
    xine_stream_t       *_xineStream;
    char                *_audioDriverName;
    int                  _audioDeviceNum;

#ifndef QWS
    Display*             x11Display;
    int                  x11Screen;
    Window               x11Window;
#endif

    std::string          _mrl;
    int                  fullscreen;
    static int           videoFrameWidth;
    static int           videoFrameHeight;
    static int           _globX, _globY;
    static double        _pixel_aspect;
    double               res_v, res_h;

    xine_event_queue_t  *eventQueue;

    bool                 _pause;
    int                  _posStream;
    int                  _posTime;
    int                  _lengthStream;
    int                  _currentZoom;
    int                  _seekOff;
    
    bool                 _fullscreen;
    std::string          _engineId;
};

#endif
