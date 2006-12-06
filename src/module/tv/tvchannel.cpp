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
#include "tvchannel.h"
//#include "svdrp.h"


TvChannel::TvChannel(QString id, QString name, QString signature)
 : Title(name, Title::TvChannelT)
{
    m_id = id;
    m_signature = signature;
    setHeader("Id;Signature");
}


QString
TvChannel::getColText(QString col)
{
    if (col == colName(1)) {
        return m_id.rightJustify(2, '0');
    }
    else if (col == colName(2)) {
        return m_signature;
    }
    return "";
}


void
TvChannel::setColText(QString col, QString text)
{
// TODO: convert strings into appropriate values for column fields.
}


// int
// TvChannel::getId(QString col)
// {
//     return 0;
// }

/*
QString
TvChannel::getIdStr()
{
    return m_id.rightJustify(2, '0');
}
*/
/*
void
TvChannel::appendProgram(Program *program)
{
    m_epg.append(Program);
}
*/
/*
TvProgram*
TvChannel::getProgram(time_t at)
{
    // iterate through all Epg entries, to find the one at time at.
    int index = 0;
    for (EpgListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
    {
        if ((*it)->getStartTime() + (*it)->getDurationTime() > at)
        {
            m_current = index;
            return (*it);
        }
        index++;
    }
    return NULL;
}


Program*
TvChannel::getCurrentProgram()
{
    QDateTime now = QDateTime::currentDateTime();
    time_t now_t = now.toTime_t();

    // iterate through all Epg entries, to find the current one.
    int index = 0;
    for (EpgListT::iterator it = m_epg.begin(); it != m_epg.end(); ++it)
    {
        if ((*it)->getStartTime() + (*it)->getDurationTime() > now_t)
        {
            m_current = index;
            return (*it);
        }
        index++;
    }
    return NULL;
}


Program*
TvChannel::getNextProgram()
{
    if (m_current < m_epg.count()-1)
        m_current++;
    return m_epg.at(m_current);
}


Program*
TvChannel::getPrevProgram()
{
    if (m_current > 0)
        m_current--;
    return m_epg.at(m_current);
}
*/

/*
TvChannelList::TvChannelList()
{
}


TvChannelList::~TvChannelList()
{
}


void
TvChannelList::insertChannel(int channelNumber, TvChannel *channel)
{
    m_channelList.insert(channelNumber, channel);
    m_numberChannels++;
}


void
TvChannelList::appendChannel(TvChannel *channel)
{
    m_channelList.append(channel);
    m_numberChannels++;
}


QString
TvChannelList::getChannelId(int channelNumber)
{
    return m_channelList.at(channelNumber)->getId();
}


int
TvChannelList::getChannelNumber(QString channelId)
{
    TvChannel *ch;
    int i = 0;
    for (ch = m_channelList.first(); ch; ch = m_channelList.next()) {
        if (ch->getId() == channelId)
            return i;
        i++;
    }
    return -1;
}


QString
TvChannelList::getChannelName(int channelNumber)
{
    return m_channelList.at(channelNumber)->getName();
}


TvChannel*
TvChannelList::getChannelPointer(int channelNumber)
{
    return m_channelList.at(channelNumber);
}


TvChannel*
TvChannelList::getChannelPointer(QString channelSignature)
{
    //qDebug("Tv::getChannelPointer(), channelSignature: %s", channelSignature.latin1());
    for (ChannelListT::iterator it = m_channelList.begin(); it != m_channelList.end(); ++it)
    {
        //qDebug("Tv::getChannelPointer(), checking channel: %s", (*it)->getName().latin1());
        if ((*it)->getSignature() == channelSignature)
        {
            //qDebug("Tv::getChannelPointer(), got channel: %s", (*it)->getName().latin1());
            return (*it);
        }
    }
    //qDebug("Tv::getChannelPointer(), channel not found!");
    return NULL;
}


int
TvChannelList::getChannelCount()
{
    return m_channelList.count();
}


void
TvChannelList::updateEpg()
{
    // TODO: synchronize with SVDRP
    //m_controlVDR->getEpg(this);
}
*/
