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

#include <qdict.h>


class Controler
{
public:
    static Controler *instance();  // only used by main() and Page::Page().
    void addModule(Module *module);
    void mainMenuAddEntry(Page *page) { m_mainMenu->addEntry(page); };
    void mainMenuShow();  // only used by GlobalKeyHandler (later by server-IP wizzard?)

    void init(int argc, char **argv);  // only used by main().
    int loop() { return m_pageStack->loop(); }
    int& getArgc() { return m_argc; }
    char** getArgv() { return m_argv; }
    StreamPlayer *streamPlayer() { return m_streamPlayer; }  // only used by MediaPlayer.
    PageStack *pageStack() { return m_pageStack; }  // only used by Page::Page().
    void showPage(Page *page);  // make showPage() a friend of class Page? -> use Page->showUp() only.
    void addPage(Page *page);  // used by Page only: addPage(this).
    Page* getCurrentPage() { return (Page*)m_pageStack->visiblePage(); };
    void goBack();

protected:
    Controler();
    ~Controler();

private:
    static Controler *m_instance;
    int              m_argc;
    char           **m_argv;
    PageStack       *m_pageStack;
    StreamPlayer    *m_streamPlayer;
    Menu            *m_mainMenu;
    QDict<Module>    m_module;
    QPtrList<Page>   m_pageHistory;  // TODO: use a QPtrStack instead of QPtrList?
    bool             m_goingBack;
};

#endif
