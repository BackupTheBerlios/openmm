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


MediaPlayer::MediaPlayer()
 : Page()
{
}


MediaPlayer::~MediaPlayer()
{
}


void
MediaPlayer::showUp()
{
    qDebug("MediaPlayer::showUp()");
    enterPage();
    Controler::streamPlayer()->setKeyHandler(this);
    Controler::streamPlayer()->showUp();
}


bool
MediaPlayer::isPlaying()
{
    return Controler::streamPlayer()->isPlaying();
}


void
MediaPlayer::play(Title* title)
{
    Controler::streamPlayer()->play(title);
}


void
MediaPlayer::stop()
{
    Controler::streamPlayer()->stop();
}


void
MediaPlayer::showOsd(QString text, uint duration)
{
    Controler::streamPlayer()->showOsd(text, duration);
}


void
MediaPlayer::hideOsd()
{
    Controler::streamPlayer()->hideOsd();
}
