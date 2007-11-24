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
#include "streamplayer.h"
#include "controler.h"
#include "debug.h"


// Have to initialize (and thus allocate in gcc) m_instance somewhere
// (not in the default ctor, which is called from getInstance())
StreamPlayer    *StreamPlayer::m_instance = 0;


StreamPlayer::StreamPlayer()
 : Page("StreamPlayer"),
   m_mrl(0)
{
    TRACE("StreamPlayer::StreamPlayer()");
    // all events are received by StreamPlayer, none are forwarded to the GUI.
    addEventType(Event::AllE);
    TRACE("StreamPlayer::StreamPlayer() adding event types, we now have: %i", m_eventTypes.size());
    m_osdTimer = new OsdTimer(this);
}


StreamPlayer*
StreamPlayer::instance()
{
    TRACE("StreamPlayer::instance()");
    if (m_instance == 0) {
        m_instance = new StreamPlayer();
    }
    return m_instance;
}


void
StreamPlayer::setKeyHandler(MediaPlayer *player)
{
    m_keyHandler = player;
}


bool
StreamPlayer::eventHandler(Event *e)
{
    TRACE("StreamPlayer::eventHandler()");
    switch (e->type()) {
    case Event::StartE:
        play();
        break;
    case Event::StopE:
        stop();
        break;
    case Event::PauseE:
        pause();
        break;
    case Event::ForwardE:
        forward();
        break;
    case Event::RewindE:
        rewind();
        break;
    case Event::PlusE:
        zoom(true);
        break;
    case Event::MinusE:
        zoom(false);
        break;
// handle the arrow keys in the specialized MediaPlayers
// the DvdPlayer should differentiate between DVD menu and playback
// and TvPlayer likes to switch channels up and down
    case Event::EnterE:
        select();
        break;
    default:
        // all other keys are forwarded to the MediaPlayer.
        m_keyHandler->eventHandler(e);
        return false;
    }
    return false;
}


void
StreamPlayer::exitPage()
{
    TRACE("StreamPlayer::exitPage");
    stop();
}


StreamPlayer::~StreamPlayer()
{
    delete m_engineLib;
    closeStream();
    delete m_osdTimer;
}


void
StreamPlayer::setEngine(EngineT engine)
{
    m_engineType = engine;
    string engineLibName;
    char* engineCtorName;

    switch(engine) {
    case EngineXine:
        engineLibName = "/home/jb/devel/cc/jambin/src/engine/libjam-engine-xine.so";
        engineCtorName = "createStreamPlayerXine";
        break;
    default:
        break;
    }

    m_engineLib = new SharedLibrary(engineLibName);
    *(void **) (&m_engineCtor) = m_engineLib->resolve(engineCtorName);
    m_engine = (StreamPlayerEngine*) (*m_engineCtor)(this);
}


void
StreamPlayer::play(Title *title)
{
    TRACE("StreamPlayer::play(Title*)");
    if (!title->getMrl()) {
        return;
    }
    if (m_isPlaying && (title->getMrl() != m_mrl)) {
        stopStream();
    }
    m_mrl = title->getMrl();
    m_isPlaying = true;
    playStream(m_mrl);
}


void
StreamPlayer::play()
{
    TRACE("StreamPlayer::play()");
    if (m_mrl) {
        playStream(m_mrl);
    }
}


void
StreamPlayer::stop()
{
    TRACE("StreamPlayer::stop()");
    m_isPlaying = false;
    stopStream();
    // go to main menu
//     Controler::instance()->queueEvent(new Event(Event::MenuE));
}


void
StreamPlayer::pause()
{
    TRACE("StreamPlayer::pause()");
    pauseStream();
}


void
StreamPlayer::forward()
{
    TRACE("StreamPlayer::forward()");
    forwardStream();
}


void
StreamPlayer::rewind()
{
    TRACE("StreamPlayer::rewind()");
    rewindStream();
}


void
StreamPlayer::zoom(bool in)
{
    TRACE("StreamPlayer::zoom()");
    zoomStream(in);
}


void
StreamPlayer::showOsd(string text, uint duration)
{
    TRACE("StreamPlayer::showOsd() with text: %s for %i sec", text.c_str(), duration);
    m_engine->showOsd(text);
    m_osdTimer->setTimeout(duration);
    m_osdTimer->start();   // TODO: this gives a segfault.
}


/* ---------------------------------------------------------------------------------- */


OsdTimer::OsdTimer(StreamPlayer *streamPlayer, uint sec)
 : Timer(sec)
{
    TRACE("OsdTimer::OsdTimer() set to %i sec", sec);
    m_streamPlayer = streamPlayer;
}


void
OsdTimer::exec()
{
    m_streamPlayer->hideOsd();
}
