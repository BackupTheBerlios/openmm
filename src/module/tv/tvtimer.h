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


class TvTimerTime
{
public:
    TvTimerTime(string time);
    TvTimerTime(time_t t);

    string str();

private:
    int m_hour;
    int m_min;
};


class TvTimerDay
{
public:
    TvTimerDay(string day);
    TvTimerDay(time_t t);

    string str() { return m_day; }

private:
    string m_day;
};


class TvTimer : public Title
{
public:
    TvTimer(string name, string id, string channelId, TvTimerDay *day, TvTimerTime *start, TvTimerTime *end,
            int active, int prio, int resist);
    TvTimer(TvChannel *channel, TvProgram *program); // TODO: why the heck is TvProgram not recognized as a type?

    //virtual int getId(string col);

protected:
    virtual string getColText(string col);
    virtual void setColText(string col, string text);

private:
    // TODO: m_header should be static in the derived class (headers are the same for all TvTimers), to save memory.
    string m_id;
    string m_channelId;
    TvTimerDay *m_day;
    TvTimerTime *m_start;
    TvTimerTime *m_end;
    int m_active;
    int m_prio;
    int m_resist;
    string m_title;
};


#endif
