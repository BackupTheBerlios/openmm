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

#include "tvrec.h"


TvRec::TvRec(string id, string name, string day, string start)
 : Title(name, Title::TvRecT)
{
    m_id = id;
    m_day = day;
    m_start = start;
    setHeader("Id;Day;Start");
}


string
TvRec::getColText(string col)
{
    if (col == colName(1)) {
        return m_id.size()==2?m_id:"0"+m_id;
    }
    else if (col == colName(2)) {
        return m_day;
    }
    else if (col == colName(3)) {
        return m_start;
    }
    return "";
}


void
TvRec::setColText(string col, string text)
{
// TODO: convert strings into appropriate values for column fields.
}
