/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
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
#include <platinum/NptThreads.h>

/**
	@author 
*/

// TODO: get rid of NPT_Thread here, we don't want to depend on it in jammrenginemplayer.so
// TODO: implementation with popen() instead of fifo
// TODO: how can we handle mplayer in slave mode on non-POSIX systems?
// TODO: renderer isn't shutdown properly since running mplayer in a thread and polling through fifo
class EngineMplayer : public Engine, NPT_Thread
{
public:
    EngineMplayer();

    ~EngineMplayer();

    virtual void next();
    virtual void pause();
    virtual void play();
    virtual void previous();
    virtual void seek(long seekval);
    virtual void setMrl(string mrl);
    virtual void stop();
    virtual long getPosition();

private:
    virtual void Run();
    string nextMplayerAnswer();
    
    string m_mrl;
    string m_mplayerBin;
    string m_mplayerOptions;
    string m_mplayerFifoIn;
    string m_mplayerFifoOut;
    fstream m_mplayerFifoStreamIn;
    fstream m_mplayerFifoStreamOut;
};

#endif
