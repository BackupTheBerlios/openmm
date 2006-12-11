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
#include "qtpage.h"
#include "controler.h"

#include <qlayout.h>


QtPage::QtPage()
{
    m_frame = new QWidget((QWidget*) Controler::instance()->pageStack()->frame());
    qDebug("QtPage::QtPage() creating Page widget: %p.", m_frame);
    m_frame->installEventFilter(GlobalKeyHandler::instance());
    QVBoxLayout *l = new QVBoxLayout(m_frame);
    l->setAutoAdd(TRUE);
}


QtPage::~QtPage()
{
}


extern "C" {
PageWidget* createPageWidget()
{
    return new QtPage();
}
}
