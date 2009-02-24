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

// TODO: indicate end of: transisioning

void
UpnpMediaRenderer::PollSlot::onSignalReceived() {
    NPT_String transportState;
    s->m_AvTransport->GetStateVariableValue("TransportState", transportState);
    if (transportState == "PLAYING") {
        // if no track length could be determined in the metadata by OnSetAVTransportURI(),
        // try to read it from the stream until we get a sensible value
        NPT_String timestamp;
        NPT_UInt32 duration;
        s->m_AvTransport->GetStateVariableValue("CurrentTrackDuration", timestamp);
        PLT_Didl::ParseTimeStamp(timestamp, duration);
        if (duration == 0) {
            float dur;
            s->m_engine->getLength(dur);
            PLT_Didl::FormatTimeStamp(timestamp, dur);
            s->m_AvTransport->SetStateVariable("CurrentMediaDuration", timestamp);
            s->m_AvTransport->SetStateVariable("CurrentTrackDuration", timestamp);
        }
        
        // read current position from stream
        float curPosition;
        s->m_engine->getPosition(curPosition);
        // set State Variables according to current position
        NPT_String pos;
        PLT_Didl::FormatTimeStamp(pos, curPosition);
        s->m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
        s->m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
    }
}


void
UpnpMediaRenderer::EndOfTrackSlot::onSignalReceived() {
    TRACE("UpnpMediaRenderer::EndOfTrackSlot::onSignalReceived()");
    s->m_AvTransport->SetStateVariable("TransportState", "STOPPED");
    // FIXME: reset position? doesn't work?
    NPT_String pos;
    PLT_Didl::FormatTimeStamp(pos, 0);
    s->m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
    s->m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
    s->m_AvTransport->SetStateVariable("Speed", "0");
}


UpnpMediaRenderer::UpnpMediaRenderer(Engine* engine,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
: PLT_MediaRenderer(friendly_name, show_ip, uuid, port),
m_engine(engine),
m_lastCurrentTrackUri("")
{
    FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", m_AvTransport);
    FindServiceByType("urn:schemas-upnp-org:service:RenderingControl:1", m_AvRenderingControl);
    
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() m_AvTransport: %p", m_AvTransport);
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() TransportState sending events: %s", m_AvTransport->FindStateVariable("TransportState")->IsSendingEvents()?"true":"false");
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() TransportState sending events indirectly: %s", m_AvTransport->FindStateVariable("TransportState")->IsSendingEvents(true)?"true":"false");
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() LastChange sending events: %s", m_AvTransport->FindStateVariable("LastChange")->IsSendingEvents()?"true":"false");
    
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() m_AvRenderingControl: %p", m_AvRenderingControl);
    TRACE("UpnpMediaRenderer::UpnpMediaRenderer() LastChange sending events: %s", m_AvRenderingControl->FindStateVariable("LastChange")->IsSendingEvents()?"true":"false");
    
    JSignal::connectNodes(&m_engine->endOfTrack, new EndOfTrackSlot(this));
    JSignal::connectNodes(&m_pollPositionTimer.fire, new PollSlot(this));
    m_pollPositionTimer.startTimer(1000);
    float volume;
    m_engine->getVolume(0, volume);
    m_AvRenderingControl->SetStateVariable("Volume", NPT_String(volume));
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
    NPT_String currentUriMetaData, instanceId, currentUri;
    action->GetArgumentValue("InstanceID", instanceId);
    action->GetArgumentValue("CurrentURI", currentUri);
    action->GetArgumentValue("CurrentURIMetaData", currentUriMetaData);
    
    /* get further info from state variables and meta data
    */ 
    // get track duration from meta data and set state variable "CurrentTrackDuration"
    // TODO: make m_currentUriMetaData local
    PLT_MediaItem* item;
    PLT_MediaObjectListReference didl;
    PLT_Didl::FromDidl(currentUriMetaData, didl);
    item = (PLT_MediaItem*)(*didl->GetFirstItem());
    
    int meta_duration = item->m_Resources.GetFirstItem()->m_Duration;
    int duration = (meta_duration>0)?meta_duration:0;
    
    /* perform some actions on the engine
    */
    m_engine->setUri((char*)currentUri);
    
    /* set state variables according to the outcome of the actions
    */
    m_AvTransport->SetStateVariable("CurrentTrackURI", currentUri);
    m_AvTransport->SetStateVariable("CurrentURIMetaData", currentUriMetaData);
    m_AvTransport->SetStateVariable("CurrentTrackMetaData", currentUriMetaData);
    TRACE("UpnpMediaRenderer::OnSetAVTransportURI() CurrentTrackMetaData: %s", (char*)currentUriMetaData);
    
    NPT_String timestamp;
    PLT_Didl::FormatTimeStamp(timestamp, duration);
    m_AvTransport->SetStateVariable("CurrentMediaDuration", timestamp);
    m_AvTransport->SetStateVariable("CurrentTrackDuration", timestamp);
    
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
    NPT_String currentTrackUri;
    m_AvTransport->GetStateVariableValue("CurrentTrackURI", currentTrackUri);
    
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
            // TODO: does it mean: play from beginning now?
            //       if yes: after STOPPED -> TRANSITIONING -> STOPPED we would play from beginning
            //               and not from the position we were seeking to.
            //       if no:  Action Stop behaves like Pause
            //       -> maybe it should be: STOPPED -> TRANSITIONING -> PAUSED_PLAYBACK
            //          contradicting AVTransport, 1.0, 2.4.12.3.
            m_engine->load();
        }
        else if (transportState == "PLAYING") {
            if (currentTrackUri != m_lastCurrentTrackUri) {
                NPT_String pos;
                PLT_Didl::FormatTimeStamp(pos, 0);
                m_AvTransport->SetStateVariable("AbsoluteTimePosition", (char*)pos);
                m_AvTransport->SetStateVariable("RelativeTimePosition", (char*)pos);
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
        // TODO: check state of engine (return value of load()).
        //       If start playing takes considerable amount of time
        //       set state to "TRANSITIONING" (-> 2.4.9.3. Effect on State, AVTransport spec)
            
    /* set state variables according to the outcome of the actions
    */
        m_AvTransport->SetStateVariable("TransportState", "PLAYING");
        m_AvTransport->SetStateVariable("Speed", speed);
        
        m_lastCurrentTrackUri = currentTrackUri;
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


NPT_Result
UpnpMediaRenderer::OnSetVolume(PLT_ActionReference& action)
{
    TRACE("UpnpMediaRenderer::OnSetVolume()");
    /* get arguments
    */
    NPT_String channel, volume;
    action->GetArgumentValue("Channel", channel);
    action->GetArgumentValue("DesiredVolume", volume);
    TRACE("UpnpMediaRenderer::OnSetVolume() channel: %s, seek volume: %s", (char*)channel, (char*)volume);
    /* perform some actions on the engine
    */
    float vol;
    volume.ToFloat(vol);
    long chan;
    channel.ToInteger(chan);
    m_engine->setVolume(chan, vol);
    
    /* set state variables according to the outcome of the actions
    */
    m_AvRenderingControl->SetStateVariable("Volume", volume);
    /* error handling
    */
    return NPT_SUCCESS;
}


NPT_Result
UpnpMediaRenderer::OnSetMute(PLT_ActionReference& /*action*/)
{
    TRACE("UpnpMediaRenderer::OnSetMute()");
//     m_engine->previous();
    return NPT_SUCCESS;
}
