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

#include <cstdlib>

EngineMplayer::EngineMplayer()
: Engine(),
  m_mplayerBin("mplayer"),
  m_mplayerFifoIn("/tmp/jammr_mplayer_fifo_in"),
  m_mplayerFifoOut("/tmp/jammr_mplayer_fifo_out")
{
    TRACE("EngineMplayer::EngineMplayer()");
    m_mplayerOptions = "-fs -input file=" + m_mplayerFifoIn + " -quiet";
    // TODO: error handling after system call
    int err = system(("mkfifo " + m_mplayerFifoIn).c_str());
    err = system(("mkfifo " + m_mplayerFifoOut).c_str());
    m_mplayerFifoStreamIn.open(m_mplayerFifoIn.c_str());
    m_mplayerFifoStreamOut.open(m_mplayerFifoOut.c_str());
}


EngineMplayer::~EngineMplayer()
{
    // TODO: this dtor is not called, why?
    TRACE("EngineMplayer::~EngineMplayer()");
    m_mplayerFifoStreamIn.close();
    m_mplayerFifoStreamOut.close();
    // TODO: error handling after system call
    int err = system(("rm " + m_mplayerFifoIn + " " + m_mplayerFifoOut).c_str());
}


void EngineMplayer::next()
{
    TRACE("EngineMplayer::next()");
}

void EngineMplayer::pause()
{
    TRACE("EngineMplayer::pause()");
    m_mplayerFifoStreamIn << "pause" << endl;
}

void EngineMplayer::play()
{
    TRACE("EngineMplayer::play()");
    
    Start();
    
//     m_mplayerFifoStreamIn << "play" << endl;
    // start mplayer asynchronously
/*    int pid;
    if (fork() == 0) {
        // fork() also starts the position polling thread, so it is run in both processes ...
        // TODO: error handling after system call
        int err = system((m_mplayerBin + " " + m_mplayerOptions + " '" + m_mrl
                      + "' 1>" + m_mplayerFifoOut).c_str());
    }*/
//     system((m_mplayerBin + " " + m_mplayerOptions + " '" + mrl + "' 2&>1 > /dev/null").c_str());
//     m_mplayerFifoStream << "loadfile " << mrl.c_str() << endl;
}

void EngineMplayer::previous()
{
    TRACE("EngineMplayer::previous()");
}

void EngineMplayer::seek(long seekval)
{
    TRACE("EngineMplayer::seek() to second: %i", seekval);
    m_mplayerFifoStreamIn << "seek " << seekval << " 2" << endl;
}

void EngineMplayer::setMrl(string mrl)
{
    TRACE("EngineMplayer::setMrl() to: %s", mrl.c_str());
    m_mrl = mrl;
}

void EngineMplayer::stop()
{
    TRACE("EngineMplayer::stop()");
    m_mplayerFifoStreamIn << "quit" << endl;
}


long
EngineMplayer::getPosition()
{
    string mplayerOut;
    TRACE("EngineMplayer::getPosition()");
    m_mplayerFifoStreamIn << "get_time_pos" << endl;
    mplayerOut = nextMplayerAnswer();
    // TODO: pick long integer out of string "ANS_TIME_POSITION=72.3"
    TRACE("EngineMplayer::getPosition() %s", mplayerOut.c_str());
    return 0;
}

void
EngineMplayer::Run()
{
    // TODO: error handling after system call
    int err = system((m_mplayerBin + " " + m_mplayerOptions + " '" + m_mrl
                      + "' 1>" + m_mplayerFifoOut).c_str());
}


string
EngineMplayer::nextMplayerAnswer()
{
    // mplayer writes answer to stdout starting with "ANS_" >> result;
    string line;
    do {
        m_mplayerFifoStreamOut >> line;
    }
    while(line.find("ANS_", 0) == string::npos);
    return line;
}

