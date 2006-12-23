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
#include "qtstyle.h"
#include "debug.h"

#include <qfontdatabase.h>


QtStyle* QtStyle::m_instance = 0;

QtStyle*
QtStyle::instance()
{
    if (m_instance == 0) {
        return new QtStyle();
    }
    return m_instance;
}


QtStyle::QtStyle()
{
    QFontDatabase fdb;
    QFont f;
    TRACE("QtStyle::QtStyle() try to get the default font");
    f = fdb.font(QString::null, QString::null, 0);
    f.setBold(true);
    f.setPointSize(24);
    m_bigFont = f;
    f.setPointSize(18);
    m_mediumFont = f;
    f.setPointSize(12);
    m_miniFont = f;
    m_foregroundColor = QColor(Qt::gray);
    m_backgroundColor = QColor(Qt::black);
}


QtStyle::~QtStyle()
{
}
