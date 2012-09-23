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

#ifndef Service_INCLUDED
#define Service_INCLUDED

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
#include <Poco/DOM/DocumentFragment.h>

namespace Omm {
namespace Dvb {

class Transponder;
class Stream;

class Service
{
    friend class Transponder;
    friend class Frontend;
    friend class Device;
    friend class Demux;

public:
//    Service(const std::string& name, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int pmtid);
    Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid);

    void addStream(Stream* pStream);
    void readXml(Poco::XML::Node* pXmlService);
    void writeXml(Poco::XML::Element* pTransponder);

private:
    Transponder*                _pTransponder;
    std::string                 _name;
    unsigned int                _sid;
    unsigned int                _pmtid;

    std::vector<Stream*>        _streams;
};

}  // namespace Omm
}  // namespace Dvb

#endif
