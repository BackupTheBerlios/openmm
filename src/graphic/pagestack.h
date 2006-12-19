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
#ifndef PAGESTACK_H
#define PAGESTACK_H

#include "page.h"
#include "pagestackwidget.h"

/**
@author Jörg Bakker
*/
class PageStack
{
public:
    PageStack();
    PageStack(bool widgetAllocated)         {}
    virtual ~PageStack();

    virtual void addPage(Page *page)        { m_pageStackWidget->addPage(page); }
    virtual void raisePage(Page *page);
    void* frame()                           { return m_pageStackWidget->frame(); }
    Page* visiblePage()                     { return m_visiblePage; }
    Page* logicalPage()                     { return m_logicalPage ? m_logicalPage : m_visiblePage; }
    void setLogicalPage(Page *page)         { m_logicalPage = page; }
    void lockGui()                          { m_pageStackWidget->lockGui(); }
    void unlockGui()                        { m_pageStackWidget->unlockGui(); }

protected:
    PageStackWidget *m_pageStackWidget;
    Page            *m_visiblePage;
    Page            *m_logicalPage;
};

#endif