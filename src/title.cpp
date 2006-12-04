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
#include "title.h"

Title::Title(QString name, TitleT type)
 : QObject()
{
    m_name = name;
    m_type = type;
    m_mrl = 0;
    m_header = QStringList("Name");
}


Title::~Title()
{
}


const QString Title::headDelimiter = ";";


QString
Title::getText(QString col)
{
    if (col == colName(0)) {
        return m_name;
    }
    return getColText(col);
}


void
Title::setText(QString col, QString text)
{
    if (col == colName(0)) {
        m_name = text;
    } else {
        setColText(col, text);
    }
}


void
Title::setHeader(QString header)
{
    m_header = QStringList("Name") + QStringList::split(headDelimiter, header); 
}


// void
// Title::appendCol(QString name)
// {
//     m_header.append(name);
// }


bool
Title::match(Title *other)
{
    QDictIterator<int> i(m_idents);
    bool commonIdents = false;

    for( ; i.current(); ++i ) {
        int otherId = other->getId(i.currentKey());
        if (otherId > -1) {
            commonIdents = true;
            if (otherId != getId(i.currentKey())) {
                return false;
            }
        }
    }
    return commonIdents;
}
