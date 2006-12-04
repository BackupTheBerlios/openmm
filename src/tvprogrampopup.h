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
#ifndef TVPROGRAMPOPUP_H
#define TVPROGRAMPOPUP_H

#include "list.h"
#include "tvplayer.h"

#include <popupmenu.h>

/**
Implements the specialized popup menu for handling programs and channels.

	@author Jörg Bakker <joerg@hakker.de>
*/
class TvProgramPopup : public PopupMenu
{
    Q_OBJECT

public:
    TvProgramPopup(List *timerList, TvPlayer *tvPlayer, QWidget *parent = 0);
    ~TvProgramPopup();

private slots:
    void addTimer();
    void switchChannel();
    void showChannel();

private:
    List *m_timerList;
    TvPlayer *m_tvPlayer;
};

#endif
