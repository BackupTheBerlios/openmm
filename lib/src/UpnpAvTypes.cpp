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
#include <Poco/DateTime.h>
#include <Poco/DateTimeParser.h>
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

#include "utf8.h"


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
    Log::instance()->upnpav().debug("writeDuration() with duration: " + Poco::NumberFormatter::format(duration, 2));
    int hours = duration / 3600.0;
    int minutes = (duration - hours * 3600) / 60.0;
    r8 seconds = duration - hours * 3600 - minutes * 60;
    
    Log::instance()->upnpav().debug("writeDuration() formating string ...");
    Log::instance()->upnpav().debug("hours: " + Poco::NumberFormatter::format(hours));
    Log::instance()->upnpav().debug("minutes: " + Poco::NumberFormatter::format0(minutes, 2));
    Log::instance()->upnpav().debug("seconds: " + Poco::NumberFormatter::format(seconds, 2));
    return
        Poco::NumberFormatter::format(hours) + ":" +
        Poco::NumberFormatter::format0(minutes, 2) + ":" +
        Poco::NumberFormatter::format(seconds, 2);
}


time
AvTypeConverter::readTime(const std::string& timeString)
{
    Poco::DateTime res;
    int timeZoneDifferential;
    // fill date part of ISO8601 date format with epoch date
    try {
        Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, "1970-01-01T" + timeString, res, timeZoneDifferential);
    }
    catch (Poco::SyntaxException& e) {
        Log::instance()->upnpav().debug("parsing time failed: " + e.message());
        return 0;
    }
    return res.timestamp();
}


std::string
AvTypeConverter::writeTime(const time& timeVal)
{
    return Poco::DateTimeFormatter::format(timeVal, Poco::DateTimeFormat::ISO8601_FORMAT, 0).substr(11);
}


AbstractResource::AbstractResource(PropertyImpl* pPropertyImpl) :
AbstractProperty(pPropertyImpl)
{
}


std::string
AbstractResource::getUri()
{
    return getValue();
}


std::string
AbstractResource::getProtInfo()
{
    return getAttributeValue("protocolInfo");
}


ui4
AbstractResource::getSize()
{
    return Poco::NumberParser::parseUnsigned(getAttributeValue("size"));
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


AbstractProperty::AbstractProperty(PropertyImpl* pPropertyImpl) :
_pPropertyImpl(pPropertyImpl)
{
}


void
AbstractProperty::setName(const std::string& name)
{
    _pPropertyImpl->setName(name);
}


void
AbstractProperty::setValue(const std::string& value)
{
    _pPropertyImpl->setValue(value);
}


void
AbstractProperty::setAttribute(const std::string& name, const std::string& value)
{
    _pPropertyImpl->setAttribute(name, value);
}


std::string
AbstractProperty::getName()
{
    return _pPropertyImpl->getName();
}


std::string
AbstractProperty::getValue()
{
    return _pPropertyImpl->getValue();
}


std::string
AbstractProperty::getAttributeName(int index)
{
    return _pPropertyImpl->getAttributeName(index);
}


std::string
AbstractProperty::getAttributeValue(int index)
{
    return _pPropertyImpl->getAttributeValue(index);
}


std::string
AbstractProperty::getAttributeValue(const std::string& name)
{
    return _pPropertyImpl->getAttributeValue(name);
}


int
AbstractProperty::getAttributeCount() 
{
    return _pPropertyImpl->getAttributeCount();
}


AbstractMediaObject::AbstractMediaObject() :
_id(0),
_pParent(0)
{
}


void
AbstractMediaObject::setObjectNumber(ui4 id)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setObjectNumber() objectId: " + Poco::NumberFormatter::format(id));
    
    _id = id;
}


void
AbstractMediaObject::setObjectNumber(const std::string& id)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setObjectId() from string, objectId" + id);
    
    _id = Poco::NumberParser::parseUnsigned(id);
}


void
AbstractMediaObject::setParent(AbstractMediaObject* pParent)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setParent()");
    
    _pParent = pParent;
}


void
AbstractMediaObject::appendChild(AbstractMediaObject* pChild)
{
    pChild->setObjectNumber(getChildCount());
    pChild->setParent(this);
    appendChildImpl(pChild);
}


void
AbstractMediaObject::setUniqueProperty(const std::string& name, const std::string& value)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setUniqueProperty() name: " +  name + ", value: " + value);

    // TODO: check if property title is already set
    AbstractProperty* pProp = createProperty();
    pProp->setName(name);
    pProp->setValue(value);
    addProperty(pProp);
}


void
AbstractMediaObject::setTitle(const std::string& title)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setTitle() title: " + title);
    
    setUniqueProperty("dc:title", title);
}


void
AbstractMediaObject::setClass(const std::string& subclass)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setClass() subclass: " + subclass);
    
    setUniqueProperty("upnp:class", subclass);
}


std::string
AbstractMediaObject::getTitle()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getTitle()");
    
    return getProperty("dc:title")->getValue();
}


AbstractMediaObject*
AbstractMediaObject::getChild(const std::string& objectId)
{
    return getChild(Poco::NumberParser::parseUnsigned(objectId));
}


AbstractMediaObject*
AbstractMediaObject::getObject(const std::string& objectId)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setObject() objectId: " + objectId);
    
    std::string::size_type slashPos = objectId.find('/');
    AbstractMediaObject* pChild;
    if (slashPos != std::string::npos) {
        // child is a container
        pChild = getChild(objectId.substr(0, slashPos));
        if (pChild == 0) {
            // child container is not a child of this container, we try the full path
            pChild = getChild(objectId);
            if (pChild == 0) {
                // child container is not a child of this container
                Log::instance()->upnpav().error("retrieving child objectId of container, but no child container found");
                return 0;
            }
            else {
                // object id of child contains slashes (subtree's implementation is a faked tree with only item objects).
                return pChild;
            }
        }
        else {
            // recurse into child container
            return pChild->getObject(objectId.substr(slashPos + 1));
        }
    }
    else {
        // child is an item
        pChild = getChild(objectId);
        if (pChild == 0) {
            // child item is not a child of this container
            Log::instance()->upnpav().error("no child item found");
            return 0;
        }
        else {
            // return child item and stop recursion
            return pChild;
        }
    }
}


ui4
AbstractMediaObject::getObjectNumber()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getObjectNumber id: " + Poco::NumberFormatter::format(_id));
    
    return _id;
}


std::string
AbstractMediaObject::getObjectId()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getObjectId()");

    AbstractMediaObject* pParent = getParent();
    if (pParent == 0) {
        return "0";
    }
    else {
        return pParent->getObjectId() + "/" + Poco::NumberFormatter::format(getObjectNumber());
    }
}


bool
AbstractMediaObject::fetchedAllChildren()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::fetchedAllChildren()");
    
    return getChildCount() >= getTotalChildCount();
}


AbstractMediaObject*
AbstractMediaObject::getParent()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getParent()");

    return _pParent;
}


std::string
AbstractMediaObject::getParentObjectId()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getParentObjectId()");
    
    AbstractMediaObject* pParent = getParent();
    if (pParent) {
        return pParent->getObjectId();
    }
    else {
        return "";
    }
}


int
AbstractMediaObject::fetchChildren()
{
    return 0;
}


ui4
AbstractMediaObject::getTotalChildCount()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getTotalChildCount()");
    
    return getChildCount();
}


AbstractResource*
AbstractMediaObject::getResource(int index)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::setResource() index: " + Poco::NumberFormatter::format(index));
    
    return static_cast<AbstractResource*>(getProperty("res", index));
}


void
AbstractMediaObject::addResource(AbstractResource* pResource)
{
    Log::instance()->upnpav().debug("AbstractMediaObject::addResource()");
    
    addProperty(pResource);
}


int
AbstractMediaObject::getResourceCount()
{
    Log::instance()->upnpav().debug("AbstractMediaObject::getResourceCount()");
    
    return getPropertyCount("res");
}


void
MemoryPropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
MemoryPropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


void
MemoryPropertyImpl::setAttribute(const std::string& name, const std::string& value)
{
    _attrMap[name] = value;
}


std::string
MemoryPropertyImpl::getName()
{
    return _name;
}


std::string
MemoryPropertyImpl::getValue()
{
    return _value;
}


std::string
MemoryPropertyImpl::getAttributeName(int index)
{
    int i = 0;
    std::map<std::string,std::string>::iterator it;
    for (it = _attrMap.begin(); (it != _attrMap.end()) && (i < index); ++it, ++i) {
    }
    return (*it).first;
}


std::string
MemoryPropertyImpl::getAttributeValue(int index)
{
    int i = 0;
    std::map<std::string,std::string>::iterator it;
    for (it = _attrMap.begin(); (it != _attrMap.end()) && (i < index); ++it, ++i) {
    }
    return (*it).second;
}


std::string
MemoryPropertyImpl::getAttributeValue(const std::string& name)
{
    return _attrMap[name];
}


int
MemoryPropertyImpl::getAttributeCount() 
{
    return _attrMap.size();
}


MemoryMediaObject::MemoryMediaObject() :
_restricted(true),
_isContainer(false)
{
}


MemoryMediaObject::~MemoryMediaObject()
{
}


AbstractMediaObject*
MemoryMediaObject::createChildObject()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::createObject()");

    AbstractMediaObject* pChild = new MemoryMediaObject;
    pChild->setObjectNumber(Poco::NumberFormatter::format(getChildCount()));
    appendChild(pChild);
    return pChild;
}


AbstractProperty*
MemoryMediaObject::createProperty()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::createProperty()");
    
    return new AbstractProperty(new MemoryPropertyImpl);
}


AbstractResource*
MemoryMediaObject::createResource()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::createResource()");
    
    AbstractResource* pResource = new MemoryResource;
    addResource(pResource);
    return pResource;
}


// void
// MemoryMediaObject::setObjectId(const std::string& id)
// {
//     Log::instance()->upnpav().debug("MemoryMediaObject::setObjectId() objectId" + objectId);
//     
//     _id = objectId;
// }

// void
// MemoryMediaObject::setObjectId(ui4 id)
// {
//     Log::instance()->upnpav().debug("MemoryMediaObject::setObjectId() objectId: " + Poco::NumberFormatter::format(id));
//     
//     _id = id;
// }


// void
// MemoryMediaObject::setParent(AbstractMediaObject* pParent)
// {
//     Log::instance()->upnpav().debug("MemoryMediaObject::setParent()");
//     
//     _parent = pParent;
// }


void
MemoryMediaObject::setIsContainer(bool isContainer)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::setIsContainer()");
    
    _isContainer = isContainer;
}


void
MemoryMediaObject::appendChildImpl(AbstractMediaObject* pChild)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::appendChildImpl()");
    
    _childVec.push_back(pChild);
}


// AbstractMediaObject*
// MemoryMediaObject::getParent()
// {
//     Log::instance()->upnpav().debug("MemoryMediaObject::getParent()");
// 
//     return _parent;
// }


ui4
MemoryMediaObject::getChildCount()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::getChildCount()");
    
    return _childVec.size();
}


bool
MemoryMediaObject::isContainer()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::isContainer()");
    
    return _isContainer;
}


AbstractMediaObject*
MemoryMediaObject::getChild(ui4 numChild)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::getChild() number: " + Poco::NumberFormatter::format(numChild));
    
    return _childVec[numChild];
}


bool
MemoryMediaObject::isRestricted()
{
    Log::instance()->upnpav().debug("MemoryMediaObject::isRestricted()");
    
    return _restricted;
}


void
MemoryMediaObject::addProperty(AbstractProperty* pProperty)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::addProperty()");
    
    _propertyVec.push_back(pProperty);
    _propertyMap.insert(make_pair(pProperty->getName(), pProperty));
}


AbstractProperty*
MemoryMediaObject::getProperty(int index)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::getProperty() index: " + Poco::NumberFormatter::format(index));
    
    return _propertyVec[index];
}


int
MemoryMediaObject::getPropertyCount(const std::string& name)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::getPropertyCount() name: " + name);
    
    if (name == "") {
        return _propertyMap.size();
    }
    else {
        return _propertyMap.count(name);
    }
}


AbstractProperty*
MemoryMediaObject::getProperty(const std::string& name, int index)
{
    Log::instance()->upnpav().debug("MemoryMediaObject::getProperty() name: " + name + ", index: " + Poco::NumberFormatter::format(index));
    
    std::pair<PropertyIterator,PropertyIterator> range = _propertyMap.equal_range(name);
    int i = 0;
    for (PropertyIterator it = range.first; it != range.second; ++it, ++i) {
        if (i == index) {
            return (*it).second;
        }
    }
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
            AbstractMediaObject* pChildObject = _pMediaObject->createChildObject();
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
        // FIXME: object number is the full object path, take only the last segment here ...? Otherwise, Poco::NumberParser will crash.
        pObject->setObjectNumber(attr->getNamedItem("id")->nodeValue());
    }
    if (pNode->nodeName() == "container") {
        pObject->setIsContainer(true);
        if (attr != 0) {
            pObject->setTotalChildCount(Poco::NumberParser::parseUnsigned(attr->getNamedItem("childCount")->nodeValue()));
        }
    }
    if (attr != 0) {
        attr->release();
    }
    
    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
            // TODO: special treatment of resources shouldn't be necessary
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
    Log::instance()->upnpav().debug("MediaObjectWriter2::MediaObjectWriter2()");
}


void
MediaObjectWriter2::write(std::string& metaData)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::write()");
    writeMetaDataHeader();
    writeMetaData(_pDidl);
    writeMetaDataClose(metaData);
}


ui4
MediaObjectWriter2::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeChildren()");
    writeMetaDataHeader();
    
    ui4 c;
    ui4 childCount = _pMediaObject->getChildCount();
    for (c = 0; (c < requestedCount) && (c < childCount - startingIndex); ++c) {
        MediaObjectWriter2 writer(_pMediaObject->getChild(startingIndex + c));
        writer.writeMetaData(_pDidl);
    }
    
    writeMetaDataClose(metaData);
    return c;
}


void
MediaObjectWriter2::writeMetaDataHeader()
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataHeader()");
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
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataClose() ...");
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataClose() returns: \n" + metaData);
}


void
MediaObjectWriter2::writeMetaData(Poco::XML::Element* pDidl)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData()");

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

    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() writing attributes ...");
    // write attributes:
    // id (String, required)
    std::string parentId = _pMediaObject->getParentObjectId();
    std::string objectId = (parentId == "") ? "0" : (parentId + "/" + Poco::NumberFormatter::format(_pMediaObject->getObjectNumber()));
    pObject->setAttribute("id", objectId);
    // parentID (String, required)
    pObject->setAttribute("parentID", parentId);
    // restricted (Boolean, required)
    pObject->setAttribute("restricted", (_pMediaObject->isRestricted() ? "1" : "0"));
    
    // searchable (Boolean)
    // refID (String)
    
    // properties
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() writing properties ...");
    // write properties
    int propCount = _pMediaObject->getPropertyCount();
    for (int propNum = 0; propNum < propCount; ++propNum) {
        AbstractProperty* pProp = _pMediaObject->getProperty(propNum);
        std::string name = pProp->getName();
        std::string value = pProp->getValue();
        // non UTF-8 characters cause the DOM writer to stop and leave an unfinished XML fragment.
        replaceNonUtf8(value);
        Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() property: " + name + ", " + value);
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement(name);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode(value);
        pProperty->appendChild(pPropertyValue);
        
        // write property attributes
        int attrCount = pProp->getAttributeCount();
        for (int attrNum = 0; attrNum < attrCount; ++attrNum) {
            pProperty->setAttribute(pProp->getAttributeName(attrNum), pProp->getAttributeValue(attrNum));
        }
        
        pObject->appendChild(pProperty);
    }
    
    pDidl->appendChild(pObject);
    // check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)
    
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() finished.");
}


void
MediaObjectWriter2::replaceNonUtf8(std::string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(temp));
    str = temp;
}


/*--------------- convenience classes ------------------*/

MemoryProperty::MemoryProperty() :
AbstractProperty(new MemoryPropertyImpl)
{
}


MemoryResource::MemoryResource() :
AbstractResource(new MemoryPropertyImpl)
{
}



/*--------------- depricated classes ------------------*/

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


} // namespace Av
} // namespace Omm
