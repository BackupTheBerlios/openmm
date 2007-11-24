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
#ifndef HALDEVICEEVENT_H
#define HALDEVICEEVENT_H

#include "event.h"

#include <string>
using namespace std;


/**
	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class HalDeviceEvent : public Event
{
public:
    enum DeviceT {AnyT, VolumeT, DvdT, AudioCdT, SoundT};
    enum DeviceAction {Add, Remove};

    HalDeviceEvent(string deviceName, DeviceAction deviceAction, DeviceT deviceType, bool hotplug = true,
                string devPath = "", string label = "", string mediaPath = "");
    ~HalDeviceEvent();

    string getDevName() { return m_deviceName; }
    DeviceAction getDevAction() { return m_deviceAction; }
    string getDevPath() { return m_devPath; }
    string getMediaPath() { return m_mediaPath; }
    bool hotplug() { return m_hotplug; }
    DeviceT deviceType() { return m_deviceType; }
    string getLabel() { return m_label; }

private:
    string          m_deviceName;
    DeviceT         m_deviceType;
    bool            m_hotplug;
    string          m_devPath;
    string          m_mediaPath;
    string          m_label;
    DeviceAction    m_deviceAction;
};

#endif
