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
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>

#include "UpnpAvTypes.h"
#include "UpnpAvControllers.h"

using namespace Jamm;
using namespace Jamm::Av;


Log* Log::m_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::FormattingChannel* pFormatLogger = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
    Poco::FileChannel* pFileChannel = new Poco::FileChannel("jamm.log");
    pSplitterChannel->addChannel(pConsoleChannel);
    pSplitterChannel->addChannel(pFileChannel);
    pFormatLogger->setChannel(pSplitterChannel);
    pFormatLogger->open();
    m_pUpnpAvLogger = &Poco::Logger::create("UPNP.AV", pFormatLogger, Poco::Message::PRIO_DEBUG);
}


Log*
Log::instance()
{
    if (!m_pInstance) {
        m_pInstance = new Log;
    }
    return m_pInstance;
}


Poco::Logger&
Log::upnpav()
{
    return *m_pUpnpAvLogger;
}

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
m_isContainer(false),
m_restricted(true)
{
}


void
MediaObject::readMetaData(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    readNode(pDidl);
}


void
MediaObject::readNode(Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        m_objectId = attr->getNamedItem("id")->nodeValue();
        m_parentId = attr->getNamedItem("parentID")->nodeValue();

    }
    if (pNode->nodeName() == "container") {
        m_isContainer = true;
        if (attr != 0) {
            m_childCount = Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue());
        }
    }
    if (attr != 0) {
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
            
            if (childNode->nodeName() == "res") {
                Poco::XML::NamedNodeMap* attr = 0;
                std::string protInfo = "";
                ui4 size = 0;
                if (childNode->hasAttributes()) {
                    attr = childNode->attributes();
                    Poco::XML::Node* attrNode = attr->getNamedItem("protocolInfo");
                    if (attrNode) {
                        protInfo = attrNode->nodeValue();
                    }
                    attrNode = attr->getNamedItem("size");
                    if (attrNode) {
                        size = Poco::NumberParser::parseUnsigned(attrNode->nodeValue());
                    }
                }
                addResource(childNode->innerText(), protInfo, size);
                if (attr != 0) {
                    attr->release();
                }
            }
            else {
                m_properties.append(childNode->nodeName(), new Variant(childNode->innerText()));
            }
            childNode = childNode->nextSibling();
        }
    }
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
MediaObject::writeMetaDataHeader()
{
    m_pDoc = new Poco::XML::Document;
    
    m_pDidl = m_pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    m_pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    m_pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    m_pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    
    m_pDoc->appendChild(m_pDidl);
    
//     std::clog << "MediaObject::writeMetaDataHeader() finished" << std::endl;

//     m_pDidl = m_pDoc->createElementNS("urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
//     Poco::AutoPtr<Poco::XML::Attr> pUpnpNs = m_pDoc->createAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp");
//     m_pDidl->setAttributeNode(pUpnpNs);
//     Poco::AutoPtr<Poco::XML::Attr> pDcNs = m_pDoc->createAttributeNS("http://purl.org/dc/elements/1.1/", "dc");
//     m_pDidl->setAttributeNode(pDcNs);
//     m_pDidl->setAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp", "");

}


void MediaObject::writeMetaDataClose(std::string& metaData)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
//     writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    std::stringstream ss;
    writer.writeNode(ss, m_pDoc);
    metaData = ss.str();
//     std::clog << "meta data:" << std::endl << ss.str() << std::endl;
}


void
MediaObject::writeMetaData(std::string& metaData)
{
    writeMetaDataHeader();
    writeMetaData(m_pDidl);
    writeMetaDataClose(metaData);
}


void
MediaObject::writeMetaData(Poco::XML::Element* pDidl)
{
//     std::clog << "MediaObject::writeMetaData() title: " << getTitle() << std::endl;
    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (m_isContainer) {
//         std::clog << "MediaObject::writeMetaData() is container" << std::endl;
        pObject = pDoc->createElement("container");
        // childCount (Integer)
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
        pChildCount->setValue(Poco::NumberFormatter::format(m_childCount));
        pObject->setAttributeNode(pChildCount);
    }
    else {
//         std::clog << "MediaObject::writeMetaData() is item" << std::endl;
        pObject = pDoc->createElement("item");
    }
    // write attributes:
    // id (String, required)
//     std::clog << "MediaObject::writeMetaData() attributes" << std::endl;
//     std::clog << "MediaObject::writeMetaData() id: " << getObjectId() << std::endl;
    pObject->setAttribute("id", getObjectId());
    // parentID (String, required)
//     std::clog << "MediaObject::writeMetaData() parentID: " << getParentId() << std::endl;
    pObject->setAttribute("parentID", getParentId());
    // restricted (Boolean, required)
//     std::clog << "MediaObject::writeMetaData() restricted: " << (m_restricted ? "1" : "0") << std::endl;
    pObject->setAttribute("restricted", (m_restricted ? "1" : "0"));
    
    // searchable (Boolean)
    // refID (String)
    
    // resources
    for (std::vector<Resource*>::iterator it = m_resources.begin(); it != m_resources.end(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement("res");
        Poco::AutoPtr<Poco::XML::Text> pUri = pDoc->createTextNode((*it)->m_uri);
        if ((*it)->m_protInfo != "") {
            pResource->setAttribute("protocolInfo", (*it)->m_protInfo);
        }
        if ((*it)->m_size > 0) {
            pResource->setAttribute("size", Poco::NumberFormatter::format((*it)->m_size));
        }
        pResource->appendChild(pUri);
        pObject->appendChild(pResource);
    }
    
    // write elements
//     std::clog << "MediaObject::writeMetaData() property elements" << std::endl;
    for (Container<Variant>::KeyIterator i = m_properties.beginKey(); i != m_properties.endKey(); ++i) {
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement((*i).first);
        std::string propVal;
        (*i).second->getValue(propVal);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode(propVal);
        pProperty->appendChild(pPropertyValue);
        pObject->appendChild(pProperty);
    }
    
    pDidl->appendChild(pObject);
// check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)
//     std::clog << "MediaObject::writeMetaData() finished" << std::endl;
}


ui4
MediaObject::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
    writeMetaDataHeader();
    
    ui4 c;
    for (c = 0; (c < requestedCount) && (c < m_children.size() - startingIndex); ++c) {
//         std::clog << "MediaObject::writeChildren() child title: " << m_children[startingIndex + c]->getTitle() << std::endl;
        m_children[startingIndex + c]->writeMetaData(m_pDidl);
    }
    
    writeMetaDataClose(metaData);
    return c;
}


std::string
MediaObject::getProperty(const std::string& name)
{
//     std::clog << "MediaObject::getProperty() name: " << name << std::endl;
//     std::clog << "MediaObject::getProperty() number of properties: " << m_properties.size() << std::endl;
//     std::clog << "MediaObject::getProperty() value: " << m_properties.getValue<std::string>(name) << std::endl;
    
    return m_properties.getValue<std::string>(name);
}


// FIXME: parent id should recurse all parents up to root
// example: parentId("0/0/1") is "0" but should be "0/0"
std::string
MediaObject::getParentId()
{
    if (m_parent != NULL) {
        return m_parent->getObjectId();
    }
    else {
        return "-1";
    }
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
    m_childCount++;
    pChild->m_objectId = objectId;
    pChild->m_parentId = m_objectId;
    pChild->m_parent = this;
}


MediaObject*
MediaObject::getObject(const std::string& objectId)
{
//     std::clog << "MediaObject::getObject() objectId: " << objectId << std::endl;
    
    std::string::size_type slashPos = objectId.find('/');
    MediaObject* pChild;
    if (slashPos != std::string::npos) {
//         std::clog << "container id: " << objectId.substr(0, slashPos - 1) << std::endl;
        pChild = m_childrenMap[objectId.substr(0, slashPos)];
        if (pChild == NULL) {
            Log::instance()->upnpav().error("child objectId of container, but no child container found");
            return NULL;
        }
        else {
            return pChild->getObject(objectId.substr(slashPos + 1));
        }
    }
    else {
//         std::clog << "item id: " << objectId << std::endl;
        pChild = m_childrenMap[objectId];
        if (pChild == NULL) {
            Log::instance()->upnpav().error("no child item found");
            return NULL;
        }
        else {
            return pChild;
        }
    }
}


std::string
MediaObject::getObjectId() const
{
    if (m_objectId == "0") {
        return m_objectId;
    }
    else {
        return m_parent->getObjectId() + "/" + m_objectId;
    }
}


ui4
MediaObject::getChildCount()
{
    return m_childCount;
}


void
MediaObject::setObjectId(const std::string& objectId)
{
    m_objectId = objectId;
}


void
MediaObject::setParentId(const std::string& parentId)
{
    m_parentId = parentId;
}


void
MediaObject::setTitle(const std::string& title)
{
//     std::clog << "MediaObject::setTitle() title: " << title << std::endl;
    Log::instance()->upnpav().debug(Poco::format("setting object title: %s", title));
    
    m_properties.append("dc:title", new Jamm::Variant(title));
//     std::clog << "MediaObject::setTitle() finished" << std::endl;
}


void
MediaObject::addResource(const std::string& uri, const std::string& protInfo, ui4 size)
{
    Resource* pRes = new Resource;
    pRes->m_uri = uri;
    pRes->m_protInfo = protInfo;
    pRes->m_size = size;
    m_resources.push_back(pRes);
}


Resource*
MediaObject::getResource(int num)
{
    return m_resources[num];
}


MediaContainer::MediaContainer() :
MediaObject()
{
    m_isContainer = true;
}


MediaContainer::MediaContainer(const std::string& title, const std::string& subClass) :
MediaObject()
{
    m_isContainer = true;
    setTitle(title);
    m_properties.append("upnp:class", new Jamm::Variant(std::string("object.container" + (subClass == "" ? "" : "." + subClass))));
}


MediaItem::MediaItem() :
MediaObject()
{
}


MediaItem::MediaItem(const std::string& title, const std::string& uri, const std::string& protInfo, ui4 size, const std::string& subClass) :
MediaObject()
{
    setTitle(title);
    addResource(uri, protInfo, size);
    m_properties.append("upnp:class", new Jamm::Variant(std::string("object.item" + (subClass == "" ? "" : "." + subClass))));
}
