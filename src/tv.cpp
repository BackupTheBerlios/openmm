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
#include "tv.h"


TvRec::TvRec(QString id, QString day, QString start, QString title)
{
    m_id = id;
    m_day = day;
    m_start = start;
    m_title = title;
}


QString
TvRec::getIdStr()
{
    return m_id.rightJustify(2, '0');
}


void
TvRec::setRecFiles(QStringList fileList)
{
    qDebug("TvRec::setRecFiles() first entry of fileList: %s", (*fileList.begin()).latin1());
    m_fileList = fileList;
}


TimerTime::TimerTime(QString time)
{
    // TODO: range checking
    m_hour = time.left(2).toInt();
    m_min = time.right(2).toInt();
}


TimerTime::TimerTime(time_t t)
{
    QDateTime date;
    date.setTime_t(t);
    m_hour = date.time().hour();
    m_min = date.time().minute();
}


QString
TimerTime::str()
{
    return QString().sprintf("%02i%02i", m_hour, m_min);
}


TimerDay::TimerDay(QString day)
{
    m_day = day;
}


TimerDay::TimerDay(time_t t)
{
    QDateTime date;
    date.setTime_t(t);
    m_day = date.toString("yyyy-MM-dd");
}


TvTimer::TvTimer(QString id, QString channelId, TimerDay *day, TimerTime *start, TimerTime *end, int active, int prio, int resist, QString title)
{
    m_id = id;
    m_channelId = channelId;
    m_day = day;
    m_start = start;
    m_end = end;
    m_active = active;
    m_prio = prio;
    m_resist = resist;
    m_title = title;
}


EpgEntry::EpgEntry(QString id, time_t start, time_t duration, QString title, QString shortText, QString description)
{
    m_id = id;
    m_start = start;
    m_duration = duration;
    m_title = title;
    m_shortText = shortText;
    m_description = description;
}


void
EpgEntry::setTitle(QString title)
{
    m_title = title;
}


void
EpgEntry::setShortText(QString shortText)
{
    m_shortText = shortText;
}


void
EpgEntry::setDescription(QString description)
{
    m_description = description;
}


QString
EpgEntry::getStartTimeStr()
{
    return timeStr(m_start);
}


QString
EpgEntry::getEndTimeStr()
{
    return timeStr(m_start + m_duration);
}


QString
EpgEntry::timeStr(time_t t)
{
    QDateTime date;
    date.setTime_t(t);
    return date.toString("hh:mm");
}


TvChannel::TvChannel(QString id, QString name, QString signature)
{
    m_id = id;
    m_name = name;
    m_signature = signature;
    m_current = 0;
}


QString
TvChannel::getIdStr()
{
    return m_id.rightJustify(2, '0');
}


void
TvChannel::appendEpgEntry(EpgEntry *epgEntry)
{
    m_epg.append(epgEntry);
}


EpgEntry*
TvChannel::getEpgEntry(time_t at)
{
    // iterate through all Epg entries, to find the one at time at.
    int index = 0;
    for (EpgListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
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


EpgEntry*
TvChannel::getCurrentEpgEntry()
{
    QDateTime now = QDateTime::currentDateTime();
    time_t now_t = now.toTime_t();

    // iterate through all Epg entries, to find the current one.
    int index = 0;
    for (EpgListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
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


EpgEntry*
TvChannel::getNextEpgEntry()
{
    if (m_current < m_epg.count()-1)
        m_current++;
    return m_epg.at(m_current);
}


EpgEntry*
TvChannel::getPrevEpgEntry()
{
    if (m_current > 0)
        m_current--;
    return m_epg.at(m_current);
}


Tv::Tv(GlobalKeyHandler *keyh, QWidget *parent, const char *name) : Menu(parent, name)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);

    m_name = "Live Tv";
    m_isPlaying = false;
    m_streamPlayer = StreamPlayerXine::getInstance(this);
    installEventFilter(keyh);
    m_streamPlayer->setFocus();
    m_currentChannel = 0;
    m_numberChannels = 0;
    m_numberTimers = 0;
    m_timerList.setAutoDelete(true);
    m_recList.setAutoDelete(true);
    m_controlVDR = new Svdrp("192.168.178.10", 2001);
    m_controlVDR->getChannels(this);  // this call is asynchronous
    // FIX: need to synchronize these two calls somehow !!!!!
    m_controlVDR->getEpg(this);
    //m_controlVDR->getTimers(this);
    //m_controlVDR->getRecs(this);
    m_vdrRecs = new VdrRecs("/video");
}


Tv::~Tv()
{
}


void
Tv::insertChannel(int channelNumber, TvChannel *channel)
{
    m_channelList.insert(channelNumber, channel);
    m_numberChannels++;
}


void
Tv::appendChannel(TvChannel *channel)
{
    m_channelList.append(channel);
    m_numberChannels++;
}


void
Tv::setCurrentChannel(int channelNumber)
{
    m_currentChannel = channelNumber;
    qDebug("Tv::setCurrentChannel() to: %s", getChannelName(channelNumber).latin1());
    qDebug("Tv::setCurrentChannel() title: %s", m_channelList.at(m_currentChannel)->getCurrentEpgEntry()->getTitle().latin1());
    qDebug("Tv::setCurrentChannel() description: %s", m_channelList.at(m_currentChannel)->getCurrentEpgEntry()->getDescription().latin1());
}


void
Tv::startLiveTv()
{
    m_isPlaying = true;
    m_streamPlayer->play(m_streamPlayer->tvMRL(getChannelId(getCurrentChannel())));
    m_streamPlayer->showOSD(getChannelName(getCurrentChannel()) + "   " + getCurrentChannelPointer()->getCurrentEpgEntry()->getTitle(), 5000);
}


void
Tv::stopLiveTv()
{
    m_isPlaying = false;
    m_streamPlayer->stop();
}


QString
Tv::getChannelId(int channelNumber)
{
    return m_channelList.at(channelNumber)->getId();
}


int
Tv::getChannelNumber(QString channelId)
{
    TvChannel *ch;
    int i = 0;
    for (ch = m_channelList.first(); ch; ch = m_channelList.next()) {
        if (ch->getId() == channelId)
            return i;
        i++;
    }
    return -1;
}


QString
Tv::getChannelName(int channelNumber)
{
    return m_channelList.at(channelNumber)->getName();
}


TvChannel*
Tv::getChannelPointer(int channelNumber)
{
    return m_channelList.at(channelNumber);
}


TvChannel*
Tv::getChannelPointer(QString channelSignature)
{
    //qDebug("Tv::getChannelPointer(), channelSignature: %s", channelSignature.latin1());
    for (ChannelListT::iterator it = m_channelList.begin(); it != m_channelList.end(); ++it)
    {
        //qDebug("Tv::getChannelPointer(), checking channel: %s", (*it)->getName().latin1());
        if ((*it)->getSignature() == channelSignature)
        {
            //qDebug("Tv::getChannelPointer(), got channel: %s", (*it)->getName().latin1());
            return (*it);
        }
    }
    //qDebug("Tv::getChannelPointer(), channel not found!");
    return NULL;
}


TvChannel*
Tv::getCurrentChannelPointer()
{
    return m_channelList.at(m_currentChannel);
}


int
Tv::getCurrentChannel()
{
    return m_currentChannel;
}


int
Tv::getChannelCount()
{
    return m_channelList.count();
}


TvTimer*
Tv::getTimerPointer(int timerNumber)
{
    return m_timerList.at(timerNumber);
}


int
Tv::getTimerCount()
{
    return m_timerList.count();
}


void
Tv::keyPressEvent(QKeyEvent *k)
{
    qDebug("Tv::keyPressEvent()");
    switch (k->key()) {
        case Qt::Key_I:                               // info
        case Qt::Key_Return:
        case Qt::Key_Enter:
            m_streamPlayer->showOSD(getChannelName(getCurrentChannel()) + "   " + getCurrentChannelPointer()->getCurrentEpgEntry()->getTitle(), 5000);
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
    stopLiveTv();
    startLiveTv();
}


void
Tv::action()
{
    qDebug("Tv::action()");
    if (!m_isPlaying)
        startLiveTv();
}


void
Tv::selectDefault()
{
}


void
Tv::appendTimer(TvTimer *timer)
{
    m_timerList.append(timer);
    m_numberTimers++;
    //m_controlVDR->setTimer(this, timer);
}


void
Tv::newTimer(TvTimer *timer)
{
    //m_timerList.append(timer);
    //m_numberTimers++;
    m_controlVDR->setTimer(this, timer);
}


void
Tv::delTimer(TvTimer *timer)
{
    //m_timerList.append(timer);
    //m_numberTimers++;
    m_controlVDR->delTimer(this, timer);
}


TvRec*
Tv::getRecPointer(int recNumber)
{
    return m_recList.at(recNumber);
}


int
Tv::getRecCount()
{
    return m_recList.count();
}


void
Tv::appendRec(TvRec *rec)
{
    m_recList.append(rec);
    m_numberRecs++;
}


void
Tv::delRec(TvRec *rec)
{
    m_controlVDR->delRec(this, rec);
}


void
Tv::updateEpg()
{
    m_controlVDR->getEpg(this);
}


void
Tv::clearTimers()
{
    m_timerList.clear();
}


void
Tv::updateTimers()
{
    clearTimers();
    m_controlVDR->getTimers(this);
}


void
Tv::clearRecs()
{
    m_recList.clear();
}


void
Tv::updateRecs()
{
    clearRecs();
    //m_controlVDR->getRecs(this);
    m_vdrRecs->getRecs(this);
}
