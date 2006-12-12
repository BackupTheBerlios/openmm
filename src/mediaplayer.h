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
#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include "streamplayer.h"
#include "page.h"
#include "title.h"


class StreamPlayer;

/**
Proxy interface for StreamPlayer (which is a singleton).
Adds functionality, which is special for the specific user-interface, e.g.
playing files, recordings, live-TV, have all different keys, OSD, ...

	@author Jörg Bakker <joerg@hakker.de>
*/
// TODO: add a method to switch three display states:
//       1. show StreamPlayer screen (with video)
//       2. show MediaPlayer screen for audio only visualizations
//       3. don't showUp() any screen to put only audio out (for warp effects in menus)

class MediaPlayer : public Page
{
public:
    MediaPlayer(string name);
    ~MediaPlayer();

    // Define the specific keys and resulting actions to control this media player.
    virtual void keyHandler(QKeyEvent *k)=0;

    // Needs to be reimplemented from base class, because it actually shows up the
    // StreamPlayer page and not the MediaPlayer page.
    virtual void showUp();

    // Actually returns if the StreamPlayer is running.
    bool isPlaying();
    //void setCurrent(Title *title) { m_current = title; }

public slots:
    void play(Title* title);
    //void start();  // starts the current title
    void stop();
    void showOsd(string text, uint duration);
    void hideOsd();

//private:
//    Title *m_current;
};

#endif
