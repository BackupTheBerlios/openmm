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
#ifndef TVRECPOPUP_H
#define TVRECPOPUP_H

#include "tvrecplayer.h"

#include "popupmenu.h"


/**
Implements the specialized popup menu for handling recordings.

	@author Jörg Bakker <joerg@hakker.de>
*/
class TvRecPopup : public PopupMenu
{
public:
    TvRecPopup(TvRecPlayer *tvRecPlayer, Page *parent = 0);
    ~TvRecPopup();

protected:
    virtual void itemDispatcher(string item);

private:
    void play();
    void del();

    TvRecPlayer *m_tvRecPlayer;
};

#endif
