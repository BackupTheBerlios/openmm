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
#include <jamm/thread.h>
#include <jamm/signode.h>
#include <jamm/iodevice.h>

using namespace Jamm;

/**
	@author 
*/

// FIXME: mplayer is too slow at loading a vdr stream:
//        mplayer http://192.168.178.22:3000/PES/S19.2E-1-1089-12003
//        vlc is much faster ...

// FIXME: mplayer can't determine length / end of vdr recording
//        vlc can seek in streamed vdr recordings ...

// TODO: three test files have no duration (everytime.mpg, pepsy piercing.mpeg, vorteile.mpg)
//       the wmv has duration but is not seekable (UFO-Reportage.wmv)

// TODO: catch seeking beyond end of track

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
    
        wait timeout milli_seconds for an answer while polling for any mplayer
        output every m_answerPollIntervall milli_seconds
    */
    enum mplayerError {Found=0, Timeout=1, EndOfTrack=2};
    int answer(string& ans, int timeout, string searchKey="ANS_");
    
private:
    virtual void run();
    
    string      m_mplayerBin;
    string      m_mplayerOptions;
    string      m_mplayerFifoIn;
    string      m_mplayerFifoOut;
    fstream     m_mplayerFifoStreamIn;
    JIoDevice   m_mplayerFifoStreamOut;
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

    /*
      AVTransport
    */
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
    virtual void getPosition(float &seconds);
    virtual void getLength(float &seconds);
    
    JSignal endOfTrack;
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);
    
    
private:
    string          m_uri;
    MplayerThread   m_mplayerThread;
    JMutex*         m_mplayerMutex;
};

#endif
