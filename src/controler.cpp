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

#include "controler.h"
#include "widgetfactory.h"
#include "streamplayer.h"
#include "tv.h"
#include "debug.h"


Controler *Controler::m_instance = 0;

Controler*
Controler::instance()
{
    if (m_instance == 0) {
        m_instance = new Controler();
    }
    return m_instance;
}


Controler::Controler()
{
    TRACE("Controler::Controler()");
}


Controler::~Controler()
{
}


void
Controler::init(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
    m_goingBack = false;
    pthread_mutex_init(&m_eventTriggerMutex, 0);
    pthread_cond_init (&m_eventTrigger, 0);

    WidgetFactory::instance()->setToolkit(WidgetFactory::ToolkitQt);

    m_pageStack = new PageStack();

    TRACE("Adding Main Menu.");
    m_mainMenu = new Menu("Main Menu");
    TRACE("Added Main Menu: %p", m_mainMenu);

    // Decide, which implementation of StreamPlayer to use.
    TRACE("Adding Xine StreamPlayer.");
    m_streamPlayer = StreamPlayer::instance();
    m_streamPlayer->setEngine(StreamPlayer::EngineXine);

    TRACE("Adding TV module.");
    addModule(new Tv());

    TRACE("Showing Main Menu.");
    mainMenuShow();
}


int
Controler::mainLoop()
{
    TRACE("Controler::mainLoop()");
    // start all other event loop threads
    for(vector<Thread*>::iterator i = m_eventLoop.begin(); i != m_eventLoop.end(); ++i) {
        (*i)->start();
    }
    // enter main event loop
    pthread_mutex_lock(&m_eventTriggerMutex);
    TRACE("Controler::mainLoop() starting event loop!!!");
    while(1) {
        TRACE("Controler::mainLoop() waiting for events...");
        pthread_cond_wait(&m_eventTrigger, &m_eventTriggerMutex);
        TRACE("Controler::mainLoop() event received");
        // process the pending events.
    }
    pthread_mutex_unlock(&m_eventTriggerMutex);
    // exit main event loop

    // exit all other event loop threads.
    for(vector<Thread*>::iterator i = m_eventLoop.begin(); i != m_eventLoop.end(); ++i) {
        (*i)->exit();
    }
    // cleanup and exit main thread.
    pthread_mutex_destroy(&m_eventTriggerMutex);
    pthread_cond_destroy(&m_eventTrigger);
    pthread_exit(0);
//     exit();
}


void
Controler::queueEvent()
{
    TRACE("Controler::queueEvent()");
    // queue event in the event fifo.

    // signal the main loop to wake up and process the event.
    pthread_mutex_lock(&m_eventTriggerMutex);
    pthread_cond_signal(&m_eventTrigger);
    pthread_mutex_unlock(&m_eventTriggerMutex);
}


void
Controler::addModule(Module *module)
{
    m_module[module->getName()] = module;
}


void
Controler::addPage(Page *page)
{
    m_pageStack->addPage(page);
}


void
Controler::addEventLoop(Thread *eventLoop)
{
    m_eventLoop.push_back(eventLoop);
}


void
Controler::showPage(Page *page)
{
    if (page != 0) { // some menu entries don't have real screen pages (for example "quit")
        TRACE("Controler::showPage() raising page: %p named: %s", page, page->getName().c_str());
        m_pageStack->raisePage(page);
        TRACE("Controler::showPage() done raising page");
        if (m_goingBack) {
            m_goingBack = false;
            m_pageHistory.pop_back();
        }
        else {
            if (page == m_mainMenu) {
                // forget the history, we are in the main menu again.
                TRACE("Controler::showPage() pageHistory clear");
                m_pageHistory.clear();
            }
            m_pageHistory.push_back(page);
        }
        TRACE("Controler::showPage() pageHistory count: %i", m_pageHistory.size());
    }
}


void
Controler::goBack()
{
    int count = m_pageHistory.size();
    if ( count > 1) {
        m_goingBack = true;
        m_pageHistory.at(count - 2)->showUp();
        TRACE("Controler::goBack() pageHistory count: %i", m_pageHistory.size());
    }
}


void
Controler::mainMenuShow()
{
    m_mainMenu->showUp();
}
