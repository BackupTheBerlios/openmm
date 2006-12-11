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
#include "list.h"


List::List()
 : QObject()
{
    m_list.setAutoDelete(true);
}


List::~List()
{
}


void
List::clear()
{
    m_list.clear();
}


void
List::update()
{
//    emit changed();
}


void
List::addTitle(Title *entry)
{
    addTitleEntry(entry);
}


void
List::delTitle(Title *entry)
{
    delTitleEntry(entry);
}


void
List::addTitleEntry(Title *entry)
{
    //TRACE("List::addTitle() with name: %s", entry->getText("Name").latin1());
    m_list.append(entry);
    emit pushTitle(entry);
}


void
List::delTitleEntry(Title *entry)
{
    m_list.remove(entry);
    emit popTitle(entry);
}


Title*
List::getTitle(int number)
{
    //TRACE("List::getTitle()");
    return m_list.at(number);
}


int
List::findTitle(Title *title)
{
    return m_list.find(title);
}


int
List::count()
{
    return m_list.count();
}


void
List::fill()
{
    clear();
    fillList();
}


void
List::fillList()
{
}

/*
void
List::setFilter(TitleFilter *filter)
{
    m_filter = filter;
//    m_filter->setList(this);
    stepReset();
}


void
List::stepReset()
{
    m_step = 0;
}


// TODO: a Title could be added to the list asynchronously. This probably breaks the iteration process.
Title*
List::step()
{
    if (m_filter) {
        while(m_step < count() && !m_filter->pass(m_list.at(m_step))) {
            m_step++;
        }
    }
    else if (m_step < count()) {
        m_step++;
    }
    else {
        return 0;
    }
    return m_list.at(m_step);
}
*/
