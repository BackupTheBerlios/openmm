/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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


#ifndef JAM_H
#define JAM_H

#include "globalkeyhandler.h"

#include <qmainwindow.h>
#include <qwidgetstack.h>

#include "menu.h"
#include "menumain.h"
#include "jamtv.h"
#include "menuproguide.h"

class GlobalKeyHandler;
class MenuMain;
class JAMTV;
class MenuProGuide;


class JAM: public QWidget
{
    Q_OBJECT

public:
    JAM();
    ~JAM();

    void showMainMenu();
    void showMenu(Menu *m);

private:
    void registerMenu(Menu* menu, bool isDefaultMenu=false);

    QWidgetStack *m_screen;
    GlobalKeyHandler *m_keyh;
    MenuMain *m_menu;
    JAMTV *m_tv;
    MenuProGuide *m_proGuide;
};


#endif
