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
#ifndef TVREC_H
#define TVREC_H

#include "title.h"


class TvRec : public Title
{
public:
    TvRec(string id, string name, string day, string start);

    //virtual int getId( string col );

/*
    string getId() { return m_id; }
    string getIdStr();
    string getDayStr() { return m_day; }
    string getStartStr() { return m_start; }
    string getTitle() { return m_title; }
    string getFirstRec() { return m_fileList[0]; }
    void setRecFiles(vector<string> fileList);
*/

protected:
    virtual string getColText(string col);
    virtual void setColText(string col, string text);

private:
    string m_id;
    string m_day;
    string m_start;
//    vector<string> m_fileList;
};

/*
class TvRecList
{
public:
    TvRecList();
    ~TvRecList();

    void clearRecList();
    void updateRecList();
    void appendRec(TvRec *rec);
    void delRec(TvRec *rec);
    int getRecCount();
    TvRec* getRecPointer(int recNumber);

private:
    typedef QPtrList<TvRec> RecListT;

    RecListT m_recList;
    int m_numberRecs;
};
*/
#endif
