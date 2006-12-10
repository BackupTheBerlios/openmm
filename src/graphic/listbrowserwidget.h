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
#ifndef LISTBROWSERWIDGET_H
#define LISTBROWSERWIDGET_H

#include "title.h"

#include <qstring.h>
#include <qstringlist.h>

/**
Page for browsing lists of timers, recordings, files, channels, ...

	@author Jörg Bakker <joerg@hakker.de>
*/


class ListBrowserWidget
{
public:
    ListBrowserWidget(QStringList *cols);
    ListBrowserWidget();
//     ~ListBrowserWidget();

    virtual void enterPage() = 0;

public slots:
    virtual void addEntry(Title *title) = 0;
    virtual void delEntry(Title *title) = 0;
    virtual Title *getCurrent() = 0;
    virtual void addViewColumn(QString colName) = 0;
//    virtual void showPopupMenu(QListViewItem *entry) = 0;
    virtual void clear() = 0;
    virtual void selectEntry(Title *title) = 0;
};

#endif
