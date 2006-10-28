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
StreamPlayer* StreamPlayer::m_instance = 0;

StreamPlayer::StreamPlayer(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
    qDebug("StreamPlayer::StreamPlayer()");
    initStream();
}


StreamPlayer*
StreamPlayer::getInstance(QWidget *parent, const char *name)
{
    qDebug("StreamPlayer::getInstance()");
    if (m_instance == 0) {
        m_instance = new StreamPlayer(parent, name);
    }
    return m_instance;
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
StreamPlayer::play(QString mrl)
{
    qDebug("StreamPlayer::play()");
}


void
StreamPlayer::stop()
{
    qDebug("StreamPlayer::stop()");
}


void
StreamPlayer::showOSD(QString text, uint duration)
{
}


void
StreamPlayer::hideOSD()
{
}


QString
StreamPlayer::tvMRL(QString channelId)
{
    return QString("");
}
