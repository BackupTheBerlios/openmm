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
#ifndef TVTIMER_H
#define TVTIMER_H

#include "title.h"
#include "tvchannel.h"
#include "tvprogram.h"


class TimerTime
{
public:
    TimerTime(QString time);
    TimerTime(time_t t);

    QString str();

private:
    int m_hour;
    int m_min;
};


class TimerDay
{
public:
    TimerDay(QString day);
    TimerDay(time_t t);

    QString str() { return m_day; }

private:
    QString m_day;
};


class TvTimer : public Title
{
public:
    TvTimer(QString name, QString id, QString channelId, TimerDay *day, TimerTime *start, TimerTime *end,
            int active, int prio, int resist);
    TvTimer(TvChannel *channel, TvProgram *program); // TODO: why the heck is TvProgram not recognized as a type?

    //virtual int getId(QString col);

protected:
    virtual QString getColText(QString col);
    virtual void setColText(QString col, QString text);

private:
    // TODO: m_header should be static in the derived class (headers are the same for all TvTimers), to save memory.
    QString m_id;
    QString m_channelId;
    TimerDay *m_day;
    TimerTime *m_start;
    TimerTime *m_end;
    int m_active;
    int m_prio;
    int m_resist;
    QString m_title;
};


#endif
