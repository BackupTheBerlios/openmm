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
    _pCtlMediaServerCode = new CtlMediaServerCode(this,
        new CtlContentDirectoryImpl(this),
        new CtlConnectionManagerImpl(this),
        new CtlAVTransportImpl(0));
    setCtlDeviceCode(_pCtlMediaServerCode);
}


CtlMediaObject*
CtlMediaServer::createMediaObject()
{
    return new CtlMediaObject;
}


CtlMediaObject*
CtlMediaServer::getRootObject() const
{
    return _pRoot;
}


void
CtlMediaServer::browseRootObject(bool useBlockCache)
{
    LOG(upnpav, debug, "browse root object ...");
    _pRoot = createMediaObject();
    try {
        std::string rootMeta;
        ui4 numberReturned;
        ui4 totalMatches;
        ui4 updateId;
        _pCtlMediaServerCode->ContentDirectory()->Browse("0", "BrowseMetadata", "*", 0, 0, "", rootMeta, numberReturned, totalMatches, updateId);
        MediaObjectReader reader;
        reader.read(_pRoot, rootMeta);
        LOG(upnpav, debug, "controller fetched root object with title: " + _pRoot->getTitle() + ", class: " + _pRoot->getClass());
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "controller could not fetch root object, setting default replacement object: " + e.displayText());
        _pRoot->setId("0");
        _pRoot->setIsContainer(true);
    }
    _pRoot->setServer(this);
    _pRoot->setServerController(_pCtlMediaServerCode);
    if (_pRoot->isContainer()) {
        // don't rely on childCount attribute being present in root container, so we have to fetch some children to get total child count.
        LOG(upnpav, debug, "controller root object is container, fetching children ...");
        if (!useBlockCache) {
            _pRoot->fetchChildren();
        }
        _pRoot->getChildForRow(0, useBlockCache);
    }
    LOG(upnpav, debug, "browse root object finished.");
}


void
CtlMediaServer::setSort(const std::string& sortText)
{
    LOG(upnpav, debug, "controller media server set sort text of server: " + getFriendlyName() + " to: " + sortText);
    _sortText = sortText;
}


const std::string&
CtlMediaServer::getSort()
{
    return _sortText;
}


ConnectionManager*
CtlMediaServer::getConnectionManager()
{
    return static_cast<CtlConnectionManagerImpl*>(_pCtlMediaServerCode->ConnectionManager());
}


CtlMediaObject*
CtlMediaServer::getMediaObjectFromResource(const std::string& resource)
{
    std::string result;
    ui4 numberReturned;
    ui4 totalMatches;
    ui4 updateId;
    _pCtlMediaServerCode->ContentDirectory()->Search("0", "res = \"" + resource + "\"", "*", 0, 1, "", result, numberReturned, totalMatches, updateId);
    if (numberReturned != 1) {
        LOG(upnpav, error, "get media object from resource failed: 0 or more than one object found.");
        return 0;
    }

    CtlMediaObject* pObject = createMediaObject();
    MediaObjectReader reader;
    try {
        reader.read(pObject, result);
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "get media object from resource failed: " + e.displayText());
        return 0;
    }
    return pObject;
}


void
CtlMediaServer::selectMediaObject(CtlMediaObject* pObject, CtlMediaObject* pParentObject, ui4 row)
{
    LOG(upnpav, debug, "media server object selected: " + pObject->getTitle());

    if (pObject->isContainer()) {
    }
    else {
        MediaObjectSelectedNotification* pNotification = new MediaObjectSelectedNotification(pObject, pParentObject, row);
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

