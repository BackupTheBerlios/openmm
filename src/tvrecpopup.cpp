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
#include "tvrecpopup.h"

TvRecPopup::TvRecPopup(TvRecPlayer *tvRecPlayer, QWidget *parent)
 : PopupMenu(parent)
{
    qDebug("TvRecPopup::TvRecPopup()");
    m_tvRecPlayer = tvRecPlayer;
    m_popupMenu->insertItem("Play", this, SLOT(play()));
    m_popupMenu->insertItem("Delete", this, SLOT(del()));
}


TvRecPopup::~TvRecPopup()
{
}


void
TvRecPopup::play()
{
    qDebug("TvRecPopup::play() title: %s", m_title->getText("Name").latin1());
    m_tvRecPlayer->play(m_title);
    m_tvRecPlayer->showUp();
}


void
TvRecPopup::del()
{
    qDebug("TvRecPopup::del() title: %s", m_title->getText("Name").latin1());
}
