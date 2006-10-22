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

#include "vdrrecs.h"
#include "jamstreamplayerxine.h"
#include "svdrp.h"

// forward declaration
class SVDRP;
class VdrRecs;
class GlobalKeyHandler;
class JAM;

// TODO: MenuTimerEdit classes

class TVRec
{
public:
    TVRec(QString id, QString day, QString start, QString title);

    QString getId() { return m_id; }
    QString getIdStr();
    QString getDayStr() { return m_day; }
    QString getStartStr() { return m_start; }
    QString getTitle() { return m_title; }
    QString getFirstRec() { return m_fileList[0]; }
    void setRecFiles(QStringList fileList);

private:
    QString m_id;
    QString m_day;
    QString m_start;
    QString m_title;
    QStringList m_fileList;
};


class TimerTime
{
public:
    TimerTime(QString time);
    TimerTime(time_t t);

    QString str();

private:
    int m_hour;
    int m_min;
};


class TimerDay
{
public:
    TimerDay(QString day);
    TimerDay(time_t t);

    QString str() { return m_day; }

private:
    QString m_day;
};


class TVTimer
{
public:
    TVTimer(QString id, QString channelId, TimerDay *day, TimerTime *start, TimerTime *end, int active, int prio, int resist, QString title);

    QString getId() { return m_id; }
    QString getChannelId() { return m_channelId; }
    QString getDayStr() { return m_day->str(); }
    QString getStartStr() { return m_start->str(); }
    QString getEndStr() { return m_end->str(); }
    QString getActiveStr() { return QString().setNum(m_active); }
    QString getPrioStr() { return QString().setNum(m_prio); }
    QString getResistStr() { return QString().setNum(m_resist); }
    QString getTitle() { return m_title; }

private:
    QString m_id;
    QString m_channelId;
    TimerDay *m_day;
    TimerTime *m_start;
    TimerTime *m_end;
    int m_active;
    int m_prio;
    int m_resist;
    QString m_title;
};


class EPGEntry
{
public:
    EPGEntry(QString id, time_t start, time_t duration, QString title = "", QString shortText = "", QString description = "");
    
    void setTitle(QString title);
    void setShortText(QString shortText);
    void setDescription(QString description);
    
    time_t getStartTime() { return m_start; }
    time_t getDurationTime() { return m_duration; }
    time_t getEndTime() { return m_start + m_duration; }
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


class TVChannel
{
public:
    TVChannel(QString id, QString name, QString signature = "");

    QString getId() { return m_id; }
    QString getIdStr();
    QString getName() { return m_name; }
    QString getSignature() { return m_signature; }
    void appendEPGEntry(EPGEntry *epgEntry);
    EPGEntry* getEPGEntry(time_t at);
    EPGEntry* getCurrentEPGEntry();
    EPGEntry* getNextEPGEntry();
    EPGEntry* getPrevEPGEntry();

private:
    typedef QPtrList<EPGEntry> EPGListT;
    EPGListT m_epg;

    QString m_id;
    QString m_name;
    QString m_signature;
    unsigned int m_current;
};


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
    QString getChannelId(int channelNumber);
    int getChannelNumber(QString channelId);
    QString getChannelName(int channelNumber);
    TVChannel* getChannelPointer(int channelNumber);
    TVChannel* getChannelPointer(QString channelSignature);
    TVChannel* getCurrentChannelPointer();
    int getChannelCount();
    TVTimer* getTimerPointer(int timerNumber);
    int getTimerCount();
    int getRecCount();
    TVRec* getRecPointer(int recNumber);

    void updateEPG();

    void clearTimers();
    void updateTimers();
    void appendTimer(TVTimer *timer);
    void newTimer(TVTimer *timer);
    void delTimer(TVTimer *timer);

    void clearRecs();
    void updateRecs();
    void appendRec(TVRec *rec);
    void delRec(TVRec *rec);

    void action();
    void selectDefault();

    int getCurrentChannel();
    void setCurrentChannel(int channelNumber);
    void startLiveTV();
    void stopLiveTV();

protected:
    void keyPressEvent(QKeyEvent *k);

private:
    typedef QPtrList<TVChannel> ChannelListT;
    typedef QPtrList<TVTimer> TimerListT;
    typedef QPtrList<TVRec> RecListT;

    ChannelListT m_channelList;
    TimerListT m_timerList;
    RecListT m_recList;
    
    int m_currentChannel;
    int m_numberChannels;
    int m_numberTimers;
    int m_numberRecs;

    bool m_isPlaying;
    JAMStreamPlayer *m_streamPlayer;
    SVDRP *m_controlVDR;
    VdrRecs *m_vdrRecs;
};

#endif
