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

#include "UpnpAvCtlObject.h"
#include "UpnpPrivate.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"


namespace Omm {
namespace Av {


CtlMediaObject::CtlMediaObject() :
Omm::Av::MediaObject(),
_childCount(0),
_fetchedAllChildren(false)
{
}


void
CtlMediaObject::addResource(Resource* pResource)
{
//     std::clog << "ControllerObject::addResource() with uri: " << pResource->getUri() << std::endl;
    _resources.push_back(pResource);
}


void
CtlMediaObject::readChildren(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            CtlMediaObject* pObject = new CtlMediaObject;
            pObject->readNode(pObjectNode);
            pObject->_parent = this;
            pObject->_server = _server;
            _children.push_back(pObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
}


void
CtlMediaObject::readMetaData(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    readNode(pObjectNode);
}


void
CtlMediaObject::readNode(Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        _objectId = attr->getNamedItem("id")->nodeValue();
//         _parentId = attr->getNamedItem("parentID")->nodeValue();

    }
    if (pNode->nodeName() == AvClass::CONTAINER) {
        _isContainer = true;
        if (attr != 0) {
            _childCount = Poco::NumberParser::parse(attr->getNamedItem(AvProperty::CHILD_COUNT)->nodeValue());
        }
    }
    if (attr != 0) {
        attr->release();
    }

//     std::clog << "isContainer: " << (_isContainer ? "1" : "0") << std::endl;
//     std::clog << "id: " << _objectId << std::endl;
// //     std::clog << "parentId: " << _parentId << std::endl;
//     std::clog << "childCount: " << _childCount << std::endl;

    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
//             std::clog << childNode->nodeName() << ": " << childNode->innerText() << std::endl;

            if (childNode->nodeName() == AvProperty::RES) {
                Poco::XML::NamedNodeMap* attr = 0;
                std::string protInfo = "";
                ui4 size = 0;
                if (childNode->hasAttributes()) {
                    attr = childNode->attributes();
                    Poco::XML::Node* attrNode = attr->getNamedItem(AvProperty::PROTOCOL_INFO);
                    if (attrNode) {
                        protInfo = attrNode->nodeValue();
                    }
                    attrNode = attr->getNamedItem(AvProperty::SIZE);
                    if (attrNode) {
                        size = Poco::NumberParser::parseUnsigned(attrNode->nodeValue());
                    }
                }
                addResource(new Resource(childNode->innerText(), protInfo, size));
//                 addResource(childNode->innerText(), protInfo, size);
                if (attr != 0) {
                    attr->release();
                }
            }
            else {
//                 _properties.append(childNode->nodeName(), new Variant(childNode->innerText()));
                _properties[childNode->nodeName()] =  childNode->innerText();
            }
            childNode = childNode->nextSibling();
        }
    }
}


int
CtlMediaObject::fetchChildren()
{
    // TODO: browse meta data for the root object with id "0"
//     std::clog << "MediaObject::fetchChildren() objectId: " << _objectId << std::endl;
    if (_server && !_fetchedAllChildren) {
        std::string result;
        Omm::ui4 numberReturned;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        try {
            _server->ContentDirectory()->Browse(_objectId, "BrowseDirectChildren", "*", _children.size(), 10, "", result, numberReturned, totalMatches, updateId);
        }
        catch (Poco::Exception& e){
//             error("");
// FIXME: if no children are fetched (network error) and _fetchedAllChildren remains false, controller doesn't stop fetching children
//             _fetchedAllChildren = true;
            Log::instance()->upnpav().error("could not fetch children: " + e.displayText());
            return 0;
        }
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
CtlMediaObject::fetchedAllChildren()
{
    return _fetchedAllChildren;
}


CtlMediaObject*
CtlMediaObject::parent()
{
    return static_cast<CtlMediaObject*>(_parent);
}


std::string
CtlMediaObject::getProperty(const std::string& name)
{
//     std::clog << "MediaObject::getProperty() name: " << name << std::endl;
//     std::clog << "MediaObject::getProperty() number of properties: " << _properties.size() << std::endl;
//     std::clog << "MediaObject::getProperty() value: " << _properties[name] << std::endl;

//     return _properties.getValue<std::string>(name);
    return _properties[name];
}


Icon*
CtlMediaObject::getIcon()
{
    // icon property is a lower resolution thumb nail of the original picture (but should be displayable on a handheld device).
    return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getProperty(AvProperty::ICON));
}


Icon*
CtlMediaObject::getImageRepresentation()
{
    std::string prop = getProperty(AvProperty::CLASS);
    if (AvClass::matchClass(prop, AvClass::ITEM, AvClass::IMAGE_ITEM))
    {
        return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getResource()->getUri());
    }
    else {
        // for any other object type, we currently don't supply any icon.
        return 0;
    }
}


Resource*
CtlMediaObject::getResource(int num)
{
//     std::clog << "ControllerObject::getResource() number: " << num << std::endl;
//     std::clog << "ControllerObject::getResource() with uri: " << _resources[num]->getUri() << std::endl;
    return _resources[num];
}


ui4
CtlMediaObject::childCount()
{
    return _childCount;
}


void
CtlMediaObject::setFetchedAllChildren(bool fetchedAllChildren)
{
    _fetchedAllChildren = fetchedAllChildren;
}


void
CtlMediaObject::setServerController(CtlMediaServerCode* _pServer)
{
    _server = _pServer;
}


MediaItemNotification::MediaItemNotification(CtlMediaObject* pObject) :
_pItem(pObject)
{
}


CtlMediaObject*
MediaItemNotification::getMediaItem() const
{
    return _pItem;
}


} // namespace Av
} // namespace Omm

