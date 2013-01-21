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
#include "UpnpAvCtlServer.h"
#include "UpnpAv.h"
#include "UpnpAvPrivate.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlObject.h"


namespace Omm {
namespace Av {


CtlMediaRenderer::CtlMediaRenderer() :
_pCurrentMediaObject(0),
//_usePlaylistResource(false)
_usePlaylistResource(true),
_pPositionTimer(0),
_positionTimerInterval(1000)
{
}


void
CtlMediaRenderer::addCtlDeviceCode()
{
    // FIXME: don't pass UserInterface but this to each service implementation
    _pCtlMediaRendererCode = new CtlMediaRendererCode(this,
        new CtlRenderingControlImpl(this),
        new CtlConnectionManagerImpl(this),
        new CtlAVTransportImpl(this));
    setCtlDeviceCode(_pCtlMediaRendererCode);
}


void
CtlMediaRenderer::setObject(CtlMediaObject* pObject, CtlMediaObject* pParentObject, ui4 row)
{
    // TODO: select the best resource, not the first one
    AbstractResource* pRes = pObject->getResource();
    AbstractResource* pContainerRes = pParentObject->getResource();
    CtlMediaServer* pServer = pObject->getServer();

    // when setting AVTransportUri, server and renderer are connected via ControlManager service
    Connection* pConnection = new Connection(pServer->getUuid(), getUuid());
    pServer->getConnectionManager()->addConnection(pConnection, pRes->getProtInfo());
    // TODO: protocol info matching and pass chosen protocol to addConnection();
    getConnectionManager()->addConnection(pConnection, "");
    ui4 AvTransportId = pConnection->getAvTransportId();

    if (_usePlaylistResource && pContainerRes
            && !AvClass::matchClass(pObject->getClass(), AvClass::ITEM, AvClass::AUDIO_BROADCAST)
            && !AvClass::matchClass(pObject->getClass(), AvClass::ITEM, AvClass::VIDEO_BROADCAST)) {
        LOG(upnpav, debug, "selected object is child of a container with playlist resource");
        std::string metaData;
        MediaObjectWriter writer;
        writer.write(metaData, pParentObject);
        try {
            _pCtlMediaRendererCode->AVTransport()->SetAVTransportURI(AvTransportId, pContainerRes->getUri(), metaData);
            Variant seekTarget;
            seekTarget.setValue(row);
            _pCtlMediaRendererCode->AVTransport()->Seek(AvTransportId, AvTransportArgument::SEEK_MODE_TRACK_NR, seekTarget.getValue());
        }
        catch (Poco::Exception e) {
//            error(e.message());
            return;
        }
        _pCurrentMediaObject = pObject;
    }
    else if (pRes) {
        std::string metaData;
        MediaObjectWriter writer;
        writer.write(metaData, pObject);
        try {
            _pCtlMediaRendererCode->AVTransport()->SetAVTransportURI(AvTransportId, pRes->getUri(), metaData);
        }
        catch (Poco::Exception e) {
//            error(e.message());
            return;
        }
        _pCurrentMediaObject = pObject;
    }
    else {
        LOG(upnpav, warning, "selected object has no resource");
    }
}


CtlMediaObject*
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
    LOG(upnpav, debug, "position moved to: " + Poco::NumberFormatter::format(position));
    try {
        _pCtlMediaRendererCode->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_ABS_TIME, AvTypeConverter::writeTime(position * 1000000));
//        _pCtlMediaRendererCode->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_ABS_TIME, AvTypeConverter::writeTime(position));
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


void
CtlMediaRenderer::setMute(bool mute)
{
    try {
        _pCtlMediaRendererCode->RenderingControl()->SetMute(0, "Master", mute);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
}


bool
CtlMediaRenderer::getMute()
{
    bool value;
    try {
        _pCtlMediaRendererCode->RenderingControl()->GetMute(0, "Master", value);
    }
    catch (Poco::Exception e){
//        error(e.message());
    }
    return value;
}


ConnectionManager*
CtlMediaRenderer::getConnectionManager()
{
    return static_cast<CtlConnectionManagerImpl*>(_pCtlMediaRendererCode->ConnectionManager());
}


void
CtlMediaRenderer::startPositionTimer(bool start)
{
    if (_pPositionTimer) {
        LOG(upnpav, debug, "stop position timer ...");
        _pPositionTimer->stop();
        delete _pPositionTimer;
        _pPositionTimer = 0;
        LOG(upnpav, debug, "position timer stopped.");
    }
    if (start) {
        LOG(upnpav, debug, "start position timer ...");
        _pPositionTimer = new Poco::Timer(0, _positionTimerInterval);
        Poco::TimerCallback<CtlMediaRenderer> callback(*this, &CtlMediaRenderer::pollPositionInfo);
        _pPositionTimer->start(callback);
    }
}


void
CtlMediaRenderer::pollPositionInfo(Poco::Timer& timer)
{
    LOG(upnpav, debug, "poll position info");
    // TODO: get TransportState and poll position info only, if it is PLAYING, RECORDING or TRANSITIONING
    ui4 Track;
    std::string TrackDuration;
    std::string TrackMetaData;
    std::string TrackURI;
    std::string RelTime;
    std::string AbsTime;
    i4 RelCount;
    i4 AbsCount;
    _pCtlMediaRendererCode->AVTransport()->GetPositionInfo(0, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
//    LOG(upnpav, debug, "TrackDuration: " + TrackDuration + ", TrackMetaData: " + TrackMetaData + ", TrackURI: " + TrackURI + ", RelTime: " + RelTime + ", AbsTime: " + AbsTime + ", RelCount: " + Poco::NumberFormatter::format(RelCount) + ", AbsCount: " + Poco::NumberFormatter::format(AbsCount));

    try {
        r8 trackDuration = AvTypeConverter::readDuration(TrackDuration);
        r8 absTime = AvTypeConverter::readDuration(AbsTime);
        newPosition(trackDuration, absTime);
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "could not parse current track duration: " + TrackDuration + ", " + e.displayText());
    }

//    if (TrackMetaData == "") {
//        newTrack("", "", "");
//    }
//    else {
//        CtlMediaObject object;
//        try {
//            object.readMetaData(TrackMetaData);
////            LOG(upnpav, debug, "new track title: " + object.getTitle());
////            LOG(upnpav, debug, "new track artist: " + object.getProperty(AvProperty::ARTIST));
////            LOG(upnpav, debug, "new track album: " + object.getProperty(AvProperty::ALBUM));
//            newTrack(object.getTitle(), object.getProperty(AvProperty::ARTIST), object.getProperty(AvProperty::ALBUM));
//        }
//        catch (Poco::Exception& e) {
//            newTrack("", "", "");
//            LOG(upnpav, error, "could not read current track meta data: " + e.displayText());
//        }
//    }
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
    LOG(upnpav, debug, "media renderer delegate init");
    Controller* pController = _pDeviceGroup->getController();
//    pController->registerDeviceNotificationHandler(Poco::Observer<MediaRendererGroupDelegate, MediaItemNotification>(*this, &MediaRendererGroupDelegate::mediaItemSelectedHandler));
    pController->registerDeviceNotificationHandler(Poco::Observer<MediaRendererGroupDelegate, MediaObjectSelectedNotification>(*this, &MediaRendererGroupDelegate::mediaItemSelectedHandler));
}


void
MediaRendererGroupDelegate::mediaItemSelectedHandler(MediaObjectSelectedNotification* pMediaItemNotification)
{
//    CtlMediaObject* pItem = pMediaItemNotification->getMediaItem();
    LOG(upnpav, debug, "media renderer delegate got media item notification: " + pMediaItemNotification->_pObject->getTitle());
    CtlMediaRenderer* pRenderer = static_cast<CtlMediaRenderer*>(_pDeviceGroup->getSelectedDevice());
    if (pRenderer) {
        pRenderer->stopPressed();
        pRenderer->setObject(pMediaItemNotification->_pObject, pMediaItemNotification->_pParentObject, pMediaItemNotification->_row);
        pRenderer->playPressed();
    }
}


} // namespace Av
} // namespace Omm

