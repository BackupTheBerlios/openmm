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
#include <stdlib.h>

// NPT_SET_LOCAL_LOGGER("platinum.core.upnp")
// NPT_SET_LOCAL_LOGGER("platinum.media.renderer")
// NPT_SET_LOCAL_LOGGER("platinum.core.action")
// NPT_SET_LOCAL_LOGGER("platinum.core.event")
// NPT_SET_LOCAL_LOGGER("platinum.core.service")
// NPT_SET_LOCAL_LOGGER("platinum.core.statevariable")
// NPT_SET_LOCAL_LOGGER("platinum.core.devicedata")
// NPT_SET_LOCAL_LOGGER("platinum.core.devicehost")


void
Timer::startTimer(int millisec)
{
    TRACE("Timer::startTimer()");
    m_delay = millisec;
    m_run = true;
    Start();
}


void
Timer::stop()
{
    TRACE("Timer::stop()");
    // TODO: lock m_run
    m_run = false;
}


void
UpnpMediaRenderer::Run()
{
    bool shootTimer;
    do {
        Wait(m_delay);
        m_curPosition = m_engine->getPosition();
        // TODO: lock m_run
        shootTimer = m_run;
        // unlock m_run
    } while(shootTimer);
}


UpnpMediaRenderer::UpnpMediaRenderer(Engine* engine,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
: PLT_MediaRenderer(friendly_name, show_ip, uuid, port), Timer(), m_engine(engine)
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
//     m_pollPositionTimer->startTimer(1000);
    startTimer(1000);
    
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
    // TODO: if pausing stream, stop position info polling thread
    //       else if start playing again, start position info polling thread
    
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
    NPT_List<NPT_String> timeVals;
    long seekVal, seekVal_h, seekVal_m, seekVal_s;
    TRACE("UpnpMediaRenderer::OnSeek()");
    action->GetArgumentValue("Unit", seekMode);
    action->GetArgumentValue("Target", seekTarget);
    TRACE("UpnpMediaRenderer::OnSeek() seek mode: %s, seek target: %s", (char*)seekMode, (char*)seekTarget);
    timeVals = seekTarget.Split(":");
    TRACE("UpnpMediaRenderer::OnSeek() to h:mm:ss: %s:%s:%s", (char*)*timeVals.GetItem(0), (char*)*timeVals.GetItem(1), (char*)*timeVals.GetItem(2));
    (*timeVals.GetItem(0)).ToInteger(seekVal_h);
    (*timeVals.GetItem(1)).ToInteger(seekVal_m);
    (*timeVals.GetItem(2)).ToInteger(seekVal_s);
    TRACE("UpnpMediaRenderer::OnSeek() to second: %i", seekVal_h*3600+seekVal_m*60+seekVal_s);     m_engine->seek(seekVal_h*3600+seekVal_m*60+seekVal_s);
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnSetAVTransportURI(PLT_ActionReference& action)

{
    action->GetArgumentValue("CurrentURI", m_currentUri);
    m_AvTransport->SetStateVariable("CurrentTrackURI", m_currentUri);
    action->GetArgumentValue("CurrentURIMetaData", m_currentUriMetaData);
    m_AvTransport->SetStateVariable("CurrentTrackMetaData", m_currentUriMetaData);
    // TODO: get track duration from meta data and set state variable "CurrentTrackDuration"
    
    NPT_String value;
    m_AvTransport->GetStateVariableValue("CurrentTrackURI", value);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() State Variable CurrentTrackURI: %s", (char*)value);
    m_AvTransport->GetStateVariableValue("CurrentTrackMetaData", value);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() State Variable CurrentTrackMetaData: %s", (char*)value);
    m_AvTransport->GetStateVariableValue("CurrentTrackDuration", value);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() State Variable CurrentTrackDuration: %s", (char*)value);
    m_AvTransport->SetStateVariable("CurrentTrackDuration", value);
    m_AvTransport->GetStateVariableValue("AbsoluteTimePosition", value);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() State Variable AbsoluteTimePosition: %s", (char*)value);
    
    
//     m_AvTransport->SetStateVariable("CurrentTrackDuration", "00:02:00");
//     m_AvTransport->SetStateVariable("CurrentMediaDuration", "00:02:00");
//     m_AvTransport->SetStateVariable("AbsoluteTimePosition", "00:01:00");
    
    m_engine->setMrl((char*)m_currentUri);
    return NPT_SUCCESS;
}
