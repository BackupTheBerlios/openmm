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

#include <string>
using namespace std;

class MediaPlayer;

/**
@author Jörg Bakker
*/
class StreamPlayerEngine
{
public:
    //virtual ~StreamPlayerEngine() = 0;
    virtual void showOsd(string text) = 0;
    virtual void hideOsd() = 0;
    virtual void initStream() = 0;
    virtual void closeStream() = 0;
    virtual void playStream(Mrl *mrl) = 0;
    virtual void stopStream() = 0;
    virtual void pauseStream() = 0;
    virtual void forwardStream() = 0;
    virtual void rewindStream() = 0;
    virtual void zoomStream(bool in) = 0;
    virtual void left() = 0;
    virtual void right() = 0;
    virtual void up() = 0;
    virtual void down() = 0;
    virtual void select() = 0;
};

#endif
