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
#ifndef GLOBALKEYHANDLER_H
#define GLOBALKEYHANDLER_H

#include "controler.h"
#include "page.h"

// #include <qobject.h>
// #include <qevent.h>

#include <vector>

class Controler;

/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class GlobalKeyHandler //: public QObject
{
//     Q_OBJECT

public:
    static GlobalKeyHandler *instance();
    void attach(Page *page);
    void detach(Page *page);
// TODO: implement toolkit independet events.
//     bool notify(QEvent *e);

protected:
//    GlobalKeyHandler();
//    ~GlobalKeyHandler();

// TODO: implement toolkit independet events.
//     bool eventFilter(QObject *o, QEvent *e);

private:
    static GlobalKeyHandler *m_instance;
    vector<Page*> m_observer;
};

#endif
