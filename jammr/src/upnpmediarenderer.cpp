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

#include <jamm/debug.h>
#include <platinum/PltUPnP.h>
#include <platinum/PltDidl.h>
#include <stdlib.h>

// TODO: timer implementation isn't correct:
//       1. killing and restarting doesn't work, seems that additional threads are started when restarting
//       2. when track finishes, and next track is started: additional thread created?
// TODO: indicate end of: transisioning, track
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
            NPT_String transportState;
            m_AvTransport->GetStateVariableValue("TransportState", transportState);
            if (transportState == "PLAYING") {
                int curPosition;
                m_engine->getPosition(curPosition);
                // set State Variables according to current position
                NPT_String pos;
                PLT_Didl::FormatTimeStamp(pos, curPosition);
                m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
                m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
            }
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


UpnpMediaRenderer::UpnpMediaRenderer(EngineMplayer* engine,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
: PLT_MediaRenderer(friendly_name, show_ip, uuid, port), Timer(), m_engine(engine),
m_uriChanged(false),
m_pollIntervall(1000)
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
    startTimer(m_pollIntervall);
}


UpnpMediaRenderer::~UpnpMediaRenderer()
{
    TRACE("UpnpMediaRenderer::~UpnpMediaRenderer()");
//     delete m_engine;
}


NPT_Result
UpnpMediaRenderer::OnSetAVTransportURI(PLT_ActionReference& action)

{
    /* check TransportState and do something only if specified for this state
    */
    NPT_String transportState;
    m_AvTransport->GetStateVariableValue("TransportState", transportState);
    // OnSetAVTransportURI is active in all transport states
    /* get arguments
    */
    NPT_String currentUriMetaData;
    action->GetArgumentValue("InstanceID", m_currentUri);
    action->GetArgumentValue("CurrentURI", m_currentUri);
    action->GetArgumentValue("CurrentURIMetaData", currentUriMetaData);
    
    /* get further info from state variables and meta data
    */ 
    // get track duration from meta data and set state variable "CurrentTrackDuration"
    // TODO: make m_currentUriMetaData local
    PLT_MediaItem* item;
    PLT_MediaObjectListReference didl;
    PLT_Didl::FromDidl(currentUriMetaData, didl);
    item = (PLT_MediaItem*)(*didl->GetFirstItem());
    // FIXME: m_duration of current track is overwritten.
    m_duration = item->m_Resources.GetFirstItem()->m_Duration;
    NPT_String timestamp;
    PLT_Didl::FormatTimeStamp(timestamp, m_duration);
    
    /* perform some actions on the engine
    */
    m_engine->setUri((char*)m_currentUri);
    
    /* set state variables according to the outcome of the actions
    */
    m_uriChanged = true;
    m_AvTransport->SetStateVariable("CurrentTrackURI", m_currentUri);
    m_AvTransport->SetStateVariable("CurrentURIMetaData", currentUriMetaData);
    m_AvTransport->SetStateVariable("CurrentTrackMetaData", currentUriMetaData);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() CurrentTrackMetaData: %s", (char*)currentUriMetaData);
    
    m_AvTransport->SetStateVariable("CurrentMediaDuration", timestamp);
    m_AvTransport->SetStateVariable("CurrentTrackDuration", timestamp);
    /* set state variables according to the outcome of the actions
    */
    if (transportState == "NO_MEDIA_PRESENT") {
        m_AvTransport->SetStateVariable("TransportState", "STOPPED");
    }
    // TODO: set state variables:
    //       NumberOfTracks
    /* error handling
    */
    return NPT_SUCCESS;
}


// TODO: set AllowedValueList of 2.2.8. TransportPlaySpeed to supported speeds

NPT_Result
UpnpMediaRenderer::OnPlay(PLT_ActionReference& action)
{
    /* check TransportState and do something only if specified for this state
    */
//     TRACE("UpnpMediaRenderer::OnPlay()");
    NPT_String transportState;
    m_AvTransport->GetStateVariableValue("TransportState", transportState);
    TRACE("UpnpMediaRenderer::OnPlay() enters in state: %s", (char*) transportState);
    if (transportState == "STOPPED"
        || transportState == "PLAYING"
        || transportState == "PAUSED_PLAYBACK"
        || transportState == "TRANSITIONING")
    {
    /* get arguments
    */
        NPT_String instanceId, speed;
        action->GetArgumentValue("InstanceID", instanceId);
        action->GetArgumentValue("Speed", speed);
        
    /* get further info from state variables
    */ 
        
    /* perform some actions on the engine
    */
        if (transportState == "STOPPED") {
            m_engine->load();
        }
        else if (transportState == "PLAYING") {
            if (m_uriChanged) {
                m_engine->load();
            }
        }
        else if (transportState == "PAUSED_PLAYBACK") {
            m_engine->pause();
        }
        else if (transportState == "TRANSITIONING") {
            // TODO: can we set another speed after transitioning, than normal speed?
            m_engine->setSpeed(1, 1);
            speed = "1";
        }
        // TODO: handle changes in speed
        // TODO: handle fractions of speed ('1/2', ...)
        // TODO: check state of engine (return value of play()).
        //       If start playing takes considerable amount of time
        //       set state to "TRANSITIONING" (-> 2.4.9.3. Effect on State, AVTransport spec)
            
        // TODO: if no track length could be determined in the metadata by OnSetAVTransportURI(),
        //       try to read it from the stream
        // TODO: better read duration from state variable then store it in a member variable?
            if (m_duration <= 0) {
                m_engine->getLength(m_duration);
                NPT_String timestamp;
                PLT_Didl::FormatTimeStamp(timestamp, m_duration);
                m_AvTransport->SetStateVariable("CurrentMediaDuration", timestamp);
                m_AvTransport->SetStateVariable("CurrentTrackDuration", timestamp);
            }
        
    /* set state variables according to the outcome of the actions
    */
        m_uriChanged = false;
        m_AvTransport->SetStateVariable("TransportState", "PLAYING");
        m_AvTransport->SetStateVariable("Speed", speed);
    }
    /* error handling
    */
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnStop(PLT_ActionReference&)

{
    /* check TransportState and do something only if specified for this state
    */
//     TRACE("UpnpMediaRenderer::OnStop()");
    NPT_String transportState;
    m_AvTransport->GetStateVariableValue("TransportState", transportState);
    TRACE("UpnpMediaRenderer::OnStop() enters in state: %s", (char*) transportState);
    if (transportState != "NO_MEDIA_PRESENT") {
    /* get arguments - no arguments for OnStop()
    */
    /* get further info from state variables - not necessary for OnStop()
    */ 
    /* perform some actions on the engine
    */  
        // be nice to the engine and don't stop when already in stopped or paused state
        // TODO: when in PAUSED_PLAYBACK we should actually stop (according to AVTransport 1.0 specs
        //       -> unfortunately mplayer has it's problems with stopping ...
        if (transportState != "STOPPED" && transportState != "PAUSED_PLAYBACK") {
            m_engine->stop();
        }
        // TODO: check if engine really stopped (by return value)
    /* set state variables according to the outcome of the actions
    */
        m_AvTransport->SetStateVariable("TransportState", "STOPPED");
        if (transportState == "PLAYING" || transportState == "PAUSED_PLAYBACK") {
        // TODO: reset positions and speed (this is not mentioned in the AVTransport 1.0 specs ...)?
        //       what does Stop() mean when in TRANSITIONING state? 
        //       -> stop transitioning and start playback at current position?
            NPT_String pos;
            PLT_Didl::FormatTimeStamp(pos, 0);
            m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
            m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
            m_AvTransport->SetStateVariable("Speed", "0");
        }
    }
    /* error handling
    */
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnPause(PLT_ActionReference&)

{
//     TRACE("UpnpMediaRenderer::OnPause()");
    /* check TransportState and do something only if specified for this state
    */
    NPT_String transportState;
    m_AvTransport->GetStateVariableValue("TransportState", transportState);
    TRACE("UpnpMediaRenderer::OnPause() enters in state: %s", (char*) transportState);
    if (transportState == "PLAYING" || transportState == "RECORDING") {
    /* get arguments - no arguments for OnPause()
    */
    /* get further info from state variables - not necessary for OnPause()
    */ 
    /* perform some actions on the engine
    */
        m_engine->pause();
    /* set state variables according to the outcome of the actions
    */
        if (transportState == "PLAYING") {
            m_AvTransport->SetStateVariable("TransportState", "PAUSED_PLAYBACK");
    
        }
        else if (transportState == "RECORDING") {
            m_AvTransport->SetStateVariable("TransportState", "PAUSED_RECORDING");
        }
    }
    /* error handling
    */
    return NPT_SUCCESS;
}


// TODO: implement OnSeek(“TRACK_NR”,”0”) and OnSeek(“TRACK_NR”,”1”)
//       -> AVTransport 1.0, 2.4.12.Seek

NPT_Result
UpnpMediaRenderer::OnSeek(PLT_ActionReference& action)
{
    /* check TransportState and do something only if specified for this state
    */
    NPT_String transportState;
    m_AvTransport->GetStateVariableValue("TransportState", transportState);
    TRACE("UpnpMediaRenderer::OnSeek() enters in state: %s", (char*) transportState);
    if (transportState == "STOPPED" || transportState == "PLAYING") {
        // TODO: does it make sense to handle "PAUSED_PLAYBACK", too?
    /* get arguments
    */
        NPT_String seekMode, seekTarget;
        action->GetArgumentValue("Unit", seekMode);
        action->GetArgumentValue("Target", seekTarget);
        TRACE("UpnpMediaRenderer::OnSeek() seek mode: %s, seek target: %s", (char*)seekMode, (char*)seekTarget);
    /* get further info from state variables - not necessary for OnSeek()
    */
    /* perform some actions on the engine
    */
        NPT_UInt32 position;
        if (seekMode == "ABS_TIME") {
            PLT_Didl::ParseTimeStamp(seekTarget, position);
        }
        m_engine->seek(position);
        // TODO: according to the specs AVTransport 1.0, 2.4.12.3.Effect on State
        //       TransportState should be set to TRANSITIONING, but only while seeking.
        //       OnSeek() should return immediately! So we are not conform here.
        // m_AvTransport->SetStateVariable("TransportState", "TRANSITIONING");
        //       For now, we just keep the transportState to make life easier.
    /* set state variables according to the outcome of the actions
    */
        m_AvTransport->SetStateVariable("RelativeTimePosition", seekTarget);
        m_AvTransport->SetStateVariable("AbsoluteTimePosition", seekTarget);
    }
    /* error handling
    */
    return NPT_SUCCESS;
}


// TODO: implement OnNext()
NPT_Result
UpnpMediaRenderer::OnNext(PLT_ActionReference& /*action*/)
{
    TRACE("UpnpMediaRenderer::OnNext()");
    m_engine->next();
    return NPT_SUCCESS;
}


// TODO: implement OnPrevious()
NPT_Result
UpnpMediaRenderer::OnPrevious(PLT_ActionReference& /*action*/)
{
    TRACE("UpnpMediaRenderer::OnPrevious()");
    m_engine->previous();
    return NPT_SUCCESS;
}
