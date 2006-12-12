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
#include "tvprogramfilter.h"
#include "debug.h"

#include <qdatetime.h>

TvProgramFilter::TvProgramFilter()
 : TitleFilter()
{
    m_at = QDateTime::currentDateTime().toTime_t();
}


TvProgramFilter::~TvProgramFilter()
{
}


bool
TvProgramFilter::passTitle(Title *title)
{
//     TRACE("TvProgramFilter::pass() m_at: %i, start: %i, end: %i", m_at, ((TvProgram*)title)->getStart(),
//           ((TvProgram*)title)->getEnd());

// TODO: philosophical question: filter out if type doesn't match, or pass all other types through?
//       for now, we filter them out. Take care of TitlePairs, they must be treated accordingly.
//    if (title->getType() != Title::TvProgram) {
//        return true;
//    }
    if (((TvProgram*)title)->getStart() <= m_at && ((TvProgram*)title)->getEnd() > m_at) {
        return true;
    }
    return false;
}
