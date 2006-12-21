/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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

#include "tv.h"
#include "controler.h"
#include "tvplayer.h"
#include "tvchannel.h"
#include "tvtimer.h"
#include "tvtimerpopup.h"
#include "tvprogramfilter.h"
#include "tvrec.h"
#include "tvrecplayer.h"
#include "tvrecpopup.h"
#include "listbrowser.h"
#include "listproxy.h"
#include "listmanager.h"
#include "listcomposer.h"
#include "tvprogrambrowser.h"
#include "tvprogrampopup.h"
#include "vdr.h"


Tv::Tv() : Module("Television")
{
    // let's plug together our TV application ...

    Controler *controler = Controler::instance();

    // decide, which backend to use for managing meta data.
    Vdr *listManagerVdr = new Vdr();
    // listManagerVdr supplies timer lists, so go and get them ...
    ListProxy *timerList = new ListProxy(listManagerVdr, Title::TvTimerT);
    ListBrowser *timerListBrowser = new ListBrowser("Timers", "Id;Name;Channel;Day;Time Start;Time End", timerList);
    timerListBrowser->setPopupMenu(new TvTimerPopup(timerListBrowser));

    ListProxy *channelList = new ListProxy(listManagerVdr, Title::TvChannelT);
    ListProxy *programList = new ListProxy(listManagerVdr, Title::TvProgramT);
    ListComposer *programGuide = new ListComposer(channelList, programList, ListComposer::OuterJoin);
    TvProgramBrowser *programGuideBrowser = new TvProgramBrowser(programGuide);
    TvPlayer *channelPlayer = new TvPlayer(channelList);
    programGuideBrowser->setPopupMenu(new TvProgramPopup(timerList, channelPlayer, programGuideBrowser));
    programGuide->fill();  // TODO: first show main menu, then fill the program guide tables in an extra thread.
    TRACE("Tv::Tv() after programGuide->fill()");

    ListProxy *recList = new ListProxy(listManagerVdr, Title::TvRecT);
    ListBrowser *recListBrowser = new ListBrowser("Recordings", "Id;Name;Day;Start", recList);
    TvRecPlayer *recPlayer = new TvRecPlayer(recList);  // TODO: recList isn't really needed ...?
    recListBrowser->setPopupMenu(new TvRecPopup(recPlayer, recListBrowser));

    controler->mainMenuAddEntry(channelPlayer);
    controler->mainMenuAddEntry(programGuideBrowser);
    controler->mainMenuAddEntry(recListBrowser);
    controler->mainMenuAddEntry(timerListBrowser);
}


Tv::~Tv()
{
}
