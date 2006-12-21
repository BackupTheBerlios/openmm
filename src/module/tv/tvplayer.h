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
#ifndef TVPLAYER_H
#define TVPLAYER_H

#include "mediaplayer.h"
#include "tvchannel.h"
#include "title.h"
#include "list.h"

/**
Plays Live-TV and offers the corresponding interactive key-functions and OSD.

	@author Jörg Bakker <joerg@hakker.de>
*/
class TvPlayer : public MediaPlayer
{
public:
    TvPlayer(List *channelList);
    ~TvPlayer();

    void startLiveTv();
    void stopLiveTv();

    Title* getCurrentChannelTitle();
    int getCurrentChannelNumber();
    void setCurrentChannel(int channelNumber);
    void setCurrentChannel(Title *channel);

protected:
    virtual bool eventHandler(Event *e);
    virtual void enterPage();

private:
    int m_currentChannel;
    List* m_channelList;
};

#endif
