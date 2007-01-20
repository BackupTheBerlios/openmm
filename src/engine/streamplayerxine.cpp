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
#include "streamplayerxine.h"
#include "debug.h"

#include <sys/stat.h>
#include <cerrno>
#include <string>
using namespace std;


int StreamPlayerXine::m_globX = 0;
int StreamPlayerXine::m_globY = 0;
double StreamPlayerXine::m_pixel_aspect = 0.0;


StreamPlayerXine::StreamPlayerXine(Page *parent)
 : m_pause(false),
   m_currentZoom(100)
{
    TRACE("StreamPlayerXine::StreamPlayerXine()");
    m_parent = parent;
    m_globX = m_parent->globalPositionX();
    m_globY = m_parent->globalPositionY();

    initStream();
}


StreamPlayerXine::~StreamPlayerXine()
{
    closeStream();
}


void
StreamPlayerXine::initStream()
{
    TRACE("StreamPlayerXine::initStream()");

    xineEngine = xine_new();
    //xine_engine_set_param(xineEngine, XINE_ENGINE_PARAM_VERBOSITY, 99);
    char* configFile = "/etc/jam/xineconfig";
    struct stat s;
    if (stat(configFile, &s) == 0)
    {
        TRACE("StreamPlayerXine::initStream() loading config file: %s", configFile);
        xine_config_load(xineEngine, configFile);
    }
    else {
        TRACE("StreamPlayerXine::initStream() no config file loaded: %s", strerror(errno));
    }
    xine_init(xineEngine);

    m_pixel_aspect = 1.0;
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
    x11Window = m_parent->windowId();

    char* videoDriverName = "xv";
    int visualType = XINE_VISUAL_TYPE_X11;
    x11_visual_t visual;
    visual.display = x11Display;
    visual.screen = x11Screen;
    visual.d = x11Window;
    //visual.dest_size_cb = DestSizeCallback;
#endif

    visual.frame_output_cb = FrameOutputCallback;
    visual.user_data = (void*)m_parent;

    videoDriver = xine_open_video_driver(xineEngine,
        videoDriverName,  visualType,
        (void *) &(visual));

    if (!videoDriver)
    {
        TRACE("StreamPlayerXine::initStream() can't init Video Driver! (%s)", videoDriverName);
    }

    char* audioDriverName = "auto";
    audioDriver = xine_open_audio_driver(xineEngine, audioDriverName, NULL);

    TRACE("StreamPlayerXine::initStream() creating new xine stream.");
    m_xineStream = xine_stream_new(xineEngine, audioDriver, videoDriver);

    m_OSD = NULL;
    // TODO: implement timer without Qt
    //connect(&m_OSDTimer, SIGNAL(timeout()), this, SLOT(hideOsd()));
}


void
StreamPlayerXine::closeStream()
{
    TRACE("StreamPlayerXine::closeStream()");
    xine_close(m_xineStream);
    xine_dispose(m_xineStream);
    xine_close_audio_driver(xineEngine, audioDriver);
    xine_close_video_driver(xineEngine, videoDriver);
    xine_exit(xineEngine);
#ifndef QWS
    if (x11Display)
        XCloseDisplay(x11Display);
    x11Display = NULL;
#endif
}


void
StreamPlayerXine::showOsd(string text)
{
    initOSD();
    TRACE("StreamPlayerXine::showOSD(), with text: %s", text.c_str());
    xine_osd_draw_text(m_OSD, 0, 0, text.c_str(), XINE_OSD_TEXT1);
    //xine_osd_draw_rect(m_OSD, 0, 0, 500, 100, 200, 1 );

    xine_osd_show(m_OSD, 0 );
//     m_OSDTimer.start(duration, TRUE);
}


void
StreamPlayerXine::hideOsd()
{
    TRACE("StreamPlayerXine::hideOSD()");
    xine_osd_hide(m_OSD, 0);
    xine_osd_free(m_OSD);
    m_OSD = NULL;
}


void
StreamPlayerXine::initOSD()
{
    TRACE("StreamPlayerXine::initOSD()");
    if (m_OSD != NULL)
        hideOsd();
//     if (m_OSDTimer.isActive())
//         m_OSDTimer.stop();

    m_marginOSD = m_parent->height() / 10;
    m_OSD = xine_osd_new(m_xineStream, m_marginOSD, (int)(m_parent->height() * 0.75), m_parent->width() - 2*m_marginOSD,
                        (int)((m_parent->height() * 0.25)) - m_marginOSD);
    if (!m_OSD)
        TRACE("StreamPlayerXine::initOSD(), initialization of OSD failed");
    if (!xine_osd_set_font(m_OSD, "sans", 24))
        TRACE("StreamPlayerXine::initOSD(), initialization of OSD font failed");
    //xine_osd_set_text_palette(m_OSD, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1);
}


// void
// StreamPlayerXine::DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
//                        int* dest_width, int* dest_height, double* dest_aspect)
// {
//     if (p == NULL) return;
//     Page* vw = (Page*) p;
// 
//     *dest_width = vw->width();
//     *dest_height = vw->height();
//     *dest_aspect = m_pixel_aspect;
// }


void
StreamPlayerXine::FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y)
{
    if (p == NULL) return;
    Page* vw = (Page*) p;

    *dest_x = 0;
    *dest_y = 0 ;
    *dest_width = vw->width();
    *dest_height = vw->height();
    *dest_aspect = 1.0;
    //*dest_aspect = m_pixel_aspect;
    //*win_x = m_globX;
    //*win_y = m_globY;
}


void
StreamPlayerXine::playStream(Mrl *mrl)
{
    string xineMrl = mrl->getProtocol() + mrl->getServer() + mrl->getPath();
    if (mrl->getType() == Mrl::TvVdr) {
         TRACE("StreamPlayerXine::playStream(), setting demuxer to mpeg_pes");
         xineMrl += "#demux:mpeg_pes";
    }
    if (mrl->getFiles().size() > 0) {
        // this is a multi-file mrl
        // TODO: loop over all files.
        xineMrl = mrl->getProtocol() + mrl->getServer() + mrl->getPath() + "/" + mrl->getFiles()[0];
        TRACE("StreamPlayerXine::play() mrl: %s", xineMrl.c_str());
        //xine_open(m_xineStream, xineMrl.utf8());
        xine_open(m_xineStream, xineMrl.c_str());
        xine_play(m_xineStream, 0, 0);
    }
    else {
        TRACE("StreamPlayerXine::play() mrl: %s", xineMrl.c_str());
        //xine_open(m_xineStream, xineMrl.utf8());
        xine_open(m_xineStream, xineMrl.c_str());
        xine_play(m_xineStream, 0, 0);
    }
}


void
StreamPlayerXine::stopStream()
{
    TRACE("StreamPlayerXine::stopStream()");
    xine_stop(m_xineStream);
    xine_close(m_xineStream);
}


void
StreamPlayerXine::pauseStream()
{
    if (m_pause) {
        TRACE("StreamPlayerXine::pauseStream() setting speed to normal");
        xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
//         xine_play(m_xineStream, m_posStream, 0);
    }
    else {
        TRACE("StreamPlayerXine::pauseStream() setting speed to pause");
        xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
//         savePosition();
    }
    m_pause = !m_pause;
}


void
StreamPlayerXine::zoomStream(bool in)
{
    if (in && (m_currentZoom + 5) <= XINE_VO_ZOOM_MAX) {
        m_currentZoom += 5;
    }
    else if (!in && (m_currentZoom - 5) >= 100) {
        m_currentZoom -= 5;
    }
    xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_X, m_currentZoom);
    xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_Y, m_currentZoom);
}


void
StreamPlayerXine::forwardStream()
{
    TRACE("StreamPlayerXine::forwardStream()");
    if (!isSeekable()) {
        TRACE("StreamPlayerXine::forwardStream() stream not seekable");
        return;
    }
//     xine_trick_mode(m_xineStream, XINE_TRICK_MODE_FAST_FORWARD, 2);
    xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_FAST_4);

}


void
StreamPlayerXine::rewindStream()
{
    TRACE("StreamPlayerXine::rewindStream()");
    if (!isSeekable()) {
        TRACE("StreamPlayerXine::rewindStream() stream not seekable");
        return;
    }
//     xine_trick_mode(m_xineStream, XINE_TRICK_MODE_FAST_REWIND, 2);
}


bool
StreamPlayerXine::isSeekable()
{
    return (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_SEEKABLE);
}


void
StreamPlayerXine::savePosition()
{
    if (xine_get_pos_length(m_xineStream, &m_posStream, &m_posTime, &m_lengthStream) == 0) {
        TRACE("StreamPlayerXine::savePosition() could not get position");
    }
}


void
StreamPlayerXine::left()
{
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;
    xev.type = XINE_EVENT_INPUT_LEFT;
    xine_event_send(m_xineStream, &xev);
}


void
StreamPlayerXine::right()
{
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;
    xev.type = XINE_EVENT_INPUT_RIGHT;
    xine_event_send(m_xineStream, &xev);
}


void
StreamPlayerXine::up()
{
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;
    xev.type = XINE_EVENT_INPUT_UP;
    xine_event_send(m_xineStream, &xev);
}


void
StreamPlayerXine::down()
{
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;
    xev.type = XINE_EVENT_INPUT_DOWN;
    xine_event_send(m_xineStream, &xev);
}


void
StreamPlayerXine::select()
{
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;
    xev.type = XINE_EVENT_INPUT_SELECT;
    xine_event_send(m_xineStream, &xev);
}


extern "C" {
StreamPlayerEngine* createStreamPlayerXine(Page *parent)
{
    return new StreamPlayerXine(parent);
}
}
