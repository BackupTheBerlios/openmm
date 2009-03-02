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
#include "enginevlc.h"

#include <jamm/debug.h>

static char * defaults_args[] = {
    "-vvv"/*,
        "--ignore-config",
        "-I",
        "dummy",
        "--no-media-library",
        "--plugin-path=../modules",
        "--vout=dummy",
        "--aout=dummy"*/
};

static const int defaults_nargs =
sizeof (defaults_args) / sizeof (defaults_args[0]);


EngineVlc::EngineVlc(int argc, char **argv)
: m_lastPos(0),
m_offsetPos(0)
{
    m_exception = mediacontrol_exception_create();
    mediacontrol_exception_init(m_exception);
    
    m_instance = mediacontrol_new(argc, argv, m_exception);
//     m_instance = mediacontrol_new(defaults_nargs, defaults_args, m_exception);
    handleException();
    
    int xWindow = openXWindow();
    mediacontrol_set_visual(m_instance, xWindow, m_exception);
    handleException();
}


EngineVlc::~EngineVlc()
{
    mediacontrol_exit(m_instance);
    mediacontrol_exception_free(m_exception);
    closeXWindow();
}

int
EngineVlc::openXWindow()
{
    Display*    xDisplay;
    int         xScreen;
    Window      xWindow;
    int xPos    = 0;
    int yPos    = 0;
    int width   = 320;
    int height  = 200;
    
    XInitThreads ();
    xDisplay = XOpenDisplay(NULL);
    xScreen = DefaultScreen(xDisplay);
    XLockDisplay(xDisplay);
    xWindow = XCreateSimpleWindow(xDisplay, XDefaultRootWindow(xDisplay),
                                  xPos, yPos, width, height, 1, 0, 0);
    XMapRaised(xDisplay, xWindow);
//     res_h = (DisplayWidth(xDisplay, xScreen) * 1000 / DisplayWidthMM(xDisplay, xScreen));
//     res_v = (DisplayHeight(xDisplay, xScreen) * 1000 / DisplayHeightMM(xDisplay, xScreen));
    XSync(xDisplay, False);
    XUnlockDisplay(xDisplay);
    return xWindow;
}


void
EngineVlc::closeXWindow()
{
/*    if (xDisplay)
        XCloseDisplay(xDisplay);
    xDisplay = NULL;*/
}


void
EngineVlc::load()
{
    m_lastPos = 0;
    m_offsetPos = 0;
    mediacontrol_set_mrl(m_instance, m_uri.c_str(), m_exception);
    handleException();
    
    mediacontrol_Position pos;
    pos.origin = mediacontrol_AbsolutePosition;
    pos.key = mediacontrol_MediaTime;
    pos.value = 0;
    
    mediacontrol_start(m_instance, &pos, m_exception);
    handleException();
}


void
EngineVlc::getStreamInfo()
{
    mediacontrol_StreamInformation* info;
/*
    info = mediacontrol_get_stream_information(m_instance, mediacontrol_ByteCount, m_exception);
    handleException();
    long long bytePos = info->position;
    long long byteLen = info->length;
*/
    info = mediacontrol_get_stream_information(m_instance, mediacontrol_MediaTime, m_exception);
    handleException();
    long long timePos = info->position;
    long long timeLen = info->length;
//     TRACE("EngineVlc::getStreamInfo() bytePos: %lli, byteLen: %lli, timePos: %lli timeLen: %lli", bytePos, byteLen, timePos, timeLen);
    TRACE("EngineVlc::getStreamInfo() timePos: %lli, timeLen: %lli, offset: %lli", timePos, timeLen, m_offsetPos);
}


void
EngineVlc::setSpeed(int /*nom*/, int /*denom*/)
{
}


void
EngineVlc::pause()
{
    mediacontrol_pause(m_instance, m_exception);
    handleException();
}


void
EngineVlc::stop()
{
    mediacontrol_stop(m_instance, m_exception);
    handleException();
}


void
EngineVlc::seek(int seconds)
{
    // FIXME: vlc jumps back to current playing position when trying to seek in
    //        vdr recording streams. vlc-player is able to seek ...?
    
    // vlc-qt4: [00000408] main input debug: control type=5 (INPUT_CONTROL_SET_POSITION)
    /*
    modules/gui/qt4/input_manager.cpp:
    
    //User update of the slider 
    void InputManager::sliderUpdate( float new_pos )
    {
       if( hasInput() )
           var_SetFloat( p_input, "position", new_pos );
    }
    
    void InputManager::UpdatePosition()
    {
        //Update position 
        int i_length, i_time; // Int is enough, since we store seconds 
        float f_pos;
        i_length = var_GetTime(  p_input , "length" ) / 1000000;
        i_time = var_GetTime(  p_input , "time") / 1000000;
        f_pos = var_GetFloat(  p_input , "position" );
        emit positionUpdated( f_pos, i_time, i_length );
    }
    
    void InputManager::setInput( input_thread_t *_p_input )
    { ...
    */
    
    // jammr: [00000402] main input debug: control type=7 (INPUT_CONTROL_SET_TIME)
    // type=1: play
    // type=0: stop
    // src/input/input_internal.h: enum input_control_e
    TRACE("seek to second: %i", seconds);
    mediacontrol_Position pos;
    pos.origin = mediacontrol_AbsolutePosition;
    pos.key = mediacontrol_MediaTime;
    pos.value = seconds * 1000 + m_offsetPos;
    TRACE("seek to position: %lli", pos.value);
    
//     mediacontrol_stop(m_instance, m_exception);
//     handleException();
//     mediacontrol_start(m_instance, &pos, m_exception);
    mediacontrol_set_media_position(m_instance, &pos, m_exception);
    handleException();
}


void
EngineVlc::next()
{
}


void
EngineVlc::previous()
{
}


void
EngineVlc::getPosition(float &seconds)
{
    getStreamInfo();
    
    mediacontrol_StreamInformation* streamInfo;
    streamInfo = mediacontrol_get_stream_information(m_instance, mediacontrol_MediaTime, m_exception);
    handleException();
    
    if (streamInfo->streamstatus == mediacontrol_EndStatus) {
        endOfTrack.emitSignal();
        seconds = 0.0;
        return;
    }
    
    if (m_lastPos == 0 && streamInfo->position > 10000) {
        m_offsetPos = streamInfo->position;
    }
//     TRACE("EngineVlc::getPosition position [ms]: %lli", streamInfo->position);
    m_lastPos = streamInfo->position;
    seconds = (streamInfo->position - m_offsetPos) / 1000.0;
}


void
EngineVlc::getLength(float &seconds)
{
    mediacontrol_StreamInformation* streamInfo;
    streamInfo = mediacontrol_get_stream_information(m_instance, mediacontrol_MediaTime, m_exception);
    handleException();
    
    // FIXME: m_offsetPos is still 0 when getLength is called -> Controller shows a completely wrong length
    seconds = (streamInfo->length - m_offsetPos)/ 1000.0;
    TRACE("EngineVlc::getLength length [ms]: %lli, float: %f", (streamInfo->length - m_offsetPos), seconds);
}


void
EngineVlc::setVolume(int /*channel*/, float vol)
{
    mediacontrol_sound_set_volume(m_instance, vol, m_exception);
    handleException();
}


void
EngineVlc::getVolume(int /*channel*/, float &vol)
{
    vol = mediacontrol_sound_get_volume(m_instance, m_exception);
    handleException();
}


void
EngineVlc::handleException()
{
    if (m_exception->code) {
        TRACE(m_exception->message);
    }
    mediacontrol_exception_cleanup(m_exception);
}