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
#include "tvprogrampopup.h"
#include "tvtimer.h"
#include "titlepair.h"
#include "debug.h"


TvProgramPopup::TvProgramPopup(List *timerList, TvPlayer *tvPlayer, Page *parent)
 : PopupMenu(parent)
{
    TRACE("TvProgramPopup::TvProgramPopup()");
    m_timerList = timerList;
    m_tvPlayer = tvPlayer;
    insertItem("Timer", this, SLOT(addTimer()));
    insertItem("Switch", this, SLOT(switchChannel()));
    insertItem("Show", this, SLOT(showChannel()));
}


TvProgramPopup::~TvProgramPopup()
{
}


void
TvProgramPopup::addTimer()
{
    if ((m_title->getType() == Title::TitlePairT)) {
        Title *l = ((TitlePair*)m_title)->getLeft();
        Title *r = ((TitlePair*)m_title)->getRight();
        if (l->getType() == Title::TvChannelT && r->getType() == Title::TvProgramT) {
            // now we know, that a TitlePair of (TvChannel, TvProgram) is passed to TvProgramPopup on selection.
            m_timerList->addTitle(new TvTimer((TvChannel*)l, (TvProgram*)r));
        }
    }
}


void
TvProgramPopup::switchChannel()
{
    TRACE("TvProgramPopup::switchChannel() title: %s", m_title->getText("Name").c_str());
    // TODO: switch to the selected channel.
    if ((m_title->getType() == Title::TitlePairT)) {
        Title *l = ((TitlePair*)m_title)->getLeft();
        if (l->getType() == Title::TvChannelT) {
            m_tvPlayer->setCurrentChannel(l);
            m_tvPlayer->showUp();
        }
    }
}

void
TvProgramPopup::showChannel()
{
    TRACE("TvProgramPopup::showChannel() title: %s", m_title->getText("Name").c_str());
    // TODO: show a Page with the EPG of this channel.
}
