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
#include "stringutil.h"
#include "debug.h"


Title::Title(string name, TitleT type)
{
    m_name = name;
    m_type = type;
    m_mrl = 0;
    m_header.push_back("Name");
}


Title::~Title()
{
    TRACE("Title::~Title()");
}


const string Title::headDelimiter = ";";


string
Title::getText(string col)
{
    if (col == colName(0)) {
        return m_name;
    }
    return getColText(col);
}


void
Title::setText(string col, string text)
{
    if (col == colName(0)) {
        m_name = text;
    } else {
        setColText(col, text);
    }
}


void
Title::setHeader(string header)
{
    //TRACE("Title::setHeader() to: %s", header.c_str());
    m_header.clear();
    StringUtil::s_split(header, headDelimiter, m_header);
    m_header.insert(m_header.begin(), "Name");
}


bool
Title::match(Title *other)
{
    //TRACE("Title::match() with title: %s", other->getText("Name").c_str());
    bool commonIdents = false;

    for(map<string,int>::iterator i = m_idents.begin(); i != m_idents.end(); ++i ) {
        int otherId = other->getId(i->first);
        if (otherId > -1) {
            commonIdents = true;
            if (otherId != i->second) {
                return false;
            }
        }
    }
    return commonIdents;
}
