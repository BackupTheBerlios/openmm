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
            Controler::mainMenuShow();
            return true;
        case Qt::Key_Q:                               // quit
            //destroy();
            qDebug("GlobalKeyHandler::eventFilter() exiting QApplication");
            QApplication::exit();  // TODO: this (sometimes) does nothing!!
            qDebug("GlobalKeyHandler::eventFilter() after exiting QApplication");
            return true;
        case Qt::Key_Backspace:                       // go back
        case Qt::Key_Escape:
            qDebug("GlobalKeyHandler::eventFilter() going back");
            Controler::goBack();
            return true;
        default:
            return false;
        }
    } else {
        // standard event processing
        return false;
    }
}
