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
#include "qtpagestack.h"
#include "controler.h"
#include "event.h"
#include "debug.h"

#include <qlayout.h>
#include <qcursor.h>


QtPageStack::QtPageStack()
{
    TRACE("QtPageStack::QtPageStack()");
    m_qtApp = new QApplication(Controler::instance()->getArgc(), Controler::instance()->getArgv());
    m_pageStack = new QWidgetStack();
    m_pageStack->setCaption("JamSession");
    m_pageStack->resize(720, 576);
    m_qtApp->setMainWidget(m_pageStack);
    m_qtApp->setOverrideCursor(Qt::BlankCursor);
    m_qtApp->installEventFilter(new QtEventFilter());
    Controler::instance()->addEventLoop(this);
}


QtPageStack::~QtPageStack()
{
    delete m_pageStack;
    delete m_qtApp;
}


void
QtPageStack::beforeExit()
{
    TRACE("QtPageStack::beforeExit()");
    m_qtApp->exit();
//     TRACE("QtPageStack::beforeExit(), Qt main loop exited");
}


void
QtPageStack::run()
{
    TRACE("QtPageStack::run(), starting event loop!!!!");
    m_pageStack->show();
    TRACE("QtPageStack::run(), entering Qt main loop.");
    m_qtApp->exec();
    TRACE("QtPageStack::run(), exiting Qt main loop.");
}


bool
QtEventFilter::eventFilter(QObject *o, QEvent *e)
{
//     TRACE( "QtEventFilter::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent *k = (QKeyEvent*)e;
//         TRACE( "QtEventFilter::eventFilter() key press %d", k->key() );
        switch (k->key()) {
        case Qt::Key_M:                               // menu
            TRACE("QtEventFilter::eventFilter() sending menu event to Controler");
            Controler::instance()->queueEvent(new Event(Event::MenuE));
//             TRACE("QtEventFilter::eventFilter() showing main menu");
//             Controler::instance()->mainMenuShow();
            return true;
        case Qt::Key_Q:                               // quit
            TRACE("QtEventFilter::eventFilter() sending quit event to Controler");
            Controler::instance()->queueEvent(new Event(Event::QuitE));
            return true;
        case Qt::Key_Backspace:                       // go back
            TRACE("QtEventFilter::eventFilter() sending back event to Controler");
            Controler::instance()->queueEvent(new Event(Event::BackE));
//             TRACE("QtEventFilter::eventFilter() going back");
//             Controler::instance()->goBack();
            return true;
        case Qt::Key_Up:
            Controler::instance()->queueEvent(new Event(Event::UpE));
            return false;
        case Qt::Key_Down:
            Controler::instance()->queueEvent(new Event(Event::DownE));
            return false;
        case Qt::Key_Left:
            Controler::instance()->queueEvent(new Event(Event::LeftE));
            return true;
        case Qt::Key_Right:
            Controler::instance()->queueEvent(new Event(Event::RightE));
            return true;
        default:
            // all other key events go to the GUI event loop.
            return false;
        }
    } else {
        // standard event processing in the GUI event loop.
        return false;
    }
}


extern "C" {
PageStackWidget* createPageStackWidget()
{
    return new QtPageStack();
}
}
