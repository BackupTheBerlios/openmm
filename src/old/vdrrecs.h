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
#ifndef VDRRECS_H
#define VDRRECS_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>

#include "tv.h"
class Tv;
class TvRec;


/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class VdrRecs : public QObject
{
Q_OBJECT
public:
    VdrRecs(QString videoPath, QObject *parent = 0, const char *name = 0);

    ~VdrRecs();

    void getRecs(Tv *tv);

private:
    QString m_videoPath;
    QDir m_videoDir;

    void getRecInfo(Tv *tv, QDir d);
};

#endif
