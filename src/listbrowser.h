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
#ifndef LISTBROWSER_H
#define LISTBROWSER_H

#include "page.h"
#include "title.h"
#include "list.h"
#include "popupmenu.h"

#include <qlistview.h>
#include <qstringlist.h>
#include <qptrdict.h>


/**
Page for browsing lists of timers, recordings, files, channels, ...

	@author Jörg Bakker <joerg@hakker.de>
*/


// TODO: ListBrowser should provide functionality like a simple SELECT statement in an RDBMS:
//       1. joining two lists.
//       2. selecting columns of the lists.
//       3. selecting rows of the lists.

class ListBrowser : public Page
{
    Q_OBJECT

public:
    ListBrowser(QString name, QString cols, List *list);
    ~ListBrowser();

    int cols() { return m_cols.count(); }
    QListView* getListView() { return m_listView; }
    void setPopupMenu(PopupMenu *popupMenu);

public slots:
    void addEntry(Title *title);
    void delEntry(Title *title);
    //void update();  // TODO: do we need this? Things are done via enterPage() and addEntry()
    Title *getCurrent() { return m_titleList[m_listView->currentItem()]; }

protected:
    virtual void enterPage();

protected slots:
    void showPopupMenu(QListViewItem *entry);
    List *m_list;
    QListView *m_listView;
    void clear();

private:
    static QString colSeperator;

    QStringList m_cols;
    QPtrDict<Title> m_titleList;
    QPtrDict<QListViewItem> m_itemList;
    PopupMenu *m_popupMenu;

    void selectEntry(int number);
};

#endif
