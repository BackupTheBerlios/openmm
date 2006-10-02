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
#ifndef MENUPROGUIDE_H
#define MENUPROGUIDE_H

#include "globalkeyhandler.h"

#include <qwidget.h>
#include <qlistview.h>

#include "jamtv.h"


class GlobalKeyHandler;
class JAMTV;

/**
@author Jörg Bakker
*/
class MenuProGuide : public Menu
{
Q_OBJECT

enum ProGuideColumns {ProGuideColNum=0, ProGuideColChan, ProGuideColStart, ProGuideColEnd, ProGuideColTitel};

public:
    MenuProGuide(JAMTV *tv, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~MenuProGuide();
    
    void showCurrentProgram();
    void showNextProgram();
    void showPrevProgram();
    void action();
    void selectDefault();
    void selectCurrent();

protected slots:
    void showChannelMenu(QListViewItem *channel);

private:
    QListView *m_list;
    JAMTV *m_tv;
    unsigned int m_defaultChannel;
    unsigned int m_currentChannel;

    void showProgram(int direction);
};


class ProGuideEventFilter : public QObject
{
Q_OBJECT
public:
    ProGuideEventFilter(MenuProGuide *controler, QObject *parent = 0, const char *name = 0);

    ~ProGuideEventFilter();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    MenuProGuide *m_controler;
};

#endif
