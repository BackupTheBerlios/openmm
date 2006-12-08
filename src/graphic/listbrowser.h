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

#include <qstringlist.h>
#include <qptrdict.h>


/**
Page for browsing lists of timers, recordings, files, channels, ...

	@author Jörg Bakker <joerg@hakker.de>
*/


class ListBrowser : public QObject, public Page
{
    Q_OBJECT

public:
    ListBrowser(QString name, QString cols, List *list);
    ListBrowser() {}
    ~ListBrowser();

    int cols() { return m_cols.count(); }
    QString colText(int i) { return m_cols[i]; }
    void setPopupMenu(PopupMenu *popupMenu);
    PopupMenu* getPopupMenu() { return m_popupMenu; }
    virtual void enterPage();

public slots:
    virtual void addEntry(Title *title);
    virtual void delEntry(Title *title);
    virtual Title *getCurrent() { return m_listBrowserWidget->getCurrent(); }

protected slots:
    virtual void clear();

protected:
    void selectEntry(int number);
    virtual void selectEntry(Title *title);
    virtual void addViewColumn(QString colName) { m_listBrowserWidget->addViewColumn(colName); }

    static QString      colSeperator;
    List               *m_list;
    QStringList         m_cols;
    PopupMenu          *m_popupMenu;

    ListBrowser        *m_listBrowserWidget;
};

#endif
