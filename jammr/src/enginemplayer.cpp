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
#include "enginemplayer.h"
#include <jamm/debug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <cstdlib>
#include <sstream>

// TODO: comand line options for jammr
// TODO: can we determine, which mime types can be played with mplayer?
// TODO: check for presence of mplayer
// TODO: check compatibility with mplayer version or with commands available (mplayer -input cmdlist)


// Usefull mplayer commandline options:
// -wid <window ID> (also see -guiwid) (X11, OpenGL and DirectX only)
// -ontop 
// -stop-xscreensaver 
// -nolirc
// -really-quiet
// -fixed-vo
// -fs
// -softvol

MplayerThread::MplayerThread()
:m_mplayerBin("mplayer"),
  // TODO: choose a unique name for the fifos so serveral instances of jammr can run.
m_mplayerFifoIn("/tmp/jammr_mplayer_fifo_in"),
m_mplayerFifoOut("/tmp/jammr_mplayer_fifo_out")
{
    m_mplayerOptions = "-input file=" + m_mplayerFifoIn + " -quiet -idle -nolirc -osdlevel 0 -softvol -fs";
    // TODO: error handling after system call
    int err = mkfifo(m_mplayerFifoIn.c_str(), S_IRUSR | S_IWUSR);
    err = mkfifo(m_mplayerFifoOut.c_str(), S_IRUSR | S_IWUSR /*| O_NONBLOCK*/);
    m_mplayerFifoStreamIn.open(m_mplayerFifoIn.c_str());
    if (!m_mplayerFifoStreamIn) {
        TRACE("MplayerThread::MplayerThread() error opening: %s", m_mplayerFifoIn.c_str());
    }
    m_mplayerFifoStreamOut.open(m_mplayerFifoOut.c_str());
/*    if (!m_mplayerFifoStreamOut) {
        TRACE("MplayerThread::MplayerThread() error opening: %s", m_mplayerFifoOut.c_str());
    }*/
}


MplayerThread::~MplayerThread()
{
    TRACE("EngineMplayer::~MplayerThread()");
    m_mplayerFifoStreamIn << "quit" << endl;
    // TODO: after quit wait(), waitpid() <sys/wait.h> for mplayer to finish
    //       then kill mplayer when it doesn't want to quit
    m_mplayerFifoStreamIn.close();
    m_mplayerFifoStreamOut.close();
    // TODO: error handling after system call
    int err = unlink(m_mplayerFifoIn.c_str());
    err = unlink(m_mplayerFifoOut.c_str());
}


void
MplayerThread::run()
{
    int err = system((m_mplayerBin + " " + m_mplayerOptions + " 1>" + m_mplayerFifoOut).c_str());
    // now run() thread goes into main loop of mplayer, until mplayer returns after issuing "quit" command
    if (err) {
        // TODO: error handling after system call
    }
}


void
MplayerThread::command(const string& command)
{
    m_mplayerFifoStreamIn << command << endl;
    TRACE("MplayerThread::command() COMMAND: %s", command.c_str());
}


int
MplayerThread::answer(string& ans, int timeout, string searchKey)
{
    string line;
    while (m_mplayerFifoStreamOut.readLine(line, timeout)) {
        TRACE("MplayerThread::answer(): %s", line.substr(0, line.length()-1).c_str());
        // mplayer writes answer to stdout starting with "ANS_"
        if (line.find(searchKey) == 0) {
            if (searchKey == "ANS_") {
                ans = line.substr(line.find("=")+1);
            }
            else {
                ans = line;
            }
            TRACE("MplayerThread::answer(): Found");
            return Found;
        }
    }
    TRACE("MplayerThread::answer(): not Found / Timeout");
    return Timeout;
}

// Usefull mplayer commands:
// use_master
// vo_fullscreen, get_vo_fullscreen
// vo_ontop
// volume
// speed_set, speed_mult, speed_incr
// get_property, set_property
// loadfile, loadlist
// get_audio_bitrate, get_video_bitrate
// dvdnav
// [brightness|contrast|gamma|hue|saturation] <value> [abs]


EngineMplayer::EngineMplayer()
// : Engine(),
{
    TRACE("EngineMplayer::EngineMplayer()");
    // start mplayer in idle-mode in a seperate thread
    m_mplayerMutex = new JMutex();
    m_mplayerThread.start();
}


EngineMplayer::~EngineMplayer()
{
}


void
EngineMplayer::setUri(string uri)
{
//     TRACE("EngineMplayer::setUri() to: %s", uri.c_str());
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    m_uri = uri;
}


void
EngineMplayer::load()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::load()");
    m_mplayerThread.command("loadfile " + m_uri);
    //     m_mplayerFifoStreamIn << "brightness 100" << endl;
    // waiting max 10 secs for answer (AVTransport 1.0 specs give 30 secs)
    string ans;
    /*int err =*/ m_mplayerThread.answer(ans, 10000, "Starting playback...");
}


// FIXME: pausing a live stream causes a buffer overflow on the server side
//        (for example vdr-streamdev-server)
// mplayer has no stop, so we try to emulate it somehow
void
EngineMplayer::stop()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::stop()");
    m_mplayerThread.command("pause");
//     m_mplayerFifoStreamIn << "seek 0 2" << endl;  // makes some noise when pressing stop
//     m_mplayerFifoStreamIn << "pause" << endl;
    // TODO: load a still picture instead of turning brightness down
//     m_mplayerFifoStreamIn << "brightness 0" << endl;
}


void
EngineMplayer::pause()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::pause()");
    m_mplayerThread.command("pause");
}


void
EngineMplayer::seek(int seconds)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::seek() to second: %i", seconds);
    // TODO: instead of stringstream, do a command(answer) << "seek " << seconds << " 2";
    stringstream s;
    s << "pausing_keep seek " << seconds << " 2";
    m_mplayerThread.command(s.str());
}


void
EngineMplayer::setSpeed(int nom, int denom)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::setSpeed() to speed: %i/%i", nom, denom);
    // TODO: implement mplayer's speed setting
}


void
EngineMplayer::next()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::next()");
}


void
EngineMplayer::previous()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::previous()");
}


void
EngineMplayer::getPosition(float &seconds)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::getPosition() pos: %f", seconds);
    m_mplayerThread.command("get_time_pos");
    // waiting max 500 milli_sec for answer
    // we don't wait longer, because no answer (== timeout) indicates end of track
    // if just mplayer had something like a sensible protocol and could inform
    // us about end of track ...
    string ans;
    int err = m_mplayerThread.answer(ans, 500);
    if (err == MplayerThread::Found) {
        seconds = atof(ans.c_str());
    }
    else if (err == MplayerThread::Timeout) {
        endOfTrack.emitSignal();
    }
}


void
EngineMplayer::getLength(float &seconds)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
//     TRACE("EngineMplayer::getLength()");
    m_mplayerThread.command("get_time_length");
    // waiting max 2 secs for answer
    string ans;
    /*int err =*/ m_mplayerThread.answer(ans, 2000);
    seconds = atof(ans.c_str());
}


void
EngineMplayer::setVolume(int /*channel*/, float vol)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::setVolume()");
    stringstream s;
    // insane mplayer API: need pausing_keep, otherwise mplayer starts
    // playing when changing the volume ...
    s << "pausing_keep volume " << vol << " 1";
    m_mplayerThread.command(s.str());
}


void
EngineMplayer::getVolume(int /*channel*/, float &vol)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    // FIXME: mplayer doesn't answer, when no stream is loaded
    //        and when stream is loaded:
    //        ANS_volume=0.000000
    //        even while hearing a sound ...
        
    TRACE("EngineMplayer::getVolume()");
    stringstream s;
    s << "get_property volume";
//     m_mplayerThread.command(s.str());
//     string ans;
//     int err = m_mplayerThread.answer(ans, 2000);
//     vol = atof(ans.c_str());
    vol = 0.6; // arbitrary value, for testing purposes, only.
}
