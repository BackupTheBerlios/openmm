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
#ifndef QTLISTBROWSER_H
#define QTLISTBROWSER_H

#include "page.h"
#include "popupmenu.h"
#include "listbrowserwidget.h"

#include <qlistview.h>
#include <qstringlist.h>
#include <qptrdict.h>
#include <string>
#include <vector>
using namespace std;


/**
Page for browsing lists of timers, recordings, files, channels, ...

	@author Jörg Bakker <joerg@hakker.de>
*/


class QtListBrowser : public QObject, public ListBrowserWidget
{
    Q_OBJECT

public:
    QtListBrowser(Page *parent, vector<string> *cols);
    ~QtListBrowser();

    virtual void enterPage();

public slots:
    virtual void addEntry(Title *title);
    virtual void delEntry(Title *title);
    virtual Title *getCurrent() { return m_titleList[m_listView->currentItem()]; }
    virtual int getPosition(Title *title);
    virtual void setPopupMenu(PopupMenu *popupMenu) { m_popupMenu = popupMenu; }
    virtual void clear();
    virtual void selectEntry(Title *title);
    virtual void selectEntry(int number);

protected slots:
    virtual void showPopupMenu(QListViewItem *entry);
    virtual void addViewColumn(string colName, int colWidth);

private:
    vector<string>         *m_cols;
    Page                   *m_parent;
    PopupMenu              *m_popupMenu;
    QListView              *m_listView;
    QPtrDict<Title>         m_titleList;
    QPtrDict<QListViewItem> m_itemList;
    vector<Title*>          m_titleVector;
};

#endif
