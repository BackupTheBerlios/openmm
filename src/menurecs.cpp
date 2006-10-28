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
#include <qlayout.h>
#include <qdatetime.h>
#include <qpopupmenu.h>
#include "menurecs.h"

MenuRecs::MenuRecs(Controler *controler, Tv *tv, TvRecPlayer *tvRecPlayer, GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Recordings";
    m_controler = controler;
    m_tv = tv;
    m_tvRecPlayer = tvRecPlayer;
    m_list = new QListView(this);
    m_list->setAllColumnsShowFocus(true);
    m_list->installEventFilter(keyh);
    m_list->addColumn("Nr.");
    m_list->addColumn("Day");
    m_list->addColumn("Start");
    m_list->addColumn("Title");

    m_cur = 0;

    connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showRecsMenu(QListViewItem*)));
}


MenuRecs::~MenuRecs()
{
}


// FIX: this is called four times at startup.
void
MenuRecs::showRecs()
{
    qDebug("MenuRecs::showRecs()");

    m_tv->updateRecs();
    m_list->clear();
    for (int i = 0; i < m_tv->getRecCount(); i++)
    {
        new RecsEntry(m_list, m_tv->getRecPointer(i));
    }
}


void
MenuRecs::action()
{
    showRecs();
}


void
MenuRecs::selectDefault()
{
    m_list->setCurrentItem(m_cur);
    m_list->setSelected(m_cur, true);
}


void
MenuRecs::showRecsMenu(QListViewItem *rec)
{
    qDebug("MenuRecs::showRecsMenu() on entry %s", rec->text(RecsColTitle).latin1());
    m_selected = (RecsEntry*)rec;
    QPopupMenu *p = new QPopupMenu();
    p->insertItem("Play", this, SLOT(recMenuPlay()));
    p->insertItem("Delete", this, SLOT(recMenuDelete()));
    QSize center = m_list->size()/2 - p->sizeHint()/2;
    p->popup(m_list->mapToGlobal(QPoint(center.width(), center.height())));
    p->setActiveItem(0);
}


void
MenuRecs::recMenuPlay()
{
    m_tv->stopLiveTv();
    m_tvRecPlayer->startFile(m_selected->getRec()->getFirstRec());
    m_controler->showMenu(m_tvRecPlayer);
    // FIX: video is visible only in Tv widget.
    //m_controler->showMenu(m_tv);
}


void
MenuRecs::recMenuDelete()
{
    m_tv->delRec(m_selected->getRec());
    showRecs();
}



RecsEntry::RecsEntry(QListView *parent, TvRec *rec)
 : QListViewItem(parent, "", "", "", "")
{
    m_rec = rec;
    setText(MenuRecs::RecsColNum, m_rec->getIdStr());
    setText(MenuRecs::RecsColDay, m_rec->getDayStr());
    setText(MenuRecs::RecsColStart, m_rec->getStartStr());
    setText(MenuRecs::RecsColTitle, m_rec->getTitle());
}


RecsEntry::~RecsEntry()
{
}

