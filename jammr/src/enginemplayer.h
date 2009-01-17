/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
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

#ifndef ENGINEMPLAYER_H
#define ENGINEMPLAYER_H

#include "engine.h"

#include <fstream>
#include <jamm/upnpav.h>
#include <platinum/NptThreads.h>

/**
	@author 
*/

// TODO: get rid of NPT_Thread here, we don't want to depend on it in libjammr_engine_mplayer.so
// TODO: implementation with popen() instead of fifo, or mkfifo (3) without call to system()
// TODO: how can we handle mplayer in slave mode on non-POSIX systems?
// TODO: renderer isn't shutdown properly since running mplayer in a thread and polling through fifo
//       "q" stops upnp, but jammr still runs and can be interrupted with ^C
// TODO: don't inherit NPT_Thread, make it a member reference.
// TODO: implement 2.2.26.CurrentTransportActions, to indicate for example seekable streams

// class EngineMplayer : public Engine, NPT_Thread
class EngineMplayer : public NPT_Thread
{
public:
    EngineMplayer();

    ~EngineMplayer();

    virtual void setUri(string uri);
    // load uri and play from beginning
    virtual void load();
    // toggle pause
    virtual void pause();
    // stop playing
    virtual void stop();
    virtual void seek(int seconds);
    virtual void setSpeed(int nom, int denom);
    virtual void next();
    virtual void previous();
    virtual void getPosition(int &seconds);
    virtual void getLength(int &seconds);

private:
    virtual void Run();
    string queryMplayer(const string &query);
    
    string      m_uri;
    string      m_mplayerBin;
    string      m_mplayerOptions;
    string      m_mplayerFifoIn;
    string      m_mplayerFifoOut;
    fstream     m_mplayerFifoStreamIn;
    fstream     m_mplayerFifoStreamOut;
    
    NPT_Mutex   m_queryMplayerMutex;
};

#endif
