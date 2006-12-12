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
#ifndef TVPROGRAM_H
#define TVPROGRAM_H

#include "title.h"

/**
A program entry - think of EPG (Electronic Program Guide)

	@author Jörg Bakker <joerg@hakker.de>
*/

class TvProgram : public Title
{
public:
    TvProgram(string id, time_t start, time_t duration, string title = "", string shortText = "", 
                string description =  "", string channel = "");
   ~TvProgram();

    time_t getStart() { return m_start; }
    time_t getEnd() { return m_start + m_duration; }
    //virtual int getId(string col);

protected:
    virtual string getColText(string col);
    virtual void setColText( string col, string text );

private:
    string m_id;
    time_t m_start;
    time_t m_duration;
    string m_shortText;
    string m_description;
    string m_channel;

    string timeStr(time_t t);
};

#endif
