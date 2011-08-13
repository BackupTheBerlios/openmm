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

#include "UpnpAvCtlObject2.h"
#include "UpnpPrivate.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"


namespace Omm {
namespace Av {


CtlMediaObject2::CtlMediaObject2() :
_pServer(0),
_pServerCode(0),
_pListWidget(0)
{
}


int
CtlMediaObject2::fetchChildren(ui4 count)
{
    std::string objectId = getId();
    ui4 lastFetchedChild = getChildCount();
    Log::instance()->upnpav().debug("controller media object fetch children of object with id: " + objectId
                                    + ", number of requested children: " + Poco::NumberFormatter::format(count)
                                    + ", child offset: " + Poco::NumberFormatter::format(lastFetchedChild));

    Omm::ui4 numberReturned;
    if (_pServerCode && !fetchedAllChildren()) {
        std::string result;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        try {
            _pServerCode->ContentDirectory()->Browse(objectId, "BrowseDirectChildren", "*", lastFetchedChild, count, "", result, numberReturned, totalMatches, updateId);
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
    return numberReturned;
}


Icon*
CtlMediaObject2::getIcon()
{
    // icon property is a lower resolution thumb nail of the original picture (but should be displayable on a handheld device).
    // TODO: adapt icon to new media object implementataion
    return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getProperty(AvProperty::ICON)->getValue());
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
CtlMediaObject2::setListWidget(Omm::Util::ListWidget* pWidget)
{
    Log::instance()->upnpav().debug("controller media object set list widget");

    _pListWidget = pWidget;
}


Omm::Util::ListWidget*
CtlMediaObject2::getListWidget()
{
    return _pListWidget;
}

//
//void
//CtlMediaObject2::selectItem(int row)
//{
//    Log::instance()->upnpav().debug("controller media object select item in row: " + Poco::NumberFormatter::format(row));
//
//}
//
//
//bool
//CtlMediaObject2::canFetchMore()
//{
//    return false;
//}
//
//
//void
//CtlMediaObject2::fetchMore(bool forward)
//{
//
//}
//
//
//int
//CtlMediaObject2::lastFetched(bool forward)
//{
//
//    return (forward ? totalItemCount() : 0);
//}


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

