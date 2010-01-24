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
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot) {}
//     virtual void device(const Device& device) {}
    virtual void serviceType(const Service& service) {}
    virtual void serviceTypeEnd(const Service& service) {}
//     virtual void stateVarEnd(const StateVar& stateVar) {}
    virtual void action(const Action& action) {}
    virtual void actionEnd(const Action& action) {}
    virtual void actionBlockEnd() {}
    virtual void argument(const Argument& argument, bool lastArgument = false) {}
    virtual void stateVar(const StateVar& stateVar) {}
//     virtual void argumentEnd(const Argument& argument) {}
    
    std::string indent(int level);
    std::string firstLetterToLower(const std::string& s);
    
    DeviceRoot*                         m_pDeviceRoot;
    std::string                         m_outputPath;
    std::string                         m_deviceName;
    std::map<std::string,std::string>   m_typeMapper;
};


class DeviceH : public StubWriter
{
public:
    DeviceH(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void actionBlockEnd();
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               m_out;
    std::vector<std::string>    m_serviceNames;
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
    DeviceImplH(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    
    std::ofstream       m_out;
};


class DeviceImplCpp : public StubWriter
{
public:
    DeviceImplCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    
    std::ofstream       m_out;
//     std::ostream*       m_out;
};

/*
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
