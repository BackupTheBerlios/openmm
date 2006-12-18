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
#include "debug.h"

/*
GlobalKeyHandler::GlobalKeyHandler()
 : QObject()
{

}


GlobalKeyHandler::~GlobalKeyHandler()
{
}
*/

GlobalKeyHandler *GlobalKeyHandler::m_instance = 0;

GlobalKeyHandler*
GlobalKeyHandler::instance()
{
    if (m_instance == 0) {
        m_instance = new GlobalKeyHandler();
    }
    return m_instance;
}

/*
bool
GlobalKeyHandler::eventFilter(QObject *o, QEvent *e)
{
    //TRACE( "GlobalKeyHandler::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        // special processing for key press
        QKeyEvent *k = (QKeyEvent *)e;
        TRACE( "GlobalKeyHandler::eventFilter() key press %d", k->key() );
        switch (k->key()) {
        case Qt::Key_M:                               // menu
            TRACE("GlobalKeyHandler::eventFilter() switching to menu");
            Controler::instance()->mainMenuShow();
            return true;
        case Qt::Key_Q:                               // quit
            //destroy();
            TRACE("GlobalKeyHandler::eventFilter() exiting QApplication");
            Controler::instance()->exit();  // TODO: this (sometimes) does nothing!!
            TRACE("GlobalKeyHandler::eventFilter() after exiting QApplication");
            return true;
        case Qt::Key_Backspace:                       // go back
        case Qt::Key_Escape:
            TRACE("GlobalKeyHandler::eventFilter() going back");
            Controler::instance()->goBack();
            return true;
        default:
            TRACE("GlobalKeyHandler::eventFilter() notifying observers");
            return notify(e);
            // all other key events go to the GUI event loop.
        }
    } else {
        // standard event processing
        return false;
    }
}
*/

// void
// GlobalKeyHandler::attach(Page *page)
// {
//     m_observer.push_back(page);
// }
// 
// 
// void
// GlobalKeyHandler::detach(Page *page)
// {
//     m_observer.erase(find(m_observer.begin(), m_observer.end(), page));
// }


// bool
// GlobalKeyHandler::notify(QEvent *e)
// {
//     TRACE("GlobalKeyHandler::notify() sending event to Page: %p named: %s", 
//             Controler::instance()->getCurrentPage(), Controler::instance()->getCurrentPage()->getName().c_str());
//     return Controler::instance()->getCurrentPage()->eventHandler(e);
// }
