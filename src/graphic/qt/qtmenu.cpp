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

#include "qtmenu.h"
#include "controler.h"
#include "qtstyle.h"
#include "debug.h"

#include <qlayout.h>
#include <qfontdatabase.h>
#include <qheader.h>


QtMenu::QtMenu(Page *parent)
{
    TRACE("QtMenu::QtMenu()");
    m_list = new QListView((QWidget*) parent->frame());
    m_entryNumber = 0;
    m_defaultEntry = 0;

    connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(selectEntry(QListViewItem*)));

/* ------------ themeing stuff... ------------ */
    m_list->setPaletteBackgroundColor(QtStyle::instance()->backgroundColor());
    m_list->setPaletteForegroundColor(QtStyle::instance()->foregroundColor());
    m_list->setMargin(100);
    m_list->setFont(QtStyle::instance()->bigFont());
    m_list->setAllColumnsShowFocus(true);
    m_list->header()->hide();
}


QtMenu::~QtMenu()
{
}


void
QtMenu::addEntry(Page* page)
{
    TRACE("QtMenu::addEntry() for page: %p", page);
    Controler::instance()->lockGui();
    m_entryNumber++;
    QString entryNumberStr = QString().setNum(m_entryNumber).rightJustify(2, '0') + ".  ";
    QListViewItem *i = new QListViewItem(m_list, entryNumberStr + page->getName(), "");
    m_entryDict.insert(i, page);

    // make the first entry the default selection.
    if (m_entryDict.count() == 1) {
        m_defaultEntry = i;
    }
    Controler::instance()->unlockGui();
}


void
QtMenu::setDefaultEntry(Page* page)
{
    TRACE("QtMenu::setDefaultEntry() for page: %p", page);
}


void
QtMenu::enterPage()
{
    TRACE("QtMenu::enterPage(), set default entry to: %p", m_defaultEntry);
    Controler::instance()->lockGui();
    if (m_defaultEntry) {
        m_list->setCurrentItem(m_defaultEntry);
        m_list->setSelected(m_defaultEntry, true);
    }
    m_list->setFocus();
    Controler::instance()->unlockGui();
}


void
QtMenu::selectEntry(QListViewItem* i)
{
    TRACE("QtMenu::selectEntry()");
    Controler::instance()->lockGui();
    Page *p = m_entryDict[i];
    p->showUp();
    Controler::instance()->unlockGui();
}


QListViewItem*
QtMenu::findEntry(Page *page)
{
    TRACE("Menu::findEntry() with page: %p", page);
    // TODO: implement findItem()
    return 0;
}


extern "C" {
MenuWidget* createMenuWidget(Page *parent)
{
    return new QtMenu(parent);
}
}
