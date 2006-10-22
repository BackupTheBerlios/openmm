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
class TVChannel;
class EPGEntry;
class ProGuideEntry;

/**
@author Jörg Bakker
*/
class MenuProGuide : public Menu
{
    Q_OBJECT

public:
    enum ProGuideColumns {ProGuideColNum=0, ProGuideColChan, ProGuideColStart, ProGuideColEnd, ProGuideColTitle};
    enum ChannelMenu {ChannelMenuTimer=0, ChannelMenuSwitch, ChannelMenuShow};

    MenuProGuide(JAM *controler, JAMTV *tv, GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~MenuProGuide();
    
    void showCurrentProgram();
    void showNextProgram();
    void showPrevProgram();
    void action();
    void selectDefault();
    ProGuideEntry *findChannel(TVChannel *channel);

protected slots:
    void showChannelMenu(QListViewItem *channel);
    void channelMenuTimer();
    void channelMenuSwitch();
    void channelMenuShow();

private:
    QListView *m_list;
    JAM *m_controler;
    JAMTV *m_tv;
    ProGuideEntry *m_cur;
    ProGuideEntry *m_selectedChannel;
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


class ProGuideEntry : public QListViewItem
{
public:
    ProGuideEntry(QListView *parent, TVChannel *channel, EPGEntry *epgEntry);

    ~ProGuideEntry();

    TVChannel *getChannel() { return m_channel; }
    EPGEntry *getEPGEntry() { return m_epgEntry; }

    void update(EPGEntry *epgEntry);

private:
    TVChannel *m_channel;
    EPGEntry *m_epgEntry;
};
#endif
