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
#ifndef QTPAGESTACK_H
#define QTPAGESTACK_H

#include "pagestack.h"
#include "controler.h"

#include <qapplication.h>
#include <qwidgetstack.h>


/**
@author Jörg Bakker
*/
class QtPageStack : public PageStack
{
public:
    QtPageStack(PageStack* pageStackLogic);
    ~QtPageStack();

    virtual void addPage(Page *page) { m_pageStack->addWidget((QWidget*) page->frame()); }
    virtual void raisePage(Page *page) { m_pageStack->raiseWidget((QWidget*) page->frame()); }
    virtual int loop();
    virtual void* frame() { return (void*) m_pageStack; }


protected:
    PageStack    *m_pageStackLogic;
    QWidgetStack *m_pageStack;
    QApplication *m_qtapp;
};

#endif
