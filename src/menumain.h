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
#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "globalkeyhandler.h"

#include <qwidget.h>
#include <qlistview.h>
#include <qptrdict.h>

#include "menu.h"
#include "jam.h"


class GlobalKeyHandler;
class JAM;

/**
@author Jörg Bakker
*/
class MenuMain : public Menu
{
    Q_OBJECT

public:
    MenuMain(JAM *controler, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~MenuMain();

    void registerMenuItem(Menu* menu, bool isDefault=false);
    void action();
    void selectDefault();

public slots:
    void menuItemSelected(QListViewItem* i);

private:
    QListView *m_list;
    QListViewItem *m_defaultItem;
    JAM *m_controler;
    QPtrDict<Menu> m_itemDict;
};

#endif
