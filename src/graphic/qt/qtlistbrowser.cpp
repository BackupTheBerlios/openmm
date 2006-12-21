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
#include "controler.h"
#include "debug.h"

#include <qlayout.h>


QtListBrowser::QtListBrowser(Page *parent, vector<string> *cols)
{
    TRACE("QtListBrowser::QtListBrowser()");

    m_cols = cols;
    QWidget *p = (QWidget*) parent->frame();
    TRACE("QtListBrowser::QtListBrowser(), parent widget: %p", p);
    m_listView = new QListView(p);
    TRACE("QtListBrowser::QtListBrowser(), build QListView widget: %p", m_listView);
    TRACE("QtListBrowser::QtListBrowser(), installed GlobalKeyHandler");

    connect(m_listView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showPopupMenu(QListViewItem*)));
}


QtListBrowser::~QtListBrowser()
{
}


void
QtListBrowser::enterPage()
{
    TRACE("QtListBrowser::enterPage()");
    Controler::instance()->lockGui();
    m_listView->setFocus();
    Controler::instance()->unlockGui();
}


void
QtListBrowser::addEntry(Title *title)
{
    Controler::instance()->lockGui();
    if (!title) {
        return;
    }
    //TRACE("QtListBrowser::addEntry() with name: %s", title->getText("Name").c_str());
    QListViewItem *entry = new QListViewItem(m_listView);
    m_titleList.insert(entry, title);
    m_itemList.insert(title, entry);
    for (uint i = 0; i < m_cols->size(); i++) {
        entry->setText(i, title->getText(m_cols->at(i)));
    }
    Controler::instance()->unlockGui();
}


void
QtListBrowser::delEntry(Title *title)
{
    TRACE("QtListBrowser::delEntry()");
    Controler::instance()->lockGui();
    if (!title) {
        return;
    }
    m_listView->takeItem(m_itemList[title]);
    m_titleList.remove(m_titleList.find(title));
    m_itemList.remove(title);
    Controler::instance()->unlockGui();
}


void
QtListBrowser::selectEntry(Title *title)
{
    // TODO: get the right entry to select.
    TRACE("QtListBrowser::selectEntry()");
    Controler::instance()->lockGui();
    m_listView->setCurrentItem(m_itemList[title]);
    m_listView->setSelected(m_itemList[title], true);
    Controler::instance()->unlockGui();
}


void
QtListBrowser::clear()
{
    TRACE("QtListBrowser::clear()");
    Controler::instance()->lockGui();
    m_listView->clear();
    m_titleList.clear();
    m_itemList.clear();
    Controler::instance()->unlockGui();
}


void
QtListBrowser::showPopupMenu(QListViewItem *entry)
{
    TRACE("QtListBrowser::showPopupMenu()");
    if (m_popupMenu) {
        m_popupMenu->popup(m_titleList[entry]);
    }
}


extern "C" {
ListBrowserWidget* createListBrowserWidget(Page *parent, vector<string> *cols)
{
    return new QtListBrowser(parent, cols);
}
}
