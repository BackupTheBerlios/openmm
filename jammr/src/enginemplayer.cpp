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

// TODO: comand line options for jammr
// TODO: can we determin, which mime types can be played with mplayer?
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
:m_mplayerBin("mplayer"),
  // TODO: choose a unique name for the fifos so serveral instances of jammr can run.
  m_mplayerFifoIn("/tmp/jammr_mplayer_fifo_in"),
  m_mplayerFifoOut("/tmp/jammr_mplayer_fifo_out")
{
    TRACE("EngineMplayer::EngineMplayer()");
    m_mplayerOptions = "-input file=" + m_mplayerFifoIn + " -quiet -idle -nolirc -osdlevel 0";
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
EngineMplayer::setUri(string uri)
{
    TRACE("EngineMplayer::setUri() to: %s", uri.c_str());
    // TODO: lock m_mrl
    m_uri = uri;
}


void
EngineMplayer::load()
{
    TRACE("EngineMplayer::load()");
    
    m_mplayerFifoStreamIn << "loadfile " << m_uri.c_str() << endl;
    // TODO: check if media is loaded successfully and running instead of waiting 400 msec
    Wait(400);
//     m_mplayerFifoStreamIn << "brightness 100" << endl;
}


// mplayer has no stop, so we try to emulate it somehow
void
EngineMplayer::stop()
{
    TRACE("EngineMplayer::stop()");
//     m_mplayerFifoStreamIn << "seek 0 2" << endl;  // makes some noise when pressing stop
    m_mplayerFifoStreamIn << "pause" << endl;
    // TODO: load a still picture instead of turning brightness down
//     m_mplayerFifoStreamIn << "brightness 0" << endl;
}


void
EngineMplayer::pause()
{
    TRACE("EngineMplayer::pause()");
    m_mplayerFifoStreamIn << "pause" << endl;
}


void
EngineMplayer::seek(int seconds)
{
    TRACE("EngineMplayer::seek() to second: %i", seconds);
    m_mplayerFifoStreamIn << "seek " << seconds << " 2" << endl;
}


void
EngineMplayer::setSpeed(int nom, int denom)
{
    TRACE("EngineMplayer::setSpeed() to speed: %i/%i", nom, denom);
    // TODO: implement mplayer's speed setting
}


void
EngineMplayer::next()
{
    TRACE("EngineMplayer::next()");
}


void
EngineMplayer::previous()
{
    TRACE("EngineMplayer::previous()");
}


void
EngineMplayer::getPosition(int &seconds)
{
    TRACE("EngineMplayer::getPosition()");
    // TODO: need a pausing_keep here?
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

