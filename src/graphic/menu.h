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
#ifndef MENU_H
#define MENU_H

#include "page.h"
#include "menuwidget.h"
#include "debug.h"

#include <string>
using namespace std;

/**
@author Jörg Bakker
*/
class Menu : public Page
{
public:
    Menu(string name);
    Menu() { TRACE("Menu::Menu() - nothing to do"); }
    ~Menu();

    virtual void addEntry(Page *page) { m_menuWidget->addEntry(page) ;}
    virtual void setDefaultEntry(Page *page) { m_menuWidget->setDefaultEntry(page) ;}
    virtual void setMenuName(string name) { m_menuWidget->setMenuName(name) ;}
    virtual void enterPage() { m_menuWidget->enterPage(); }
    virtual void exitPage() { TRACE("Menu::exitPage()"); }

protected:
    MenuWidget *m_menuWidget;
};

#endif
