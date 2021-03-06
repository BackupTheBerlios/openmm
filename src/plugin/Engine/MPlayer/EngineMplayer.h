/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef ENGINEMPLAYER_H
#define ENGINEMPLAYER_H

#include "engine.h"

#include <fstream>
#include <omm/thread.h>
#include <omm/signode.h>
#include <omm/iodevice.h>

using namespace Omm;

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
        output every _answerPollIntervall milli_seconds
    */
    enum mplayerError {Found=0, Timeout=1, EndOfTrack=2};
    int answer(string& ans, int timeout, string searchKey="ANS_");
    
private:
    virtual void run();
    
    string      _mplayerBin;
    string      _mplayerOptions;
    string      _mplayerFifoIn;
    string      _mplayerFifoOut;
    fstream     _mplayerFifoStreamIn;
    JIoDevice   _mplayerFifoStreamOut;
};



// TODO: implementation with popen() instead of fifo, or mkfifo (3) without call to system()
//       or use something like fstream fp("| cat | cat",ios::in|ios::out);
//       -> http://okmij.org/ftp/Communications.html
// TODO: how can we handle mplayer in slave mode on non-POSIX systems?
// TODO: renderer isn't shutdown properly since running mplayer in a thread and polling through fifo
//       "q" stops upnp, but ommr still runs and can be interrupted with ^C
// TODO: implement 2.2.26.CurrentTransportActions, to indicate for example seekable streams

class EngineMplayer : public Engine
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
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);
    
    
private:
    string          _uri;
    MplayerThread   _mplayerThread;
    JMutex*         _mplayerMutex;
};

#endif
