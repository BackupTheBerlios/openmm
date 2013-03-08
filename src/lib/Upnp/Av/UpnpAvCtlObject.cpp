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

#include "../UpnpPrivate.h"
#include "UpnpAvCtlObject.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlServer.h"


namespace Omm {
namespace Av {


CtlMediaObject::CtlMediaObject() :
_pServer(0),
_pServerCode(0),
_pParent(0)
{
}


CtlMediaObject::CtlMediaObject(const CtlMediaObject& mediaObject) :
MemoryMediaObject(mediaObject),
_id(mediaObject._id),
_pServer(mediaObject._pServer),
_pServerCode(mediaObject._pServerCode),
_searchText(mediaObject._searchText)
{

}


AbstractMediaObject*
CtlMediaObject::createChildObject()
{
    CtlMediaObject* pChildObject = _pServer->createMediaObject();
    pChildObject->_pServer = _pServer;
    pChildObject->_pServerCode = _pServerCode;
    pChildObject->_pParent = this;
    return pChildObject;
}


int
CtlMediaObject::fetchChildren(ui4 count)
{
    std::string objectId = getId();
    ui4 offset = getFetchedChildCount();
    LOG(upnpav, debug, "controller media object fetch children of object with id: " + objectId\
                                    + ", number of requested children: " + Poco::NumberFormatter::format(count)\
                                    + ", child offset: " + Poco::NumberFormatter::format(offset));

    Omm::ui4 numberReturned = 0;
    if (_pServerCode) {
        std::string result;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        try {
            _pServerCode->ContentDirectory()->Browse(objectId, "BrowseDirectChildren", "*", offset, count, "", result, numberReturned, totalMatches, updateId);
        }
        catch (Poco::Exception& e){
            LOG(upnpav, error, "could not fetch children: " + e.displayText());
            return 0;
        }
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        setChildCount(totalMatches);
        MediaObjectReader reader;
        reader.readChildren(_childVec, result, this);
    }
    else {
        LOG(upnpav, error, "controller media object fetch children failed");
    }
    return numberReturned;
}


bool
CtlMediaObject::fetchedAllChildren()
{
    LOG(upnpav, debug, "CtlMediaObject::fetchedAllChildren() fetched children: " + Poco::NumberFormatter::format(getFetchedChildCount()) + ", total: " + Poco::NumberFormatter::format(getChildCount()));
    return getChildCount() <= getFetchedChildCount();
}


AbstractMediaObject*
CtlMediaObject::getChildForRow(ui4 row, bool useBlockCache)
{
    if (useBlockCache) {
        return BlockCache::getMediaObjectForRow(row);
    }
    else {
        return MemoryMediaObject::getChildForRow(row);
    }
}


AbstractMediaObject*
CtlMediaObject::getParent()
{
    return _pParent;
}


Icon*
CtlMediaObject::getIcon()
{
    // icon property is a lower resolution thumb nail of the original picture (but should be displayable on a handheld device).
    // TODO: adapt icon to new media object implementataion
    AbstractProperty* pProperty = getProperty(AvProperty::ICON);
    if (pProperty) {
        return new Icon(0, 0, 0, Mime::IMAGE_JPEG, pProperty->getValue());
    }
    return 0;
}


Icon*
CtlMediaObject::getImageRepresentation()
{
    std::string prop = getProperty(AvProperty::CLASS)->getValue();
    if (AvClass::matchClass(prop, AvClass::ITEM, AvClass::IMAGE_ITEM))
    {
        return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getResource()->getUri());
    }
    else {
        // for any other object type, we currently don't supply any icon.
        return 0;
    }
}


void
CtlMediaObject::writeResource(const std::string& sourceUri, int index)
{
    ui4 transferId;
    std::string importUri = getResource(index)->getAttributeValue(AvProperty::IMPORT_URI);
    if (importUri != "") {
        LOG(upnpav, debug, "writing resource: " + importUri + ", from uri: " + sourceUri);
        _pServerCode->ContentDirectory()->ImportResource(uri(sourceUri), uri(importUri), transferId);
    }
    else {
        LOG(upnpav, warning, "resource read only, writing failed");
    }
}


void
CtlMediaObject::createChildObject(CtlMediaObject* pObject)
{
    LOG(upnpav, debug, "create child media object: " + pObject->getTitle());
    MediaObjectWriter writer;
    std::string elements;
    writer.write(elements, pObject);
    std::string objectId;
    std::string result;
    _pServerCode->ContentDirectory()->CreateObject(getId(), elements, objectId, result);
}


void
CtlMediaObject::destroyObject(const std::string& objectId)
{
    LOG(upnpav, debug, "distroy child media object with object id: " + objectId);
    _pServerCode->ContentDirectory()->DestroyObject(objectId);
}


void
CtlMediaObject::getBlock(std::vector<AbstractMediaObject*>& block, ui4 offset, ui4 size)
{
    std::string objectId = getId();
    LOG(upnpav, debug, "controller media object get block of children of object with id: " + objectId\
                                    + ", number of requested children: " + Poco::NumberFormatter::format(size)\
                                    + ", child offset: " + Poco::NumberFormatter::format(offset));

    Omm::ui4 numberReturned = 0;
    if (_pServerCode) {
        std::string result;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        try {
            if (_searchText.size()) {
                _pServerCode->ContentDirectory()->Search(objectId, _searchText, "*", offset, size, "", result, numberReturned, totalMatches, updateId);
            }
            else {
                _pServerCode->ContentDirectory()->Browse(objectId, "BrowseDirectChildren", "*", offset, size, "", result, numberReturned, totalMatches, updateId);
            }
        }
        catch (Poco::Exception& e){
            LOG(upnpav, error, "could not fetch children: " + e.displayText());
            return;
        }
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        setChildCount(totalMatches);
        MediaObjectReader reader;
        reader.readChildren(block, result, this);
    }
    else {
        LOG(upnpav, error, "controller media object fetch children failed");
    }
}


CtlMediaServer*
CtlMediaObject::getServer() const
{
    return _pServer;
}


void
CtlMediaObject::setServer(CtlMediaServer* pServer)
{
    _pServer = pServer;
}


void
CtlMediaObject::setServerController(CtlMediaServerCode* pServerCode)
{
    _pServerCode = pServerCode;
}


std::string
CtlMediaObject::getId()
{
    return _id;
}


void
CtlMediaObject::setId(const std::string& id)
{
    _id = id;
}


void
CtlMediaObject::setSearch(const std::string& searchText)
{
    _searchText = searchText;
}


MediaObjectSelectedNotification::MediaObjectSelectedNotification(CtlMediaObject* pObject, CtlMediaObject* pParentObject, ui4 row) :
_pObject(pObject),
_pParentObject(pParentObject),
_row(row)
{
}


//CtlMediaObject*
//MediaObjectSelectedNotification::getMediaItem() const
//{
//    return _pObject;
//}


} // namespace Av
} // namespace Omm

