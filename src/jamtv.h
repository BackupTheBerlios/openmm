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
#ifndef JAMTV_H
#define JAMTV_H

#include "globalkeyhandler.h"

#include <qwidget.h>
#include <qstring.h>
#include <qptrvector.h>
#include <time.h>

#include "jamstreamplayerxine.h"
#include "svdrp.h"

// forward declaration
class SVDRP;
class GlobalKeyHandler;
class JAM;

class EPGEntry
{
public:
    EPGEntry(QString id, time_t start, time_t duration, QString title = "", QString shortText = "", QString description = "");
    
    void setTitle(QString title);
    void setShortText(QString shortText);
    void setDescription(QString description);
    
    time_t getStartTime() { return m_start; }
    time_t getDurationTime() { return m_duration; }
    QString getTitle() { return m_title; }
    QString getShortText() { return m_shortText; }
    QString getDescription() { return m_description; }
    QString getStartTimeStr();
    QString getEndTimeStr();

private:
    QString m_id;
    time_t m_start;
    time_t m_duration;
    QString m_title;
    QString m_shortText;
    QString m_description;
    QString timeStr(time_t t);
};


typedef QPtrList<EPGEntry> EPGListT;

class TVChannel
{
public:
    TVChannel(QString id, QString name, QString signature = "");

    QString getId();
    QString getName();
    QString getSignature();
    void appendEPGEntry(EPGEntry *epgEntry);
    EPGEntry* getEPGEntry(time_t at);
    EPGEntry* getCurrentEPGEntry();
    EPGEntry* getNextEPGEntry();
    EPGEntry* getPrevEPGEntry();

private:
    QString m_id;
    QString m_name;
    QString m_signature;
    unsigned int m_current;

    EPGListT m_epg;
};


typedef QPtrList<TVChannel> ChannelListT;

/**
Metadata and menu for watching TV and recordings

@author Jörg Bakker
*/
class JAMTV: public Menu
{
    Q_OBJECT

public:
    JAMTV(GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~JAMTV();

    void insertChannel(int channelNumber, TVChannel *channel);
    void appendChannel(TVChannel *channel);
    void setCurrentChannel(int channelNumber);
    void startLiveTV();
    void stopLiveTV();
    QString getChannelId(int channelNumber);
    QString getChannelName(int channelNumber);
    TVChannel* getChannelPointer(int channelNumber);
    TVChannel* getChannelPointer(QString channelSignature);
    TVChannel* getCurrentChannelPointer();
    int getCurrentChannel();
    int getChannelCount();
    void action();
    void selectDefault();

protected:
    void keyPressEvent(QKeyEvent *k);

private:
    ChannelListT m_channelList;
    
    int m_currentChannel;
    int m_numberChannels;
    bool m_isPlaying;
    JAMStreamPlayer *m_streamPlayer;
    SVDRP *m_controlVDR;
};

#endif
