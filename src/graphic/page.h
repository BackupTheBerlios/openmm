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
#ifndef PAGE_H
#define PAGE_H

#include "pagewidget.h"
#include "event.h"
#include "debug.h"

#include <string>
#include <set>
using namespace std;


/**
  A full page on the screen, which can be a menu, mediaplayer, list, ...
  Basically, it can be shown, or not.

	@author Jörg Bakker <joerg@hakker.de>
*/
class Page
{
public:
    Page(string name);
    Page()                               { TRACE("Page::Page() - nothing to do"); }
    virtual ~Page();

    string getName()                     { return m_name; };
    void setName(string name)            { m_name = name; };
    void addEventType(Event::EventT e)   { m_eventTypes.insert(e); }
    bool hasEventType(Event::EventT e);

    // for all widgets common part of the class
    virtual void showUp();
    virtual void exitPage()              { TRACE("Page::exitPage()"); }
    virtual bool eventHandler(Event *e)  { return false; }

    // widget specific part of the class
    virtual int globalPositionX()        { return m_pageWidget->globalPositionX(); }
    virtual int globalPositionY()        { return m_pageWidget->globalPositionY(); }
    virtual int width()                  { return m_pageWidget->width(); }
    virtual int height()                 { return m_pageWidget->height(); }
    virtual unsigned long windowId()     { return m_pageWidget->windowId(); }
    virtual void* frame()                { return m_pageWidget->frame(); }
    virtual void enterPage()             { m_pageWidget->enterPage(); }

protected:
    string               m_name;
    PageWidget          *m_pageWidget;
    set<Event::EventT>   m_eventTypes;
};

#endif
