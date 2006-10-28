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
#include "menumain.h"

MenuMain::MenuMain(Controler *controler, GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Main Menu";
    m_controler = controler;
    m_list = new QListView(this);
    m_list->installEventFilter(keyh);
    m_list->addColumn(m_name);
    m_defaultItem = 0;
    connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(mainMenuItemSelected(QListViewItem*)));
}


MenuMain::~MenuMain()
{
}


void
MenuMain::mainMenuItemSelected(QListViewItem* i)
{
    qDebug("MenuMain::mainMenuItemSelected()");
    Menu *m = m_itemDict[i];
    m_controler->showMenu(m);
}


void
MenuMain::registerMenuItem(Menu* menu, bool isDefault)
{
    QListViewItem *i = new QListViewItem(m_list, menu->getName(), "");
    if (isDefault)
        m_defaultItem = i;
    m_itemDict.insert(i, menu);
}


void
MenuMain::selectDefault()
{
    m_list->setCurrentItem(m_defaultItem);
    m_list->setSelected(m_defaultItem, true);
}

void
MenuMain::action()
{
/*
    Do nothing in the top level menu. Only real menu entries have an action.
*/ 
}
