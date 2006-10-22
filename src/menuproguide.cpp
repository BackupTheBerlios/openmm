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
#include <qlayout.h>
#include <qdatetime.h>
#include <qpopupmenu.h>
#include "menuproguide.h"

MenuProGuide::MenuProGuide(JAM *controler, JAMTV *tv, GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Program Guide";
    m_controler = controler;
    m_tv = tv;
    ProGuideEventFilter *proguidekeyh = new ProGuideEventFilter(this);
    m_list = new QListView(this);
    m_list->setAllColumnsShowFocus(true);
    m_list->installEventFilter(keyh);
    m_list->installEventFilter(proguidekeyh);
    m_list->addColumn("Nr.");
    m_list->addColumn("Channel Name");
    m_list->addColumn("Start");
    m_list->addColumn("End");
    m_list->addColumn("Title");
    m_defaultChannel = 0;
    m_currentChannel = 0;
    m_cur = 0;

    connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(showChannelMenu(QListViewItem*)));
}


MenuProGuide::~MenuProGuide()
{
}


void
MenuProGuide::showCurrentProgram()
{
    showProgram(0);
}


void
MenuProGuide::showNextProgram()
{
    showProgram(1);
}


void
MenuProGuide::showPrevProgram()
{
    showProgram(-1);
}


void
MenuProGuide::showProgram(int direction)
{
    qDebug("MenuProGuide::showProgram()");

    for (int i = 0; i < m_tv->getChannelCount(); i++)
    {
        //qDebug("MenuProGuide::showProgram() channel %i: %s", i, m_tv->getChannelName(i).latin1());
        EPGEntry *currentEPGEntry;
        if (direction == 0)
            currentEPGEntry = m_tv->getChannelPointer(i)->getCurrentEPGEntry();
        else if (direction == 1)
            currentEPGEntry = m_tv->getChannelPointer(i)->getNextEPGEntry();
        else
            currentEPGEntry = m_tv->getChannelPointer(i)->getPrevEPGEntry();

        ProGuideEntry *p;
        if ((p = findChannel(m_tv->getChannelPointer(i)))) {
            p->update(currentEPGEntry);
        }
        else
            p = new ProGuideEntry(m_list, m_tv->getChannelPointer(i), currentEPGEntry);

        if (i == m_tv->getCurrentChannel()) {
            m_cur = p;
        }
    }
}


void
MenuProGuide::action()
{
    showCurrentProgram();
}


void
MenuProGuide::selectDefault()
{
    m_list->setCurrentItem(m_cur);
    m_list->setSelected(m_cur, true);
}


void
MenuProGuide::showChannelMenu(QListViewItem *channel)
{
    qDebug("MenuProGuide::showChannelMenu() on entry %s", channel->text(ProGuideColTitle).latin1());
    m_selectedChannel = (ProGuideEntry*)channel;
    QPopupMenu *p = new QPopupMenu();
    p->insertItem("Add Timer", this, SLOT(channelMenuTimer()));
    p->insertItem("Switch to Channel", this, SLOT(channelMenuSwitch()));
//    p->insertItem("Show Channel", this, SLOT(channelMenuShow()));
    QSize center = m_list->size()/2 - p->sizeHint()/2;
    p->popup(m_list->mapToGlobal(QPoint(center.width(), center.height())));
    p->setActiveItem(0);
}


void
MenuProGuide::channelMenuTimer()
{
    TVTimer *timer;
    EPGEntry *e = m_selectedChannel->getEPGEntry();
    timer = new TVTimer(QString(""), m_selectedChannel->getChannel()->getId(), new TimerDay(e->getStartTime()), new TimerTime(e->getStartTime()), new TimerTime(e->getEndTime()), 1, 50, 90, e->getTitle());
    m_tv->newTimer(timer);
}


void
MenuProGuide::channelMenuSwitch()
{
    m_tv->setCurrentChannel(m_tv->getChannelNumber(m_selectedChannel->getChannel()->getId()));
    m_tv->startLiveTV();
    m_controler->showMenu(m_tv);
}

void
MenuProGuide::channelMenuShow()
{
    //TODO: show EPG entries of one channel.
}


ProGuideEntry *
MenuProGuide::findChannel(TVChannel *channel)
{
    QListViewItemIterator it(m_list);

    while ((*it)) {
        if (((ProGuideEntry*)(*it))->getChannel() == channel)
            return (ProGuideEntry*)(*it);
        it++;
    }
    return 0;
}


ProGuideEventFilter::ProGuideEventFilter(MenuProGuide *controler, QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_controler = controler;
}


ProGuideEventFilter::~ProGuideEventFilter()
{
}


bool
ProGuideEventFilter::eventFilter(QObject *o, QEvent *e)
{
    //qDebug( "ProGuideEventFilter::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        // special processing for key press
        QKeyEvent *k = (QKeyEvent *)e;
        //qDebug( "ProGuideEventFilter::eventFilter() key press %d", k->key() );
        switch (k->key()) {
        case Qt::Qt::Key_Right:                               //
            qDebug("ProGuideEventFilter::eventFilter() show next program");
            m_controler->showNextProgram();
            return TRUE;
        case Qt::Qt::Key_Left:                               //
            qDebug("ProGuideEventFilter::eventFilter() show previous program");
            m_controler->showPrevProgram();
            return TRUE;
        default:
            return FALSE;
        }
    } else {
        // standard event processing
        return FALSE;
    }
}


ProGuideEntry::ProGuideEntry(QListView *parent, TVChannel *channel, EPGEntry *epgEntry)
 : QListViewItem(parent, "", "", "", "", "")
{
    m_channel = channel;
    setText(MenuProGuide::ProGuideColNum, m_channel->getIdStr());
    setText(MenuProGuide::ProGuideColChan, m_channel->getName());
    update(epgEntry);
}


ProGuideEntry::~ProGuideEntry()
{
}


void
ProGuideEntry::update(EPGEntry *epgEntry)
{
    m_epgEntry = epgEntry;
    if (m_epgEntry) {
        setText(MenuProGuide::ProGuideColStart, m_epgEntry->getStartTimeStr());
        setText(MenuProGuide::ProGuideColEnd, m_epgEntry->getEndTimeStr());
        setText(MenuProGuide::ProGuideColTitle, m_epgEntry->getTitle());
    }
}
