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
    m_mplayerOptions = "-input file=" + m_mplayerFifoIn + " -quiet";
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
}


EngineMplayer::~EngineMplayer()
{
    // TODO: this dtor is not called, why?
    TRACE("EngineMplayer::~EngineMplayer()");
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
    // TODO: better synchronize the threads to get rid of those Wait()'s
    Wait(100);
    Start();
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
    m_mrl = mrl;
}

void
EngineMplayer::stop()
{
    TRACE("EngineMplayer::stop()");
    m_mplayerFifoStreamIn << "quit" << endl;
}


void
EngineMplayer::getPosition(int &timesec, int &percent)
{
    TRACE("EngineMplayer::getPosition()");
    string mplayerOut;
    queryMplayer(mplayerOut, "get_time_pos");
    // TODO: replace 18 by size() ...
    timesec = atof(mplayerOut.substr(18).c_str());
    
    queryMplayer(mplayerOut, "get_percent_pos");
    TRACE("EngineMplayer::getPosition() percent_pos: %s", mplayerOut.c_str());
    percent = 99;
}

void
EngineMplayer::Run()
{
    // TODO: error handling after system call
    int err = system((m_mplayerBin + " " + m_mplayerOptions + " '" + m_mrl
                      + "' 1>" + m_mplayerFifoOut).c_str());
    //     m_mplayerFifoStream << "loadfile " << mrl.c_str() << endl;
}


void
EngineMplayer::queryMplayer(string &answer, const string &query)
{
    // TODO: lock the whole method, it will be called from position polling thread
    //       and other methods in the main thread.
//     m_queryMplayerMutex.Lock();
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
    answer = line;
    TRACE("EngineMplayer::queryMplayer() m_queryResult: %s", answer.c_str());
//     m_queryMplayerMutex.Unlock();
}

