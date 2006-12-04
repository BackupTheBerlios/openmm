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
#include "listproxy.h"

ListProxy::ListProxy(ListManager *listManager, Title::TitleT type)
 : List()
{
    m_listManager = listManager;
    m_type = type;

    if (m_listManager->pushUpdates()) {
        fill();  // Initially fill the list with Titles. Later listManager will push updates.
        connect(listManager, SIGNAL(pushTitle(Title*)), this, SLOT(addTitleEntry(Title*)));
        connect(listManager, SIGNAL(popTitle(Title*)), this, SLOT(delTitleEntry(Title*)));
    }
}


ListProxy::~ListProxy()
{
}


void
ListProxy::update()
{
    if (!m_listManager->pushUpdates()) {
        // no updates are pushed from ListManager, so we need to pull them ourselves.
        fill();
    }
}


void
ListProxy::fillList()
{
    m_listManager->fill(this, m_type);
}
