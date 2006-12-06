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
#ifndef TVCHANNEL_H
#define TVCHANNEL_H

#include "listproxy.h"

#include <qstring.h>
#include <qptrlist.h>
#include <qdatetime.h>


class TvChannel : public Title
{
public:
    TvChannel(QString id, QString name, QString signature = "");

    //virtual int getId(QString col);

protected:
    virtual QString getColText(QString col);
    virtual void setColText(QString col, QString text);

    //void appendProgram(Program *program);
//     EpgEntry* getEpgEntry(time_t at);
//     EpgEntry* getCurrentEpgEntry();
//     EpgEntry* getNextEpgEntry();
//     EpgEntry* getPrevEpgEntry();

private:
    QString m_id;
    QString m_signature;
//    unsigned int m_current;
};


/**
	@author Jörg Bakker <joerg@hakker.de>
*/
/*
class TvChannelList{
public:
    TvChannelList();
    ~TvChannelList();

    void insertChannel(int channelNumber, TvChannel *channel);
    void appendChannel(TvChannel *channel);
    QString getChannelId(int channelNumber);
    int getChannelNumber(QString channelId);
    QString getChannelName(int channelNumber);
    TvChannel* getChannelPointer(int channelNumber);
    TvChannel* getChannelPointer(QString channelSignature);
    int getChannelCount();
    void updateEpg();

private:
    typedef QPtrList<TvChannel> ChannelListT;

    ChannelListT m_channelList;
    int m_numberChannels;
};
*/

#endif
