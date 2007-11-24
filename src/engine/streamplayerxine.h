/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#ifndef STREAMPLAYERXINE_H
#define STREAMPLAYERXINE_H

#include "page.h"
#include "streamplayerengine.h"

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <xine.h>


// TODO: gapless playback with multiple .vdr files
// TODO: gapless playback while switching streams (possible with xine 1.1.1)
// TODO: DirectFB support

/**
@author Jörg Bakker
*/
class StreamPlayerXine : public StreamPlayerEngine
{
public:
    StreamPlayerXine(Page *parent);
    ~StreamPlayerXine();

    virtual void initStream();
    virtual void closeStream();
    virtual void playStream(Mrl *mrl);
    virtual void stopStream();
    virtual void pauseStream();
    virtual void forwardStream();
    virtual void rewindStream();
    virtual void zoomStream(bool in);
    virtual void left();
    virtual void right();
    virtual void up();
    virtual void down();
    virtual void select();
    virtual void showOsd(string text);
    virtual void hideOsd();
    virtual void switchAudioDevice(int maxDeviceNum);

private:
    bool isSeekable();
    void savePosition();
//     void savePositionTime();
    void seekRelative(int offSec);

    Page *m_parent;

//     static void DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
//                        int* dest_width, int* dest_height, double* dest_aspect);

    static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

    void initOSD();

    xine_t              *m_xineEngine;
    xine_audio_port_t   *m_audioDriver;
    xine_video_port_t   *m_videoDriver;
    xine_stream_t       *m_xineStream;
    string               m_xineMrl;
    char                *m_audioDriverName;
    int                  m_audioDeviceNum;

#ifndef QWS
    Display*             x11Display;
    int                  x11Screen;
    Window               x11Window;
#endif

    int                  videoFrameWidth;
    int                  videoFrameHeight;
    static int           m_globX, m_globY;
    static double        m_pixel_aspect;
    double               res_v, res_h;

    xine_osd_t          *m_OSD;

    xine_event_queue_t  *eventQueue;

    bool                 m_pause;
    int                  m_posStream;
    int                  m_posTime;
    int                  m_lengthStream;
    int                  m_currentZoom;
    int                  m_seekOff;

    uint                 m_marginOSD;
};

#endif
