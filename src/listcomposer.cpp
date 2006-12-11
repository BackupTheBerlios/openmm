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
#include "listcomposer.h"
#include "titlepair.h"
#include "debug.h"


ListComposer::ListComposer(List *left, List *right, JoinT join)
 : List()
{
    TRACE("ListComposer::ListComposer()");
    m_left = left;
    m_right = right;
    m_filter = 0;
    m_join = join;

    connect(left, SIGNAL(pushTitle(Title*)), this, SLOT(addTitleLeft(Title*)));
    connect(right, SIGNAL(pushTitle(Title*)), this, SLOT(addTitleRight(Title*)));
}


ListComposer::~ListComposer()
{
}


// try to match with all available idents of the new entry Title.
// check for corresponding Ident columns
// what, if more than one Ident column corresponds?
//       1. name them explicitely? (does this make much sense in inhomogenous lists?)
//       2. match all? sounds reasonable: if a Title is matched with itself, this condition holds only for itself.
//       3. match only one (idents are priority ordered)? (seems rather arbitrary)
void
ListComposer::addTitleLeft(Title *entry)
{
    //TRACE("ListComposer::addTitleLeft() with name: %s", entry->getText("Name").latin1());
    // find a matching ident in the right Titles (columns of type Identifier with same name).
    bool match = false;
    for (int i = 0; i < m_right->count(); i++) {
        if (entry->match(m_right->getTitle(i))) {
            match = true;
            TitlePair *newPair = new TitlePair(entry, m_right->getTitle(i));
            m_list.append(newPair);
        }
    }
    if (m_join == OuterJoin && !match) {
        // no match on the right side, but nevertheless make an outer join.
        TitlePair *newPair = new TitlePair(entry, 0);
        m_list.append(newPair);
    }
}


// TODO: channels disappear, that have partial program entries,
//       while channels with no entries go through the filter.
//       -> all channels should always appear on the left side.

// TODO: channels appear double, because of errors in the EPG data
//       for example: N24, two programs are overlapping on the same channel.

void
ListComposer::pushFiltered()
{
    TRACE("ListComposer::pushFiltered()");
    for (int i = 0; i < count(); i++) {
        if (m_filter) {  // only push a filtered Title to the outside (for example to a TitleBrowser)
            if (!((TitlePair*)getTitle(i))->getLeft() 
                || !((TitlePair*)getTitle(i))->getRight()
                || m_filter->pass(getTitle(i))) {
                emit pushTitle(getTitle(i));
            }
        }
        else {
            emit pushTitle(getTitle(i));
        }
    }
}


void
ListComposer::addTitleRight(Title *entry)
{
/*
    // TODO: eliminate identical code shared with addTitleLeft().
    for (int i = 0; i < m_left->count(); i++) {
        if (entry->match(m_left->getTitle(i))) {
            TitlePair *newPair = new TitlePair(m_left->getTitle(i), entry);
            m_list.append(newPair);

            if (m_filter) {  // only push a filtered Title to the outside (for example to a TitleBrowser)
                if (m_filter->pass(newPair)) {
                    emit pushTitle(newPair);
                }
            }
            else {
                emit pushTitle(newPair);
            }
        }
    }
*/
}


void
ListComposer::fillList()
{
    m_left->fill();
    m_right->fill();
    for (int i = 0; i < m_left->count(); i++) {
        addTitleLeft(m_left->getTitle(i));
    }
    pushFiltered();
}
