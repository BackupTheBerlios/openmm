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
#include "tvprogram.h"

#include <qdatetime.h>


TvProgram::TvProgram(QString id, time_t start, time_t duration, QString name, QString shortText, QString description, QString channel)
 : Title(name, Title::TvProgramT)
{
    m_id = id;
    m_start = start;
    m_duration = duration;
    m_shortText = shortText;
    m_description = description;
    m_channel = channel;
    setHeader("Id;Start;Duration;Short Text;Description;Channel;End");
}


TvProgram::~TvProgram()
{
}


QString
TvProgram::getColText(QString col)
{
    //TRACE("TvProgram::getColText()");
    if (col == colName(1)) {
        return m_id;
    }
    else if (col == colName(2)) {
        return timeStr(m_start);
    }
    else if (col == colName(3)) {
        return timeStr(m_duration);
    }
    else if (col == colName(4)) {
        return m_shortText;
    }
    else if (col == colName(5)) {
        return m_description;
    }
    else if (col == colName(6)) {
        return m_channel;
    }
    else if (col == colName(7)) {
        return timeStr(m_start + m_duration);
    }
    return "";
}


void
TvProgram::setColText(QString col, QString text)
{
    //TRACE("TvProgram::setColText() col: %s, text: %s", col.latin1(), text.latin1());
    if (col == colName(1)) {
        m_id = text;
    }
    else if (col == colName(2)) {
    }
    else if (col == colName(3)) {
    }
    else if (col == colName(4)) {
        m_shortText = text;
    }
    else if (col == colName(5)) {
        m_description = text;
    }
    else if (col == colName(6)) {
        m_channel = text;
    }
}


QString
TvProgram::timeStr(time_t t)
{
    //TRACE("TvProgram::timeStr()");
    QDateTime date;
    date.setTime_t(t);
    return date.toString("dd@hh:mm");
}


// int
// TvProgram::getId(QString col)
// {
//     return 0;
// }
