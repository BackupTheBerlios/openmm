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
#include "menu.h"
#include "streamplayer.h"
#include "module.h"

#include <qmainwindow.h>
#include <qwidgetstack.h>
#include <qdict.h>


class Controler: public QWidget
{
    Q_OBJECT

public:
    static void addModule(Module *module);
    static void mainMenuAddEntry(Page *page) { m_mainMenu->addEntry(page); };
    static void mainMenuShow();  // only used by GlobalKeyHandler (later by server-IP wizzard?)

    // TODO: can we hide the following? (should not be exposed to third party modules!).
    static Controler *instance();  // only used by main() and Page::Page().
    static void init();  // only used by main().
    // TODO: store StreamPlayer* pointing to e.g. instance of StreamPlayerXine in class StreamPlayer instead of
    //       in Controler? Controler decides which implementation of StreamPlayer to use.
    //       -> problem of inheriting a singleton.
    static StreamPlayer *streamPlayer() { return m_streamPlayer; }  // only used by MediaPlayer.
    static QWidgetStack *pageStack() { return m_pageStack; }  // only used by Page::Page().
    static void showPage(Page *page);  // make showPage() a friend of class Page? -> use Page->showUp() only.
    static void addPage(Page *page);  // used by Page only: addPage(this).
    //static Page* getPrevPage();  // used by Page only: prevPage()->postShowAction().
    static Page* getCurrentPage() { return (Page*)m_pageStack->visibleWidget(); };
    static void goBack();

protected:
    Controler();
    ~Controler();

private:
    static Controler *m_instance;
    static QWidgetStack *m_pageStack;
    static StreamPlayer *m_streamPlayer;
    static Menu *m_mainMenu;
    static QDict<Module> m_module;
    static QPtrList<Page> m_pageHistory;  // TODO: use a QPtrStack instead of QPtrList?
    //static Page *m_previousPage;
    static bool m_goingBack;
};

#endif
