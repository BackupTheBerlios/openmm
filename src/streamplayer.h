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
#ifndef STREAMPLAYER_H
#define STREAMPLAYER_H

#include "page.h"
#include "title.h"
#include "event.h"
#include "mediaplayer.h"
#include "streamplayerengine.h"
#include "sharedlibrary.h"

#include <string>
using namespace std;

class MediaPlayer;


/**
@author Jörg Bakker
*/
class StreamPlayer : public Page
{
public:
    enum EngineT {EngineXine};

    static StreamPlayer *instance();
    void setEngine(EngineT engine);
    void setKeyHandler(MediaPlayer *player);
    bool isPlaying() { return m_isPlaying; };
    void play(Title *title);
    void stop();

    // interface to StreamPlayerEngine
    void showOsd(string text, uint duration) { m_engine->showOsd(text, duration); }
    void hideOsd() { m_engine->hideOsd(); }

protected:
    StreamPlayer();
    ~StreamPlayer();

    virtual bool eventHandler(Event *e);
    virtual void exitPage();

private:
    // interface to StreamPlayerEngine
    void initStream() { m_engine->initStream(); }
    void closeStream() { m_engine->closeStream(); }
    void playStream(Mrl *mrl) { m_engine->playStream(mrl); }
    void stopStream() { m_engine->stopStream(); }

    static StreamPlayer  *m_instance;
    bool                  m_isPlaying;
    MediaPlayer          *m_keyHandler;
    EngineT               m_engineType;
    StreamPlayerEngine   *m_engine;
    SharedLibrary        *m_engineLib;
    StreamPlayerEngine* (*m_engineCtor)(Page *parent);
};

#endif
