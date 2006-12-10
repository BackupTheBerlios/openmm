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
#ifndef QTMENU_H
#define QTMENU_H

#include "menuwidget.h"
#include "qtpage.h"

#include <qlistview.h>
#include <qptrdict.h>


/**
@author Jörg Bakker
*/
class QtMenu : public QtPage, public MenuWidget
{
    Q_OBJECT

public:
    QtMenu();
    ~QtMenu();

    virtual void addEntry(Page *page);
    virtual void setDefaultEntry(Page *page);
    virtual void setMenuName(QString name) { m_list->addColumn(name); }
    virtual void enterPage();

private slots:
    void selectEntry(QListViewItem* i);

private:
    QListViewItem *findEntry(Page *page);

    QPtrDict<Page> m_entryDict;
    QListView     *m_list;
    QListViewItem *m_defaultEntry;
    QListViewItem *m_selectedEntry;
    int            m_entryNumber;
};

#endif
