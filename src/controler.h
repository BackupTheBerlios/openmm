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


#ifndef CONTROLER_H
#define CONTROLER_H

#include "globalkeyhandler.h"
#include "pagestack.h"
#include "menu.h"
#include "streamplayer.h"
#include "module.h"
#include "thread.h"
#include "event.h"

#include <pthread.h>

#include <map>
#include <vector>
#include <queue>

class Controler
{
public:
    static Controler *instance();  // only used by main() and Page::Page().
    void addModule(Module *module);
    void mainMenuAddEntry(Page *page)   { m_mainMenu->addEntry(page); };
    void mainMenuShow();  // only used by GlobalKeyHandler (later by server-IP wizzard?)

    void init(int argc, char **argv);  // only used by main().
    int loop();
    int& getArgc()                      { return m_argc; }
    char** getArgv()                    { return m_argv; }
    Page* getCurrentPage()              { return (Page*)m_pageStack->logicalPage(); };
    void goBack();
    StreamPlayer *streamPlayer()        { return m_streamPlayer; }  // only used by MediaPlayer.
    PageStack *pageStack()              { return m_pageStack; }
    void showPage(Page *page);
    void addPage(Page *page);
    void addEventLoop(Thread *eventLoop);
    void queueEvent(Event *event);
    void lockGui()                      { m_pageStack->lockGui(); }
    void unlockGui()                    { m_pageStack->unlockGui(); }

protected:
    Controler();
    ~Controler();

private:
    bool dispatchEvents();

    static Controler       *m_instance;
    int                     m_argc;
    char                  **m_argv;
    PageStack              *m_pageStack;
    StreamPlayer           *m_streamPlayer;
    Menu                   *m_mainMenu;
    map<string, Module*>    m_module;
    vector<Page*>           m_pageHistory;
    vector<Thread*>         m_eventLoop;
    bool                    m_goingBack;
    queue<Event*>           m_eventQueue;
    pthread_mutex_t         m_eventTriggerMutex;
    pthread_cond_t          m_eventTrigger;
};

#endif