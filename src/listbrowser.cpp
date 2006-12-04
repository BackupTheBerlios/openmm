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

#include <qlayout.h>


QString ListBrowser::colSeperator = ";";

ListBrowser::ListBrowser(QString name, QString cols, List *list)
 : Page(name)
{
    qDebug("ListBrowser::ListBrowser()");
    m_cols = QStringList::split(colSeperator, cols);
    m_list = list;
    m_popupMenu = 0;

    m_listView = new QListView(this);
    m_listView->installEventFilter(GlobalKeyHandler::instance());

    for (QStringList::iterator i = m_cols.begin(); i != m_cols.end(); i++) {
        m_listView->addColumn(*i);
    }

    connect(m_listView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showPopupMenu(QListViewItem*)));
    connect(m_list, SIGNAL(changed()), this, SLOT(update()));
    connect(m_list, SIGNAL(pushTitle(Title*)), this, SLOT(addEntry(Title*)));
    connect(m_list, SIGNAL(popTitle(Title*)), this, SLOT(delEntry(Title*)));
}


ListBrowser::~ListBrowser()
{
}


// void
// ListBrowser::update()
// {
//     qDebug("ListBrowser::update()");
//     clear();
//     if (!m_list) {
//         return;
//     }
// 
// //    m_list->stepReset();
// //    while (Title *t = m_list->step()) {
// //        addEntry(t);
// //    }
//     for (int i = 0; i < m_list->count(); i++) {
//         addEntry(m_list->getTitle(i));
//     }
// }


void
ListBrowser::enterPage()
{
    qDebug("ListBrowser::enterPage()");
    clear();  // TODO: update(), fill(), ... when and what?
    if (!m_list) {
        return;
    }
    m_list->update();
    if (m_list->count()) {
        selectEntry(0);  // TODO: list in browser is sorted somehow, how to select top most entry?
    }
}


void
ListBrowser::addEntry(Title *title)
{
    //qDebug("ListBrowser::addEntry() with name: %s", title->getText("Name").latin1());
    if (!title) {
        return;
    }
    QListViewItem *entry = new QListViewItem(m_listView);
    m_titleList.insert(entry, title);
    m_itemList.insert(title, entry);
    for (uint i = 0; i < m_cols.count(); i++) {
        entry->setText(i, title->getText(m_cols[i]));
    }
}


void
ListBrowser::delEntry(Title *title)
{
    qDebug("ListBrowser::delEntry()");
    if (!title) {
        return;
    }
    m_listView->takeItem(m_itemList[title]);
}


void
ListBrowser::selectEntry(int number)
{
    // TODO: get the right entry to select.
    qDebug("ListBrowser::selectEntry()");
    m_listView->setCurrentItem(m_itemList[m_list->getTitle(number)]);
    m_listView->setSelected(m_itemList[m_list->getTitle(number)], true);
}


void
ListBrowser::clear()
{
    qDebug("ListBrowser::clear()");
    m_listView->clear();
    m_titleList.clear();
    m_itemList.clear();
}


void
ListBrowser::setPopupMenu(PopupMenu *popupMenu)
{
    m_popupMenu = popupMenu;
    m_popupMenu->setParent(this);
    m_popupMenu->setList(m_list);
}


void
ListBrowser::showPopupMenu(QListViewItem *entry)
{
    qDebug("ListBrowser::showPopupMenu()");
    if (m_popupMenu) {
        m_popupMenu->popup(m_titleList[entry]);
    }
}

