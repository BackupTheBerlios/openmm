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
}


QtPageStack::~QtPageStack()
{
    delete m_pageStack;
    delete m_qtApp;
}


int
QtPageStack::loop()
{
    m_pageStack->show();
    TRACE("QtPageStack::loop(), entering main loop.");
    return m_qtApp->exec();
    TRACE("QtPageStack::loop(), exiting main loop.");
}


void
QtPageStack::exit()
{
    TRACE("QtPageStack::loop(), exiting main loop.");
    m_qtApp->exit();
    TRACE("QtPageStack::loop(), exited.");
}


extern "C" {
PageStackWidget* createPageStackWidget()
{
    return new QtPageStack();
}
}
