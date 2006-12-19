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
#include "pagestack.h"
#include "widgetfactory.h"
#include "debug.h"


PageStack::PageStack()
{
    TRACE("PageStack::PageStack()");
    m_pageStackWidget = WidgetFactory::instance()->createPageStackWidget();
    m_visiblePage = 0;
    m_logicalPage = 0;
    TRACE("PageStack::PageStack() complete");
}


PageStack::~PageStack()
{
    delete m_pageStackWidget;
}


void
PageStack::raisePage(Page *page)
{
    TRACE("PageStack::raisePage()");
    m_pageStackWidget->raisePage(page);
    m_visiblePage = page;
    TRACE("PageStack::raisePage() complete");
}
