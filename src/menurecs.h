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
#ifndef MENURECS_H
#define MENURECS_H

#include "globalkeyhandler.h"
#include "tv.h"
#include "tvrecplayer.h"

#include <qwidget.h>
#include <qlistview.h>


class GlobalKeyHandler;
class Tv;
class TvRecPlayer;
class TvChannel;
class TvRec;
class RecsEntry;

/**
@author Jörg Bakker
*/
class MenuRecs : public Menu
{
    Q_OBJECT

public:
    enum RecsColumns {RecsColNum=0, RecsColDay, RecsColStart, RecsColTitle};
    enum RecsMenu {RecsMenuPlay=0, RecsMenuDelete};

    MenuRecs(Controler *controler, Tv *tv, TvRecPlayer *tvRecPlayer, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~MenuRecs();
    
    void showRecs();
    void action();
    void selectDefault();

protected slots:
    void showRecsMenu(QListViewItem *rec);
    void recMenuPlay();
    void recMenuDelete();

private:
    QListView *m_list;
    Controler *m_controler;
    Tv *m_tv;
    TvRecPlayer *m_tvRecPlayer;
    RecsEntry *m_cur;
    RecsEntry *m_selected;
};


class RecsEntry : public QListViewItem
{
public:
    RecsEntry(QListView *parent, TvRec *rec);

    ~RecsEntry();

    TvRec *getRec() { return m_rec; }

private:
    TvRec *m_rec;
};
#endif
