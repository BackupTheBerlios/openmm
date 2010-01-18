/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef JAMMGEN_H
#define JAMMGEN_H

#include <iostream>
#include <jamm/upnp.h>

using Jamm::DeviceRoot;
using Jamm::Device;
using Jamm::Service;
using Jamm::StateVar;
using Jamm::Action;
using Jamm::Argument;


class StubWriter
{
public:
    StubWriter(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    virtual void write();
    
protected:
    virtual void deviceRoot(const DeviceRoot& deviceRoot) {}
    virtual void device(const Device& device) {}
    virtual void service(const Service& service) {}
    virtual void stateVar(const StateVar& stateVar) {}
    virtual void action(const Action& action) {}
    virtual void argument(const Argument& argument) {}
    
private:
    DeviceRoot*     m_pDeviceRoot;
    std::string     m_outputPath;
};


class DeviceH : public StubWriter
{
public:
    DeviceH(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void device(const Device& device);
    virtual void service(const Service& service);
    virtual void stateVar(const StateVar& stateVar);
    virtual void action(const Action& action);
    virtual void argument(const Argument& argument);
};



/*
class DeviceCpp : public StubWriter
{
public:
    DeviceCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
    
private:
    void deviceRoot(const DeviceRoot& deviceRoot);
    void device(const Device& device);
    void service(const Service& service);
    void action(const Action& action);
};
*/

class DeviceImplH : public StubWriter
{
public:
    DeviceImplH(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void device(const Device& device);
    virtual void service(const Service& service);
    virtual void stateVar(const StateVar& stateVar);
    virtual void action(const Action& action);
    virtual void argument(const Argument& argument);
};

/*
class DeviceImplCpp : public StubWriter
{
public:
    DeviceImplCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};


class DeviceAppH : public StubWriter
{
public:
    DeviceAppH(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};


class DeviceAppCpp : public StubWriter
{
public:
    DeviceAppCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};


class DeviceCtlH : public StubWriter
{
public:
    DeviceCtlH(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};


class DeviceCtlCpp : public StubWriter
{
public:
    DeviceCtlCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};


class Cmake : public StubWriter
{
public:
    Cmake(DeviceRoot* pDeviceRoot, const std::string& outputPath) : StubWriter(pDeviceRoot, outputPath) {}
    virtual void write();
};
*/

#endif
