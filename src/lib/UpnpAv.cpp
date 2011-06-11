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

#include <sstream>

#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>

#include "UpnpAvLogger.h"
#include "UpnpAvPrivate.h"
#include "UpnpInternal.h"


namespace Omm {
namespace Av {


LastChange::LastChange(Service* pService) :
_pService(pService),
_pDoc(0),
_pMessage(0)
{

}


LastChange::~LastChange()
{

}


LastChange::StateVarIterator
LastChange::beginStateVar()
{
    return _stateVars.begin();
}


LastChange::StateVarIterator
LastChange::endStateVar()
{
    return _stateVars.end();
}


LastChange::StateVarValIterator
LastChange::beginStateVarVal(const std::string& stateVar)
{
    return _stateVars[stateVar].begin();
}


LastChange::StateVarValIterator
LastChange::endStateVarVal(const std::string& stateVar)
{
    return _stateVars[stateVar].end();
}


void
LastChange::setStateVar(const ui4& InstanceID, const std::string& stateVarName)
{
    // FIXME: InstanceID ignored by now, val should be retrieved from the corresponding InstanceID
    std::string val = _pService->getStateVar<std::string>(stateVarName);
    _stateVars[stateVarName]["val"] = val;
}


void
LastChange::notify()
{

}


void
LastChange::write()
{
    writeMessageHeader();
    writeMessageData();
    writeMessageClose();
}


void
LastChange::writeMessageHeader()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageHeader()");
    _pDoc = new Poco::XML::Document;

    _pMessage = _pDoc->createElement("Event");

    // FIXME: is this the right way to set the namespaces?
    writeSchemeAttribute();
//    _pMessage->setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
//    _pMessage->setAttribute("xsi:schemaLocation", "urn:schemas-upnp-org:metadata-1-0/RCS/http://www.upnp.org/schemas/av/rcs-event-v1-20060531.xsd");

    _pDoc->appendChild(_pMessage);
}


void
LastChange::writeMessageClose()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageClose() ...");
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    _message = ss.str();
    Log::instance()->upnpav().debug("LastChange::writeMessageClose() writes message: \n" + _message);
}


void
LastChange::writeMessageData()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageData()");

    Poco::XML::Document* pDoc = _pMessage->ownerDocument();

    Poco::AutoPtr<Poco::XML::Element> pInstanceId = pDoc->createElement("InstanceID");
    Poco::AutoPtr<Poco::XML::Attr> pInstanceIdVal = pDoc->createAttribute("val");
    pInstanceIdVal->setValue("0"); // FIXME: only one instance supported, by now.

    for (StateVarIterator varIt = beginStateVar(); varIt != endStateVar(); ++varIt) {
        std::string varName = (*varIt).first;
        Poco::AutoPtr<Poco::XML::Element> pStateVar = pDoc->createElement(varName);
        for (StateVarValIterator it = beginStateVarVal(varName); it != endStateVarVal(varName); ++it) {
            Poco::AutoPtr<Poco::XML::Attr> pVal = pDoc->createAttribute((*it).first);
            pVal->setValue((*it).second);
            pStateVar->setAttributeNode(pVal);
        }
        pInstanceId->appendChild(pStateVar);
    }

    Log::instance()->upnpav().debug("LastChange::writeMessageData() finished.");
}


RenderingControlLastChange::RenderingControlLastChange(Service* pService) :
LastChange(pService)
{
}


void
RenderingControlLastChange::writeSchemeAttribute()
{
    _pMessage->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/RCS/");
}


}  // namespace Omm
}  // namespace Av
