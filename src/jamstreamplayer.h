/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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
#ifndef JAMSTREAMPLAYER_H
#define JAMSTREAMPLAYER_H

#include <qwidget.h>
#include <qstring.h>

/**
@author J?g Bakker
*/
class JAMStreamPlayer : public QWidget
{
    Q_OBJECT

public:
    JAMStreamPlayer(QWidget *parent = 0, const char *name = 0);

    ~JAMStreamPlayer();

    virtual QString tvMRL(QString channelId);
    
public slots:
    virtual void play(QString mrl);
    virtual void stop();
    
    virtual void showOSD(QString text, uint duration);
    virtual void hideOSD();
    
};

#endif
