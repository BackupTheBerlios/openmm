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
#ifndef STREAMPLAYERENGINE_H
#define STREAMPLAYERENGINE_H

#include "page.h"
#include "title.h"

#include <qstring.h>

class MediaPlayer;

/**
@author Jörg Bakker
*/
class StreamPlayerEngine
{
public:
    virtual void showOsd(QString text, uint duration) = 0;
    virtual void hideOsd() = 0;
    virtual void initStream() = 0;
    virtual void closeStream() = 0;
    virtual void playStream(Mrl *mrl) = 0;
    virtual void stopStream() = 0;
};

#endif
