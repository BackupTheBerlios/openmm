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
#ifndef MODULE_H
#define MODULE_H

// #include <qobject.h>
#include <string>
using namespace std;

/**
An application-like collection, that defines a group of functionality, e.g. Television:
A StreamPlayer and some menues for channels, timers, ...

Nothing exciting happens here. Just to group somethings together and not spread it in
class Controler. Maybe for use as a plugin, later.

	@author Jörg Bakker <joerg@hakker.de>
*/
class Module //: public QObject
{
// Q_OBJECT
public:
    Module(string name);
    ~Module();

    string getName() { return m_name; }

private:
    string m_name;
};

#endif
