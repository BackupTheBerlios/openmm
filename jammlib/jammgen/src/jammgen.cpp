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

#include <Poco/String.h>

#include "jammgen.h"

StubWriter::StubWriter(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
m_pDeviceRoot(pDeviceRoot),
m_outputPath(outputPath)
{
    Jamm::Urn deviceType(pDeviceRoot->getRootDevice()->getDeviceType());
    m_deviceName = deviceType.getTypeName();
    m_outputPath += "/";
    
    m_typeMapper["boolean"] = "bool";
    m_typeMapper["ui1"] = "Jamm::ui1";
    m_typeMapper["ui2"] = "Jamm::ui2";
    m_typeMapper["ui4"] = "Jamm::ui4";
    m_typeMapper["i1"] = "Jamm::i1";
    m_typeMapper["i2"] = "Jamm::i2";
    m_typeMapper["i4"] = "Jamm::i4";
    m_typeMapper["r4"] = "Jamm::r4";
    m_typeMapper["r8"] = "Jamm::r8";
    m_typeMapper["number"] = "Jamm::number";
    m_typeMapper["string"] = "std::string";
}


void
StubWriter::write()
{
    deviceRoot(*m_pDeviceRoot);
    for (DeviceRoot::ServiceTypeIterator s = m_pDeviceRoot->beginServiceType(); s != m_pDeviceRoot->endServiceType(); ++s) {
        Service& rs = *((*s).second);
        serviceType(rs);
        for (Service::ActionIterator a = rs.beginAction(); a != rs.endAction(); ++a) {
            Action& ra = **a;
            action(ra);
            for (Action::ArgumentIterator arg = ra.beginArgument(); arg != ra.endArgument(); ++arg) {
                int dist = distance(arg, ra.endArgument());
                Argument& rarg = **arg;
                argument(rarg, dist == 1);
            }
            actionEnd(ra);
        }
        actionBlockEnd();
        for (Service::StateVarIterator sv = rs.beginStateVar(); sv != rs.endStateVar(); ++sv) {
            StateVar& rsv = **sv;
            stateVar(rsv);
        }
        serviceTypeEnd(rs);
    }
    deviceRootEnd(*m_pDeviceRoot);
}


std::string
StubWriter::indent(int level)
{
    std::string res = "";
    while(level--) {
        res += "    ";
    }
    return res;
}


std::string
StubWriter::firstLetterToLower(const std::string& s)
{
    return Poco::toLower(s.substr(0, 1)) + s.substr(1);
}


DeviceH::DeviceH(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
StubWriter(pDeviceRoot, outputPath),
m_out((m_outputPath + m_deviceName + ".h").c_str())
{
}


void
DeviceH::deviceRoot(const DeviceRoot& deviceRoot)
{
    m_out
        << "#ifndef " << Poco::toUpper(m_deviceName) << "_H" << std::endl
        << "#define " << Poco::toUpper(m_deviceName) << "_H" << std::endl
        << std::endl
        << "#include <jamm/upnp.h>" << std::endl
        << std::endl
        << "using Jamm::DeviceRootImplAdapter;" << std::endl
        << "using Jamm::Service;" << std::endl
        << "using Jamm::Action;" << std::endl
        << std::endl
        << "class " << m_deviceName << ";"
        << std::endl;
}


void
DeviceH::deviceRootEnd(const DeviceRoot& deviceRoot)
{
    std::string ctorArgs = "";
//     std::string implPointers = "";
    int i = m_serviceNames.size();
    while (i--) {
        ctorArgs += m_serviceNames[i] + "* p" + m_serviceNames[i] + "Impl" + (i ? ", " : "");
    }
    
    m_out << std::endl
        << "class " << m_deviceName << " : public DeviceRootImplAdapter" << std::endl
        << "{" << std::endl
        << "public:" << std::endl
        << indent(1) << m_deviceName << "("
        << ctorArgs
        << ");" << std::endl
        << std::endl
        << "private:" << std::endl
        << indent(1) << "virtual void actionHandler(Action* action);" << std::endl
        << std::endl
        << indent(1) << "static std::string m_deviceDescription;" << std::endl
        ;
    
    i = m_serviceNames.size();
    while (i--) {
        m_out << indent(1) << m_serviceNames[i] << "* m_p" << m_serviceNames[i] << "Impl;" << std::endl;
    }
    
    m_out
        << "};" << std::endl
        << std::endl
        << "#endif" << std::endl
        << std::endl;
}


void
DeviceH::serviceType(const Service& service)
{
    Jamm::Urn serviceType(service.getServiceType());
    std::string serviceName = serviceType.getTypeName();
    m_serviceNames.push_back(serviceName);
    
    m_out << std::endl
        << "class " << serviceName << std::endl
        << "{" << std::endl
        << "friend class " << m_deviceName << ";" << std::endl
        << std::endl
        << "public:"
        << std::endl;
}


void
DeviceH::serviceTypeEnd(const Service& service)
{
    m_out << std::endl
        << "private:" << std::endl
        << indent(1) << "static std::string  m_description;" << std::endl
        << indent(1) << "Service*            m_pService;" << std::endl
        << "};" << std::endl
        << std::endl;
}


void
DeviceH::action(const Action& action)
{
    m_out
        << indent(1) << "virtual void " << action.getName() << "("
        ;
}


void
DeviceH::actionEnd(const Action& action)
{
    m_out
        << ") = 0;"
        << std::endl;
}


void
DeviceH::actionBlockEnd()
{
    m_out << std::endl;
}


void
DeviceH::argument(const Argument& argument, bool lastArgument)
{
    m_out
        << ((argument.getDirection() == "in") ? "const " : "")
        << m_typeMapper[argument.getRelatedStateVarReference()->getType()]
        << "& " << argument.getName()
        << (lastArgument ? "" : ", ")
        ;
}


void
DeviceH::stateVar(const StateVar& stateVar)
{
    if (stateVar.getName().substr(0, std::string("A_ARG_TYPE_").size()) == "A_ARG_TYPE_") {
        return;
    }
    
    m_out
        << indent(1) << "void _set" << stateVar.getName() << "(const "
        << m_typeMapper[stateVar.getType()] << "& val);"
        << std::endl
        << indent(1) << m_typeMapper[stateVar.getType()]
        << " _get" << stateVar.getName() << "();"
        << std::endl;
}


DeviceCpp::DeviceCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
StubWriter(pDeviceRoot, outputPath),
m_out((m_outputPath + m_deviceName + ".cpp").c_str())
// m_out(&std::cout)
{
}


void
DeviceCpp::deviceRoot(const DeviceRoot& deviceRoot)
{
    m_out
        << "#include <jamm/upnp.h>" << std::endl
        << "#include \"" << m_deviceName << ".h\"" << std::endl
        << "#include \"" << m_deviceName << "Descriptions.h\"" << std::endl
        << std::endl
        << std::endl
        << "void" << std::endl
        << m_deviceName << "::actionHandler(Action* pAction)" << std::endl
        << "{" << std::endl
        << indent(1) << "// the great action dispatcher" << std::endl;
}


void
DeviceCpp::deviceRootEnd(const DeviceRoot& deviceRoot)
{
    std::string deviceDescriptionPath = deviceRoot.getDescriptionUri().getPath();
    std::string ctorArgs = "";
    int i = m_serviceNames.size();
    while (i--) {
        ctorArgs += m_serviceNames[i] + "* p" + m_serviceNames[i] + "Impl" + (i ? ", " : "");
    }
    m_firstService = true;
    
    m_out
        << "}" << std::endl
        << std::endl
        << std::endl
        << m_deviceName << "::" << m_deviceName << "("
        << ctorArgs
        << ") :" << std::endl
        << "DeviceRootImplAdapter()," << std::endl
        ;
    
    i = m_serviceNames.size();
    while (i--) {
        m_out 
            << "m_p" << m_serviceNames[i] << "Impl(p" << m_serviceNames[i] << "Impl)"
            << (i ? ", " : "") << std::endl;
    }
    
    m_out
        << "{" << std::endl
        << indent(1) << "m_descriptions[\"" << deviceDescriptionPath << "\"]"
        << " = &" << m_deviceName << "::m_deviceDescription;" << std::endl;
    
    i = m_serviceNames.size();
    while (i--) {
        m_out
            << indent(1) << "m_descriptions[\"" << m_servicePaths[i] << "\"]"
            << " = &" << m_serviceNames[i] << "::m_description;" << std::endl;
    }
    
    m_out << std::endl
        << indent(1) << "Jamm::StringDescriptionReader descriptionReader(m_descriptions, \""
        << deviceDescriptionPath << "\");" << std::endl
        << indent(1) << "m_pDeviceRoot = descriptionReader.deviceRoot();" << std::endl
        << "}" << std::endl
        << std::endl
        << std::endl
        << m_getSet.str()
        << std::endl;
}


void
DeviceCpp::serviceType(const Service& service)
{
    Jamm::Urn serviceType(service.getServiceType());
    std::string serviceName = serviceType.getTypeName();
    m_serviceNames.push_back(serviceName);
    m_currentService = serviceName;
    m_servicePaths.push_back(service.getDescriptionPath());
    m_firstAction = true;
    
    m_out
        << indent(1) << (m_firstService ? "" : "else ")
        << "if (pAction->getService()->getServiceType() == \""
        << service.getServiceType() << "\") {" << std::endl
        << indent(2) << "m_p" << serviceName << "Impl->m_pService = pAction->getService();" << std::endl
        << indent(2) << "std::string actionName = pAction->getName();" << std::endl
        << std::endl;
    
    m_firstService = false;
}


void
DeviceCpp::serviceTypeEnd(const Service& service)
{
    m_out
        << indent(1) << "}"
        << std::endl;
}


void
DeviceCpp::action(const Action& action)
{
    m_out
        << indent(2) << (m_firstAction ? "" : "else ")
        << "if (actionName == \"" << action.getName() << "\") {" << std::endl
        ;
    m_currentOutArgs = "";
    m_currentOutArgSetter.str("");
    m_firstAction = false;
}


void
DeviceCpp::actionEnd(const Action& action)
{
    m_out
        // do the implementation callback here
        << indent(3) << "m_p" << m_currentService << "Impl->" << action.getName()
        << "(" << m_currentOutArgs << ");" << std::endl
        << m_currentOutArgSetter.str()
        << indent(2) << "}"
        << std::endl;
}


void
DeviceCpp::argument(const Argument& argument, bool lastArgument)
{
    std::string argType = m_typeMapper[argument.getRelatedStateVarReference()->getType()];
    m_out
        << indent(3) << argType
        << " " /*<< argument.getDirection()*/ << argument.getName();
    if (argument.getDirection() == "in") {
        m_out <<
            " = pAction->getArgument<" << argType << ">(\"" << argument.getName() << "\")";
    }
    m_out
        << ";" << std::endl;
    m_currentOutArgs += /*(argument.getDirection() == "in" ? "const " : "")*/
//         + argType + "& "
        /*+ */argument.getName()
        + (lastArgument ? "" : ", ");
    if (argument.getDirection() == "out") {
        m_currentOutArgSetter
            << indent(3) << "pAction->setArgument<"
            << argType << ">(\"" << argument.getName()
            << "\", " << argument.getName() 
            << ");" << std::endl;
    }
}


void
DeviceCpp::stateVar(const StateVar& stateVar)
{
    if (stateVar.getName().substr(0, std::string("A_ARG_TYPE_").size()) == "A_ARG_TYPE_") {
        return;
    }
    
    std::string stateVarType = m_typeMapper[stateVar.getType()];
    m_getSet
        // StateVar setter method
        << "void" << std::endl
        << m_currentService << "::_set" << stateVar.getName() 
        << "(const " << stateVarType << "& val)" << std::endl
        << "{" << std::endl
        << indent(1) << "m_pService->setStateVar<" << stateVarType 
        << ">(\"" << stateVar.getName() << "\", val);" << std::endl
        << "}" << std::endl
        << std::endl
        << stateVarType << std::endl
        
        // StateVar getter method
        << m_currentService << "::_get" << stateVar.getName() 
        << "()" << std::endl
        << "{" << std::endl
        << indent(1) << "return m_pService->getStateVar<" << stateVarType 
        << ">(\"" << stateVar.getName() << "\");" << std::endl
        << "}" << std::endl
        << std::endl;
}


DeviceImplH::DeviceImplH(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
StubWriter(pDeviceRoot, outputPath),
m_out((m_outputPath + m_deviceName + "Impl.h.sample").c_str())
// m_out(&std::cout)
{
}


void
DeviceImplH::deviceRoot(const DeviceRoot& deviceRoot)
{
    m_out
        << "#ifndef " << Poco::toUpper(m_deviceName) << "_IMPLEMENTATION_H" << std::endl
        << "#define " << Poco::toUpper(m_deviceName) << "_IMPLEMENTATION_H" << std::endl
        << std::endl
        << "#include <jamm/upnp.h>" << std::endl
        << "#include \"" << m_deviceName << ".h\"" << std::endl
        << std::endl;
}


void
DeviceImplH::deviceRootEnd(const DeviceRoot& deviceRoot)
{
    m_out
        << "#endif" << std::endl
        << std::endl;
}


void
DeviceImplH::serviceType(const Service& service)
{
    Jamm::Urn serviceType(service.getServiceType());
    std::string serviceName = serviceType.getTypeName();

    m_out
        << "class " << serviceName << "Implementation : public " << serviceName << std::endl
        << "{" << std::endl
        << "public:"
        << std::endl;
}


void
DeviceImplH::serviceTypeEnd(const Service& service)
{
    m_out
        << "};" << std::endl
        << std::endl;
}


void
DeviceImplH::action(const Action& action)
{
    m_out
        << indent(1) << "virtual void " << action.getName() << "("
        ;
}


void
DeviceImplH::actionEnd(const Action& action)
{
    m_out
        << ");"
        << std::endl;
}


void
DeviceImplH::argument(const Argument& argument, bool lastArgument)
{
    m_out
        << ((argument.getDirection() == "in") ? "const " : "")
        << m_typeMapper[argument.getRelatedStateVarReference()->getType()]
        << "& " << argument.getName()
        << (lastArgument ? "" : ", ")
        ;
}


DeviceImplCpp::DeviceImplCpp(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
StubWriter(pDeviceRoot, outputPath),
m_out((m_outputPath + m_deviceName + "Impl.cpp.sample").c_str())
// m_out(&std::cout)
{
}


void
DeviceImplCpp::deviceRoot(const DeviceRoot& deviceRoot)
{
    m_out
        << "#include <jamm/upnp.h>" << std::endl
        << "#include \"" << m_deviceName << "Impl.h\"" << std::endl
        << std::endl;
}


void
DeviceImplCpp::action(const Action& action)
{
    Jamm::Urn urn(action.getService()->getServiceType());
    m_out
        << "void" << std::endl << urn.getTypeName() << "Implementation::" << action.getName() 
        << "(";
}


void
DeviceImplCpp::actionEnd(const Action& action)
{
    m_out
        << ")" << std::endl
        << "{" << std::endl
        << "// begin of your own code" << std::endl
        << std::endl
        << "// end of your own code" << std::endl
        << "}" << std::endl
        << std::endl
        << std::endl;
}


void
DeviceImplCpp::argument(const Argument& argument, bool lastArgument)
{
    m_out
        << ((argument.getDirection() == "in") ? "const " : "")
        << m_typeMapper[argument.getRelatedStateVarReference()->getType()]
        << "& " << argument.getName()
        << (lastArgument ? "" : ", ")
        ;
}


DeviceDescH::DeviceDescH(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
StubWriter(pDeviceRoot, outputPath),
m_out((m_outputPath + m_deviceName + "Descriptions.h").c_str())
// m_out(&std::cout)
{
}


void
DeviceDescH::deviceRoot(const DeviceRoot& deviceRoot)
{
    m_out
        << "#ifndef " << Poco::toUpper(m_deviceName) << "_DESCRIPTIONS_H" << std::endl
        << "#define " << Poco::toUpper(m_deviceName) << "_DESCRIPTIONS_H" << std::endl
        << std::endl
        << "std::string " << m_deviceName << "::m_deviceDescription =" << std::endl
        << escapeDescription(deviceRoot.getDeviceDescription())
        << ";" << std::endl
        << std::endl;
}


void
DeviceDescH::deviceRootEnd(const DeviceRoot& deviceRoot)
{
    m_out
        << "#endif" << std::endl
        << std::endl;
}


void
DeviceDescH::serviceType(const Service& service)
{
    Jamm::Urn serviceType(service.getServiceType());
    std::string serviceName = serviceType.getTypeName();
    
    m_out
        << "std::string " << serviceName << "::m_description =" << std::endl
        << escapeDescription(service.getDescription())
        << ";" << std::endl
        << std::endl;
}


std::string
DeviceDescH::escapeDescription(const std::string& description)
{
    std::string res = description;
    
    // escape all quotes
    std::string::size_type i = 0;
    while ((i = res.find("\"", i)) != std::string::npos) {
        res.insert(i, "\\");
        ++i; ++i;
    }
    
    // delete all carriage returns
    i = 0;
    while ((i = res.find("\r", i)) != std::string::npos) {
        res.erase(i, 1);
    }
    
    // escape all newlines
    i = 0;
    while ((i = res.find("\n", i)) != std::string::npos) {
        res.insert(i, "\\");
        ++i; ++i;
    }
    
    return "\"" + res + "\"";
}
