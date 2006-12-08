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
#include "streamplayerxine.h"
#include "tv.h"


Controler           *Controler::m_instance = 0;

Controler*
Controler::instance()
{
//    qDebug("Controler::instance()");
    if (m_instance == 0) {
        m_instance = new Controler();
    }
//    qDebug("Controler::instance() m_instance: %p", m_instance);
    return m_instance;
}


Controler::Controler()
{
    qDebug("Controler::Controler()");
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

    WidgetFactory::instance()->setToolkit(WidgetFactory::ToolkitQt);

    m_pageStack = new PageStack();

    qDebug("Adding Main Menu.");
//    m_mainMenu = new Menu(QString("Main Menu"));
    m_mainMenu = new Menu("Main Menu");
    qDebug("Added Main Menu: %p", m_mainMenu);

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
    m_pageStack->addPage(page);
}


void
Controler::showPage(Page *page)
{
    if (page != 0) { // some menu entries don't have real screen pages (for example "quit")
        qDebug("Controler::showPage() raising page: %p named: %s", page, page->getName().latin1());
        m_pageStack->raisePage(page);
        qDebug("Controler::showPage() done raising page");
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
