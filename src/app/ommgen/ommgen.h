/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef OMMGEN_H
#define OMMGEN_H

#include <iostream>
#include <sstream>

#include <Omm/Upnp.h>

using Omm::DeviceContainer;
using Omm::Device;
using Omm::Service;
using Omm::StateVar;
using Omm::Action;
using Omm::Argument;

// TODO: the original UPnP-AV XML service descriptions contain eventedAttributes as
//       elements and not as attributes. This case is not handled here and segfaults.

class StubWriter
{
public:
    StubWriter(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    virtual void write();
    
protected:
    virtual void deviceContainer(const DeviceContainer& deviceContainer) {}
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer) {}
    virtual void serviceType(const Service& service) {}
    virtual void serviceTypeEnd(const Service& service) {}
    virtual void action(const Action& action) {}
    virtual void actionEnd(const Action& action) {}
    virtual void actionBlockEnd() {}
    virtual void argument(const Argument& argument, bool lastArgument = false) {}
    virtual void stateVar(const StateVar& stateVar) {}
    
    static std::string preamble;
    static std::string samplePreamble;
    std::string indent(int level);
    std::string firstLetterToLower(const std::string& s);
    
    DeviceContainer*                    _pDeviceContainer;
    std::string                         _outputPath;
    std::string                         _deviceType;
    std::string                         _deviceName;
    std::map<std::string,std::string>   _typeMapper;
};


class DevDeviceH : public StubWriter
{
public:
    DevDeviceH(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void actionBlockEnd();
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               _out;
    std::vector<std::string>    _serviceNames;
};




class DevDeviceCpp : public StubWriter
{
public:
    DevDeviceCpp(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               _out;
    std::vector<std::string>    _serviceTypes;
    std::vector<std::string>    _servicePaths;
    std::vector<std::string>    _serviceNames;
    std::string                 _currentService;
    std::string                 _currentOutArgs;
    std::stringstream           _currentOutArgSetter;
    std::stringstream           _getSet;
    std::stringstream           _stateVarInitializer;
    std::stringstream           _serviceActionDispatcher;
    bool                        _firstService;
    bool                        _firstAction;
};


class DevDeviceImplH : public StubWriter
{
public:
    DevDeviceImplH(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    
    std::ofstream       _out;
};


class DevDeviceImplCpp : public StubWriter
{
public:
    DevDeviceImplCpp(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    
    std::ofstream       _out;
    std::string         _serviceName;
};


class DeviceDescH : public StubWriter
{
public:
    DeviceDescH(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
//    std::string escapeDescription(const std::string& description);
    
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
//    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
//    virtual void serviceType(const Service& service);
    
    std::ofstream       _out;
};


class DeviceDescCpp : public StubWriter
{
public:
    DeviceDescCpp(DeviceContainer* pDeviceContainer, const std::string& outputPath);

private:
    std::string escapeDescription(const std::string& description);

    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    
    std::ofstream               _out;
    std::vector<std::string>    _serviceNames;
    std::vector<std::string>    _serviceTypes;
};


class CtlDeviceImplH : public StubWriter
{
public:
    CtlDeviceImplH(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream       _out;
    std::stringstream   _eventedStateVars;
};


class CtlDeviceImplCpp : public StubWriter
{
public:
    CtlDeviceImplCpp(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream       _out;
    std::string         _serviceName;
};


class CtlDeviceH : public StubWriter
{
public:
    CtlDeviceH(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               _out;
    std::stringstream           _reqAction;
    std::stringstream           _ansAction;
    std::stringstream           _threadAction;
    std::stringstream           _reqActionArgs;
    std::stringstream           _ansActionArgs;
    std::stringstream           _getEventedStateVars;
    std::stringstream           _changeEventedStateVars;
    std::vector<std::string>    _serviceNames;
};


class CtlDeviceCpp : public StubWriter
{
public:
    CtlDeviceCpp(DeviceContainer* pDeviceContainer, const std::string& outputPath);
    
private:
    virtual void deviceContainer(const DeviceContainer& deviceContainer);
    virtual void deviceContainerEnd(const DeviceContainer& deviceContainer);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               _out;
    std::stringstream           _reqAction;
    std::stringstream           _ansAction;
    std::stringstream           _threadAction;
    std::stringstream           _reqActionArgs;
//     std::stringstream           _ansActionArgs;
    std::stringstream           _getEventedStateVars;
    std::stringstream           _changeEventedStateVars;
    std::stringstream           _inArgSetter;
    std::stringstream           _inArgGetter;
    std::stringstream           _outArgGetter;
    std::stringstream           _outArgGetterLocal;
    std::stringstream           _allArgsCall;
    std::stringstream           _eventDispatcher;
    std::vector<std::string>    _serviceTypes;
    std::vector<std::string>    _serviceNames;
    std::string                 _currentService;
    bool                        _firstStateVar;
};

#endif
