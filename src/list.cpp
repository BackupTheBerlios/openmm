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
#include "debug.h"


List::List()
{
}


List::~List()
{
    TRACE("List::~List()");
    m_list.clear(); // TODO: does this delete all Titles?
}


void
List::clear()
{
    m_list.clear();
}


void
List::addTitle(Title *title)
{
    addTitleEntry(title);
}


void
List::delTitle(Title *title)
{
    delTitleEntry(title);
}


void
List::addTitleEntry(Title *entry)
{
    //TRACE("List::addTitle() with name: %s", entry->getText("Name").c_str());
    m_list.push_back(entry);
//     pushTitle(entry);
    pushTitle(entry);
}


void
List::delTitleEntry(Title *entry)
{
    ListT::iterator pos;
    pos = find(m_list.begin(), m_list.end(), entry);
    if (pos != m_list.end()) {
        m_list.erase(pos);
    }
//     popTitle(entry);
    popTitle(entry);
}


void
List::pushTitle(Title* title)
{
//     TRACE("List::pushTitle()");
    for (vector<List*>::iterator i = m_sinkList.begin(); i != m_sinkList.end(); ++i) {
        (*i)->addTitle(title);
    }
}


void
List::popTitle(Title* title)
{
    TRACE("List::popTitle()");
    for (vector<List*>::iterator i = m_sinkList.begin(); i != m_sinkList.end(); ++i) {
        (*i)->delTitle(title);
    }
}


void
List::addSink(List* sink)
{
    TRACE("List::link()");
    m_sinkList.push_back(sink);
}


void
List::delSink(List* sink)
{
    TRACE("List::link()");
    m_sinkList.erase(find(m_sinkList.begin(), m_sinkList.end(), sink));
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
    ListT::iterator pos = find(m_list.begin(), m_list.end(), title);
    return distance(m_list.begin(), pos);
}


int
List::count()
{
    return m_list.size();
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
