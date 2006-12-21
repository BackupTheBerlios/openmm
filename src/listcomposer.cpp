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
{
    TRACE("ListComposer::ListComposer()");
    m_left = left;
    m_right = right;
    m_filter = 0;
    m_join = join;

//     m_left->addSink(this);
}


ListComposer::~ListComposer()
{
}


void
ListComposer::addTitle(Title *title)
{
    addTitleLeft(title);  // TODO: we don't know, if title is pushed from m_left or m_right ...
                          //       this works at the moment, because addTitleRight() is not implemented, yet.
}


void
ListComposer::delTitle(Title *title)
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
    TRACE("ListComposer::addTitleLeft() with name: %s", entry->getText("Name").c_str());
    // find a matching ident in the right Titles (columns of type Identifier with same name).
    bool match = false;
    for (int i = 0; i < m_right->count(); i++) {
        if (entry->match(m_right->getTitle(i))) {
//             TRACE("ListComposer::addTitleLeft() inner join");
            match = true;
            TitlePair *newPair = new TitlePair(entry, m_right->getTitle(i));
            m_list.push_back(newPair);
//             TRACE("ListComposer::addTitleLeft() inner join done.");
        }
    }
//     if (m_join == OuterJoin && !match) {
    if (m_join == OuterJoin) {  // always push a right-empty entry into the result
//         TRACE("ListComposer::addTitleLeft() outer join");
        // no match on the right side, but nevertheless make an outer join.
        TitlePair *newPair = new TitlePair(entry, 0);
        m_list.push_back(newPair);
    }
}


void
ListComposer::pushFiltered()
{
    TRACE("ListComposer::pushFiltered()");
    Title *lastLeftTitle = 0;
    for (int i = 0; i < count(); i++) {
        TitlePair *currentTitle = (TitlePair*)getTitle(i);
        if (!currentTitle || currentTitle->getType() != Title::TitlePairT) {
            continue;
        }
        Title *currentLeftTitle = currentTitle->getLeft();
        Title *currentRightTitle = currentTitle->getRight();
//         TRACE("ListComposer::pushFiltered() for Title: %s", currentTitle->getText("Name").c_str());
        if (m_filter) {  // only push a filtered Title to the outside (for example to a TitleBrowser)
//             TRACE("ListComposer::pushFiltered() filter currentRightTitle: %p", currentRightTitle);
            if (currentRightTitle && m_filter->pass(currentRightTitle)) {
                if ((currentLeftTitle != lastLeftTitle)) {
                    pushTitle(currentTitle);
                }
//                 TRACE("ListComposer::pushFiltered() lastLeft, currentLeft: %p, %p", lastLeftTitle, currentLeftTitle);
                lastLeftTitle = currentLeftTitle;
            }
            else if(!currentRightTitle && (lastLeftTitle != currentLeftTitle)) {
                    pushTitle(currentTitle);
            }
        }
        else {  // no filter, we can push all Titles
            pushTitle(currentTitle);
        }
    }
    TRACE("ListComposer::pushFiltered() done");
}


void
ListComposer::addTitleRight(Title *entry)
{
}


void
ListComposer::fillList()
{
    TRACE("ListComposer::fillList() fill left List.");
    m_left->fill();
    TRACE("ListComposer::fillList() fill right List.");
    m_right->fill();
    for (int i = 0; i < m_left->count(); i++) {
        addTitleLeft(m_left->getTitle(i));
    }
    pushFiltered();
}
