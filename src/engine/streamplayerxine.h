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

#include <X11/Xlib.h>
// undef some nasty preprocessor macros in Xlib, they mess up the Qt headers
#undef Always
#undef KeyPress
#undef Unsorted

#include <xine.h>
// undef some nasty preprocessor macros in xine, they mess up the Qt headers
#undef None
#undef KeyRelease
#undef FocusIn
#undef FocusOut

#include <qtimer.h>


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

public slots:
    virtual void showOsd(QString text, uint duration);
    virtual void hideOsd();
    virtual void initStream();
    virtual void closeStream();
    virtual void playStream(Mrl *mrl);
    virtual void stopStream();

private:
    Page *m_parent;

//     static void DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
//                        int* dest_width, int* dest_height, double* dest_aspect);

    static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

    void initOSD();

    xine_t *xineEngine;
    xine_audio_port_t *audioDriver;
    xine_video_port_t *videoDriver;
    xine_stream_t *xineStream;

#ifndef QWS
    Display* x11Display;
    int x11Screen;
    Window x11Window;
#endif

    int videoFrameWidth;
    int videoFrameHeight;
    static int m_globX, m_globY;
    static double m_pixel_aspect;
    double res_v, res_h;

    xine_osd_t *m_OSD;

    xine_event_queue_t *eventQueue;

    uint m_marginOSD;
    QTimer m_OSDTimer;
};

#endif
