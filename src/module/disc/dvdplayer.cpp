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
#include "dvdplayer.h"
#include "controler.h"
#include "streamplayer.h"
#include "debug.h"


DvdPlayer::DvdPlayer()
 : MediaPlayer("DVD")
{
}


DvdPlayer::~DvdPlayer()
{
}


void
DvdPlayer::enterPage()
{
    TRACE("DvdPlayer::enterPage()");
    playDvd();
}


void
DvdPlayer::playDvd()
{
    //TRACE("DvdPlayer::startFile() playing: %s", ("file://" + mrl).c_str());
    Title *title = new Title("DVD", Title::DvdT);
    title->setMrl(new Mrl("dvd:/", ""));
    play(title);
}


bool
DvdPlayer::eventHandler(Event *e)
{
    TRACE("DvdPlayer::eventHandler()");
    switch (e->type()) {
    case Event::LeftE:
        StreamPlayer::instance()->left();
        break;
    case Event::RightE:
        StreamPlayer::instance()->right();
        break;
    case Event::UpE:
        StreamPlayer::instance()->up();
        break;
    case Event::DownE:
        StreamPlayer::instance()->down();
        break;
    default:
        // all other keys are returned to the GUI loop.
        return false;
    }
    return false;
}
