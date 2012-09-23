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

#include "Log.h"
#include "DvbLogger.h"
#include "Stream.h"
#include "Service.h"


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


Service::Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid) :
_pTransponder(pTransponder),
_name(name),
_sid(sid),
_pmtid(pmtid)
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
        Poco::XML::Node* pXmlStream = pXmlService->firstChild();
        while (pXmlStream && pXmlStream->nodeName() == "stream") {
            std::string type = static_cast<Poco::XML::Element*>(pXmlStream)->getAttribute("type");
            int pid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlStream)->getAttribute("pid"));
            Stream* pStream = new Stream(type, pid);
            addStream(pStream);
            pXmlStream = pXmlStream->nextSibling();
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
    pService->setAttribute("name", _name);
    pService->setAttribute("sid", Poco::NumberFormatter::format(_sid));
    pService->setAttribute("pmtid", Poco::NumberFormatter::format(_pmtid));

    for (std::vector<Stream*>::iterator it = _streams.begin(); it != _streams.end(); ++it) {
        (*it)->writeXml(pService);
    }

    LOG(dvb, debug, "wrote service.");
}


}  // namespace Omm
}  // namespace Dvb
