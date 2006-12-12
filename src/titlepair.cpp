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
#include "otherutil.h"
#include "debug.h"


TitlePair::TitlePair(Title *left, Title *right)
 : Title("", Title::TitlePairT)
{
/*    TRACE("TitlePair::TitlePair() of left: %s, right: %s", left->getText("Name").c_str(), 
          right->getText("Name").c_str());*/
    m_left = left;
    m_right = right;

    if (left && right) { // left and right are defined.
        // union of both headers (with duplicates).
        // TODO: add right side of header !!! (vector + vector in STL?)
        m_header = vector<string>(left->getHeader());
        //OtherUtil::stringVectorConcat(m_header, left->getHeader(), right->getHeader());
        //m_header.assign(left->getHeader().begin(), left->getHeader().end());
        //m_header.insert(left->getHeader().end(), right->getHeader().begin(), right->getHeader().end());

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
string
TitlePair::getColText(string col)
{
    //TRACE("TitlePair::getColText() left # right: %s # %s", m_left->getText(col).c_str(), m_right->getText(col).c_str());
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
TitlePair::setColText(string col, string text)
{
    if (m_left) {
        m_left->setText(col, text);
    }
    if (m_right) {
        m_right->setText(col, text);
    }
}
