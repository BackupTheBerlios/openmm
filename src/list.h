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
#ifndef LIST_H
#define LIST_H

#include "title.h"
#include "titlefilter.h"

#include <qobject.h>
#include <qptrlist.h>

class TitleFilter;

// TODO: list of unique Titles? -> only addTitle(), when Title is not in the list already.

/**
List to use for all types of Titles: timers, recordings, channels, files, ...

	@author Jörg Bakker <joerg@hakker.de>
*/
class List : public QObject
{
    Q_OBJECT

public:
    List();
    ~List();

    virtual void clear();
    virtual void update();  // only actually does somethin in subclass ListProxy (but is called from ListBrowser)
    Title* getTitle(int number);
    int findTitle(Title *title);
    void fill();
    int count();

signals:
    void changed();
    void pushTitle(Title*);
    void popTitle(Title*);

public slots:
    // use these methods for actively adding titles (for example timers)
    virtual void addTitle(Title *entry);
    virtual void delTitle(Title *entry);
    // use these methods for synchronization with a backend
    void addTitleEntry(Title *entry);
    void delTitleEntry(Title *entry);

protected:
    virtual void fillList();

//    void stepReset();  // resets the stepper.
//    Title *step();  // steps through the list. Returns 0 if no Title is left. Honors a TitleFilter, if set.

protected:
    typedef QPtrList<Title> ListT;

    ListT m_list;
//    int m_step;
};

#endif
