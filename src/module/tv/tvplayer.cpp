/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#include "tvplayer.h"
#include "controler.h"
#include "debug.h"


TvPlayer::TvPlayer(List *channelList)
 : MediaPlayer("Live TV")
{
    m_channelList = channelList;
    m_currentChannel = 0;
}


TvPlayer::~TvPlayer()
{
}


void
TvPlayer::enterPage()
{
    TRACE("TvPlayer::enterPage()");
    //m_channelList->update();
    startLiveTv();
}


Title*
TvPlayer::getCurrentChannelTitle()
{
    TRACE("TvPlayer::getCurrentChannelTitle() returns: %p", m_channelList->getTitle(getCurrentChannelNumber()));
    return m_channelList->getTitle(getCurrentChannelNumber());
}


bool
TvPlayer::eventHandler(Event *e)
{
    TRACE("TvPlayer::eventHandler()");
    switch (e->type()) {
    case Event::EnterE:                          // info
        showOsd(getCurrentChannelTitle()->getText("Name"), 5);
        break;
    case Event::UpE:                                 // channel up
        setCurrentChannel(getCurrentChannelNumber() + 1);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::DownE:                               // channel down
        setCurrentChannel(getCurrentChannelNumber() - 1);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key1E:                               // channel 1
        setCurrentChannel(0);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key2E:                               // channel 2
        setCurrentChannel(1);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key3E:                               // channel 3
        setCurrentChannel(2);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key4E:                               // channel 4
        setCurrentChannel(3);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key5E:                               // channel 5
        setCurrentChannel(4);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key6E:                               // channel 6
        setCurrentChannel(5);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key7E:                               // channel 7
        setCurrentChannel(6);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key8E:                               // channel 8
        setCurrentChannel(7);
        stopLiveTv();
        startLiveTv();
        break;
    case Event::Key9E:                               // channel 9
        setCurrentChannel(8);
        stopLiveTv();
        startLiveTv();
        break;
    default:
        // all other keys are returned to the GUI loop.
        return false;
    }
    return true;
}


void
TvPlayer::startLiveTv()
{
    if (!getCurrentChannelTitle()) {
        return;
    }
    TRACE("TvPlayer::startLiveTv() on channel: %s", getCurrentChannelTitle()->getMrl()->getPath().c_str());
    //play("/data/video/001.vdr");
    play(getCurrentChannelTitle());
    showOsd(getCurrentChannelTitle()->getText("Name") + "   "
            , 5);  // TODO: need to get Program (current channel and time)
}


void
TvPlayer::stopLiveTv()
{
    TRACE("TvPlayer::stopLiveTv()");
    stop();
}


void
TvPlayer::setCurrentChannel(int channelNumber)
{
    m_currentChannel = channelNumber;
}


void
TvPlayer::setCurrentChannel(Title *channel)
{
    m_currentChannel = m_channelList->findTitle(channel);
}


int
TvPlayer::getCurrentChannelNumber()
{
    return m_currentChannel;
}
