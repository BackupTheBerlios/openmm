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
    readNode(pDidl);
}


void
MediaObject::readNode(Poco::XML::Node* pNode)
{
//     std::clog << "MediaObject::readNode()" << std::endl;
    
    Poco::XML::NamedNodeMap* attr = NULL;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        m_objectId = attr->getNamedItem("id")->nodeValue();
        m_parentId = attr->getNamedItem("parentID")->nodeValue();

    }
    if (pNode->nodeName() == "container") {
        m_isContainer = true;
        if (attr != NULL) {
            m_childCount = Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue());
        }
    }
    if (attr != NULL) {
        attr->release();
    }
    
//     std::clog << "isContainer: " << (isContainer() ? "1" : "0") << std::endl;
//     std::clog << "id: " << m_objectId << std::endl;
//     std::clog << "parentId: " << m_parentId << std::endl;
//     std::clog << "childCount: " << m_childCount << std::endl;
    
    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
//             std::clog << childNode->nodeName() << ": " << childNode->innerText() << std::endl;
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
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            MediaObject* pObject = new MediaObject();
            pObject->readNode(pObjectNode);
            pObject->m_parent = this;
            pObject->m_server = m_server;
            m_children.push_back(pObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
}


void
MediaObject::writeMetaData(std::string& metaData)
{
}


void
MediaObject::writeObject(MediaObject* object)
{
}


void
MediaObject::writeChildren(Jamm::ui4 startingIndex, Jamm::ui4 requestedCount, std::string& metaData)
{
}


std::string
MediaObject::getProperty(const std::string& name)
{
//     std::clog << "MediaObject::getProperty() name: " << name << std::endl;
    
    return m_properties.getValue<std::string>(name);
}


std::string
MediaObject::getTitle()
{
//     std::clog << "MediaObject::getTitle()" << std::endl;
    
    std::string res = m_properties.getValue<std::string>("dc:title");
//     if (res == "") {
//         return "foo";
//     }
    return res;
}


int
MediaObject::fetchChildren()
{
    // TODO: browse meta data for the root object with id "0"
//     std::clog << "MediaObject::fetchChildren() objectId: " << m_objectId << std::endl;
    if (m_server && !m_fetchedAllChildren) {
        std::string result;
        Jamm::ui4 numberReturned;
        Jamm::ui4 totalMatches;
        Jamm::ui4 updateId;
        m_server->ContentDirectory()->Browse(m_objectId, "BrowseDirectChildren", "*", m_children.size(), 10, "", result, numberReturned, totalMatches, updateId);
        readChildren(result);
//         UpnpBrowseResult res = m_server->browseChildren(this, m_children.size(), UpnpServer::m_sliceSize);
        // m_totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        if (m_children.size() >= totalMatches) {
            m_fetchedAllChildren = true;
        }
        m_childCount = totalMatches;
    }
    return m_childCount;
}


void
MediaObject::setIsContainer(bool isContainer)
{
    m_isContainer = isContainer;
}


void
MediaObject::appendChild(const std::string& objectId, MediaObject* pChild)
{
    m_children.push_back(pChild);
    m_childrenMap[objectId] = pChild;
    pChild->setObjectId(objectId);
}


MediaObject*
MediaObject::getObject(const std::string& objectId)
{
    MediaObject* pChild = m_childrenMap[objectId];
    if (pChild->isContainer()) {
        return pChild->getObject(objectId);
    }
    else {
        return pChild;
    }
}


void
MediaObject::setParentId(const std::string& parentId)
{
    m_parentId = parentId;
}


void
MediaObject::setObjectId(const std::string& objectId)
{
    m_objectId = objectId;
}


void
MediaObject::setTitle(const std::string& title)
{
    m_properties.append("dc:title", new Jamm::Variant(title));
}


void
MediaObject::setResource(const std::string& resource)
{
    m_properties.append("res", new Jamm::Variant(resource));
}


const std::string&
MediaObject::getObjectId() const
{
    return m_objectId;
}


unsigned int
MediaObject::getChildCount()
{
    return m_childCount;
}
