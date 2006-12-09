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

#include <qlayout.h>
#include <qcursor.h>


QtPageStack::QtPageStack(PageStack *pageStackLogic)
 : PageStack(true)
{
    qDebug("QtPageStack::QtPageStack()");
    m_pageStackLogic = pageStackLogic;
    m_qtapp = new QApplication(Controler::instance()->getArgc(), Controler::instance()->getArgv());
    m_pageStack = new QWidgetStack();
    m_pageStack->setCaption("Jam");
    m_pageStack->resize(720, 576);
    m_qtapp->setMainWidget(m_pageStack);
    m_qtapp->setOverrideCursor(Qt::BlankCursor);
}


QtPageStack::~QtPageStack()
{
    delete m_pageStack;
    delete m_qtapp;
}


int
QtPageStack::loop()
{
    m_pageStack->show();
    qDebug("QtPageStack::loop(), entering main loop.");
    return m_qtapp->exec();
    qDebug("QtPageStack::loop(), exiting main loop.");
}


extern "C" {

PageStack* createPageStack(PageStack *pageStackLogic)
{
    return new QtPageStack(pageStackLogic);
}

}
