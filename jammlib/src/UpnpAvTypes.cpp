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

#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include "UpnpAvTypes.h"

using namespace Jamm;
using namespace Jamm::Av;

r8
AvTypeConverter::readDuration(const std::string& duration)
{
    if (duration == "NOT_IMPLEMENTED") {
        return 0.0;
    }
    
    ui4 res;
    Poco::StringTokenizer tok(duration, ":./");
    
    res = 3600 * Poco::NumberParser::parse(tok[0])
        + 60 * Poco::NumberParser::parse(tok[1])
        + Poco::NumberParser::parse(tok[2]);
    
    if (tok.count() == 4) {
        res += Poco::NumberParser::parseFloat(tok[3]);
    }
    else if (tok.count() == 5) {
        res += Poco::NumberParser::parse(tok[3]) / Poco::NumberParser::parse(tok[4]);
    }
    return res;
}


std::string
AvTypeConverter::writeDuration(const r8& duration)
{
    std::string res;
    int hours = duration / 3600 - 0.5;
    int minutes = (duration - hours * 3600) / 60 - 0.5;
    r8 seconds = duration - hours * 3600 - minutes * 60;
    
    res = Poco::NumberFormatter::format(hours) + ":"
        + Poco::NumberFormatter::format0(minutes, 2) + ":"
        + Poco::NumberFormatter::format(seconds, 2, 3) + ":";
    
    return res;
}


MediaObject::MediaObject() :
m_parent(NULL),
m_childCount(0),
m_fetchedAllChildren(false),
m_isContainer(false)
{
}


void
MediaObject::readMetaData(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    // FIXME: this should be the DIDL-Lite element and not the object.
    //        because of missing <?xml> document tag?
    readNode(pDidl);
//     if (pDidl->hasChildNodes()) {
//         Poco::XML::Node* pObjectNode = pDidl->firstChild();
//         readNode(pObjectNode);
//     }
}


void
MediaObject::readNode(Poco::XML::Node* pNode)
{
    std::clog << "MediaObject::readNode()" << std::endl;
    
//     std::clog << "type: " << pNode->nodeName() << std::endl;
    if (pNode->hasAttributes()) {
        Poco::XML::NamedNodeMap* attr = pNode->attributes();
        m_objectId = attr->getNamedItem("id")->nodeValue();
        m_parentId = attr->getNamedItem("parentID")->nodeValue();
        m_childCount = Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue());
        attr->release();
    }
    if (pNode->nodeName() == "container") {
        m_isContainer = true;
    }
    std::clog << "isContainer: " << (isContainer() ? "1" : "0") << std::endl;
    std::clog << "id: " << m_objectId << std::endl;
    std::clog << "parentId: " << m_parentId << std::endl;
    std::clog << "childCount: " << m_childCount << std::endl;
    
    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
            std::clog << childNode->nodeName() << ": " << childNode->innerText() << std::endl;
            m_properties.append(childNode->nodeName(), new Variant(childNode->innerText()));
            childNode = childNode->nextSibling();
        }
    }
    std::clog << std::endl;
}


void
MediaObject::readChildren(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
//     Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
//     if (pDidl->hasChildNodes()) {
//         Poco::XML::Node* pObjectNode = pDidl->firstChild();
        while (pObjectNode)
        {
            if (pObjectNode->hasChildNodes()) {
                MediaObject* pObject = new MediaObject();
                pObject->readNode(pObjectNode);
                pObject->m_parent = this;
                m_children.push_back(pObject);
            }
            pObjectNode = pObjectNode->nextSibling();
        }
//     }
}


const std::string&
MediaObject::getProperty(const std::string& name)
{
    return m_properties.getValue<std::string>(name);
}


const std::string&
MediaObject::getTitle()
{
    return m_properties.getValue<std::string>("dc:title");
}


int
MediaObject::fetchChildren()
{
}
