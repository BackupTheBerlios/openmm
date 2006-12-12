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
#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include "list.h"

#include <qobject.h>
#include <string>
using namespace std;

/**
Produces and also deletes titles which it gets from servers on the net or local filesystems or just about anywhere.

	@author Jörg Bakker <joerg@hakker.de>
*/
class ListManager : public QObject
{
    Q_OBJECT

public:
    ListManager();
    ~ListManager();

    // TODO: if ListManager is not a singleton, we can have exactly one instance for each List object (and TitleT type).
    //       This would avoid the List *list (and TitleT type) parameter in each method call.
    //       -> problem is "chicken and egg": ListManager is paramater of List ctor, so List cannot be part of ctor of ListManager.
    //          could solve this with a manage(List*) method, which is called from the ctor of List: m_listManager->manage(this).

    // TODO: pushUpdates() varies the behaviour of the class (could also implement this in derived classes ...)
    //       right now there are two modes: push and pull. More modes could be usefull (time-triggered pull, ...)
    //       these modes could be different for different TitleT types.
    virtual bool pushUpdates() = 0;
    virtual void fill(List *list, Title::TitleT type) = 0;
    //virtual void addTitle(List *list, Title *title) = 0;
    //virtual void delTitle(List *list, Title *title) = 0;

    virtual void addProxyTitle(Title *title) = 0;
    virtual void delProxyTitle(Title *title) = 0;

signals:
    // These signals are only triggered, when in push-mode (see pushUpdates()).
    void pushTitle(Title *title);
    void popTitle(Title *title);
};

#endif
