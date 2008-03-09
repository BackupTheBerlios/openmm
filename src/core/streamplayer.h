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
#include "timer.h"

#include <string>
using namespace std;

class MediaPlayer;
class OsdTimer;

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
    void play();
    void stop();
    void pause();
    void forward();
    void rewind();
    void zoom(bool in = true);
    void left() { m_engine->left(); }
    void right() { m_engine->right(); }
    void up() { m_engine->up(); }
    void down() { m_engine->down(); }
    void select() { m_engine->select(); }

    // interface to StreamPlayerEngine
    void showOsd(string text, uint duration);
    void hideOsd() { m_engine->hideOsd(); }
    void switchAudioDevice(int maxDeviceNum) { m_engine->switchAudioDevice(maxDeviceNum); }

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
    void pauseStream() { m_engine->pauseStream(); }
    void forwardStream() { m_engine->forwardStream(); }
    void rewindStream() { m_engine->rewindStream(); }
    void zoomStream(bool in) { m_engine->zoomStream(in); }

    static StreamPlayer  *m_instance;
    bool                  m_isPlaying;
    Mrl                  *m_mrl;
    MediaPlayer          *m_keyHandler;
    EngineT               m_engineType;
    StreamPlayerEngine   *m_engine;
    SharedLibrary        *m_engineLib;
    StreamPlayerEngine* (*m_engineCtor)(Page *parent);
    OsdTimer             *m_osdTimer;
};


/* ---------------------------------------------------------------------------------- */

// #include <pthread.h>


class OsdTimer : public Timer
{
public:
    OsdTimer(StreamPlayer *streamPlayer, uint sec = 0);

//     void setTimeout(uint sec) { m_timout = sec; }
//     void start();

private:
    virtual void exec();

    StreamPlayer *m_streamPlayer;
//     uint m_timout;
//     pthread_t        m_thread;
//     pthread_attr_t   m_attr;
};

#endif
