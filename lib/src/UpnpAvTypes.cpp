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

namespace Omm {
namespace Av {


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


// Resource::Resource(const std::string& resourceId, const std::string& protInfo, ui4 size) :
// _resourceId(resourceId),
// _protInfo(protInfo),
// _size(size)
// {
// }

Resource::Resource(const std::string& uri, const std::string& protInfo, ui4 size) :
_uri(uri),
_protInfo(protInfo),
_size(size)
{
}


const std::string&
Resource::getUri()
{
    return _uri;
}


const std::string&
Resource::getProtInfo()
{
    return _protInfo;
}


ui4
Resource::getSize()
{
    return _size;
}


void
Resource::setUri(const std::string& uri)
{
    _uri = uri;
}


void
Resource::setProtInfo(const std::string& protInfo)
{
    _protInfo = protInfo;
}


AbstractResource::AbstractResource(const std::string& uri, const std::string& protInfo, ui4 size)
{
    setName("res");
    setUri(uri);
    setProtInfo(protInfo);
    setSize(size);
}


std::string
AbstractResource::getUri()
{
    return getValue();
}


std::string
AbstractResource::getProtInfo()
{
    return getAttribute("protocolInfo");
}


ui4
AbstractResource::getSize()
{
    return Poco::NumberParser::parseUnsigned64(getAttribute("size"));
}


void
AbstractResource::setUri(const std::string& uri)
{
    setValue(uri);
}


void
AbstractResource::setProtInfo(const std::string& protInfo)
{
    setAttribute("protocolInfo", protInfo);
}


void
AbstractResource::setSize(ui4 size)
{
    setAttribute("size", Poco::NumberFormatter::format(size));
}


MediaObject::MediaObject() :
_parent(0),
_isContainer(false),
_restricted(true)
{
}


bool
MediaObject::isContainer()
{
    return _isContainer;
}


bool
MediaObject::isRestricted()
{
    return _restricted;
}

void
MediaObject::setIsContainer(bool isContainer)
{
    _isContainer = isContainer;
}


void
MediaObject::appendChild(MediaObject* pChild)
{
//     std::clog << "MediaObject::appendChild() with objectId: " << pChild->_objectId << std::endl;
    
    _children.push_back(pChild);
//     _childrenMap[pChild->_objectId] = pChild;
//     _childCount++;
//     pChild->_objectId = objectId;
//     pChild->_parentId = _objectId;
    pChild->_parent = this;
    
//     std::clog << "MediaObject::appendChild() finished" << std::endl;
}


void
MediaObject::addResource(Resource* pResource)
{
    _resources.push_back(pResource);
}


std::string
MediaObject::getObjectId() const
{
    if (_objectId == "0") {
        return _objectId;
    }
    else {
        if (_parent) {
            return _parent->getObjectId() + "/" + _objectId;
        }
        else {
            return "-1";
        }
    }
}


MediaObject::PropertyIterator
MediaObject::beginProperty()
{
    return _properties.begin();
}


MediaObject::PropertyIterator
MediaObject::endProperty()
{
    return _properties.end();
}


MediaObject::ChildIterator
MediaObject::beginChildren()
{
    return _children.begin();
}


MediaObject::ChildIterator
MediaObject::endChildren()
{
    return _children.end();
}


MediaObject::ResourceIterator
MediaObject::beginResource()
{
    return _resources.begin();
}

MediaObject::ResourceIterator
MediaObject::endResource()
{
    return _resources.end();
}


ui4
MediaObject::getChildCount()
{
    return _children.size();
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


// void
// MediaObject::setParentId(const std::string& parentId)
// {
//     _parentId = parentId;
// }


void
MediaObject::setTitle(const std::string& title)
{
//     std::clog << "MediaObject::setTitle() title: " << title << std::endl;
    Log::instance()->upnpav().debug("setting object title: " + title);
    
//     _properties.append("dc:title", new Omm::Variant(title));
    _properties["dc:title"] = title;
//     std::clog << "MediaObject::setTitle() finished" << std::endl;
}


std::string
MediaObject::getTitle()
{
//     std::clog << "MediaObject::getTitle()" << std::endl;
    
//     std::string res = _properties.getValue<std::string>("dc:title");
//     if (res == "") {
//         return "foo";
//     }
//     return res;
    return _properties["dc:title"];
}


void
MediaObject::setProperty(const std::string& name, const std::string& value)
{
//     std::clog << "MediaObject::setProperty() name : " << name << ", value: " << value << std::endl;
//     _properties.append(name, new Omm::Variant(value));
    _properties[name] = value;
}


// void
// MediaObject::addResource(const std::string& uri, const std::string& protInfo, ui4 size)
// {
//     Resource* pRes = new Resource;
//     pRes->_uri = uri;
//     pRes->_protInfo = protInfo;
//     pRes->_size = size;
//     _resources.push_back(pRes);
// }


// void
// MediaObject::addResource(Resource* pResource)
// {
//     pResource->_uri = _objectId + "$" + pResource->_resourceId;
//     // FIXME: transfer protocol (http-get) should be added in ItemServer
//     pResource->_protInfo = "http-get:*:" + pResource->_protInfo;
//     
//     _resources.push_back(pResource);
//     _resourceMap[pResource->_resourceId] = pResource;
// }


// void
// MediaObject::addResource(const std::string& resourceId, const std::string& privateUri, const std::string& protInfo, ui4 size)
// {
//     Resource* pRes = new Resource;
//     pRes->_uri = _objectId + "$" + resourceId;
//     // FIXME: transfer protocol (http-get) should be added in ItemServer
//     pRes->_protInfo = "http-get:*:" + protInfo;
//     pRes->_size = size;
//     pRes->_resourceId = resourceId;
//     pRes->_privateUri = privateUri;
//     _resources.push_back(pRes);
//     _resourceMap[resourceId] = pRes;
// }


// Resource*
// MediaObject::getResource(int num)
// {
//     return _resources[num];
// }


// Resource*
// MediaObject::getResource(const std::string& resourceId)
// {
//     return _resourceMap[resourceId];
// }


MediaObjectWriter::MediaObjectWriter(MediaObject* pMediaObject) :
_pMediaObject(pMediaObject),
_pDoc(0),
_pDidl(0)
// _pDoc(new Poco::XML::Document),
// _pDidl(_pDoc->createElement("DIDL-Lite"))
{
}


void
MediaObjectWriter::write(std::string& metaData)
{
//     std::clog << "MediaObjectWriter::write()" << std::endl;
    writeMetaDataHeader();
    writeMetaData(_pDidl);
    writeMetaDataClose(metaData);
//     std::clog << "MediaObjectWriter::write() finished" << std::endl;
}


ui4
MediaObjectWriter::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
//     std::clog << "MediaObjectWriter::writeChildren()" << std::endl;
    writeMetaDataHeader();
    
    ui4 c;
    for (c = 0; (c < requestedCount) && (c < _pMediaObject->getChildCount() - startingIndex); ++c) {
//         std::clog << "MediaObject::writeChildren() child title: " << _children[startingIndex + c]->getTitle() << std::endl;
//         _pMediaObject->getChild(startingIndex + c)->writeMetaData(_pDidl);
        MediaObjectWriter writer(_pMediaObject->getChild(startingIndex + c));
        writer.writeMetaData(_pDidl);
    }
    
    writeMetaDataClose(metaData);
//     std::clog << "MediaObjectWriter::writeChildren() finished" << std::endl;
    return c;
}


void
MediaObjectWriter::writeMetaDataHeader()
{
//     std::clog << "MediaObjectWriter::writeMetaDataHeader()" << std::endl;
//     Poco::XML::Document* _pDoc = new Poco::XML::Document;
    _pDoc = new Poco::XML::Document;
    
    _pDidl = _pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    
    _pDoc->appendChild(_pDidl);
    
//     std::clog << "MediaObjectWriter::writeMetaDataHeader() finished" << std::endl;

//     _pDidl = _pDoc->createElementNS("urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
//     Poco::AutoPtr<Poco::XML::Attr> pUpnpNs = _pDoc->createAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp");
//     _pDidl->setAttributeNode(pUpnpNs);
//     Poco::AutoPtr<Poco::XML::Attr> pDcNs = _pDoc->createAttributeNS("http://purl.org/dc/elements/1.1/", "dc");
//     _pDidl->setAttributeNode(pDcNs);
//     _pDidl->setAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp", "");

}


void MediaObjectWriter::writeMetaDataClose(std::string& metaData)
{
//     std::clog << "MediaObjectWriter::writeMetaDataClose() pDoc: " << _pDoc << std::endl;
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
//     writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
//     std::clog << "meta data:" << std::endl << ss.str() << std::endl;
//     std::clog << "MediaObjectWriter::writeMetaDataClose() finished" << std::endl;
}


void
MediaObjectWriter::writeMetaData(Poco::XML::Element* pDidl)
{
//     std::clog << "MediaObjectWriter::writeMetaData() object title: " << _pMediaObject->getTitle() << std::endl;
    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (_pMediaObject->isContainer()) {
//         std::clog << "MediaObjectWriter::writeMetaData() is container" << std::endl;
        pObject = pDoc->createElement("container");
        // childCount (Integer)
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
        pChildCount->setValue(Poco::NumberFormatter::format(_pMediaObject->getChildCount()));
        pObject->setAttributeNode(pChildCount);
    }
    else {
//         std::clog << "MediaObjectWriter::writeMetaData() is item" << std::endl;
        pObject = pDoc->createElement("item");
    }
    // write attributes:
    // id (String, required)
//     std::clog << "MediaObjectWriter::writeMetaData() attributes" << std::endl;
//     std::clog << "MediaObjectWriter::writeMetaData() id: " << _pMediaObject->getObjectId() << std::endl;
    // FIXME: when writing meta data in SetAVTransportURI(), 0/ is prepended
    pObject->setAttribute("id", _pMediaObject->getObjectId());
    // parentID (String, required)
//     std::clog << "MediaObjectWriter::writeMetaData() parentID: " << _pMediaObject->getParentId() << std::endl;
    pObject->setAttribute("parentID", _pMediaObject->getParentId());
    // restricted (Boolean, required)
//     std::clog << "MediaObjectWriter::writeMetaData() restricted: " << (_pMediaObject->isRestricted() ? "1" : "0") << std::endl;
    pObject->setAttribute("restricted", (_pMediaObject->isRestricted() ? "1" : "0"));
    
    // searchable (Boolean)
    // refID (String)
    
    // resources
    for (MediaObject::ResourceIterator it = _pMediaObject->beginResource(); it != _pMediaObject->endResource(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement("res");
        Poco::AutoPtr<Poco::XML::Text> pUri = pDoc->createTextNode((*it)->getUri());
        if ((*it)->getProtInfo() != "") {
            pResource->setAttribute("protocolInfo", (*it)->getProtInfo());
        }
        if ((*it)->getSize() > 0) {
            pResource->setAttribute("size", Poco::NumberFormatter::format((*it)->getSize()));
        }
        pResource->appendChild(pUri);
        pObject->appendChild(pResource);
    }
    
    // write properties
//     std::clog << "MediaObjectWriter::writeMetaData() property elements" << std::endl;
    for (MediaObject::PropertyIterator it = _pMediaObject->beginProperty(); it != _pMediaObject->endProperty(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement((*it).first);
//         std::string propVal;
//         (*it).second->getValue(propVal);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode((*it).second);
        pProperty->appendChild(pPropertyValue);
        pObject->appendChild(pProperty);
    }
    
    pDidl->appendChild(pObject);
// check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)
//     std::clog << "MediaObjectWriter::writeMetaData() finished" << std::endl;
}



std::string
AbstractMediaObject::getTitle()
{
    return getProperty("dc:title")->getValue();
}


AbstractMediaObject*
AbstractMediaObject::getObject(const std::string& objectId)
{
    std::string::size_type slashPos = objectId.find('/');
    AbstractMediaObject* pChild;
    if (slashPos != std::string::npos) {
        pChild = getChild(objectId.substr(0, slashPos));
        if (pChild == 0) {
            Log::instance()->upnpav().error("child objectId of container, but no child container found");
            return 0;
        }
        else {
            return pChild->getObject(objectId.substr(slashPos + 1));
        }
    }
    else {
        pChild = getChild(objectId);
        if (pChild == 0) {
            Log::instance()->upnpav().error("no child item found");
            return 0;
        }
        else {
            return pChild;
        }
    }
}


bool
AbstractMediaObject::fetchedAllChildren()
{
    return getChildCount() >= getTotalChildCount();
}


std::string
AbstractMediaObject::getParentObjectId()
{
    return getParent()->getObjectId();
}


ui4
AbstractMediaObject::getTotalChildCount()
{
    return getChildCount();
}


AbstractResource*
AbstractMediaObject::getResource(int index)
{
    return static_cast<AbstractResource*>(getProperty("res", index));
}


AbstractResource*
AbstractMediaObject::getResource(const std::string& resourceId)
{
    return static_cast<AbstractResource*>(getProperty("res", resourceId));
}


void
AbstractMediaObject::addResource(AbstractResource* pResource)
{
    addProperty(pResource);
}


int
AbstractMediaObject::getResourceCount()
{
    return getPropertyCount("res");
}


MediaObjectReader::MediaObjectReader(AbstractMediaObject* pMediaObject) :
_pMediaObject(pMediaObject)
{
}


void
MediaObjectReader::readChildren(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            AbstractMediaObject* pChildObject = _pMediaObject->createObject();
            readNode(pChildObject, pObjectNode);
            _pMediaObject->appendChild(pChildObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
}


void
MediaObjectReader::read(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    readNode(_pMediaObject, pDidl);
}


void
MediaObjectReader::readNode(AbstractMediaObject* pObject, Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        pObject->setObjectId(attr->getNamedItem("id")->nodeValue());
    }
    if (pNode->nodeName() == "container") {
        pObject->setIsContainer(true);
        if (attr != 0) {
            pObject->setChildCount(Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue()));
        }
    }
    if (attr != 0) {
        attr->release();
    }
    
    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
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
                AbstractResource* pResource = pObject->createResource();
                pResource->setUri(childNode->innerText());
                pResource->setProtInfo(protInfo);
                pResource->setSize(size);
                pObject->addResource(pResource);
                if (attr != 0) {
                    attr->release();
                }
            }
            else {
                AbstractProperty* pProp = pObject->createProperty();
                pProp->setName(childNode->nodeName());
                pProp->setValue(childNode->innerText());
                pObject->addProperty(pProp);
//                 pObject->addProperty(childNode->nodeName(), childNode->innerText());
            }
            childNode = childNode->nextSibling();
        }
    }
}


MediaObjectWriter2::MediaObjectWriter2(AbstractMediaObject* pMediaObject) :
_pMediaObject(pMediaObject),
_pDoc(0),
_pDidl(0)
{
}


void
MediaObjectWriter2::write(std::string& metaData)
{
writeMetaDataHeader();
writeMetaData(_pDidl);
writeMetaDataClose(metaData);
}


ui4
MediaObjectWriter2::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
    writeMetaDataHeader();
    
    ui4 c;
    for (c = 0; (c < requestedCount) && (c < _pMediaObject->getChildCount() - startingIndex); ++c) {
        MediaObjectWriter2 writer(_pMediaObject->getChild(startingIndex + c));
        writer.writeMetaData(_pDidl);
    }
    
    writeMetaDataClose(metaData);
    return c;
}


void
MediaObjectWriter2::writeMetaDataHeader()
{
    _pDoc = new Poco::XML::Document;
    
    _pDidl = _pDoc->createElement("DIDL-Lite");
    
    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    
    _pDoc->appendChild(_pDidl);
}


void
MediaObjectWriter2::writeMetaDataClose(std::string& metaData)
{
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
}


void
MediaObjectWriter2::writeMetaData(Poco::XML::Element* pDidl)
{
    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (_pMediaObject->isContainer()) {
        pObject = pDoc->createElement("container");
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
        pChildCount->setValue(Poco::NumberFormatter::format(_pMediaObject->getChildCount()));
        pObject->setAttributeNode(pChildCount);
    }
    else {
        pObject = pDoc->createElement("item");
    }
    // write attributes:
    // FIXME: when writing meta data in SetAVTransportURI(), 0/ is prepended
    pObject->setAttribute("id", _pMediaObject->getObjectId());
    // parentID (String, required)
    pObject->setAttribute("parentID", _pMediaObject->getParentObjectId());
    // restricted (Boolean, required)
    pObject->setAttribute("restricted", (_pMediaObject->isRestricted() ? "1" : "0"));
    
    // searchable (Boolean)
    // refID (String)
    
    // resources
    for (int resNum = 0; resNum < _pMediaObject->getResourceCount(); ++resNum) {
        AbstractResource* pRes = _pMediaObject->getResource(resNum);
        Poco::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement("res");
        Poco::AutoPtr<Poco::XML::Text> pUri = pDoc->createTextNode(pRes->getUri());
        if (pRes->getProtInfo() != "") {
            pResource->setAttribute("protocolInfo", pRes->getProtInfo());
        }
        if (pRes->getSize() > 0) {
            pResource->setAttribute("size", Poco::NumberFormatter::format(pRes->getSize()));
        }
        pResource->appendChild(pUri);
        pObject->appendChild(pResource);
    }
    
    // write properties
    for (int propNum = 0; propNum < _pMediaObject->getPropertyCount(); ++propNum) {
        AbstractProperty* pProp = _pMediaObject->getProperty(propNum);
        std::string name = pProp->getName();
        std::string value = pProp->getValue();
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement(name);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode(value);
        pProperty->appendChild(pPropertyValue);
        pObject->appendChild(pProperty);
    }
    
    pDidl->appendChild(pObject);
    // check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)
}


void
MemoryMediaObject::setParent(AbstractMediaObject* pParent)
{
    _parent = pParent;
}


void
MemoryMediaObject::appendChild(AbstractMediaObject* pChild)
{
    _childVec.push_back(pChild);
    _childMap[pChild->getObjectId()] = pChild;
    pChild->setParent(this);

}


void
MemoryMediaObject::addResource(AbstractResource* pResource)
{
    _resourceMap[pResource->getValue()] = pResource;
}


AbstractMediaObject*
MemoryMediaObject::getChild(const std::string& objectId)
{
    return _childMap[objectId];
}


ui4
MemoryMediaObject::getChildCount()
{
    return _childVec.size();
}


bool
MemoryMediaObject::isContainer()
{
    return _isContainer;
}


AbstractMediaObject*
MemoryMediaObject::getChild(ui4 numChild)
{
    return _childVec[numChild];
}


std::string
MemoryMediaObject::getObjectId()
{
    return _id;
}


// int
// MemoryMediaObject::getPropertyCount()
// {
//     return _propertyMap.size();
// }


int
MemoryMediaObject::getResourceCount()
{
    return _resourceMap.size();
}


bool
MemoryMediaObject::isRestricted()
{
    return _restricted;
}


// std::string
// MemoryMediaObject::getPropertyName(int numProperty)
// {
//     std::map<std::string,std::string>::iterator it = _propertyMap.begin()++;
// //     _propertyMap.begin() += numProperty;
// //     return *(_propertyMap.begin() + numProperty).first;
// }
// 
// 
// std::string
// MemoryMediaObject::getProperty(const std::string& property)
// {
// }
// 
// 
AbstractResource*
MemoryMediaObject::getResource(int numResource)
{
}


} // namespace Av
} // namespace Omm
