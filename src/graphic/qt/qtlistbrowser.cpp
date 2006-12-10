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
#include "qtlistbrowser.h"
#include "globalkeyhandler.h"

#include <qlayout.h>


QtListBrowser::QtListBrowser(QStringList *cols)
{
    qDebug("QtListBrowser::QtListBrowser()");

    m_cols = cols;
    QWidget *parent = m_frame;
    qDebug("QtListBrowser::QtListBrowser(), parent widget: %p", parent);
    m_listView = new QListView(parent);
    qDebug("QtListBrowser::QtListBrowser(), build QListView widget: %p", m_listView);
    m_listView->installEventFilter(GlobalKeyHandler::instance());
    qDebug("QtListBrowser::QtListBrowser(), installed GlobalKeyHandler");

    connect(m_listView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showPopupMenu(QListViewItem*)));
    qDebug("QtListBrowser::QtListBrowser() done");
}


QtListBrowser::~QtListBrowser()
{
}


void
QtListBrowser::enterPage()
{
    qDebug("QtListBrowser::enterPage()");
    m_listView->setFocus();
}


void
QtListBrowser::addEntry(Title *title)
{
    if (!title) {
        return;
    }
    //qDebug("QtListBrowser::addEntry() with name: %s", title->getText("Name").latin1());
    QListViewItem *entry = new QListViewItem(m_listView);
    m_titleList.insert(entry, title);
    m_itemList.insert(title, entry);
    for (uint i = 0; i < m_cols->count(); i++) {
        entry->setText(i, title->getText((*m_cols->at(i))));
    }
}


void
QtListBrowser::delEntry(Title *title)
{
    qDebug("QtListBrowser::delEntry()");
    if (!title) {
        return;
    }
    m_listView->takeItem(m_itemList[title]);
    m_titleList.remove(m_titleList.find(title));
    m_itemList.remove(title);
}


void
QtListBrowser::selectEntry(Title *title)
{
    // TODO: get the right entry to select.
    qDebug("QtListBrowser::selectEntry()");
    m_listView->setCurrentItem(m_itemList[title]);
    m_listView->setSelected(m_itemList[title], true);
}


void
QtListBrowser::clear()
{
    qDebug("QtListBrowser::clear()");
    m_listView->clear();
    m_titleList.clear();
    m_itemList.clear();
}


void
QtListBrowser::showPopupMenu(QListViewItem *entry)
{
    qDebug("QtListBrowser::showPopupMenu()");
//    if (m_listBrowserLogic->getPopupMenu()) {
//        m_listBrowserLogic->getPopupMenu()->popup(m_titleList[entry]);
//    }
}

