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
#include "streamplayerxine.h"
#include "tv.h"

#include <qapplication.h>
#include <qlayout.h>


Controler           *Controler::m_instance = 0;
QWidgetStack        *Controler::m_pageStack = 0;
StreamPlayer        *Controler::m_streamPlayer = 0;
Menu                *Controler::m_mainMenu = 0;
QDict<Module>        Controler::m_module;
QPtrList<Page>       Controler::m_pageHistory;
//Page                *Controler::m_previousPage = 0;
bool                 Controler::m_goingBack = false;


Controler*
Controler::instance()
{
    qDebug("Controler::instance()");
    if (m_instance == 0) {
        m_instance = new Controler();
    }
    qDebug("Controler::instance() m_instance: %p", m_instance);
    return m_instance;
}


Controler::Controler()
    : QWidget(0, "Controler")
{
    qDebug("Controler::Controler()");
    setCaption("Jam V0.1");
    resize(720, 576);

    QHBoxLayout *l = new QHBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_pageStack = new QWidgetStack(this);
    installEventFilter(GlobalKeyHandler::instance());
}


Controler::~Controler()
{
}


void
Controler::init()
{
    qDebug("Adding Main Menu.");
    m_mainMenu = new Menu("Main Menu");
    qDebug("Added: %p", m_mainMenu);

    // Decide, which implementation of StreamPlayer to use.
    qDebug("Adding Xine StreamPlayer.");
    m_streamPlayer = StreamPlayerXine::instance();

    qDebug("Adding TV module.");
    addModule(new Tv());

    qDebug("Showing Main Menu.");
    mainMenuShow();
}


void
Controler::addModule(Module *module)
{
    // register module in hash array.
    m_module.insert(module->getName(), module);
}


void
Controler::addPage(Page *page)
{
    m_pageStack->addWidget(page);
}


void
Controler::showPage(Page *page)
{
    if (page != 0) { // some menu entries don't have real screen pages (for example "quit")
        qDebug("Controler::showPage() raising page: %p named: %s", page, page->getName().latin1());
        m_pageStack->raiseWidget(page);
        //m_previousPage = m_pageHistory.getLast();
        if (m_goingBack) {
            m_goingBack = false;
            m_pageHistory.removeLast();
        }
        else {
            if (page == m_mainMenu) {
                // forget the history, we are in the main menu again.
                qDebug("Controler::showPage() pageHistory clear");
                m_pageHistory.clear();
            }
            m_pageHistory.append(page);
        }
        qDebug("Controler::showPage() pageHistory count: %i", m_pageHistory.count());
    }
}

/*
Page*
Controler::getPrevPage()
{
//    if (m_pageHistory.count() > 1) {
//        qDebug("Controler::getPrevPage() pageHistory count: %i", m_pageHistory.count());
//        return m_pageHistory.at(m_pageHistory.count() - 2);
//    }
//    return 0;
    return m_previousPage;
}
*/

void
Controler::goBack()
{
    int count = m_pageHistory.count();
    if ( count > 1) {
        m_goingBack = true;
        m_pageHistory.at(count - 2)->showUp();
        //m_pageHistory.remove(count - 1);
        qDebug("Controler::goBack() pageHistory count: %i", m_pageHistory.count());
    }
}


void
Controler::mainMenuShow()
{
    m_mainMenu->showUp();
}
