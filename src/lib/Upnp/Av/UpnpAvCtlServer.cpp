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
#include "UpnpAvCtlServer.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"


namespace Omm {
namespace Av {


void
CtlMediaServer::addCtlDeviceCode()
{
    // FIXME: don't pass UserInterface but this to each service implementation
    _pCtlMediaServerCode = new CtlMediaServerCode(this,
        new CtlContentDirectoryImpl(0),
        new CtlConnectionManagerImpl(0),
        new CtlAVTransportImpl(0));
    setCtlDeviceCode(_pCtlMediaServerCode);
}


CtlMediaObject2*
CtlMediaServer::createMediaObject()
{
    return new CtlMediaObject2;
}


CtlMediaObject2*
CtlMediaServer::getRootObject() const
{
    return _pRoot;
}


void
CtlMediaServer::browseRootObject()
{
    Log::instance()->upnpav().debug("browse root object ...");
    _pRoot = createMediaObject();
    try {
        std::string rootMeta;
        ui4 numberReturned;
        ui4 totalMatches;
        ui4 updateId;
        _pCtlMediaServerCode->ContentDirectory()->Browse("0", "BrowseMetadata", "*", 0, 0, "", rootMeta, numberReturned, totalMatches, updateId);
        MediaObjectReader reader(_pRoot);
        reader.read(rootMeta);
        Log::instance()->upnpav().debug("controller fetched root object with title: " + _pRoot->getTitle() + ", class: " + _pRoot->getProperty(AvProperty::CLASS)->getValue());
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("controller could not fetch root object, setting default replacement object: " + e.displayText());
        _pRoot->setId("0");
        _pRoot->setIsContainer(true);
    }
    _pRoot->setServer(this);
    _pRoot->setServerController(_pCtlMediaServerCode);
//    _pRoot->setFetchedAllChildren(false);
    Log::instance()->upnpav().debug("browse root object finished.");
}


void
CtlMediaServer::selectMediaObject(CtlMediaObject2* pObject)
{
    Log::instance()->upnpav().debug("media server object selected: " + pObject->getTitle());

    if (pObject->isContainer()) {
    }
    else {
        MediaItemNotification2* pNotification = new MediaItemNotification2(pObject);
        getDeviceContainer()->getDeviceManager()->postDeviceNotification(pNotification);
    }
}


std::string
MediaServerGroupDelegate::getDeviceType()
{
    return DeviceType::MEDIA_SERVER_1;
}


std::string
MediaServerGroupDelegate::shortName()
{
    return "Media";
}


} // namespace Av
} // namespace Omm

