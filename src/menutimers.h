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
#ifndef MENUTIMERS_H
#define MENUTIMERS_H

#include "globalkeyhandler.h"

#include <qwidget.h>
#include <qlistview.h>

#include "jamtv.h"


class GlobalKeyHandler;
class JAMTV;
class TVChannel;
class TVTimer;
class TimersEntry;

/**
@author Jörg Bakker
*/
class MenuTimers : public Menu
{
    Q_OBJECT

public:
    enum TimersColumns {TimersColNum=0, TimersColChan, TimersColDay, TimersColStart, TimersColEnd, TimersColActive, TimersColPrio, TimersColTitle};
    enum TimersMenu {TimersMenuEdit=0, TimersMenuDelete};

    MenuTimers(JAM *controler, JAMTV *tv, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~MenuTimers();
    
    void showTimers();
    void action();
    void selectDefault();

protected slots:
    void showTimersMenu(QListViewItem *timer);
    void timerMenuEdit();
    void timerMenuDelete();

private:
    QListView *m_list;
    JAM *m_controler;
    JAMTV *m_tv;
    TimersEntry *m_cur;
    TimersEntry *m_selected;
};


class TimersEntry : public QListViewItem
{
public:
    TimersEntry(QListView *parent, TVTimer *timer);

    ~TimersEntry();

    TVTimer *getTimer() { return m_timer; }

private:
    TVTimer *m_timer;
};
#endif
