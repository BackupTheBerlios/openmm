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


TvRec::TvRec(QString id, QString name, QString day, QString start)
 : Title(name, Title::TvRecT)
{
    m_id = id;
    m_day = day;
    m_start = start;
    setHeader("Id;Day;Start");
}


QString
TvRec::getColText(QString col)
{
    if (col == colName(1)) {
        return m_id.rightJustify(2, '0');
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
TvRec::setColText(QString col, QString text)
{
// TODO: convert strings into appropriate values for column fields.
}


// int
// TvRec::getId(QString col)
// {
//     return 0;
// }

/*
QString
TvRec::getIdStr()
{
    return m_id.rightJustify(2, '0');
}


void
TvRec::setRecFiles(QStringList fileList)
{
    qDebug("TvRec::setRecFiles() first entry of fileList: %s", (*fileList.begin()).latin1());
    m_fileList = fileList;
}


TvRecList::TvRecList()
{
}


TvRecList::~TvRecList()
{
}


void
TvRecList::clearRecList()
{
    m_recList.clear();
}


void
TvRecList::updateRecList()
{
    clearRecList();
    //m_controlVDR->getRecs(this);
    m_vdrRecs->getRecs(this);
}


TvRec*
TvRecList::getRecPointer(int recNumber)
{
    return m_recList.at(recNumber);
}


int
TvRecList::getRecCount()
{
    return m_recList.count();
}


void
TvRecList::appendRec(TvRec *rec)
{
    m_recList.append(rec);
    m_numberRecs++;
}


void
TvRecList::delRec(TvRec *rec)
{
    m_controlVDR->delRec(this, rec);
}
*/
