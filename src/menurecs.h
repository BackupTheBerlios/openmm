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
#include "jamtv.h"
#include "tvrecplayer.h"

#include <qwidget.h>
#include <qlistview.h>


class GlobalKeyHandler;
class JAMTV;
class TvRecPlayer;
class TVChannel;
class TVRec;
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

    MenuRecs(JAM *controler, JAMTV *tv, TvRecPlayer *tvRecPlayer, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

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
    JAM *m_controler;
    JAMTV *m_tv;
    TvRecPlayer *m_tvRecPlayer;
    RecsEntry *m_cur;
    RecsEntry *m_selected;
};


class RecsEntry : public QListViewItem
{
public:
    RecsEntry(QListView *parent, TVRec *rec);

    ~RecsEntry();

    TVRec *getRec() { return m_rec; }

private:
    TVRec *m_rec;
};
#endif
