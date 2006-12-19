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
#ifndef TVRECPLAYER_H
#define TVRECPLAYER_H

#include "mediaplayer.h"
#include "list.h"

// #include <qobject.h>


/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class TvRecPlayer : /*public QObject,*/ public MediaPlayer
{
//     Q_OBJECT

public:
    TvRecPlayer(List *recList);
    ~TvRecPlayer();

    void startRec(Title *title);

protected:
//     bool eventHandler(QEvent *e);
    void enterPage();

private:
    List *m_recList;
};

#endif