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
    TvProgram(QString id, time_t start, time_t duration, QString title = "", QString shortText = "", 
                QString description =  "", QString channel = "");
   ~TvProgram();

    time_t getStart() { return m_start; }
    time_t getEnd() { return m_start + m_duration; }
    //virtual int getId(QString col);

protected:
    virtual QString getColText(QString col);
    virtual void setColText( QString col, QString text );

private:
    QString m_id;
    time_t m_start;
    time_t m_duration;
    QString m_shortText;
    QString m_description;
    QString m_channel;

    QString timeStr(time_t t);
};

#endif
