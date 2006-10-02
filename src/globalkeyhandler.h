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

#include <qobject.h>
#include <qevent.h>
#include <qapplication.h>

#include "jam.h"

class JAM;

/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class GlobalKeyHandler : public QObject
{
Q_OBJECT
public:
    GlobalKeyHandler(JAM *controler, QObject *parent = 0, const char *name = 0);

    ~GlobalKeyHandler();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    JAM *m_controler;
};

#endif
