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
#ifndef TVPROGRAMFILTER_H
#define TVPROGRAMFILTER_H

#include <tvprogram.h>
#include <titlefilter.h>

/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class TvProgramFilter : public TitleFilter
{
public:
    TvProgramFilter();
    ~TvProgramFilter();

    void setTime(time_t at) { m_at = at; }
    virtual bool passTitle(Title *title);

private:
    time_t m_at;
};

#endif