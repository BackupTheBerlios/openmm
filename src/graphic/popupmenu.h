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
#ifndef POPUPMENU_H
#define POPUPMENU_H

#include "title.h"
#include "list.h"
#include "page.h"
#include "popupmenuwidget.h"
#include "debug.h"

#include <string>
#include <map>
using namespace std;

/**
Just a simple popup menu.

	@author Jörg Bakker <joerg@hakker.de>
*/
class PopupMenu
{
public:
    PopupMenu(Page *parent);
    PopupMenu() { TRACE("PopupMenu::PopupMenu() - nothing to do"); }
    virtual ~PopupMenu();

    void popup()                                  { itemDispatcher(m_popupMenuWidget->popup()); }
    void popup(Title *title);
    void insertItem(string text)                  { m_popupMenuWidget->insertItem(text); }
    // TODO: itemDispatcher is a bit ugly, but simple, I may change this ...

// TODO: connect the slots to the event handling
//     virtual void insertItem(string text, QObject *receiver, const char *member) 
//             { m_popupMenuWidget->insertItem(text, receiver, member); }

//    virtual void insertItem(string text, unary_function<void, void>) {}

    void setParent(Page *parent)                  { m_parent = parent; }
    Page* getParent()                             { return m_parent; }
    void setList(List *list)                      { m_list = list; }

protected:
    virtual void itemDispatcher(string item) = 0;
    Title      *m_title;
    List       *m_list;

private:
    PopupMenuWidget *m_popupMenuWidget;
    Page            *m_parent;
};

#endif
