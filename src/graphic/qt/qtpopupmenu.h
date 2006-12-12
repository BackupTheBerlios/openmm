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
#ifndef QTPOPUPMENU_H
#define QTPOPUPMENU_H

#include "page.h"
#include "popupmenuwidget.h"

#include <qobject.h>
#include <qpopupmenu.h>
#include <string>
using namespace std;

/**
Qt implementation of PopupMenu.

	@author Jörg Bakker <joerg@hakker.de>
*/
class QtPopupMenu : public QObject, public PopupMenuWidget
{
    Q_OBJECT

public:
    QtPopupMenu(Page *parent);
    ~QtPopupMenu();

    virtual void popup();
    virtual void insertItem(string text, QObject *receiver, const char *member) 
            { m_popupMenu->insertItem(text, receiver, member); }

private:
    QPopupMenu   *m_popupMenu;
    Page         *m_parent;
};

#endif
