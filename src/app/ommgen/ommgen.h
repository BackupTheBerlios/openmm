/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

using Omm::DeviceRoot;
using Omm::Device;
using Omm::Service;
using Omm::StateVar;
using Omm::Action;
using Omm::Argument;

// TODO: the original UPnP-AV XML service description contain eventedAttributes as
//       elements and not as attributes. This case is not handled here and segfaults.

class StubWriter
{
public:
    StubWriter(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    virtual void write();
    
protected:
    virtual void deviceRoot(const DeviceRoot& deviceRoot) {}
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot) {}
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
    
    DeviceRoot*                         _pDeviceRoot;
    std::string                         _outputPath;
    std::string                         _deviceName;
    std::map<std::string,std::string>   _typeMapper;
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
    
    std::ofstream               _out;
    std::vector<std::string>    _serviceNames;
};




class DeviceCpp : public StubWriter
{
public:
    DeviceCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream               _out;
    std::vector<std::string>    _serviceNames;
    std::vector<std::string>    _servicePaths;
    std::string                 _currentService;
    std::string                 _currentOutArgs;
    std::stringstream           _currentOutArgSetter;
    std::stringstream           _getSet;
    std::stringstream           _stateVarInitializer;
    bool                        _firstService;
    bool                        _firstAction;
};


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
    
    std::ofstream       _out;
};


class DeviceImplCpp : public StubWriter
{
public:
    DeviceImplCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
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
    DeviceDescH(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    std::string escapeDescription(const std::string& description);
    
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    
    std::ofstream       _out;
//     std::ostream*       _out;
};


class DeviceCtrlImplH : public StubWriter
{
public:
    DeviceCtrlImplH(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    virtual void serviceTypeEnd(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream       _out;
//     std::ostream*       _out;
    std::stringstream   _eventedStateVars;
};


class DeviceCtrlImplCpp : public StubWriter
{
public:
    DeviceCtrlImplCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void serviceType(const Service& service);
    virtual void action(const Action& action);
    virtual void actionEnd(const Action& action);
    virtual void argument(const Argument& argument, bool lastArgument = false);
    virtual void stateVar(const StateVar& stateVar);
    
    std::ofstream       _out;
//     std::ostream*       _out;
    std::string         _serviceName;
};


class DeviceCtrlH : public StubWriter
{
public:
    DeviceCtrlH(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
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


class DeviceCtrlCpp : public StubWriter
{
public:
    DeviceCtrlCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath);
    
private:
    virtual void deviceRoot(const DeviceRoot& deviceRoot);
    virtual void deviceRootEnd(const DeviceRoot& deviceRoot);
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
