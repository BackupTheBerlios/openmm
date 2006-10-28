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
#include "menutimers.h"

MenuTimers::MenuTimers(Controler *controler, Tv *tv, GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Timers";
    m_controler = controler;
    m_tv = tv;
//    TimersEventFilter *timerskeyh = new TimersEventFilter(this);
    m_list = new QListView(this);
    m_list->setAllColumnsShowFocus(true);
    m_list->installEventFilter(keyh);
//    m_list->installEventFilter(timerskeyh);
    m_list->addColumn("Nr.");
    m_list->addColumn("Channel");
    m_list->addColumn("Day");
    m_list->addColumn("Start");
    m_list->addColumn("End");
    m_list->addColumn("Active");
    m_list->addColumn("Prio");
    m_list->addColumn("Title");

    m_cur = 0;

    connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showTimersMenu(QListViewItem*)));
}


MenuTimers::~MenuTimers()
{
}


// FIX: this is called four times at startup.
void
MenuTimers::showTimers()
{
    qDebug("MenuTimers::showTimers()");

    m_tv->updateTimers();
    m_list->clear();
    for (int i = 0; i < m_tv->getTimerCount(); i++)
    {
        new TimersEntry(m_list, m_tv->getTimerPointer(i));
    }
}


void
MenuTimers::action()
{
    showTimers();
}


void
MenuTimers::selectDefault()
{
    m_list->setCurrentItem(m_cur);
    m_list->setSelected(m_cur, true);
}


void
MenuTimers::showTimersMenu(QListViewItem *timer)
{
    qDebug("MenuTimers::showTimersMenu() on entry %s", timer->text(TimersColTitle).latin1());
    m_selected = (TimersEntry*)timer;
    QPopupMenu *p = new QPopupMenu();
    p->insertItem("Edit", this, SLOT(timerMenuEdit()));
    p->insertItem("Delete", this, SLOT(timerMenuDelete()));
    QSize center = m_list->size()/2 - p->sizeHint()/2;
    p->popup(m_list->mapToGlobal(QPoint(center.width(), center.height())));
    p->setActiveItem(0);
}


void
MenuTimers::timerMenuEdit()
{
    // TODO: show screen with fields to edit the timer entries.
}


void
MenuTimers::timerMenuDelete()
{
    m_tv->delTimer(m_selected->getTimer());
    showTimers();
}



TimersEntry::TimersEntry(QListView *parent, TvTimer *timer)
 : QListViewItem(parent, "", "", "", "", "", "", "", "")
{
    m_timer = timer;
    setText(MenuTimers::TimersColNum, m_timer->getId());
    setText(MenuTimers::TimersColChan, m_timer->getChannelId());
    setText(MenuTimers::TimersColDay, m_timer->getDayStr());
    setText(MenuTimers::TimersColStart, m_timer->getStartStr());
    setText(MenuTimers::TimersColEnd, m_timer->getEndStr());
    setText(MenuTimers::TimersColActive, m_timer->getActiveStr());
    setText(MenuTimers::TimersColPrio, m_timer->getPrioStr());
    setText(MenuTimers::TimersColTitle, m_timer->getTitle());
}


TimersEntry::~TimersEntry()
{
}

