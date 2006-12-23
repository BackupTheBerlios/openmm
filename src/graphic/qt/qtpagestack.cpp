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
    m_pageStack->setCaption("Jam");
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


bool
QtPageStack::suicide()
{
    TRACE("QtPageStack::suicide()");
    m_qtApp->exit();
    // don't try to kill this thread from outside, as we already did this job with m_qtApp->exit().
    return true;
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


QtEventFilter::QtEventFilter()
{
    // map Qt keys to internal Events.
    m_eventMap[Qt::Key_M] = Event::MenuE;
    m_eventMap[Qt::Key_Return] = Event::EnterE;
    m_eventMap[Qt::Key_Q] = Event::QuitE;
    m_eventMap[Qt::Key_Backspace] = Event::BackE;
    m_eventMap[Qt::Key_Up] = Event::UpE;
    m_eventMap[Qt::Key_Down] = Event::DownE;
    m_eventMap[Qt::Key_Left] = Event::LeftE;
    m_eventMap[Qt::Key_Right] = Event::RightE;
    m_eventMap[Qt::Key_0] = Event::Key0E;
    m_eventMap[Qt::Key_1] = Event::Key1E;
    m_eventMap[Qt::Key_2] = Event::Key2E;
    m_eventMap[Qt::Key_3] = Event::Key3E;
    m_eventMap[Qt::Key_4] = Event::Key4E;
    m_eventMap[Qt::Key_5] = Event::Key5E;
    m_eventMap[Qt::Key_6] = Event::Key6E;
    m_eventMap[Qt::Key_7] = Event::Key7E;
    m_eventMap[Qt::Key_8] = Event::Key8E;
    m_eventMap[Qt::Key_9] = Event::Key9E;
}


bool
QtEventFilter::eventFilter(QObject *o, QEvent *e)
{
//     TRACE( "QtEventFilter::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent *k = (QKeyEvent*)e;
        TRACE( "QtEventFilter::eventFilter() key press %d", k->key() );
        if (Controler::instance()->getCurrentPage()->hasEventType(m_eventMap.find(k->key())->second)) {
            Controler::instance()->queueEvent(new Event(m_eventMap.find(k->key())->second));
            // don't forward the event to the Qt event loop.
            return true;
        }
    }
    // standard event processing in the Qt event loop.
    return false;
}


extern "C" {
PageStackWidget* createPageStackWidget()
{
    return new QtPageStack();
}
}
