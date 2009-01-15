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
#ifndef UPNPMEDIARENDERER_H
#define UPNPMEDIARENDERER_H

#include "engine.h"
#include <platinum/PltMediaRenderer.h>
#include <platinum/NptThreads.h>

class Timer : public NPT_Thread
{
public:
    void startTimer(int millisec = 0);
    void stop();
    void kill();
    bool isActive() { return m_run; }
    
protected:
    int  m_delay;
    bool m_run, m_kill;
};


class UpnpMediaRenderer : public PLT_MediaRenderer, Timer
{
public:
    UpnpMediaRenderer(Engine*              engine,
                      const char*          friendly_name,
                      bool                 show_ip = false,
                      const char*          uuid = NULL,
                      unsigned int         port = 0);

    ~UpnpMediaRenderer();
    
protected:
    // ConnectionManager
//     virtual NPT_Result OnGetCurrentConnectionInfo(PLT_ActionReference& action);
    
    // AVTransport
    virtual NPT_Result OnNext(PLT_ActionReference& action);
    virtual NPT_Result OnPause(PLT_ActionReference& action);
    virtual NPT_Result OnPlay(PLT_ActionReference& action);
    virtual NPT_Result OnPrevious(PLT_ActionReference& action);
    virtual NPT_Result OnSeek(PLT_ActionReference& action);
    virtual NPT_Result OnStop(PLT_ActionReference& action);
    virtual NPT_Result OnSetAVTransportURI(PLT_ActionReference& action);
//     virtual NPT_Result OnSetPlayMode(PLT_ActionReference& action);
    
    // RenderingControl
//     //virtual NPT_Result OnGetVolume(PLT_ActionReference& action);
//     virtual NPT_Result OnSetVolume(PLT_ActionReference& action);
//     virtual NPT_Result OnSetMute(PLT_ActionReference& action);

private:
    virtual void Run();
    
    Engine      *m_engine;
    NPT_String   m_currentUri, m_currentUriMetaData;
    PLT_Service *m_AvTransport;
    PLT_Service *m_AvRenderingControl;
    
//     Timer*       m_pollPositionTimer;
    int          m_pollIntervall;
};

#endif
