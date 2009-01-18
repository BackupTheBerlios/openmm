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
#include <jamm/thread.h>

using namespace Jamm;

/**
	@author 
*/

// TODO: need to implement a signal for "end of track"

class MplayerThread : public JThread
{
public:
    MplayerThread();
    ~MplayerThread();
    
    /**
        string command(const string& command)
    
        issue comman to mplayer
        return the first occurence starting with "ANS_" in the response of mplayer
        otherwise return empty string
    
        command is a blocking method
    */
    void command(const string& command);
    
    /**
        string answer(int timeout, string searchKey="ANS_")
    
        wait timeout milli seconds for an answer while polling for any mplayer
        output every m_answerPollIntervall milli seconds
    */
    string answer(int timeout, string searchKey="ANS_");
    
private:
    virtual void run();
    
    string      m_mplayerBin;
    string      m_mplayerOptions;
    string      m_mplayerFifoIn;
    string      m_mplayerFifoOut;
    fstream     m_mplayerFifoStreamIn;
    fstream     m_mplayerFifoStreamOut;
    
    int         m_answerPollIntervall;
};



// TODO: implementation with popen() instead of fifo, or mkfifo (3) without call to system()
//       or use something like fstream fp("| cat | cat",ios::in|ios::out);
//       -> http://okmij.org/ftp/Communications.html
// TODO: how can we handle mplayer in slave mode on non-POSIX systems?
// TODO: renderer isn't shutdown properly since running mplayer in a thread and polling through fifo
//       "q" stops upnp, but jammr still runs and can be interrupted with ^C
// TODO: implement 2.2.26.CurrentTransportActions, to indicate for example seekable streams

// class EngineMplayer : public Engine, NPT_Thread
class EngineMplayer /*: public NPT_Thread*/
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
    string          m_uri;
    MplayerThread   m_mplayerThread;
    JMutex*         m_mplayerMutex;
};

#endif
