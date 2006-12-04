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
#include "page.h"
#include "controler.h"

#include <qlayout.h>


Page::Page(QString name)
 : QWidget(Controler::instance()->pageStack())
{
    m_name = name;
    // add this page to the global widget stack.
    qDebug("Page::Page() adding Page %s: %p to Controler.", m_name.latin1(), this);
    Controler::addPage(this);
    installEventFilter(GlobalKeyHandler::instance());

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);
}


Page::~Page()
{
}


void
Page::showUp()
{
    qDebug("Page::showUp()");
    if (Controler::getCurrentPage()) {
        Controler::getCurrentPage()->exitPage();
    }
    enterPage();
    Controler::showPage(this);
}


void
Page::enterPage()
{
    qDebug("Page::enterPage()");
}


void
Page::exitPage()
{
    qDebug("Page::postShowAction()");
}
