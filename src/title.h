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

#include <qobject.h>
#include <qstringlist.h>
#include <qdict.h>

/**
An entity that represents the meta data of a playable or other handlable item,
for example a TV channel, a video file, a recording entry, but also a timer entry, ...

Titles can be browsed with ListBrowser and played with StreamPlayer.

Lists of Titles are produced by a TitleManager.


	@author Jörg Bakker <joerg@hakker.de>
*/
class Title : public QObject
{
    Q_OBJECT

public:
    enum TitleT {TitlePairT, TvChannelT, TvProgramT, TvTimerT, TvRecT};

    Title(QString name, TitleT type);
    ~Title();

    void setText(QString col, QString text);
    void setId(QString col, int id) { m_idents.insert(col, new int(id)); }
    void setMrl(Mrl *mrl) { m_mrl = mrl; }
    QString getText(QString col);
    int getId(QString col) { return m_idents[col]?(*m_idents[col]):-1; }
    Mrl *getMrl() { return m_mrl; }
    TitleT getType() { return m_type; }
    QStringList getHeader() { return m_header; }
    //QStringList getHeaderIdents() { return m_headerIdents; }
    QString colName(int i) { return m_header[i]; }
    bool match(Title *other);

    static const QString headDelimiter;

protected:
    virtual QString getColText(QString col) = 0;
    virtual void setColText(QString col, QString text) = 0;
    void setHeader(QString header);
    //void appendCol(QString name);

    QStringList m_header;
    //QStringList m_headerIdents;
    QDict<int> m_idents;
    Mrl *m_mrl;

private:
    QString m_name;
    TitleT m_type;
};

#endif
