/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
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
#include "lircwatcher.h"
#include "controler.h"
#include "debug.h"

#include <cstdlib>


LircWatcher::LircWatcher()
 : Thread()
{
    if (lirc_init("jam",1) == -1) {
        TRACE("LircWatcher::LircWatcher() couldn't connect lircd");
    }
    else {
        TRACE("LircWatcher::LircWatcher() connected to lircd");
    }

    if (lirc_readconfig("/etc/jam/lircrc", &m_config, NULL) != 0) {
        TRACE("LircWatcher::LircWatcher() failed to read config file");
    }
    else {
        TRACE("LircWatcher::LircWatcher() read config file /etc/jam/lircrc");
    }
    // map lirc commands to internal Events.
    m_eventMap["menu"] = Event::MenuE;
    m_eventMap["ok"] = Event::EnterE;
    m_eventMap["onoff"] = Event::QuitE;
    m_eventMap["back"] = Event::BackE;
    m_eventMap["up"] = Event::UpE;
    m_eventMap["down"] = Event::DownE;
    m_eventMap["left"] = Event::LeftE;
    m_eventMap["right"] = Event::RightE;
    m_eventMap["0"] = Event::Key0E;
    m_eventMap["1"] = Event::Key1E;
    m_eventMap["2"] = Event::Key2E;
    m_eventMap["3"] = Event::Key3E;
    m_eventMap["4"] = Event::Key4E;
    m_eventMap["5"] = Event::Key5E;
    m_eventMap["6"] = Event::Key6E;
    m_eventMap["7"] = Event::Key7E;
    m_eventMap["8"] = Event::Key8E;
    m_eventMap["9"] = Event::Key9E;
    m_eventMap["play"] = Event::StartE;
    m_eventMap["stop"] = Event::StopE;
    m_eventMap["pause"] = Event::PauseE;
    m_eventMap["forward"] = Event::ForwardE;
    m_eventMap["rewind"] = Event::RewindE;
}


LircWatcher::~LircWatcher()
{
    lirc_freeconfig(m_config);
    lirc_deinit();
}


void
LircWatcher::run()
{
    TRACE("LircWatcher::run() starting event loop!!!");
    char *code;
    char *command;
    int ret;
    while (lirc_nextcode(&code) == 0) {
//         TRACE("LircWatcher::run() received event with code: %s", code);
        if(code == NULL) continue;
        while((ret = lirc_code2char(m_config, code, &command)) == 0 && command != NULL) {
            TRACE("LircWatcher::run() received command: %s", command);
            Controler::instance()->queueEvent(new Event(m_eventMap.find(string(command))->second));
        }
        free(code);
        if (ret == -1) break;
    }
}
