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
#ifndef HAL_H
#define HAL_H

/**
  Hal keeps a list of all devices and their properties.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/

#include <dbus-c++/dbus.h>

#include <vector>
#include <map>
using namespace std;


class HalDevice : public DBus::InterfaceProxy, public DBus::ObjectProxy
{
public:
    HalDevice(DBus::Connection& connection, DBus::Path& udi);

private:
    map<string, DBus::Variant> getAllProperties();
    void printProperties(map<string, DBus::Variant> props);
    void propertyModifiedCb(const DBus::SignalMessage& sig);
    void conditionCb(const DBus::SignalMessage& sig);

    friend class HalManager;
};


class HalManager : public DBus::InterfaceProxy, public DBus::ObjectProxy
{
public:
    HalManager(DBus::Connection& connection);

private:
    vector<string> getAllDevices();
    void deviceAddedCb(const DBus::SignalMessage& sig);
    void deviceRemovedCb(const DBus::SignalMessage& sig);

    map<string, DBus::RefPtr<HalDevice> > m_devices;
    map<string, map<string, DBus::Variant> > m_properties;
};

#endif
