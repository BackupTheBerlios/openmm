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
#ifndef TITLE_H
#define TITLE_H

#include "mrl.h"

#include <vector>
#include <map>
#include <string>
using namespace std;

/**
An entity that represents the meta data of a playable or other handlable item,
for example a TV channel, a video file, a recording entry, but also a timer entry, ...

Titles can be browsed with ListBrowser and played with StreamPlayer.

Lists of Titles are produced by a TitleManager.


	@author Jörg Bakker <joerg@hakker.de>
*/
class Title
{
public:
    enum TitleT {AnyT, TitlePairT, FileT, TvChannelT, TvProgramT, TvTimerT, TvRecT, DvdT};

    Title(string name, TitleT type);
    ~Title();

    void setText(string col, string text);
    void setId(string col, int id) { m_idents[col] = id; }
    void setMrl(Mrl *mrl) { m_mrl = mrl; }
    string getText(string col);
    int getId(string col) { return (m_idents.find(col) != m_idents.end())?(m_idents[col]):-1; }
    Mrl *getMrl() { return m_mrl; }
    TitleT getType() { return m_type; }
    vector<string> getHeader() { return m_header; }
    string colName(int i) { return m_header[i]; }
    bool match(Title *other);

    static const string headDelimiter;

protected:
    virtual string getColText(string col) { return ""; }
    virtual void setColText(string col, string text) {}
    void setHeader(string header);

    vector<string> m_header;
    map<string,int> m_idents;
    Mrl *m_mrl;

private:
    string m_name;
    TitleT m_type;
};

#endif
