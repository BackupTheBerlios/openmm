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
#include "globalkeyhandler.h"
// somewhere in the included headers, there is this macro hidden which conflicts with QEvent::KeyPress
// and results in stragen error messages.


GlobalKeyHandler::GlobalKeyHandler(JAM *controler, QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_controler = controler;
}


GlobalKeyHandler::~GlobalKeyHandler()
{
}


bool
GlobalKeyHandler::eventFilter(QObject *o, QEvent *e)
{
    //qDebug( "GlobalKeyHandler::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        // special processing for key press
        QKeyEvent *k = (QKeyEvent *)e;
        //qDebug( "GlobalKeyHandler::eventFilter() key press %d", k->key() );
        switch (k->key()) {
        case Qt::Key_M:                               // menu
            qDebug("GlobalKeyHandler::eventFilter() switching to menu");
            m_controler->showMainMenu();
            return TRUE;
        case Qt::Key_Q:                               // quit
/*
            m_menu->close();
            streamplayer->close();
            m_screen->close();
            delete streamplayer;
            delete tv;
            delete m_menu;
            delete m_screen;
*/
            //destroy();
            qDebug("GlobalKeyHandler::eventFilter() exiting QApplication");
            QApplication::exit();  // FIX this does nothing!!
            qDebug("GlobalKeyHandler::eventFilter() after exiting QApplication");
            return TRUE;
        default:
            return FALSE;
        }
    } else {
        // standard event processing
        return FALSE;
    }
}
