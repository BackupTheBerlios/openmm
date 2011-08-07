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

#include "UpnpAvCtlRenderer.h"
#include "UpnpPrivate.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlObject.h"


namespace Omm {
namespace Av {


void
CtlMediaRenderer::addCtlDeviceCode()
{
    // FIXME: don't pass UserInterface but this to each service implementation
    _pCtlMediaRendererCode = new CtlMediaRendererCode(this,
        new CtlRenderingControlImpl(0, this),
        new CtlConnectionManagerImpl(0),
        new CtlAVTransportImpl(0));
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


} // namespace Av
} // namespace Omm

