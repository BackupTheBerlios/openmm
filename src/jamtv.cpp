/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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
#include "jamtv.h"


EPGEntry::EPGEntry(QString id, time_t start, time_t duration, QString title, QString shortText, QString description)
{
    m_id = id;
    m_start = start;
    m_duration = duration;
    m_title = title;
    m_shortText = shortText;
    m_description = description;
}


void
EPGEntry::setTitle(QString title)
{
    m_title = title;
}


void
EPGEntry::setShortText(QString shortText)
{
    m_shortText = shortText;
}


void
EPGEntry::setDescription(QString description)
{
    m_description = description;
}


QString
EPGEntry::getStartTimeStr()
{
    return timeStr(m_start);
}


QString
EPGEntry::getEndTimeStr()
{
    return timeStr(m_start + m_duration);
}


QString
EPGEntry::timeStr(time_t t)
{
    QDateTime date;
    date.setTime_t(t);
    return date.toString("hh:mm");
}


TVChannel::TVChannel(QString id, QString name, QString signature)
{
    m_id = id;
    m_name = name;
    m_signature = signature;
    m_current = 0;
}

QString
TVChannel::getId()
{
    return m_id;
}


QString
TVChannel::getName()
{
    return m_name;
}


QString
TVChannel::getSignature()
{
    return m_signature;
}


void
TVChannel::appendEPGEntry(EPGEntry *epgEntry)
{
    m_epg.append(epgEntry);
}


EPGEntry*
TVChannel::getEPGEntry(time_t at)
{
    // iterate through all EPG entries, to find the one at time at.
    int index = 0;
    for (EPGListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
    {
        if ((*it)->getStartTime() + (*it)->getDurationTime() > at)
        {
            m_current = index;
            return (*it);
        }
        index++;
    }
    return NULL;
}


EPGEntry*
TVChannel::getCurrentEPGEntry()
{
    QDateTime now = QDateTime::currentDateTime();
    time_t now_t = now.toTime_t();

    // iterate through all EPG entries, to find the current one.
    int index = 0;
    for (EPGListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
    {
        if ((*it)->getStartTime() + (*it)->getDurationTime() > now_t)
        {
            m_current = index;
            return (*it);
        }
        index++;
    }
    return NULL;
}


EPGEntry*
TVChannel::getNextEPGEntry()
{
    if (m_current < m_epg.count()-1)
        m_current++;
    return m_epg.at(m_current);
}


EPGEntry*
TVChannel::getPrevEPGEntry()
{
    if (m_current > 0)
        m_current--;
    return m_epg.at(m_current);
}


JAMTV::JAMTV(GlobalKeyHandler *keyh, QWidget *parent, const char *name) : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Live TV";
    m_isPlaying = false;
    m_streamPlayer = new JAMStreamPlayerXine(this);
    installEventFilter(keyh);
    m_streamPlayer->setFocus();
    m_currentChannel = 0;
    m_numberChannels = 0;
    m_controlVDR = new SVDRP("192.168.178.10", 2001);
    m_controlVDR->getChannels(this);  // this call is asynchronous
    // FIX: need to synchronize these two calls somehow !!!!!
    m_controlVDR->getEPG(this);
}


JAMTV::~JAMTV()
{
}


void
JAMTV::insertChannel(int channelNumber, TVChannel *channel)
{
    m_channelList.insert(channelNumber, channel);
    m_numberChannels++;
}


void
JAMTV::appendChannel(TVChannel *channel)
{
    m_channelList.append(channel);
    m_numberChannels++;
}


void
JAMTV::setCurrentChannel(int channelNumber)
{
    m_currentChannel = channelNumber;
    qDebug("JAMTV::setCurrentChannel() to: %s", getChannelName(channelNumber).latin1());
    qDebug("JAMTV::setCurrentChannel() title: %s", m_channelList.at(m_currentChannel)->getCurrentEPGEntry()->getTitle().latin1());
    qDebug("JAMTV::setCurrentChannel() description: %s", m_channelList.at(m_currentChannel)->getCurrentEPGEntry()->getDescription().latin1());
}


void
JAMTV::startLiveTV()
{
    //m_streamplayer->play("file:///data/video/001.vdr");
    m_isPlaying = true;
    m_streamPlayer->play(m_streamPlayer->tvMRL(getChannelId(getCurrentChannel())));
    m_streamPlayer->showOSD(getChannelName(getCurrentChannel()) + "   " + getCurrentChannelPointer()->getCurrentEPGEntry()->getTitle(), 5000);
}


void
JAMTV::stopLiveTV()
{
    m_isPlaying = false;
    m_streamPlayer->stop();
}


QString
JAMTV::getChannelId(int channelNumber)
{
    return m_channelList.at(channelNumber)->getId();
}


QString
JAMTV::getChannelName(int channelNumber)
{
    return m_channelList.at(channelNumber)->getName();
}


TVChannel*
JAMTV::getChannelPointer(int channelNumber)
{
    return m_channelList.at(channelNumber);
}


TVChannel*
JAMTV::getChannelPointer(QString channelSignature)
{
    //qDebug("JAMTV::getChannelPointer(), channelSignature: %s", channelSignature.latin1());
    for (ChannelListT::iterator it = m_channelList.begin(); it != m_channelList.end(); ++it)
    {
        //qDebug("JAMTV::getChannelPointer(), checking channel: %s", (*it)->getName().latin1());
        if ((*it)->getSignature() == channelSignature)
        {
            //qDebug("JAMTV::getChannelPointer(), got channel: %s", (*it)->getName().latin1());
            return (*it);
        }
    }
    //qDebug("JAMTV::getChannelPointer(), channel not found!");
    return NULL;
}


TVChannel*
JAMTV::getCurrentChannelPointer()
{
    return m_channelList.at(m_currentChannel);
}


int
JAMTV::getCurrentChannel()
{
    return m_currentChannel;
}

int
JAMTV::getChannelCount()
{
    return m_channelList.count();
}


void
JAMTV::keyPressEvent(QKeyEvent *k)
{
    qDebug("JAMTV::keyPressEvent()");
    switch (k->key()) {
        case Qt::Key_I:                               // info
        case Qt::Key_Return:
        case Qt::Key_Enter:
            m_streamPlayer->showOSD(getChannelName(getCurrentChannel()) + "   " + getCurrentChannelPointer()->getCurrentEPGEntry()->getTitle(), 5000);
            return;
        case Qt::Key_Up:                              // channel up
            setCurrentChannel(getCurrentChannel() + 1);
            break;
        case Qt::Key_Down:                            // channel down
            setCurrentChannel(getCurrentChannel() - 1);
            break;
        case Qt::Key_1:                               // channel 1
            setCurrentChannel(0);
            break;
        case Qt::Key_2:                               // channel 2
            setCurrentChannel(1);
            break;
        case Qt::Key_3:                               // channel 3
            setCurrentChannel(2);
            break;
        case Qt::Key_4:                               // channel 4
            setCurrentChannel(3);
            break;
        case Qt::Key_5:                               // channel 5
            setCurrentChannel(4);
            break;
        case Qt::Key_6:                               // channel 6
            setCurrentChannel(5);
            break;
        case Qt::Key_7:                               // channel 7
            setCurrentChannel(6);
            break;
        case Qt::Key_8:                               // channel 8
            setCurrentChannel(7);
            break;
        case Qt::Key_9:                               // channel 9
            setCurrentChannel(8);
            break;
    }
    stopLiveTV();
    startLiveTV();
}


void
JAMTV::action()
{
    qDebug("JAMTV::action()");
    if (!m_isPlaying)
        startLiveTV();
}


void
JAMTV::selectDefault()
{
}
