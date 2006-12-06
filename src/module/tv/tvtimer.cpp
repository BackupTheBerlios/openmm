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
#include "tvtimer.h"

#include <qdatetime.h>


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


TvTimer::TvTimer(QString name, QString id, QString channelId, TimerDay *day, TimerTime *start, TimerTime *end, int active, int prio, int resist)
 : Title(name, Title::TvTimerT)
{
    m_id = id;
    m_channelId = channelId;
    m_day = day;
    m_start = start;
    m_end = end;
    m_active = active;
    m_prio = prio;
    m_resist = resist;
    setHeader("Id;Channel;Day;Time Start;Time End;Active;Priority;Resistance");
}


TvTimer::TvTimer(TvChannel *channel, TvProgram *program)
 : Title(program->getText("Name"), Title::TvTimerT)
{
    m_id = "";  // not yet known, ListManager backend will fill in the right value.
    m_day = new TimerDay(program->getStart());
    m_channelId = channel->getText("Signature");
    m_start = new TimerTime(program->getStart() - 300);  // TODO: get default timer offset from config.
    m_end = new TimerTime(program->getEnd() + 300);  // TODO: get default timer offset from config.
    m_active = 1;
    m_prio = 50;  // TODO: get these default settings from config.
    m_resist = 99;  // TODO: get these default settings from config.
    setHeader("Id;Channel;Day;Time Start;Time End;Active;Priority;Resistance");

    qDebug("TvTimer::TvTimer(TvProgram) with channelId: %s, start: %s, end: %s, day: %s", 
            m_channelId.latin1(), m_start->str().latin1(), m_end->str().latin1(), m_day->str().latin1());
}


QString
TvTimer::getColText(QString col)
{
    if (col == colName(1)) { // Id
        return m_id;
    }
    else if (col == colName(2)) {  // Channel
        return m_channelId;
    }
    else if (col == colName(3)) {  // Day
        return m_day->str();
    }
    else if (col == colName(4)) {  // Time Start
        return m_start->str();
    }
    else if (col == colName(5)) {  // Time End
        return m_end->str();
    }
    else if (col == colName(6)) {  // Active
        return QString().setNum(m_active);
    }
    else if (col == colName(7)) {  // Priority
        return QString().setNum(m_prio);
    }
    else if (col == colName(8)) {  // Resistance
        return QString().setNum(m_resist);
    }
    return "";
}


void
TvTimer::setColText(QString col, QString text)
{
// TODO: convert strings into appropriate values for column fields.
}


// int
// TvTimer::getId(QString col)
// {
//     return 0;
// }
