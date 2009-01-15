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
#include "upnpmediarenderer.h"
#include "engine.h"
#include "debug.h"

#include <platinum/PltUPnP.h>
#include <platinum/PltDidl.h>
#include <stdlib.h>

// NPT_SET_LOCAL_LOGGER("platinum.core.upnp")
// NPT_SET_LOCAL_LOGGER("platinum.media.renderer")
// NPT_SET_LOCAL_LOGGER("platinum.core.action")
// NPT_SET_LOCAL_LOGGER("platinum.core.event")
// NPT_SET_LOCAL_LOGGER("platinum.core.service")
// NPT_SET_LOCAL_LOGGER("platinum.core.statevariable")
// NPT_SET_LOCAL_LOGGER("platinum.core.devicedata")
// NPT_SET_LOCAL_LOGGER("platinum.core.devicehost")

// TODO: timer implementation isn't correct:
//       1. killing and restarting doesn't work, seems that additional threads are started when restarting
//       2. when track finishes, and next track is started: additional thread created?
void
Timer::startTimer(int millisec)
{
    TRACE("Timer::startTimer()");
    m_run = true;
    m_kill = false;
    if (millisec > 0) {
        m_delay = millisec;
        Start();
    }
}


void
Timer::stop()
{
    TRACE("Timer::stop()");
    // TODO: lock m_run
    m_run = false;
}


void
Timer::kill()
{
    TRACE("Timer::stop()");
    // TODO: lock m_run
    m_kill = true;
}


void
UpnpMediaRenderer::Run()
{
    /* TODO: figure out, why this stops after the first m_engine->getPosition()
    bool shootTimer;
    do {
        Wait(m_delay);
        m_curPosition = m_engine->getPosition();
        // TODO: lock m_run
        shootTimer = m_run;
        // unlock m_run
    } while(shootTimer);*/
    while(1) {
        Wait(m_delay);
        if (m_run) {
            int curPosition;
            m_engine->getPosition(curPosition);
            // set State Variables according to current position
            NPT_String pos;
            PLT_Didl::FormatTimeStamp(pos, curPosition);
            m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
            m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
        }
        if (m_kill) {
            break;
        }
/*        else {
            break;
        }*/
    }
/*    while(m_run) {
        Wait(m_delay);
        m_curPosition = m_engine->getPosition();
    }*/
    
}


UpnpMediaRenderer::UpnpMediaRenderer(Engine* engine,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
: PLT_MediaRenderer(friendly_name, show_ip, uuid, port), Timer(), m_engine(engine), m_pollIntervall(1000)
{
    FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", m_AvTransport);
//     FindServiceById("urn:upnp-org:serviceId:AVT_1-0", m_AvTransport);
//     m_AvTransport = GetServices()[0];
    FindServiceByType("urn:schemas-upnp-org:service:AVRenderingControl:1", m_AvRenderingControl);
    
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() m_AvTransport: %p", m_AvTransport);
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() TransportState sending events: %s", m_AvTransport->FindStateVariable("TransportState")->IsSendingEvents()?"true":"false");
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() TransportState sending events indirectly: %s", m_AvTransport->FindStateVariable("TransportState")->IsSendingEvents(true)?"true":"false");
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() LastChange sending events: %s", m_AvTransport->FindStateVariable("LastChange")->IsSendingEvents()?"true":"false");
    
//     m_pollPositionTimer = new Timer();
}


UpnpMediaRenderer::~UpnpMediaRenderer()
{
    TRACE("UpnpMediaRenderer::~UpnpMediaRenderer()");
//     delete m_engine;
}


NPT_Result
UpnpMediaRenderer::OnNext(PLT_ActionReference& action)
{
    TRACE("UpnpMediaRenderer::OnNext()");
    m_engine->next();
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnPlay(PLT_ActionReference& action)
{
    TRACE("UpnpMediaRenderer::OnPlay()");
    m_engine->play();
    m_AvTransport->SetStateVariable("TransportState", "PLAYING");
    // start position info polling thread
    // TODO: check first state of engine and start timer if engine is playing and not in transfer state.
    //       This delay is just a cheap replacement.
    Wait(1000);
    startTimer(m_pollIntervall);
    
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnStop(PLT_ActionReference& action)

{
    TRACE("UpnpMediaRenderer::OnStop()");
    // stop position info polling thread
//     m_pollPositionTimer->stop();
    stop();
    
    m_engine->stop();
    m_AvTransport->SetStateVariable("TransportState", "STOPPED");
    
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnPause(PLT_ActionReference& action)

{
    TRACE("UpnpMediaRenderer::OnPause()");
    m_engine->pause();
    m_AvTransport->SetStateVariable("TransportState", "PAUSED_PLAYBACK");
    
    // if pausing stream, stop position info polling thread
    // else if start playing again, start position info polling thread
    if (isActive()) {
        stop();
    }
    else {
        startTimer();
    }
    
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnPrevious(PLT_ActionReference& action)
{
    TRACE("UpnpMediaRenderer::OnPrevious()");
    m_engine->previous();
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnSeek(PLT_ActionReference& action)
{
    NPT_String seekMode, seekTarget;
    action->GetArgumentValue("Unit", seekMode);
    action->GetArgumentValue("Target", seekTarget);
    TRACE("UpnpMediaRenderer::OnSeek() seek mode: %s, seek target: %s", (char*)seekMode, (char*)seekTarget);
    if (seekMode == "ABS_TIME") {
        NPT_UInt32 position;
        PLT_Didl::ParseTimeStamp(seekTarget, position);
        m_engine->seek(position);
        m_AvTransport->SetStateVariable("RelativeTimePosition", seekTarget);
        m_AvTransport->SetStateVariable("AbsoluteTimePosition", seekTarget);
    }
    return NPT_SUCCESS;
}


/*
Example mp3 UpnpMediaRenderer::OnSetAVTransportURI() CurrentTrackMetaData:

<DIDL-Lite xmlns="urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:upnp="urn:schemas-upnp-org:metadata-1-0/upnp/" xmlns:dlna="urn:schemas-dlna-org:metadata-1-0"><?xml version="1.0" encoding="UTF-8"?>
<item id="831" parentID="623" restricted="1">
  <dc:title>Spaceship Landing</dc:title>
  <upnp:class>object.item.audioItem.musicTrack</upnp:class>
  <upnp:artist>Kyuss</upnp:artist>
  <upnp:album>...And the Circus Leaves Town</upnp:album>
  <dc:date>1995-01-01</dc:date>
  <upnp:genre>Blues</upnp:genre>
  <upnp:originalTrackNumber>11</upnp:originalTrackNumber>
  <dc:description>Kyuss, ...And the Circus Leaves Town, Spaceship Landing, 1995, Blues</dc:description>
  <res protocolInfo="http-get:*:audio/mpeg:*" size="16338944" bitrate="24576" duration="00:11:21" 
   sampleFrequency="44100" nrAudioChannels="2">
    http://127.0.0.1:49152/content/media/object_id=831&amp;res_id=0&amp;ext=.mp3
  </res>
</item></DIDL-Lite>
*/
/*
Example avi UpnpMediaRenderer::OnSetAVTransportURI() CurrentTrackMetaData:
<DIDL-Lite xmlns="urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:upnp="urn:schemas-upnp-org:metadata-1-0/upnp/" xmlns:dlna="urn:schemas-dlna-org:metadata-1-0"><?xml version="1.0" encoding="UTF-8"?>
<item id="3075" parentID="3074" restricted="1">
  <dc:title>1_Einfaedeln_2min10.avi</dc:title>
  <upnp:class>object.item.videoItem</upnp:class>
  <res protocolInfo="http-get:*:video/x-msvideo:*" size="12754894" bitrate="780" resolution="720x576">
    http://192.168.178.22:49152/content/media/object_id=3075&amp;res_id=0&amp;ext=.avi
  </res>
</item></DIDL-Lite>
*/
NPT_Result
UpnpMediaRenderer::OnSetAVTransportURI(PLT_ActionReference& action)

{
    action->GetArgumentValue("CurrentURI", m_currentUri);
    m_AvTransport->SetStateVariable("CurrentTrackURI", m_currentUri);
    m_engine->setMrl((char*)m_currentUri);
    
    action->GetArgumentValue("CurrentURIMetaData", m_currentUriMetaData);
    m_AvTransport->SetStateVariable("CurrentURIMetaData", m_currentUriMetaData);
    m_AvTransport->SetStateVariable("CurrentTrackMetaData", m_currentUriMetaData);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() CurrentTrackMetaData: %s", (char*)m_currentUriMetaData);
    
    // get track duration from meta data and set state variable "CurrentTrackDuration"
    PLT_MediaItem* item;
    PLT_MediaObjectListReference didl;
    PLT_Didl::FromDidl(m_currentUriMetaData, didl);
    item = (PLT_MediaItem*)(*didl->GetFirstItem());
    int duration = item->m_Resources.GetFirstItem()->m_Duration;
    // if no track length is in the metadata, try to read it from the stream
    // TODO: This delay is just a cheap replacement for proper synchronisation
    Wait(200);
    if (duration <= 0) {
        m_engine->getLength(duration);
    }
    NPT_String timestamp;
    PLT_Didl::FormatTimeStamp(timestamp, duration);
    m_AvTransport->SetStateVariable("CurrentMediaDuration", timestamp);
    m_AvTransport->SetStateVariable("CurrentTrackDuration", timestamp);
    
    return NPT_SUCCESS;
}
