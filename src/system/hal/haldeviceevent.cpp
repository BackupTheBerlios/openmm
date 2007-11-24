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
#include "haldeviceevent.h"
#include "debug.h"


HalDeviceEvent::HalDeviceEvent(string deviceName, DeviceAction deviceAction, DeviceT deviceType, bool hotplug,
                            string devPath, string label, string mediaPath)
 : Event(Event::DeviceE)
{
    TRACE("HalDeviceEvent::HalDeviceEvent() type: %i, path: %s", deviceType, devPath.c_str());
    m_deviceName = deviceName;
    m_deviceType = deviceType;
    m_hotplug = hotplug;
    m_devPath = devPath;
    m_mediaPath = mediaPath;
    m_label = label;
    m_deviceAction = deviceAction;
}


HalDeviceEvent::~HalDeviceEvent()
{
}


