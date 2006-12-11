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


// Have to initialize (and thus allocate in gcc) m_instance somewhere
// (not in the default ctor, which is called from getInstance())
StreamPlayer    *StreamPlayer::m_instance = 0;


StreamPlayer::StreamPlayer()
 : Page("StreamPlayer")
{
    qDebug("StreamPlayer::StreamPlayer()");
}


StreamPlayer*
StreamPlayer::instance()
{
    qDebug("StreamPlayer::instance()");
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


void
StreamPlayer::keyPressEvent(QKeyEvent *k)
{
    qDebug("StreamPlayer::keyPressEvent()");
    m_keyHandler->keyHandler(k);
}


void
StreamPlayer::exitPage()
{
    stop();
}


StreamPlayer::~StreamPlayer()
{
    delete m_engineLib;
    closeStream();
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
    qDebug("StreamPlayer::play()");
    if (m_isPlaying) {
        stopStream();
    }
    if (!title->getMrl()) {
        return;
    }
    m_isPlaying = true;
    playStream(title->getMrl());
}


void
StreamPlayer::stop()
{
    qDebug("StreamPlayer::stop()");
    m_isPlaying = false;
    stopStream();
}
