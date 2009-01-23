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

MplayerThread::MplayerThread()
:m_mplayerBin("mplayer"),
  // TODO: choose a unique name for the fifos so serveral instances of jammr can run.
m_mplayerFifoIn("/tmp/jammr_mplayer_fifo_in"),
m_mplayerFifoOut("/tmp/jammr_mplayer_fifo_out"),
m_answerPollIntervall(10)
{
    m_mplayerOptions = "-input file=" + m_mplayerFifoIn + " -quiet -idle -nolirc -osdlevel 0";
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
    TRACE("MplayerThread::command() command: %s", command.c_str());
}


// TODO: no answer isn't handled correctly:
/*
    MplayerThread::command() command: get_time_pos
    MplayerThread::answer(): ANS_TIME_POSITION=129.2
    MplayerThread::answer(): returns Found
    MplayerThread::command() command: get_time_pos
    MplayerThread::answer(): ANS_TIME_POSITION=130.2
    MplayerThread::answer(): returns Found
    MplayerThread::command() command: get_time_pos
    MplayerThread::answer():
    EngineMplayer::~MplayerThread()
    Segmentation fault
*/


// bool
// MplayerThread::readLine(string& line, int timeout)
// {
//     const int lineBufLen = 256;
//     char buf[lineBufLen];
//     line = "";
//     std::streamsize bytesRead;
//     
//     while (timeout > 0 && (bytesRead = m_mplayerFifoStreamOut.readsome(buf, lineBufLen))) {
//         line += string(buf, bytesRead);
//     }
//     return false;
// }


int
MplayerThread::answer(string& ans, int /*timeout*/, string searchKey)
{
    string line;
//     const int lineBufLen = 256;
//     char buf[lineBufLen];
//     bool eot = false;
//     int to = timeout;
/*    m_mplayerFifoStreamOut.peek();
    if (m_mplayerFifoStreamOut.eof()) {
        TRACE("MplayerThread::answer(): EOF");
    }*/
//     while (timeout > 0 && getline(m_mplayerFifoStreamOut, line)) {
//     while (timeout > 0 && m_mplayerFifoStreamOut.readsome(buf, lineBufLen)) {
    while (m_mplayerFifoStreamOut.readLine(line)) {
        TRACE("MplayerThread::answer(): %s", line.c_str());
        // mplayer writes answer to stdout starting with "ANS_"
//         string line(buf);
        if (/*line.length() &&*/ line.find(searchKey) == 0) {
            if (searchKey == "ANS_") {
                ans = line.substr(line.find("=")+1);
            }
            else {
                ans = line;
            }
            TRACE("MplayerThread::answer(): returns Found");
            return Found;
        }
        // TODO: check all answers for an mplayer message that indicates end of track
//         else if (line.length()) {
//             TRACE("MplayerThread::answer(): %s", line.c_str());
/*            if (line.find("Cannot") == 0) {
                eot = true;
                TRACE("MplayerThread::answer() set eot: %i", eot);
            }*/
//         }
//         usleep(m_answerPollIntervall*1000);
//         TRACE("MplayerThread::answer() timeout: %i", timeout);
//         timeout -= m_answerPollIntervall;
    }
//     TRACE("MplayerThread::answer() eot: %i", eot);
/*    if (eot) {
        return EndOfTrack;
    }
    else {
        return Timeout;
    }*/
    TRACE("MplayerThread::answer(): returns Timeout");
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
    
    TRACE("EngineMplayer::load()");
    m_mplayerThread.command("loadfile " + m_uri);
    //     m_mplayerFifoStreamIn << "brightness 100" << endl;
    // waiting max 10 secs for answer (AVTransport 1.0 specs give 30 secs)
    string ans;
    m_mplayerThread.answer(ans, 10000, "Starting playback...");
}


// mplayer has no stop, so we try to emulate it somehow
void
EngineMplayer::stop()
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::stop()");
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
    
    TRACE("EngineMplayer::pause()");
    m_mplayerThread.command("pause");
}


void
EngineMplayer::seek(int seconds)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::seek() to second: %i", seconds);
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
    // waiting max 1 sec for answer (AVTransport 1.0 specs give 30 secs)
    string ans;
    int err = m_mplayerThread.answer(ans, 1000);
    if (err == MplayerThread::Found) {
        seconds = atof(ans.c_str());
    }
    else if (err == MplayerThread::Timeout) {
        endOfTrack.emitSignal();
    }
    // end of track detection doesn't work this way:
/*    EngineMplayer::getPosition() cur: 175.800003 last: 174.800003
    EngineMplayer::getPosition() cur: 176.800003 last: 175.800003
    EngineMplayer::getPosition() cur: 177.800003 last: 176.800003
    EngineMplayer::getPosition() cur: 178.800003 last: 177.800003
    MplayerThread::answer(): Cannot sync MAD frame
    MplayerThread::answer(): Cannot sync MAD frame
    MplayerThread::answer(): Cannot sync MAD frame*/
}


void
EngineMplayer::getLength(float &seconds)
{
    JMutexLocker mplayerMutexLocker(m_mplayerMutex);
    
    TRACE("EngineMplayer::getLength()");
    m_mplayerThread.command("get_time_length");
    // waiting max 2 secs for answer (AVTransport 1.0 specs give 30 secs)
    // this reads max 200 lines from mplayer out
    // with m_answerPollIntervall set to 10 milli_sec
    string ans;
    seconds = m_mplayerThread.answer(ans, 2000);
    seconds = atof(ans.c_str());
}
