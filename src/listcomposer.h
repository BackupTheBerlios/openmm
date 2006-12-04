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
#ifndef LISTCOMPOSER_H
#define LISTCOMPOSER_H

#include "list.h"
#include "titlefilter.h"


/**
ListComposer looks like a List from the outside, and for example can be viewed with a ListBrowser.

In addition to a normal List, ListComposer is able to perform some very simple relational database functionality, like joining and selecting (=filtering out rows) of Lists. The big difference compared to real relational table algebra is, that Lists can hold Titles of differend kind, that is, they can have different columns, whereas tables in a database are homogenous in terms of columns (each row in a table has the same number and type of columns).


	@author Jörg Bakker <joerg@hakker.de>
*/

class ListComposer : public List
{
public:
    enum JoinT {InnerJoin, OuterJoin};

    ListComposer(List *left, List *right = 0, JoinT join = InnerJoin);
    ~ListComposer();

    void setFilter(TitleFilter *filter) { m_filter = filter; }

public slots:
    void addTitleLeft(Title *entry);
    void addTitleRight(Title *entry);
    void pushFiltered();

protected:
    virtual void fillList();

private:
    List *m_left;
    List *m_right;
    TitleFilter *m_filter;
    JoinT m_join;
};

#endif
