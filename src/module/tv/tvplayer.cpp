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
    //setNameP("Live TV");
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


void
TvPlayer::keyHandler(QKeyEvent *k)
{
    TRACE("TvPlayer::keyHandler()");
    switch (k->key()) {
        case Qt::Key_I:                               // info
        case Qt::Key_Return:
        case Qt::Key_Enter:
            //showOsd(m_channelList->getChannelName(getCurrentChannelNumber()) + "   " +
            //     getCurrentChannelPointer()->getCurrentEpgEntry()->getTitle(), 5000);
            return;
        case Qt::Key_Up:                              // channel up
            setCurrentChannel(getCurrentChannelNumber() + 1);
            break;
        case Qt::Key_Down:                            // channel down
            setCurrentChannel(getCurrentChannelNumber() - 1);
            break;
        case Qt::Key_1:                               // channel 1
            setCurrentChannel(0);
            break;
        case Qt::Key_2:                               // channel 2
            setCurrentChannel(1);
            break;
        case Qt::Key_3:                               // channel 3
            setCurrentChannel(2);
            break;
        case Qt::Key_4:                               // channel 4
            setCurrentChannel(3);
            break;
        case Qt::Key_5:                               // channel 5
            setCurrentChannel(4);
            break;
        case Qt::Key_6:                               // channel 6
            setCurrentChannel(5);
            break;
        case Qt::Key_7:                               // channel 7
            setCurrentChannel(6);
            break;
        case Qt::Key_8:                               // channel 8
            setCurrentChannel(7);
            break;
        case Qt::Key_9:                               // channel 9
            setCurrentChannel(8);
            break;
    }
    stopLiveTv();
    startLiveTv();
}


void
TvPlayer::startLiveTv()
{
    TRACE("TvPlayer::startLiveTv() on channel: %s", getCurrentChannelTitle()->getMrl()->getPath().latin1());
    //play("/data/video/001.vdr");
    play(getCurrentChannelTitle());
    showOsd(getCurrentChannelTitle()->getText("Name") + "   "
            , 5000);  // TODO: need to get Program (current channel and time)
}


void
TvPlayer::stopLiveTv()
{
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
