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

QtPopupMenu::QtPopupMenu(Page *parent)
{
    qDebug("QtPopupMenu::QtPopupMenu()");
    m_popupMenu = new QPopupMenu();
    m_parent = parent;
}


QtPopupMenu::~QtPopupMenu()
{
}


void
QtPopupMenu::popup()
{
    if (m_parent) {
        QSize center = QSize(m_parent->width(), m_parent->height())/2 - m_popupMenu->sizeHint()/2;
        m_popupMenu->popup(QPoint(center.width() + m_parent->globalPositionX(),
                                  center.height() + m_parent->globalPositionY()));
        m_popupMenu->setActiveItem(0);
    }
}
