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
#include "titlepair.h"

TitlePair::TitlePair(Title *left, Title *right)
 : Title("", Title::TitlePairT)
{
    //qDebug("TitlePair::TitlePair() of left: %s, right: %s", left->getText("Name").latin1(), 
    //      right->getText("Name").latin1());
    m_left = left;
    m_right = right;

    if (left && right) { // left and right are defined.
        // union of both headers (with duplicates).
        m_header = left->getHeader() + right->getHeader();
        // first valid Mrl (from left to right).
        m_mrl = left->getMrl()?left->getMrl():right->getMrl();
        setText("Name", left->getText("Name") + "#" + right->getText("Name"));
    }
    else if (left || right) {  // exactly one of left or right is defined.
        m_header = left?left->getHeader():right->getHeader();
        m_mrl = left?left->getMrl():right->getMrl();
        left?setText("Name", left->getText("Name")):setText("Name", right->getText("Name"));
    }
}


TitlePair::~TitlePair()
{
}


// return first fitting column (from left to right).
QString
TitlePair::getColText(QString col)
{
    //qDebug("TitlePair::getColText() left # right: %s # %s", m_left->getText(col).latin1(), m_right->getText(col).latin1());
    if (m_left && col == "Left.Name") {
        return m_left->getText("Name");
    }
    if (m_right && col == "Right.Name") {
        return m_right->getText("Name");
    }
    if (m_left && m_left->getText(col) != "") {
        return m_left->getText(col);
    }
    if (m_right) {
        return m_right->getText(col);
    }
    return "";
}


// set each fitting column.
void
TitlePair::setColText(QString col, QString text)
{
    if (m_left) {
        m_left->setText(col, text);
    }
    if (m_right) {
        m_right->setText(col, text);
    }
}
