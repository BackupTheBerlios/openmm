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
#include "UpnpAvCtlRenderer.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlObject.h"
#include "UpnpAvCtlObject2.h"


namespace Omm {
namespace Av {


CtlMediaRenderer::CtlMediaRenderer() :
_pCurrentMediaObject(0),
//_usePlaylistResource(false)
_usePlaylistResource(true)
{
}


void
CtlMediaRenderer::addCtlDeviceCode()
{
    // FIXME: don't pass UserInterface but this to each service implementation
    _pCtlMediaRendererCode = new CtlMediaRendererCode(this,
        new CtlRenderingControlImpl(0, this),
        new CtlConnectionManagerImpl(0),
        new CtlAVTransportImpl(0, this));
    setCtlDeviceCode(_pCtlMediaRendererCode);
}


void
CtlMediaRenderer::setObject(CtlMediaObject* pObject)
{
    Resource* pRes = pObject->getResource();
    if (pRes) {
        std::string metaData;
        MediaObjectWriter writer(pObject);
        writer.write(metaData);
        try {
            _pCtlMediaRendererCode->AVTransport()->SetAVTransportURI(0, pRes->getUri(), metaData);
        }
        catch (Poco::Exception e) {
//            error(e.message());
            return;
        }
    }
}


void
CtlMediaRenderer::setObject2(CtlMediaObject2* pObject, CtlMediaObject2* pParentObject, ui4 row)
{
    // TODO: select the best resource, not the first one
    AbstractResource* pRes = pObject->getResource();
    AbstractResource* pContainerRes = pParentObject->getResource();
    if (_usePlaylistResource && pContainerRes) {
        Log::instance()->upnpav().debug("selected object is child of a container with playlist resource");
        std::string metaData;
        MediaObjectWriter2 writer;
        writer.write(metaData, pParentObject);
        try {
            _pCtlMediaRendererCode->AVTransport()->SetAVTransportURI(0, pContainerRes->getUri(), metaData);
            Variant seekTarget;
            seekTarget.setValue(row);
            _pCtlMediaRendererCode->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_TRACK_NR, seekTarget.getValue());
        }
        catch (Poco::Exception e) {
//            error(e.message());
            return;
        }
        _pCurrentMediaObject = pObject;
    }
    else if (pRes) {
        std::string metaData;
        MediaObjectWriter2 writer;
        writer.write(metaData, pObject);
        try {
            _pCtlMediaRendererCode->AVTransport()->SetAVTransportURI(0, pRes->getUri(), metaData);
        }
        catch (Poco::Exception e) {
//            error(e.message());
            return;
        }
        _pCurrentMediaObject = pObject;
    }
    else {
        Log::instance()->upnpav().warning("selected object has no resource");
    }
}


CtlMediaObject2*
CtlMediaRenderer::getObject()
{
    return _pCurrentMediaObject;
}


void
CtlMediaRenderer::playPressed()
{
    try {
        _pCtlMediaRendererCode->AVTransport()->Play(0, "1");
    }
    catch (Poco::Exception e) {
//        error(e.message());
        return;
    }
}


void
CtlMediaRenderer::stopPressed()
{
    try {
        _pCtlMediaRendererCode->AVTransport()->Stop(0);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
//    newPosition(0, 0);
}


void
CtlMediaRenderer::pausePressed()
{
    try {
        _pCtlMediaRendererCode->AVTransport()->Pause(0);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
}


void
CtlMediaRenderer::forwardPressed()
{
    try {
        _pCtlMediaRendererCode->AVTransport()->Next(0);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
}


void
CtlMediaRenderer::backPressed()
{
    try {
        _pCtlMediaRendererCode->AVTransport()->Previous(0);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
}


void
CtlMediaRenderer::positionMoved(int position)
{
    Log::instance()->upnpav().debug("position moved to: " + Poco::NumberFormatter::format(position));
    try {
        _pCtlMediaRendererCode->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_ABS_TIME, AvTypeConverter::writeTime(position * 1000000));
    }
    catch (Poco::Exception& e){
//        error(e.message());
    }
}


void
CtlMediaRenderer::volumeChanged(int value)
{
    try {
        _pCtlMediaRendererCode->RenderingControl()->SetVolume(0, "Master", value);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
}


ui2
CtlMediaRenderer::getVolume()
{
    ui2 value;
    try {
        _pCtlMediaRendererCode->RenderingControl()->GetVolume(0, "Master", value);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
    return value;
}


std::string
MediaRendererGroupDelegate::getDeviceType()
{
    return DeviceType::MEDIA_RENDERER_1;
}


std::string
MediaRendererGroupDelegate::shortName()
{
    return "Player";
}


void
MediaRendererGroupDelegate::init()
{
    Log::instance()->upnpav().debug("media renderer delegate init");
    Controller* pController = _pDeviceGroup->getController();
    pController->registerDeviceNotificationHandler(Poco::Observer<MediaRendererGroupDelegate, MediaItemNotification>(*this, &MediaRendererGroupDelegate::mediaItemSelectedHandler));
    pController->registerDeviceNotificationHandler(Poco::Observer<MediaRendererGroupDelegate, MediaObjectSelectedNotification>(*this, &MediaRendererGroupDelegate::mediaItemSelectedHandler2));
}


void
MediaRendererGroupDelegate::mediaItemSelectedHandler(MediaItemNotification* pMediaItemNotification)
{
    CtlMediaObject* pItem = pMediaItemNotification->getMediaItem();
    Log::instance()->upnpav().debug("media renderer delegate got media item notification: " + pItem->getTitle());
    CtlMediaRenderer* pRenderer = static_cast<CtlMediaRenderer*>(_pDeviceGroup->getSelectedDevice());
    if (pRenderer) {
        pRenderer->setObject(pItem);
        pRenderer->playPressed();
    }
}


void
MediaRendererGroupDelegate::mediaItemSelectedHandler2(MediaObjectSelectedNotification* pMediaItemNotification)
{
//    CtlMediaObject2* pItem = pMediaItemNotification->getMediaItem();
    Log::instance()->upnpav().debug("media renderer delegate got media item notification: " + pMediaItemNotification->_pObject->getTitle());
    CtlMediaRenderer* pRenderer = static_cast<CtlMediaRenderer*>(_pDeviceGroup->getSelectedDevice());
    if (pRenderer) {
        pRenderer->setObject2(pMediaItemNotification->_pObject, pMediaItemNotification->_pParentObject, pMediaItemNotification->_row);
        pRenderer->playPressed();
    }
}


} // namespace Av
} // namespace Omm

