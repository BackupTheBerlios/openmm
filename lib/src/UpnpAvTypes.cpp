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

using namespace Omm;
using namespace Omm::Av;


Av::Log* Av::Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Av::Log::Log()
{
    Poco::FormattingChannel* pFormatLogger = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
//     Poco::FileChannel* pFileChannel = new Poco::FileChannel("omm.log");
    pSplitterChannel->addChannel(pConsoleChannel);
//     pSplitterChannel->addChannel(pFileChannel);
    pFormatLogger->setChannel(pSplitterChannel);
    pFormatLogger->open();
    _pUpnpAvLogger = &Poco::Logger::create("UPNP.AV", pFormatLogger, Poco::Message::PRIO_DEBUG);
}


Av::Log*
Av::Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Av::Log::upnpav()
{
    return *_pUpnpAvLogger;
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
_parent(NULL),
_childCount(0),
_fetchedAllChildren(false),
_isContainer(false),
_restricted(true)
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
        _objectId = attr->getNamedItem("id")->nodeValue();
        _parentId = attr->getNamedItem("parentID")->nodeValue();

    }
    if (pNode->nodeName() == "container") {
        _isContainer = true;
        if (attr != 0) {
            _childCount = Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue());
        }
    }
    if (attr != 0) {
        attr->release();
    }
    
//     std::clog << "isContainer: " << (isContainer() ? "1" : "0") << std::endl;
//     std::clog << "id: " << _objectId << std::endl;
//     std::clog << "parentId: " << _parentId << std::endl;
//     std::clog << "childCount: " << _childCount << std::endl;
    
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
                _properties.append(childNode->nodeName(), new Variant(childNode->innerText()));
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
            pObject->_parent = this;
            pObject->_server = _server;
            _children.push_back(pObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
}


void
MediaObject::setServerController(MediaServerController* _pServer)
{
    _server = _pServer;
}


void
MediaObject::setFetchedAllChildren(bool fetchedAllChildren)
{
    _fetchedAllChildren = fetchedAllChildren;
}


void
MediaObject::writeMetaDataHeader()
{
    _pDoc = new Poco::XML::Document;
    
    _pDidl = _pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    
    _pDoc->appendChild(_pDidl);
    
//     std::clog << "MediaObject::writeMetaDataHeader() finished" << std::endl;

//     _pDidl = _pDoc->createElementNS("urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
//     Poco::AutoPtr<Poco::XML::Attr> pUpnpNs = _pDoc->createAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp");
//     _pDidl->setAttributeNode(pUpnpNs);
//     Poco::AutoPtr<Poco::XML::Attr> pDcNs = _pDoc->createAttributeNS("http://purl.org/dc/elements/1.1/", "dc");
//     _pDidl->setAttributeNode(pDcNs);
//     _pDidl->setAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp", "");

}


void MediaObject::writeMetaDataClose(std::string& metaData)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
//     writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
//     std::clog << "meta data:" << std::endl << ss.str() << std::endl;
}


void
MediaObject::writeMetaData(std::string& metaData)
{
    writeMetaDataHeader();
    writeMetaData(_pDidl);
    writeMetaDataClose(metaData);
}


void
MediaObject::writeMetaData(Poco::XML::Element* pDidl)
{
//     std::clog << "MediaObject::writeMetaData() title: " << getTitle() << std::endl;
    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (_isContainer) {
//         std::clog << "MediaObject::writeMetaData() is container" << std::endl;
        pObject = pDoc->createElement("container");
        // childCount (Integer)
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
        pChildCount->setValue(Poco::NumberFormatter::format(_childCount));
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
    // FIXME: when writing meta data in SetAVTransportURI(), 0/ is prepended
    pObject->setAttribute("id", getObjectId());
    // parentID (String, required)
//     std::clog << "MediaObject::writeMetaData() parentID: " << getParentId() << std::endl;
    pObject->setAttribute("parentID", getParentId());
    // restricted (Boolean, required)
//     std::clog << "MediaObject::writeMetaData() restricted: " << (_restricted ? "1" : "0") << std::endl;
    pObject->setAttribute("restricted", (_restricted ? "1" : "0"));
    
    // searchable (Boolean)
    // refID (String)
    
    // resources
    for (std::vector<Resource*>::iterator it = _resources.begin(); it != _resources.end(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement("res");
        Poco::AutoPtr<Poco::XML::Text> pUri = pDoc->createTextNode((*it)->_uri);
        if ((*it)->_protInfo != "") {
            pResource->setAttribute("protocolInfo", (*it)->_protInfo);
        }
        if ((*it)->_size > 0) {
            pResource->setAttribute("size", Poco::NumberFormatter::format((*it)->_size));
        }
        pResource->appendChild(pUri);
        pObject->appendChild(pResource);
    }
    
    // write elements
//     std::clog << "MediaObject::writeMetaData() property elements" << std::endl;
    for (Container<Variant>::KeyIterator i = _properties.beginKey(); i != _properties.endKey(); ++i) {
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
    for (c = 0; (c < requestedCount) && (c < _children.size() - startingIndex); ++c) {
//         std::clog << "MediaObject::writeChildren() child title: " << _children[startingIndex + c]->getTitle() << std::endl;
        _children[startingIndex + c]->writeMetaData(_pDidl);
    }
    
    writeMetaDataClose(metaData);
    return c;
}


std::string
MediaObject::getProperty(const std::string& name)
{
//     std::clog << "MediaObject::getProperty() name: " << name << std::endl;
//     std::clog << "MediaObject::getProperty() number of properties: " << _properties.size() << std::endl;
//     std::clog << "MediaObject::getProperty() value: " << _properties.getValue<std::string>(name) << std::endl;
    
    return _properties.getValue<std::string>(name);
}


MediaObject*
MediaObject::parent()
{
    return _parent;
}


std::string
MediaObject::getParentId()
{
    if (_parent) {
        return _parent->getObjectId();
    }
    else {
        return "-1";
    }
}


std::string
MediaObject::getTitle()
{
//     std::clog << "MediaObject::getTitle()" << std::endl;
    
    std::string res = _properties.getValue<std::string>("dc:title");
//     if (res == "") {
//         return "foo";
//     }
    return res;
}


MediaObject::childIterator
MediaObject::beginChildren()
{
    return _children.begin();
}


MediaObject::childIterator
MediaObject::endChildren()
{
    return _children.end();
}


int
MediaObject::fetchChildren()
{
    // TODO: browse meta data for the root object with id "0"
//     std::clog << "MediaObject::fetchChildren() objectId: " << _objectId << std::endl;
    if (_server && !_fetchedAllChildren) {
        std::string result;
        Omm::ui4 numberReturned;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        _server->ContentDirectory()->Browse(_objectId, "BrowseDirectChildren", "*", _children.size(), 10, "", result, numberReturned, totalMatches, updateId);
        readChildren(result);
//         UpnpBrowseResult res = _server->browseChildren(this, _children.size(), UpnpServer::_sliceSize);
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        if (_children.size() >= totalMatches) {
            _fetchedAllChildren = true;
        }
        _childCount = totalMatches;
    }
    return _childCount;
}


bool
MediaObject::fetchedAllChildren()
{
    return _fetchedAllChildren;
}


void
MediaObject::setIsContainer(bool isContainer)
{
    _isContainer = isContainer;
}


void
MediaObject::appendChild(const std::string& objectId, MediaObject* pChild)
{
    std::clog << "MediaObject::appendChild() with objectId: " << objectId << std::endl;

    _children.push_back(pChild);
    _childrenMap[objectId] = pChild;
    _childCount++;
    pChild->_objectId = objectId;
    pChild->_parentId = _objectId;
    pChild->_parent = this;
    
    std::clog << "MediaObject::appendChild() finished" << std::endl;
}


void
MediaObject::appendChild(MediaObject* pChild)
{
    std::clog << "MediaObject::appendChild() with objectId: " << pChild->_objectId << std::endl;
    
    _children.push_back(pChild);
    _childrenMap[pChild->_objectId] = pChild;
    _childCount++;
//     pChild->_objectId = objectId;
    pChild->_parentId = _objectId;
    pChild->_parent = this;
    
    std::clog << "MediaObject::appendChild() finished" << std::endl;
}


MediaObject*
MediaObject::getObject(const std::string& objectId)
{
//     std::clog << "MediaObject::getObject() objectId: " << objectId << std::endl;
    
    std::string::size_type slashPos = objectId.find('/');
    MediaObject* pChild;
    if (slashPos != std::string::npos) {
//         std::clog << "container id: " << objectId.substr(0, slashPos - 1) << std::endl;
        pChild = _childrenMap[objectId.substr(0, slashPos)];
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
        pChild = _childrenMap[objectId];
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
    if (_objectId == "0") {
        return _objectId;
    }
    else {
        return _parent->getObjectId() + "/" + _objectId;
    }
}


ui4
MediaObject::getChildCount()
{
    return _childCount;
}


ui4
MediaObject::childCount()
{
    return _children.size();
}


MediaObject*
MediaObject::getChild(ui4 num)
{
    return _children[num];
}


std::string
MediaObject::objectId()
{
    return _objectId;
}


void
MediaObject::setObjectId(const std::string& objectId)
{
    _objectId = objectId;
}


void
MediaObject::setParentId(const std::string& parentId)
{
    _parentId = parentId;
}


void
MediaObject::setTitle(const std::string& title)
{
//     std::clog << "MediaObject::setTitle() title: " << title << std::endl;
    Log::instance()->upnpav().debug(Poco::format("setting object title: %s", title));
    
    _properties.append("dc:title", new Omm::Variant(title));
//     std::clog << "MediaObject::setTitle() finished" << std::endl;
}


void
MediaObject::setProperty(const std::string& name, const std::string& value)
{
    _properties.append(name, new Omm::Variant(value));
}


void
MediaObject::addResource(const std::string& uri, const std::string& protInfo, ui4 size)
{
    Resource* pRes = new Resource;
    pRes->_uri = uri;
    pRes->_protInfo = protInfo;
    pRes->_size = size;
    _resources.push_back(pRes);
}


Resource*
MediaObject::getResource(int num)
{
    return _resources[num];
}


MediaContainer::MediaContainer() :
MediaObject()
{
    _isContainer = true;
}


MediaContainer::MediaContainer(const std::string& title, const std::string& subClass) :
MediaObject()
{
    _isContainer = true;
    setTitle(title);
    _properties.append("upnp:class", new Omm::Variant(std::string("object.container" + (subClass == "" ? "" : "." + subClass))));
}


MediaItem::MediaItem() :
MediaObject()
{
}


// MediaItem::MediaItem(const std::string& title, const std::string& subClass, bool restricted) :
// MediaObject()
// {
//     setTitle(title);
// }


MediaItem::MediaItem(const std::string& title, const std::string& uri, const std::string& protInfo, ui4 size, const std::string& subClass) :
MediaObject()
{
    setTitle(title);
    addResource(uri, protInfo, size);
    _properties.append("upnp:class", new Omm::Variant(std::string("object.item" + (subClass == "" ? "" : "." + subClass))));
}


// void
// MediaItem::addResource(const std::string& uri, const std::string& profile, ui4 size)
// {
//     MediaObject::addResource(uri, "http-get:*:" + profile, size);
// }
