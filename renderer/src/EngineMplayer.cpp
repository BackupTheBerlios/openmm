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

#include "enginemplayer.h"
#include <omm/debug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <cstdlib>
#include <sstream>

// TODO: comand line options for ommr
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
:_mplayerBin("mplayer"),
  // TODO: choose a unique name for the fifos so serveral instances of ommr can run.
_mplayerFifoIn("/tmp/ommr_mplayer_fifo_in"),
_mplayerFifoOut("/tmp/ommr_mplayer_fifo_out")
{
    _mplayerOptions = "-input file=" + _mplayerFifoIn + " -quiet -idle -nolirc -osdlevel 0 -softvol";
    // TODO: error handling after system call
    int err = mkfifo(_mplayerFifoIn.c_str(), S_IRUSR | S_IWUSR);
    err = mkfifo(_mplayerFifoOut.c_str(), S_IRUSR | S_IWUSR /*| O_NONBLOCK*/);
    _mplayerFifoStreamIn.open(_mplayerFifoIn.c_str());
    if (!_mplayerFifoStreamIn) {
        TRACE("MplayerThread::MplayerThread() error opening: %s", _mplayerFifoIn.c_str());
    }
    _mplayerFifoStreamOut.open(_mplayerFifoOut.c_str());
/*    if (!_mplayerFifoStreamOut) {
        TRACE("MplayerThread::MplayerThread() error opening: %s", _mplayerFifoOut.c_str());
    }*/
}


MplayerThread::~MplayerThread()
{
    TRACE("EngineMplayer::~MplayerThread()");
    _mplayerFifoStreamIn << "quit" << endl;
    // TODO: after quit wait(), waitpid() <sys/wait.h> for mplayer to finish
    //       then kill mplayer when it doesn't want to quit
    _mplayerFifoStreamIn.close();
    _mplayerFifoStreamOut.close();
    // TODO: error handling after system call
    int err = unlink(_mplayerFifoIn.c_str());
    err = unlink(_mplayerFifoOut.c_str());
}


void
MplayerThread::run()
{
    int err = system((_mplayerBin + " " + _mplayerOptions + " 1>" + _mplayerFifoOut).c_str());
    // now run() thread goes into main loop of mplayer, until mplayer returns after issuing "quit" command
    if (err) {
        // TODO: error handling after system call
    }
}


void
MplayerThread::command(const string& command)
{
    _mplayerFifoStreamIn << command << endl;
    TRACE("MplayerThread::command() COMMAND: %s", command.c_str());
}


int
MplayerThread::answer(string& ans, int timeout, string searchKey)
{
    string line;
    while (_mplayerFifoStreamOut.readLine(line, timeout)) {
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
: Engine()
{
    TRACE("EngineMplayer::EngineMplayer()");
    // start mplayer in idle-mode in a seperate thread
    _mplayerMutex = new JMutex();
    _mplayerThread.start();
}


EngineMplayer::~EngineMplayer()
{
}


void
EngineMplayer::setUri(string uri)
{
//     TRACE("EngineMplayer::setUri() to: %s", uri.c_str());
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    _uri = uri;
}


void
EngineMplayer::load()
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::load()");
    _mplayerThread.command("loadfile " + _uri);
    //     _mplayerFifoStreamIn << "brightness 100" << endl;
    // waiting max 10 secs for answer (AVTransport 1.0 specs give 30 secs)
    string ans;
    /*int err =*/ _mplayerThread.answer(ans, 10000, "Starting playback...");
}


// FIXME: pausing a live stream causes a buffer overflow on the server side
//        (for example vdr-streamdev-server)
// mplayer has no stop, so we try to emulate it somehow
void
EngineMplayer::stop()
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::stop()");
    _mplayerThread.command("pause");
//     _mplayerFifoStreamIn << "seek 0 2" << endl;  // makes some noise when pressing stop
//     _mplayerFifoStreamIn << "pause" << endl;
    // TODO: load a still picture instead of turning brightness down
//     _mplayerFifoStreamIn << "brightness 0" << endl;
}


void
EngineMplayer::pause()
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::pause()");
    _mplayerThread.command("pause");
}


void
EngineMplayer::seek(int seconds)
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::seek() to second: %i", seconds);
    // TODO: instead of stringstream, do a command(answer) << "seek " << seconds << " 2";
    stringstream s;
    s << "pausing_keep seek " << seconds << " 2";
    _mplayerThread.command(s.str());
}


void
EngineMplayer::setSpeed(int nom, int denom)
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    TRACE("EngineMplayer::setSpeed() to speed: %i/%i", nom, denom);
    // TODO: implement mplayer's speed setting
}


void
EngineMplayer::next()
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    TRACE("EngineMplayer::next()");
}


void
EngineMplayer::previous()
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    TRACE("EngineMplayer::previous()");
}


void
EngineMplayer::getPosition(float &seconds)
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::getPosition() pos: %f", seconds);
    _mplayerThread.command("get_time_pos");
    // waiting max 500 milli_sec for answer
    // we don't wait longer, because no answer (== timeout) indicates end of track
    // if just mplayer had something like a sensible protocol and could inform
    // us about end of track ...
    string ans;
    int err = _mplayerThread.answer(ans, 500);
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
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
//     TRACE("EngineMplayer::getLength()");
    _mplayerThread.command("get_time_length");
    // waiting max 2 secs for answer
    string ans;
    /*int err =*/ _mplayerThread.answer(ans, 2000);
    seconds = atof(ans.c_str());
}


void
EngineMplayer::setVolume(int /*channel*/, float vol)
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    TRACE("EngineMplayer::setVolume()");
    stringstream s;
    // insane mplayer API: need pausing_keep, otherwise mplayer starts
    // playing when changing the volume ...
    s << "pausing_keep volume " << vol << " 1";
    _mplayerThread.command(s.str());
}


void
EngineMplayer::getVolume(int /*channel*/, float &vol)
{
    JMutexLocker mplayerMutexLocker(_mplayerMutex);
    
    // FIXME: mplayer doesn't answer, when no stream is loaded
    //        and when stream is loaded:
    //        ANS_volume=0.000000
    //        even while hearing a sound ...
        
    TRACE("EngineMplayer::getVolume()");
    stringstream s;
    s << "get_property volume";
//     _mplayerThread.command(s.str());
//     string ans;
//     int err = _mplayerThread.answer(ans, 2000);
//     vol = atof(ans.c_str());
    vol = 0.6; // arbitrary value, for testing purposes, only.
}
