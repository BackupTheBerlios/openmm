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
#include "enginemplayer.h"
#include "debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <cstdlib>

EngineMplayer::EngineMplayer()
: Engine(),
  m_mplayerBin("mplayer"),
  // TODO: choose a unique name for the fifos so serveral instances of jammr can be run.
  m_mplayerFifoIn("/tmp/jammr_mplayer_fifo_in"),
  m_mplayerFifoOut("/tmp/jammr_mplayer_fifo_out")
{
    TRACE("EngineMplayer::EngineMplayer()");
//     m_mplayerOptions = "-fs -input file=" + m_mplayerFifoIn + " -quiet";  // full screen
    // also usefull options:
    // -wid <window ID> (also see -guiwid) (X11, OpenGL and DirectX only)
    // -ontop 
    // -stop-xscreensaver 
    // -nolirc
    // -really-quiet
    // -fixed-vo
    // -fs
    m_mplayerOptions = "-input file=" + m_mplayerFifoIn + " -quiet -idle -fs";
    // TODO: error handling after system call
    int err = mkfifo(m_mplayerFifoIn.c_str(), S_IRUSR | S_IWUSR);
    err = mkfifo(m_mplayerFifoOut.c_str(), S_IRUSR | S_IWUSR);
    m_mplayerFifoStreamIn.open(m_mplayerFifoIn.c_str());
    if (!m_mplayerFifoStreamIn) {
        TRACE("EngineMplayer::EngineMplayer() error opening: %s", m_mplayerFifoIn.c_str());
    }
    m_mplayerFifoStreamOut.open(m_mplayerFifoOut.c_str());
    if (!m_mplayerFifoStreamOut) {
        TRACE("EngineMplayer::EngineMplayer() error opening: %s", m_mplayerFifoOut.c_str());
    }
    // start mplayer in idle-mode in a seperate thread
    Start();
}


EngineMplayer::~EngineMplayer()
{
    // TODO: dtor is never called, why?
    TRACE("EngineMplayer::~EngineMplayer()");
    m_mplayerFifoStreamIn << "quit" << endl;
    m_mplayerFifoStreamIn.close();
    m_mplayerFifoStreamOut.close();
    // TODO: error handling after system call
    int err = unlink(m_mplayerFifoIn.c_str());
    err = unlink(m_mplayerFifoOut.c_str());
}


void
EngineMplayer::next()
{
    TRACE("EngineMplayer::next()");
}

void
EngineMplayer::pause()
{
    TRACE("EngineMplayer::pause()");
    m_mplayerFifoStreamIn << "pause" << endl;
}

void
EngineMplayer::play()
{
    TRACE("EngineMplayer::play()");
//     m_mplayerFifoStreamIn << "play" << endl;
}

void
EngineMplayer::previous()
{
    TRACE("EngineMplayer::previous()");
}

void
EngineMplayer::seek(long seekval)
{
    TRACE("EngineMplayer::seek() to second: %i", seekval);
    m_mplayerFifoStreamIn << "seek " << seekval << " 2" << endl;
}

void
EngineMplayer::setMrl(string mrl)
{
    TRACE("EngineMplayer::setMrl() to: %s", mrl.c_str());
    // TODO: lock m_mrl
//     m_mrl = mrl;
    // TODO: should be "pause loadfile" and Play actually starts playing
    m_mplayerFifoStreamIn << "loadfile " << mrl.c_str() << endl;
}

void
EngineMplayer::stop()
{
    TRACE("EngineMplayer::stop()");
    // TODO: this doesn't work properly
    m_mplayerFifoStreamIn << "pause seek 0 1" << endl;
}


void
EngineMplayer::getPosition(int &seconds)
{
    TRACE("EngineMplayer::getPosition()");
    seconds = atof(queryMplayer("get_time_pos").c_str());
}


void
EngineMplayer::getLength(int &seconds)
{
    TRACE("EngineMplayer::getLength()");
    seconds = atof(queryMplayer("get_time_length").c_str());
}


void
EngineMplayer::Run()
{
/*    int err = system((m_mplayerBin + " " + m_mplayerOptions + " '" + m_mrl
                      + "' 1>" + m_mplayerFifoOut).c_str());*/
    int err = system((m_mplayerBin + " " + m_mplayerOptions + " 1>" + m_mplayerFifoOut).c_str());
    if (err) {
        // TODO: error handling after system call
    }
}

// also usefull:
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


string
EngineMplayer::queryMplayer(const string &query)
{
    // TODO: lock the whole method, it will be called from position polling thread
    //       and other methods in the main thread.
//     m_queryMplayerMutex.Lock();
//     m_mplayerFifoStreamIn << "pausing_keep " << query << endl;
    m_mplayerFifoStreamIn << query << endl;
    TRACE("EngineMplayer::queryMplayer() query: %s", query.c_str());
    string line;
    // TODO: race condition may occur when reading before mplayer can answer?
    //       or does getline block, until a line appears on the stream?
    //       more likely: it only hangs, when mplayer is not running
    while (getline(m_mplayerFifoStreamOut, line)) {
//         TRACE("EngineMplayer::queryMplayer(): %s", line.c_str());
        // mplayer writes answer to stdout starting with "ANS_" >> result;
        if (line.substr(0, 4) == "ANS_") {
                break;
        }
    }
    string answer = line.substr(line.find("=")+1);
    TRACE("EngineMplayer::queryMplayer() m_queryResult: %s", answer.c_str());
//     m_queryMplayerMutex.Unlock();
    return answer;
}

