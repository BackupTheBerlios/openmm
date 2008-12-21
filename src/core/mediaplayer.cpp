/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
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
#include "mediaplayer.h"
#include "streamplayer.h"
#include "debug.h"


MediaPlayer::MediaPlayer(string name)
 : Page(name)
{
}


MediaPlayer::~MediaPlayer()
{
}


void
MediaPlayer::showUp()
{
    TRACE("MediaPlayer::showUp()");
    enterPage();
    StreamPlayer::instance()->setKeyHandler(this);
    StreamPlayer::instance()->showUp();
}


bool
MediaPlayer::isPlaying()
{
    return StreamPlayer::instance()->isPlaying();
}


void
MediaPlayer::play(Title* title)
{
    StreamPlayer::instance()->play(title);
}


void
MediaPlayer::stop()
{
    StreamPlayer::instance()->stop();
}


void
MediaPlayer::showOsd(string text, unsigned int duration)
{
    StreamPlayer::instance()->showOsd(text, duration);
}


void
MediaPlayer::hideOsd()
{
    StreamPlayer::instance()->hideOsd();
}
