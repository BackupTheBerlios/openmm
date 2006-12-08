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

// #ifdef QWS
// #include <qwindowsystem_qws.h>
// #include <qcolor.h>
// #endif

#include <qdir.h>
#include <iostream>


StreamPlayerXine* StreamPlayerXine::m_instance = 0;

StreamPlayerXine::StreamPlayerXine()
 : StreamPlayer()
{
    qDebug("StreamPlayerXine::StreamPlayerXine()");
    globX = globalPositionX();
    globY = globalPositionY();

    initStream();
}


StreamPlayerXine::~StreamPlayerXine()
{
    closeStream();
}


StreamPlayerXine*
StreamPlayerXine::instance()
{
    qDebug("StreamPlayerXine::instance()");
    if (m_instance == 0) {
        m_instance = new StreamPlayerXine();
    }
    return m_instance;
}


void
StreamPlayerXine::initStream()
{
    qDebug("StreamPlayerXine::initStream()");

    xineEngine = xine_new();
    //xine_engine_set_param(xineEngine, XINE_ENGINE_PARAM_VERBOSITY, 99);
    //QString configFile = QDir::homeDirPath();
    //configFile.append("/.jam/xineconfig");
    char* configFile = "/etc/jam/xineconfig";
    if (QFile::exists(configFile))
    {
        xine_config_load(xineEngine, configFile);
    }
    xine_init(xineEngine);

#ifdef QWS
//    QWSServer::setDesktopBackground(QColor(QColor::black));
//    QString videoDriverName = "fb";
//    QString videoDriverName = "vidixfb";
    char* videoDriverName = "directfb";
//    int visualType = XINE_VISUAL_TYPE_FB;
    int visualType = XINE_VISUAL_TYPE_DFB;
    fb_visual_t visual;
    pixel_aspect = 1.0;  // TODO: set this to a proper value.
#else
    XInitThreads ();
    //xineDisplay = XOpenDisplay( getenv("DISPLAY") );
    x11Display = XOpenDisplay(NULL);
    x11Screen = DefaultScreen(x11Display);
    x11Window = windowId();

    XLockDisplay( x11Display );
//    XSelectInput( x11Display, x11Window, ExposureMask );
    res_h = (DisplayWidth(x11Display, x11Screen) * 1000 / DisplayWidthMM(x11Display, x11Screen));
    res_v = (DisplayHeight(x11Display, x11Screen) * 1000 / DisplayHeightMM(x11Display, x11Screen));
    pixel_aspect = res_v / res_h;
    XSync(x11Display, False);
    XUnlockDisplay(x11Display);

    char* videoDriverName = "xv";
    int visualType = XINE_VISUAL_TYPE_X11;
    x11_visual_t visual;
    visual.display = x11Display;
    visual.screen = x11Screen;
    visual.d = x11Window;
    visual.dest_size_cb = DestSizeCallback;
#endif

    visual.frame_output_cb = FrameOutputCallback;
    visual.user_data = (void*)this;

    videoDriver = xine_open_video_driver(xineEngine,
        videoDriverName,  visualType,
        (void *) &(visual));

    if (!videoDriver)
    {
        std::cout << "Controler: Can't init Video Driver! (" << videoDriverName << ")\n";
    }

    char* audioDriverName = "auto";
    audioDriver = xine_open_audio_driver(xineEngine, audioDriverName, NULL);

    std::cout << "Controler: Creating new xine stream.\n";
    xineStream = xine_stream_new(xineEngine, audioDriver, videoDriver);

    m_OSD = NULL;
    // TODO: implement timer without Qt
    //connect(&m_OSDTimer, SIGNAL(timeout()), this, SLOT(hideOsd()));
}


void
StreamPlayerXine::closeStream()
{
    qDebug("StreamPlayerXine::closeStream()");
    xine_close(xineStream);
    xine_dispose(xineStream);
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
StreamPlayerXine::showOsd(QString text, uint duration)
{
    initOSD();
    qDebug("StreamPlayerXine::showOSD(), with text: %s", text.latin1());
    xine_osd_draw_text(m_OSD, 0, 0, text, XINE_OSD_TEXT1);
    //xine_osd_draw_rect(m_OSD, 0, 0, 500, 100, 200, 1 );

    xine_osd_show(m_OSD, 0 );
    m_OSDTimer.start(duration, TRUE);
}


void
StreamPlayerXine::hideOsd()
{
    qDebug("StreamPlayerXine::hideOSD()");
    xine_osd_hide(m_OSD, 0);
    xine_osd_free(m_OSD);
    m_OSD = NULL;
}


void
StreamPlayerXine::initOSD()
{
    qDebug("StreamPlayerXine::initOSD()");
    if (m_OSD != NULL)
        hideOsd();
    if (m_OSDTimer.isActive())
        m_OSDTimer.stop();

    m_marginOSD = height() / 10;
    m_OSD = xine_osd_new(xineStream, m_marginOSD, (int)(height() * 0.75), width() - 2*m_marginOSD,
                        (int)((height() * 0.25)) - m_marginOSD);
    if (!m_OSD)
        qDebug("StreamPlayerXine::initOSD(), initialization of OSD failed");
    if (!xine_osd_set_font(m_OSD, "sans", 24))
        qDebug("StreamPlayerXine::initOSD(), initialization of OSD font failed");
    //xine_osd_set_text_palette(m_OSD, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1);
}


void
StreamPlayerXine::DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
                       int* dest_width, int* dest_height, double* dest_aspect)
{
    if (p == NULL) return;
    StreamPlayerXine* vw = (StreamPlayerXine*) p;

    *dest_width = vw->width();
    *dest_height = vw->height();
    *dest_aspect = vw->pixel_aspect;
}


void
StreamPlayerXine::FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y)
{
    if (p == NULL) return;
    StreamPlayerXine* vw = (StreamPlayerXine*) p;

    *dest_x = 0;
    *dest_y = 0 ;
    *dest_width = vw->width();
    *dest_height = vw->height();
    *dest_aspect = vw->pixel_aspect;
    *win_x = vw->globX;
    *win_y = vw->globY;
}


void
StreamPlayerXine::playStream(Mrl *mrl)
{
    QString xineMrl = mrl->getProtocol() + mrl->getServer() + mrl->getPath();
    if (mrl->getType() == Mrl::TvVdr) {
         qDebug("StreamPlayerXine::playStream(), setting demuxer to mpeg_pes");
         xineMrl += "#demux:mpeg_pes";
    }
    if (mrl->getFiles().count() > 0) {
        // this is a multi-file mrl
        // TODO: loop over all files.
        xineMrl = mrl->getProtocol() + mrl->getServer() + mrl->getPath() + "/" + mrl->getFiles()[0];
        qDebug("StreamPlayerXine::play() mrl: %s", xineMrl.ascii());
        xine_open(xineStream, xineMrl.utf8());
        xine_play(xineStream, 0, 0);
    }
    else {
        qDebug("StreamPlayerXine::play() mrl: %s", xineMrl.ascii());
        xine_open(xineStream, xineMrl.utf8());
        xine_play(xineStream, 0, 0);
    }
}


void
StreamPlayerXine::stopStream()
{
    qDebug("StreamPlayerXine::stop()");
    xine_stop(xineStream);
    xine_close(xineStream);
}
