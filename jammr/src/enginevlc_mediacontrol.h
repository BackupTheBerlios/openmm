/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
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
#ifndef ENGINEVLC_H
#define ENGINEVLC_H

#include "engine.h"

#ifdef __X11__
#include <X11/Xlib.h>
#endif
#include <vlc/vlc.h>
#include <vlc/mediacontrol.h>


class EngineVlc : public Engine
{
public:
    EngineVlc(int argc, char **argv);
    ~EngineVlc();
    
    /*
      AVTransport
    */
    virtual void setUri(string uri) { m_uri = uri; }
    virtual void load();
    
    /**
        void setSpeed(int nom, int denom)
        set speed to nom/denom
    */
    virtual void setSpeed(int nom, int denom);
    /**
        void pause()
        toggle pause
    */
    virtual void pause();
    virtual void stop();
    virtual void seek(int seconds);
    virtual void next();
    virtual void previous();
    virtual void getPosition(float &seconds);
    virtual void getLength(float &seconds);
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);
    
private:
    void handleException();
    int openXWindow();
    void closeXWindow();
    
    void getStreamInfo();
    
    mediacontrol_Exception* m_exception;
    mediacontrol_Instance*  m_instance;
    
    string                  m_uri;
    long long               m_lastPos;
    long long               m_offsetPos;
};


#endif
