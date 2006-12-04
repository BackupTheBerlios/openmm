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
#include "titlefilter.h"
#include "titlepair.h"


TitleFilter::TitleFilter()
{
}


TitleFilter::~TitleFilter()
{
}


bool
TitleFilter::pass(Title *title)
{
//    qDebug("TitleFilter::pass()");
    // if title is a TitlePair, recurse down left and right Title and return if one of them passes.
    if (title->getType() == Title::TitlePairT) {
        //qDebug("TitleFilter::pass(), type: TitlePair, left: %i,right: %i", pass(((TitlePair*)title)->getLeft())?1:0, 
        //    pass(((TitlePair*)title)->getRight())?1:0);
        return pass(((TitlePair*)title)->getLeft()) || pass(((TitlePair*)title)->getRight());
    }
    else {
        return passTitle(title);
    }
}
