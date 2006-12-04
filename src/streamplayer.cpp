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

//#include <qcursor.h>


// Have to initialize (and thus allocate in gcc) m_instance somewhere
// (not in the default ctor, which is called from getInstance())
StreamPlayer    *StreamPlayer::m_instance = 0;
MediaPlayer     *StreamPlayer::m_keyHandler = 0;
bool             StreamPlayer::m_isPlaying = false;


StreamPlayer::StreamPlayer()
 : Page()
{
    qDebug("StreamPlayer::StreamPlayer()");
    //initStream();
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
StreamPlayer::enterPage()
{
    setFocus();
//    setCursor(QCursor(Qt::BlankCursor));
}


void
StreamPlayer::exitPage()
{
    stop();
}


StreamPlayer::~StreamPlayer()
{
    closeStream();
}


void
StreamPlayer::initStream()
{
    qDebug("StreamPlayer::initStream()");
}


void
StreamPlayer::closeStream()
{
    qDebug("StreamPlayer::closeStream()");
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


void
StreamPlayer::playStream(Mrl *mrl)
{
}


void
StreamPlayer::stopStream()
{
}


void
StreamPlayer::showOsd(QString text, uint duration)
{
}


void
StreamPlayer::hideOsd()
{
}
