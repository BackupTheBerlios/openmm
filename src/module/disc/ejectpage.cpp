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
#include "ejectpage.h"
#include "debug.h"

#include <stdlib.h>


EjectPage::EjectPage(string name, HalDeviceEvent::DeviceT devType, string devPath)
 : Page(name)
{
    TRACE("EjectPage::EjectPage()");
    m_devType = devType;
    m_devPath = devPath;
}


void
EjectPage::showUp()
{
    TRACE("EjectPage::showUp()");
    if (m_devType == HalDeviceEvent::VolumeT) {
        // umount medium
        TRACE("EjectPage::showUp() trying to umount volume.");
        system(("pumount -l " + m_devPath).c_str());
    }
    if (m_devType == HalDeviceEvent::DvdT || m_devType == HalDeviceEvent::AudioCdT || m_devType == HalDeviceEvent::VolumeT) {
        // eject medium
        TRACE("EjectPage::showUp() trying to eject medium.");
        system(("eject " + m_devPath).c_str());
    }
    // return to main menu
}


EjectPage::~EjectPage()
{
}


