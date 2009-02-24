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


EngineVlc::EngineVlc(int argc, char **argv)
{
    clearException();
    m_vlcInstance = libvlc_new(argc, argv, &m_exception);
    handleException();
    clearException();
    m_vlcPlayer = libvlc_media_player_new(m_vlcInstance, &m_exception);
    handleException();
}


EngineVlc::~EngineVlc()
{
    libvlc_release(m_vlcInstance);
}


void
EngineVlc::load()
{
    clearException();
    libvlc_media_t* media = libvlc_media_new(m_vlcInstance, m_uri.c_str(), &m_exception);
    handleException();
    clearException();
    libvlc_media_player_set_media(m_vlcPlayer, media, &m_exception);
    handleException();
    clearException();
    libvlc_media_player_play(m_vlcPlayer, &m_exception);
    handleException();
}


void
EngineVlc::setSpeed(int nom, int denom)
{
}


void
EngineVlc::pause()
{
}


void
EngineVlc::stop()
{
}


void
EngineVlc::seek(int seconds)
{
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
}


void
EngineVlc::getLength(float &seconds)
{
}


void
EngineVlc::setVolume(int channel, float vol)
{
}


void
EngineVlc::getVolume(int channel, float &vol)
{
}


void
EngineVlc::clearException()
{
    libvlc_exception_init (&m_exception);
}


void
EngineVlc::handleException()
{
    if (libvlc_exception_raised(&m_exception)) {
        TRACE(libvlc_exception_get_message(&m_exception));
    }
}