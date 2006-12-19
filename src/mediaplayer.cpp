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
#include "controler.h"
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
    Controler::instance()->streamPlayer()->setKeyHandler(this);
    Controler::instance()->streamPlayer()->showUp();
    Controler::instance()->pageStack()->setLogicalPage(this);
}


bool
MediaPlayer::isPlaying()
{
    return Controler::instance()->streamPlayer()->isPlaying();
}


void
MediaPlayer::play(Title* title)
{
    Controler::instance()->streamPlayer()->play(title);
}


void
MediaPlayer::stop()
{
    Controler::instance()->streamPlayer()->stop();
}


void
MediaPlayer::showOsd(string text, uint duration)
{
    Controler::instance()->streamPlayer()->showOsd(text, duration);
}


void
MediaPlayer::hideOsd()
{
    Controler::instance()->streamPlayer()->hideOsd();
}
