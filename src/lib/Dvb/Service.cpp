/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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

#include <Poco/NumberParser.h>
#include <Poco/DOM/AbstractContainerNode.h>
#include <Poco/DOM/DOMException.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <Poco/NumberFormatter.h>

#include "Log.h"
#include "DvbLogger.h"
#include "Stream.h"
#include "Service.h"
#include "Transponder.h"


namespace Omm {
namespace Dvb {

//Service::Service(const std::string& name, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid) :
//_name(name),
//_vpid(vpid),
//_cpid(cpid),
//_apid(apid),
//_sid(sid),
//_tid(tid),
//_pmtid(pmtid)
//{
//}

const unsigned int Service::InvalidPcrPid(0);
const std::string Service::StatusUndefined("Undefined");
const std::string Service::StatusNotRunning("NotRunning");
const std::string Service::StatusStartsShortly("StartsShortly");
const std::string Service::StatusPausing("Pausing");
const std::string Service::StatusRunning("Running");
const std::string Service::StatusOffAir("OffAir");

Service::Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid) :
_pTransponder(pTransponder),
_name(name),
_sid(sid),
_pmtPid(pmtid),
_pcrPid(InvalidPcrPid),
_status(StatusUndefined),
_scrambled(false)
{
}


void
Service::addStream(Stream* pStream)
{
    _streams.push_back(pStream);
}


void
Service::readXml(Poco::XML::Node* pXmlService)
{
    LOG(dvb, debug, "read service ...");

    if (pXmlService->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlService->firstChild();
        while (pXmlParam) {
            if (pXmlParam->nodeName() == "stream") {
                std::string type = static_cast<Poco::XML::Element*>(pXmlParam)->getAttribute("type");
                int pid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlParam)->getAttribute("pid"));
                Stream* pStream = new Stream(type, pid);
                addStream(pStream);
            }
            else if (pXmlParam->nodeName() == "status") {
                Poco::XML::Node* pVal = pXmlParam->firstChild();
                if (!pVal) {
                    LOG(dvb, error, "dvb service status has no value");
                }
                else {
                    _status = pVal->innerText();
                }
            }
            else if (pXmlParam->nodeName() == "scrambled") {
                Poco::XML::Node* pVal = pXmlParam->firstChild();
                if (!pVal) {
                    LOG(dvb, error, "dvb service scrambled has no value");
                }
                else {
                    _scrambled = pVal->innerText() == "true" ? true : false;
                }
            }
            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb service description contains no stream");
        return;
    }

    LOG(dvb, debug, "read service.");
}


void
Service::writeXml(Poco::XML::Element* pTransponder)
{
    LOG(dvb, debug, "write service ...");

    Poco::XML::Document* pDoc = pTransponder->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pService = pDoc->createElement("service");
    pTransponder->appendChild(pService);
//    pService->setAttribute("name", _name);
    pService->setAttribute("name", getName());
    pService->setAttribute("sid", Poco::NumberFormatter::format(_sid));
    pService->setAttribute("pmtid", Poco::NumberFormatter::format(_pmtPid));

    for (std::vector<Stream*>::iterator it = _streams.begin(); it != _streams.end(); ++it) {
        (*it)->writeXml(pService);
    }

    Poco::AutoPtr<Poco::XML::Element> pStatus = pDoc->createElement("status");
    Poco::AutoPtr<Poco::XML::Text> pStatusVal = pDoc->createTextNode(_status);
    pStatus->appendChild(pStatusVal);
    pService->appendChild(pStatus);

    Poco::AutoPtr<Poco::XML::Element> pScrambled = pDoc->createElement("scrambled");
    Poco::AutoPtr<Poco::XML::Text> pScrambledVal = pDoc->createTextNode(std::string(_scrambled ? "true" : "false"));
    pScrambled->appendChild(pScrambledVal);
    pService->appendChild(pScrambled);

    LOG(dvb, debug, "wrote service.");
}


std::string
Service::getName()
{
    if (_name == "" || _name == ".") {
        return "Service " + Poco::NumberFormatter::format(_sid) + " (TS " + Poco::NumberFormatter::format(_pTransponder->_transportStreamId) + ")";
    }
    else {
        return _name;
    }
}


Transponder*
Service::getTransponder()
{
    return _pTransponder;
}



std::string
Service::statusToString(Poco::UInt8 status)
{
    switch (status) {
        case 0x00:
            return StatusUndefined;
        case 0x01:
            return StatusNotRunning;
        case 0x02:
            return StatusStartsShortly;
        case 0x03:
            return StatusPausing;
        case 0x04:
            return StatusRunning;
        case 0x05:
            return StatusOffAir;
        default:
            return "";
    }
}


}  // namespace Omm
}  // namespace Dvb
