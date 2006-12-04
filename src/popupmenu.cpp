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
#include "popupmenu.h"

PopupMenu::PopupMenu(QWidget *parent)
 : QObject(parent)
{
    qDebug("PopupMenu::PopupMenu()");
    m_parent = parent;
    m_title = 0;
    m_list = 0;
    m_popupMenu = new QPopupMenu();
}


PopupMenu::~PopupMenu()
{
}


void
PopupMenu::setParent(QWidget *parent)
{
    m_parent = parent;
}


void
PopupMenu::popup()
{
    if (m_parent) {
        QSize center = m_parent->size()/2 - m_popupMenu->sizeHint()/2;
        m_popupMenu->popup(m_parent->mapToGlobal(QPoint(center.width(), center.height())));
        m_popupMenu->setActiveItem(0);
    }
}


void
PopupMenu::popup(Title *title)
{
    m_title = title;
    popup();
}
