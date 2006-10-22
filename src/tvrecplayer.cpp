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
#include "tvrecplayer.h"

TvRecPlayer::TvRecPlayer(GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    m_streamPlayer = new JAMStreamPlayerXine(this);
    installEventFilter(keyh);
    m_streamPlayer->setFocus();
    m_isPlaying = false;
}


TvRecPlayer::~TvRecPlayer()
{
}


void
TvRecPlayer::action()
{
}


void
TvRecPlayer::selectDefault()
{
}


void
TvRecPlayer::startFile(QString mrl)
{
    m_isPlaying = true;
    //m_streamPlayer->play("file:///data/video/001.vdr");
    qDebug("TvRecPlayer::startFile() playing: %s", ("file://" + mrl).ascii());
    m_streamPlayer->play(("file://" + mrl).utf8());
}
