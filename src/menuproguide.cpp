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

MenuProGuide::MenuProGuide(JAMTV *tv, GlobalKeyHandler *keyh, QWidget *parent, const char *name)
 : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Program Guide";
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
        qDebug("MenuProGuide::showProgram() channel %s", m_tv->getChannelName(i).latin1());
        EPGEntry *currentEPGEntry;
        if (direction == 0)
            currentEPGEntry = m_tv->getChannelPointer(i)->getCurrentEPGEntry();
        else if (direction == 1)
            currentEPGEntry = m_tv->getChannelPointer(i)->getNextEPGEntry();
        else
            currentEPGEntry = m_tv->getChannelPointer(i)->getPrevEPGEntry();

        QString currentEPGStart, currentEPGEnd, currentEPGTitle;
        if (currentEPGEntry)
        {
            currentEPGStart = currentEPGEntry->getStartTimeStr();
            currentEPGEnd = currentEPGEntry->getEndTimeStr();
            currentEPGTitle = currentEPGEntry->getTitle();
        }
        QString channelNr = QString().setNum(i+1).rightJustify(2, '0');

        QListViewItem *p;
        // FIX: what if channel has no (empty) channel name?
        if ((p = m_list->findItem(m_tv->getChannelName(i), ProGuideColChan))) {
            p->setText(ProGuideColStart, currentEPGStart);
            p->setText(ProGuideColEnd, currentEPGEnd);
            p->setText(ProGuideColTitel, currentEPGTitle);
        }
        else
            p = new QListViewItem(m_list, channelNr, m_tv->getChannelName(i), currentEPGStart, currentEPGEnd, currentEPGTitle);
        /*
        if (i == m_tv->getCurrentChannel()) {
            m_list->setCurrentItem(p);
            m_list->setSelected(p, true);
        }
        */
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
    m_list->setCurrentItem(m_list->firstChild());
    m_list->setSelected(m_list->firstChild(), true);

}


void
MenuProGuide::selectCurrent()
{
//    m_list->setCurrentItem(m_list->at(m_currentItem));
//    m_list->setSelected(m_list->at(m_currentItem), true);
}


void
MenuProGuide::showChannelMenu(QListViewItem *channel)
{
    qDebug("MenuProGuide::showChannelMenu() on entry %s", channel->text(ProGuideColTitel).latin1());
    QPopupMenu *p = new QPopupMenu();
    p->insertItem("Timer");
    p->insertItem("Switch to Channel");
    p->insertItem("Show Channel");
    p->exec(m_list->mapToGlobal(QPoint(0, 0)));
}


ProGuideEventFilter::ProGuideEventFilter(MenuProGuide *controler, QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_controler = controler;
}


ProGuideEventFilter::~ProGuideEventFilter()
{
}

#undef KeyPress

bool
ProGuideEventFilter::eventFilter(QObject *o, QEvent *e)
{
    qDebug( "ProGuideEventFilter::eventFilter() event type %d", e->type() );
    if ( e->type() == QEvent::KeyPress ) {
        // special processing for key press
        QKeyEvent *k = (QKeyEvent *)e;
        qDebug( "ProGuideEventFilter::eventFilter() key press %d", k->key() );
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
