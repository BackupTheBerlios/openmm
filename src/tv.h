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
#ifndef TV_H
#define TV_H

#include "globalkeyhandler.h"

#include <qwidget.h>
#include <qstring.h>
#include <qptrvector.h>
#include <time.h>

#include "vdrrecs.h"
#include "streamplayerxine.h"
#include "svdrp.h"

// forward declaration
class Svdrp;
class VdrRecs;
class GlobalKeyHandler;
class Controler;

// TODO: MenuTimerEdit classes

class TvRec
{
public:
    TvRec(QString id, QString day, QString start, QString title);

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


class TvTimer
{
public:
    TvTimer(QString id, QString channelId, TimerDay *day, TimerTime *start, TimerTime *end, int active, int prio, int resist, QString title);

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


class EpgEntry
{
public:
    EpgEntry(QString id, time_t start, time_t duration, QString title = "", QString shortText = "", QString description = "");
    
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


class TvChannel
{
public:
    TvChannel(QString id, QString name, QString signature = "");

    QString getId() { return m_id; }
    QString getIdStr();
    QString getName() { return m_name; }
    QString getSignature() { return m_signature; }
    void appendEpgEntry(EpgEntry *epgEntry);
    EpgEntry* getEpgEntry(time_t at);
    EpgEntry* getCurrentEpgEntry();
    EpgEntry* getNextEpgEntry();
    EpgEntry* getPrevEpgEntry();

private:
    typedef QPtrList<EpgEntry> EpgListT;
    EpgListT m_epg;

    QString m_id;
    QString m_name;
    QString m_signature;
    unsigned int m_current;
};


/**
Metadata and menu for watching Tv and recordings

@author Jörg Bakker
*/
class Tv: public Menu
{
    Q_OBJECT

public:
    Tv(GlobalKeyHandler *keyh, QWidget *parent = 0, const char *name = 0);

    ~Tv();

    void insertChannel(int channelNumber, TvChannel *channel);
    void appendChannel(TvChannel *channel);
    QString getChannelId(int channelNumber);
    int getChannelNumber(QString channelId);
    QString getChannelName(int channelNumber);
    TvChannel* getChannelPointer(int channelNumber);
    TvChannel* getChannelPointer(QString channelSignature);
    TvChannel* getCurrentChannelPointer();
    int getChannelCount();
    TvTimer* getTimerPointer(int timerNumber);
    int getTimerCount();
    int getRecCount();
    TvRec* getRecPointer(int recNumber);

    void updateEpg();

    void clearTimers();
    void updateTimers();
    void appendTimer(TvTimer *timer);
    void newTimer(TvTimer *timer);
    void delTimer(TvTimer *timer);

    void clearRecs();
    void updateRecs();
    void appendRec(TvRec *rec);
    void delRec(TvRec *rec);

    void action();
    void selectDefault();

    int getCurrentChannel();
    void setCurrentChannel(int channelNumber);
    void startLiveTv();
    void stopLiveTv();

protected:
    void keyPressEvent(QKeyEvent *k);

private:
    typedef QPtrList<TvChannel> ChannelListT;
    typedef QPtrList<TvTimer> TimerListT;
    typedef QPtrList<TvRec> RecListT;

    ChannelListT m_channelList;
    TimerListT m_timerList;
    RecListT m_recList;
    
    int m_currentChannel;
    int m_numberChannels;
    int m_numberTimers;
    int m_numberRecs;

    bool m_isPlaying;
    StreamPlayer *m_streamPlayer;
    Svdrp *m_controlVDR;
    VdrRecs *m_vdrRecs;
};

#endif
