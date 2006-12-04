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
#ifndef TITLEPAIR_H
#define TITLEPAIR_H

#include <title.h>

/**
Entries for the ListComposer. Each Entry is of type Title and is composed of a left and a right Title.

	@author Jörg Bakker <joerg@hakker.de>
*/

// TODO: add the union of left and right identifiers.

class TitlePair : public Title
{
public:
    TitlePair(Title *left, Title *right = 0);
    ~TitlePair();

    Title *getLeft() { return m_left; }
    Title *getRight() { return m_right; }

protected:
    virtual QString getColText(QString col);
    virtual void setColText(QString col, QString text);

private:
    Title *m_left;
    Title *m_right;
};

#endif
