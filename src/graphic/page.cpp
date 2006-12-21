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
#include "widgetfactory.h"
#include "debug.h"


Page::Page(string name)
{
    m_name = name;
    m_pageWidget = WidgetFactory::instance()->createPageWidget();

    // add this page to the global widget stack.
    TRACE("Page::Page() adding Page %s: %p to PageStack.", m_name.c_str(), this);
    Controler::instance()->addPage(this);
    // catch all global key events for all pages.
    addEventType(Event::MenuE);
    addEventType(Event::QuitE);
    addEventType(Event::BackE);
    TRACE("Page::Page() adding event types, we now have: %i", m_eventTypes.size());
}


Page::~Page()
{
}


void
Page::showUp()
{
    TRACE("Page::showUp()");
    if (Controler::instance()->getCurrentPage()) {
        Controler::instance()->getCurrentPage()->exitPage();
    }
    enterPage();
    Controler::instance()->showPage(this);
}


bool
Page::hasEventType(Event::EventT e)
{
    TRACE("Page::hasEventType() type: %i", e);
    if (m_eventTypes.find(Event::AllE) != m_eventTypes.end()) {
        TRACE("Page::hasEventType() all events are accepted by: %s", getName().c_str());
        return true;
    }
    else {
        TRACE("Page::hasEventType() event accepted by: %s, yes/no: %i", getName().c_str(),
                 m_eventTypes.find(e) != m_eventTypes.end() ? 1 : 0);
        return (m_eventTypes.find(e) != m_eventTypes.end());
    }
}
