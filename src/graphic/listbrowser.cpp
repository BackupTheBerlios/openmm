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


QString ListBrowser::colSeperator = ";";

ListBrowser::ListBrowser(QString name, QString cols, List *list)
 : Page(name)
{
    qDebug("ListBrowser::ListBrowser()");
    m_cols = QStringList::split(colSeperator, cols);
    m_list = list;
    m_popupMenu = 0;

    qDebug("ListBrowser::ListBrowser() creating ListBrowser widget");
    m_listBrowserWidget = WidgetFactory::instance()->createListBrowserWidget(&m_cols);

    for (QStringList::iterator i = m_cols.begin(); i != m_cols.end(); i++) {
        addViewColumn(*i);
    }

    connect(m_list, SIGNAL(pushTitle(Title*)), this, SLOT(addEntry(Title*)));
    connect(m_list, SIGNAL(popTitle(Title*)), this, SLOT(delEntry(Title*)));
}


ListBrowser::~ListBrowser()
{
}


void
ListBrowser::enterPage()
{
    qDebug("ListBrowser::enterPage()");
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
ListBrowser::addEntry(Title *title)
{
    m_listBrowserWidget->addEntry(title);
}


void
ListBrowser::delEntry(Title *title)
{
    m_listBrowserWidget->delEntry(title);
}


void
ListBrowser::selectEntry(int number)
{
    qDebug("ListBrowser::selectEntry() number: %i", number);
    selectEntry(m_list->getTitle(number));
}


void
ListBrowser::selectEntry(Title *title)
{
    qDebug("ListBrowser::selectEntry() from Title");
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
}

