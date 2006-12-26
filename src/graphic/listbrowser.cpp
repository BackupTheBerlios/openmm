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
#include "listbrowser.h"
#include "widgetfactory.h"
#include "stringutil.h"
#include "debug.h"

#include <cstdlib>


string ListBrowser::m_colSeperator = ";";
string ListBrowser::m_colFormatSeperator = "%";
string ListBrowser::m_colLabelSeperator = "$";

ListBrowser::ListBrowser(string name, string cols, List *list)
 : Page(name)
{
    TRACE("ListBrowser::ListBrowser()");
    StringUtil::s_split(cols, m_colSeperator, m_cols);
    m_list = list;
    m_popupMenu = 0;

    TRACE("ListBrowser::ListBrowser() creating ListBrowser widget");
    m_listBrowserWidget = WidgetFactory::instance()->createListBrowserWidget(this, &m_cols);

    for (vector<string>::iterator i = m_cols.begin(); i != m_cols.end(); i++) {
        int formatPos = (*i).find(m_colFormatSeperator, 0);
        string formatString = (*i).substr(formatPos + 1);
        int labelPos = formatString.find(m_colLabelSeperator, 0);
        int colWidth = 0;
        if (formatPos != string::npos) {
            string colWidthStr = formatString.substr(0, labelPos);
            if (colWidthStr != "") {
                colWidth = atoi(colWidthStr.c_str());
            }
            (*i) = (*i).substr(0, formatPos);
        }
        if (labelPos != string::npos) {
            addViewColumn(formatString.substr(labelPos + 1), colWidth);
        }
        else {
            addViewColumn(*i, colWidth);
        }
    }

    m_list->addSink(this);
    addEventType(Event::LeftE);
    addEventType(Event::RightE);
    addEventType(Event::PopupE);
}


ListBrowser::~ListBrowser()
{
}


void
ListBrowser::enterPage()
{
    TRACE("ListBrowser::enterPage()");
    clear();  // TODO: update(), fill(), ... when and what?
    if (!m_list) {
        return;
    }
    m_list->update();
    m_listBrowserWidget->enterPage();
    if (m_list->count()) {
        selectEntry(0);  // TODO: list in browser is sorted somehow, how to select top most entry?
    }
}


void
ListBrowser::addTitle(Title *title)
{
//     TRACE("ListBrowser::addEntry(), title: %s", title->getText("Name").c_str());
    m_listBrowserWidget->addEntry(title);
}


void
ListBrowser::delTitle(Title *title)
{
    m_listBrowserWidget->delEntry(title);
}


void
ListBrowser::clear()
{
    m_listBrowserWidget->clear();
}


void
ListBrowser::setPopupMenu(PopupMenu *popupMenu)
{
    m_popupMenu = popupMenu;
    m_popupMenu->setParent(this);
    m_popupMenu->setList(m_list);
    m_listBrowserWidget->setPopupMenu(popupMenu);
}


bool
ListBrowser::eventHandler(Event *e)
{
    baseEventHandler(e);
}


bool
ListBrowser::baseEventHandler(Event *e)
{
    TRACE("ListBrowser::baseEventHandler() event: %p, type: %i", e, e->type());
    switch (e->type()) {
        case Event::PopupE:
//             getPopupMenu()->itemDispatcher(((PopupMenuEvent*)e)->getItem());
            if (getPopupMenu()) {
                return getPopupMenu()->eventHandler(e);
            }
            return true;
        default:
            return static_cast<Page*>(this)->baseEventHandler(e);
//             return ((Page*)(this))->baseEventHandler(e);
    }
}
