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

#include <streamplayer.h>
#include <qtimer.h>

/**
@author Jörg Bakker
*/
class StreamPlayerXine : public StreamPlayer
{
public:
    static StreamPlayerXine *getInstance(QWidget *parent = 0, const char *name = 0);

    virtual QString tvMRL(QString channelId);

public slots:
    virtual void play(QString mrl);
    virtual void stop();

    virtual void showOSD(QString text, uint duration);
    virtual void hideOSD();

protected:
    StreamPlayerXine(QWidget *parent = 0, const char *name = 0);
    //~StreamPlayerXine();

    virtual void initStream();
    virtual void closeStream();

private:
    static StreamPlayerXine *m_instance;

    static void DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
                       int* dest_width, int* dest_height, double* dest_aspect);

    static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

    void initOSD();
    
    xine_t *xineEngine;
    xine_audio_port_t *audioDriver;
    xine_video_port_t *videoDriver;
    xine_stream_t *xineStream;

    Window xineWindow;
    Display* xineDisplay;
    int xineScreen;

    int videoFrameWidth;
    int videoFrameHeight;
    int globX;
    int globY;
    double res_v, res_h;
    double pixel_aspect;
    
    xine_osd_t *m_OSD;

    x11_visual_t visual;
    xine_event_queue_t *eventQueue;
    
    uint m_marginOSD;
    QTimer m_OSDTimer;
};

#endif
