/*****************************************************************
|
|   Platinum - Test UPnP A/V MediaRenderer
|
|   Copyright (c) 2004-2006 Sylvain Rebaud
|   Author: Sylvain Rebaud (sylvain@rebaud.com)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltUPnP.h"
#include "mediarenderer.h"
#include "streamplayer.h"
#include "debug.h"

#include <stdlib.h>

// NPT_SET_LOCAL_LOGGER("platinum.core.upnp")

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
struct Options {
    const char* friendly_name;
} Options;


Renderer::Renderer(PlaybackCmdListener* listener,
                      const char*          friendly_name,
                      bool                 show_ip,
                      const char*          uuid,
                      unsigned int         port)
 : PLT_MediaRenderer(listener, friendly_name, show_ip, uuid, port), MediaPlayer("Renderer")
{
//     fprintf(stderr, "JammRenderer::JammRenderer()\n");
//     NPT_LOG_INFO("JammRenderer::JammRenderer()\n");
    m_currentTitle = new Title("Current Title", Title::AnyT);
    m_exposed = false;
}


NPT_Result
Renderer::OnPlay(PLT_ActionReference& action)

{
    TRACE("JammRenderer::OnPlay()");
    m_currentTitle->setMrl(new Mrl((char*)m_currentUri, "", ""));
//     StreamPlayer::instance()->play(m_currentTitle);
//     StreamPlayer::instance()->showUp();
    if (!m_exposed) {
        showUp();
        m_exposed = true;
    }
    play(m_currentTitle);
    return NPT_SUCCESS;
}


NPT_Result
Renderer::OnStop(PLT_ActionReference& action)

{
    TRACE("JammRenderer::OnStop()");
//     StreamPlayer::instance()->stop();
    stop();
    return NPT_SUCCESS;
}


NPT_Result
Renderer::OnSetAVTransportURI(PLT_ActionReference& action)

{
    TRACE("JammRenderer::OnSetAVTransportURI()");
    action->GetArgumentValue("CurrentURI", m_currentUri);
    TRACE("JammRenderer::OnSetAVTransportURI() setting currentUri to: %s", (char*)m_currentUri);
//     StreamPlayer::instance()->play();
    return NPT_SUCCESS;
}

// bool
// RendererThread::suicide()
// {
//     TRACE("RendererThread::suicide()");
//     return true;
// }
// 
// void
// RendererThread::run()
// {
//     TRACE("RendererThread::run()");
// }
