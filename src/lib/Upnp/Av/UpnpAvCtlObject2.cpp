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
#include "UpnpAvCtlObject2.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlServer.h"


namespace Omm {
namespace Av {


CtlMediaObject2::CtlMediaObject2() :
_pServer(0),
_pServerCode(0)
{
}


AbstractMediaObject*
CtlMediaObject2::createChildObject()
{
    CtlMediaObject2* pChildObject = _pServer->createMediaObject();
    pChildObject->_pServer = _pServer;
    pChildObject->_pServerCode = _pServerCode;
    return pChildObject;
}


int
CtlMediaObject2::fetchChildren(ui4 count, ui4 offset)
{
    std::string objectId = getId();
    if (offset == -1) {
        offset = getChildCount();
    }
    Log::instance()->upnpav().debug("controller media object fetch children of object with id: " + objectId
                                    + ", number of requested children: " + Poco::NumberFormatter::format(count)
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
            Log::instance()->upnpav().error("could not fetch children: " + e.displayText());
            return 0;
        }
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        setTotalChildCount(totalMatches);
        MediaObjectReader reader(this);
        reader.readChildren(result);
    }
    else {
        Log::instance()->upnpav().error("controller media object fetch children failed");
    }
    return numberReturned;
}


AbstractMediaObject*
CtlMediaObject2::getChildForRow(ui4 row)
{
    return BlockCache::getMediaObjectForRow(row);
}


Icon*
CtlMediaObject2::getIcon()
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
CtlMediaObject2::getImageRepresentation()
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
CtlMediaObject2::getBlock(std::vector<AbstractMediaObject*>& block, ui4 offset, ui4 size)
{
    std::string objectId = getId();
    Log::instance()->upnpav().debug("controller media object get block of children of object with id: " + objectId
                                    + ", number of requested children: " + Poco::NumberFormatter::format(size)
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
            Log::instance()->upnpav().error("could not fetch children: " + e.displayText());
            return;
        }
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        setTotalChildCount(totalMatches);
        MediaObjectReader reader(this);
        reader.readChildren(result, &block);
    }
    else {
        Log::instance()->upnpav().error("controller media object fetch children failed");
    }
}


CtlMediaServer*
CtlMediaObject2::getServer() const
{
    return _pServer;
}


void
CtlMediaObject2::setServer(CtlMediaServer* pServer)
{
    _pServer = pServer;
}


void
CtlMediaObject2::setServerController(CtlMediaServerCode* pServerCode)
{
    _pServerCode = pServerCode;
}


std::string
CtlMediaObject2::getId()
{
    return _id;
}


void
CtlMediaObject2::setId(const std::string& id)
{
    _id = id;
}


void
CtlMediaObject2::setSearch(const std::string& searchText)
{
//    _searchText = "artist like \"%" + searchText + "%\" OR album like \"%" + searchText + "%\" OR title like \"%" + searchText + "%\"";
    _searchText = "artist like \"%" + searchText + "%\" OR title like \"%" + searchText + "%\"";
//    _searchText = "upnp:artist contains \"" + searchText + "\" or upnp:album contains \"" + searchText + "\" or dc:title contains \"" + searchText + "\"";
//    _searchText = "upnp:artist contains \"" + searchText + "\" or dc:title contains \"" + searchText + "\"";
//    _searchText = "dc:title contains \"" + searchText + "\"";
}


MediaItemNotification2::MediaItemNotification2(CtlMediaObject2* pObject) :
_pItem(pObject)
{
}


CtlMediaObject2*
MediaItemNotification2::getMediaItem() const
{
    return _pItem;
}


} // namespace Av
} // namespace Omm

