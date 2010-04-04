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
#include "qtpopupmenu.h"
#include "debug.h"

QtPopupMenu::QtPopupMenu(Page *parent)
{
    TRACE("QtPopupMenu::QtPopupMenu()");
    m_popupMenu = new QPopupMenu();
    m_parent = parent;

//     connect(m_popupMenu, SIGNAL(activated(int id)), this, SLOT(itemDispatcher(int id)));
}


QtPopupMenu::~QtPopupMenu()
{
}


string
QtPopupMenu::popup()
{
    TRACE("QtPopupMenu::popup()");
    if (m_parent) {
        QSize center = QSize(m_parent->width(), m_parent->height())/2 - m_popupMenu->sizeHint()/2;
        // TODO: synchronous execution has the disadvantage that secActivateItem() has no effect 
        //       (only after the menu is shown).
        m_popupMenu->setActiveItem(0);
        // execute the QPopupMenu synchronously (otherwise use popup() and the signal activated(int id))
        int id = m_popupMenu->exec(QPoint(center.width() + m_parent->globalPositionX(),
                                          center.height() + m_parent->globalPositionY()));
        return m_idMap[id];
    }
    return "";
}


extern "C" {
PopupMenuWidget* createPopupMenuWidget(Page *parent)
{
    return new QtPopupMenu(parent);
}
}
