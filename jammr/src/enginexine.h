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

#ifndef ENGINEXINE_H
#define ENGINEXINE_H

#include "engine.h"

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <xine.h>
#include <xine/xineutils.h>

/**
@author Jörg Bakker
*/
class EngineXine : public Engine
{
public:
    EngineXine();
    ~EngineXine();
    
    // virtual void setVideoDriver(string);
    // virtual void setAudioDriver(string);
    // virtual void setFullscreen(bool on);
    // virtual vector<string> getAudioAdapters();
    // virtual void setAudioAdapter(string);
    virtual void setMrl(string mrl);
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void seek(long seekval);
    virtual void next();
    virtual void previous();

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

    xine_t              *m_xineEngine;
    xine_audio_port_t   *m_audioDriver;
    xine_video_port_t   *m_videoDriver;
    xine_stream_t       *m_xineStream;
    char                *m_audioDriverName;
    int                  m_audioDeviceNum;

#ifndef QWS
    Display*             x11Display;
    int                  x11Screen;
    Window               x11Window;
#endif

    string               m_mrl;
    int                  fullscreen;
    static int           videoFrameWidth;
    static int           videoFrameHeight;
    static int           m_globX, m_globY;
    static double        m_pixel_aspect;
    double               res_v, res_h;

    xine_event_queue_t  *eventQueue;

    bool                 m_pause;
    int                  m_posStream;
    int                  m_posTime;
    int                  m_lengthStream;
    int                  m_currentZoom;
    int                  m_seekOff;
};

#endif
