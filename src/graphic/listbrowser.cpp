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
#include "globalkeyhandler.h"
#include "widgetfactory.h"
#include "stringutil.h"
#include "debug.h"


string ListBrowser::m_colSeperator = ";";

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
        addViewColumn(*i);
    }

    m_list->addSink(this);
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
ListBrowser::selectEntry(int number)
{
    TRACE("ListBrowser::selectEntry() number: %i", number);
    selectEntry(m_list->getTitle(number));
}


void
ListBrowser::selectEntry(Title *title)
{
    TRACE("ListBrowser::selectEntry() from Title");
    m_listBrowserWidget->selectEntry(title);
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

