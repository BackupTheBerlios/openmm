/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
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
#ifndef QTSTYLE_H
#define QTSTYLE_H

#include "qfont.h"
#include "qcolor.h"

/**
  Themeing information for all Qt-Widgets.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class QtStyle{
public:
    static QtStyle* instance();

    QFont bigFont()             { return m_bigFont; }
    QFont mediumFont()          { return m_mediumFont; }
    QFont miniFont()            { return m_miniFont; }
    QColor backgroundColor()    { return m_backgroundColor; }
    QColor foregroundColor()    { return m_foregroundColor; }

private:
    QtStyle();
    ~QtStyle();

    static QtStyle*     m_instance;
    QFont               m_bigFont;
    QFont               m_mediumFont;
    QFont               m_miniFont;
    QColor              m_backgroundColor;
    QColor              m_foregroundColor;
};

#endif
