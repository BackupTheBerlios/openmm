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

#include "UpnpAvController.h"
#include "UpnpAvControllerPrivate.h"
#include "UpnpPrivate.h"
#include "UpnpAv.h"
#include "UpnpAvCtlImpl.h"
#include "UpnpAvCtlDevices.h"
#include "UpnpAvCtlObject.h"



namespace Omm {
namespace Av {


//////////////////////// deprecated ///////////////////////////



AvServerView::AvServerView(CtlMediaServerCode* pCtlMediaServer) :
_pCtlMediaServer(pCtlMediaServer)
{
}


void
AvServerView::browseRootObject()
{
    _pRoot = new CtlMediaObject;
    try {
        std::string rootMeta;
        ui4 numberReturned;
        ui4 totalMatches;
        ui4 updateId;
        _pCtlMediaServer->ContentDirectory()->Browse("0", "BrowseMetadata", "*", 0, 0, "", rootMeta, numberReturned, totalMatches, updateId);
        _pRoot->readMetaData(rootMeta);
        Log::instance()->upnpav().debug("controller fetched root object with title: " + _pRoot->getTitle() + ", class: " + _pRoot->getProperty(AvProperty::CLASS));
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("controller could not fetch root object, setting default replacement object: " + e.displayText());
        _pRoot->setObjectId("0");
        _pRoot->setIsContainer(true);
    }
    _pRoot->setTitle(_pCtlMediaServer->getDevice()->getFriendlyName());
    _pRoot->setServerController(_pCtlMediaServer);
    _pRoot->setFetchedAllChildren(false);
}


AvRendererView::AvRendererView(CtlMediaRendererCode* pCtlMediaRenderer) :
_pCtlMediaRenderer(pCtlMediaRenderer)
{
}

const std::string
AvRendererView::getName()
{
    return _pCtlMediaRenderer->getDevice()->getFriendlyName();
}


const std::string
AvRendererView::getUuid()
{
    return _pCtlMediaRenderer->getDevice()->getUuid();
}


void
AvController::setUserInterface(AvUserInterface* pUserInterface)
{
    Controller::setUserInterface(pUserInterface);
    pUserInterface->_pAvController = this;
}


int
AvController::rendererCount()
{
    return _renderers.size();
}


AvRendererView*
AvController::rendererView(int numRenderer)
{
    return &_renderers.get(numRenderer);
}


int
AvController::serverCount()
{
    return _servers.size();
}


int
AvController::rendererIndex(AvRendererView* pRendererView)
{
    if (pRendererView) {
        return _renderers.position(pRendererView->getUuid());
    }
    else {
        return -1;
    }
}


CtlMediaObject*
AvController::serverRootObject(int numServer)
{
    return _servers.get(numServer).root();
}


Device*
AvController::serverDevice(int numServer)
{
    return _servers.get(numServer).controller()->getDevice();
}


void
AvController::addDeviceContainer(DeviceContainer* pDeviceContainer)
{
    AvUserInterface* pUserInterface = static_cast<AvUserInterface*>(_pUserInterface);

    for (DeviceContainer::DeviceIterator it = pDeviceContainer->beginDevice(); it != pDeviceContainer->endDevice(); ++it) {
        Device* pDevice = *it;
        Log::instance()->upnpav().information("AV controller add device, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());

        if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
            Log::instance()->upnpav().debug("AV controller add media renderer");
            CtlMediaRendererCode* pRendererImpl = new CtlMediaRendererCode(
                pDevice,
                new CtlRenderingControlImpl(pUserInterface),
                new CtlConnectionManagerImpl(pUserInterface),
                new CtlAVTransportImpl(pUserInterface));
            pDevice->setCtlDeviceCode(pRendererImpl);
            AvRendererView* pRendererView = new AvRendererView(pRendererImpl);
            pUserInterface->beginAddRenderer(_renderers.size());
            _renderers.append(pDevice->getUuid(), pRendererView);
            pUserInterface->endAddRenderer(_renderers.size() - 1);
        }
        else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
            Log::instance()->upnpav().debug("AV controller add media server");
            CtlMediaServerCode* pServerImpl = new CtlMediaServerCode(
                pDevice,
                new CtlContentDirectoryImpl(pUserInterface),
                new CtlConnectionManagerImpl(pUserInterface),
                new CtlAVTransportImpl(pUserInterface));
            pDevice->setCtlDeviceCode(pServerImpl);
            AvServerView* pServer = new AvServerView(pServerImpl);
            pServer->browseRootObject();
            pUserInterface->beginAddServer(_servers.size());
            _servers.append(pDevice->getUuid(), pServer);
            pUserInterface->endAddServer(_servers.size() - 1);
        }

        // if device container contains a MediaRenderer or a MediaServer, add device container
        if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1" ||
            pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
            Controller::addDeviceContainer(pDeviceContainer);
            pDevice->initControllerEventing();
        }

        Log::instance()->upnpav().information("AV controller add device finished, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
    }
}


void
AvController::removeDeviceContainer(DeviceContainer* pDeviceContainer)
{
    AvUserInterface* pUserInterface = static_cast<AvUserInterface*>(_pUserInterface);

    for (DeviceContainer::DeviceIterator it = pDeviceContainer->beginDevice(); it != pDeviceContainer->endDevice(); ++it) {
        Device* pDevice = *it;
        Log::instance()->upnpav().information("av controller removed device, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());

        if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1" && _renderers.contains(pDevice->getUuid())) {
            // TODO: delete renderer controller
            int position = _renderers.position(pDevice->getUuid());
            pUserInterface->beginRemoveRenderer(position);
            _renderers.remove(pDevice->getUuid());
            pUserInterface->endRemoveRenderer(position);
        }
        else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1" && _servers.contains(pDevice->getUuid())) {
            // TODO: delete server controller
            int position = _servers.position(pDevice->getUuid());
            pUserInterface->beginRemoveServer(position);
            _servers.remove(pDevice->getUuid());
            pUserInterface->endRemoveServer(position);
        }

        Controller::removeDeviceContainer(pDeviceContainer);

        Log::instance()->upnpav().information("av controller removed device finished, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
    }
}


AvUserInterface::AvUserInterface() :
_pAvController(0),
_pSelectedRendererView(0),
_pSelectedRenderer(0),
_pSelectedObject(0)
{
    _positionInfoTimer.setPeriodicInterval(1000);
    Poco::TimerCallback<AvUserInterface> callback(*this, &AvUserInterface::pollPositionInfo);
    // NOTE: position info polling disabled for now. Reason: engine throws exceptions
    // when stream is not seekable. We should check if stream is seekable somewhere
    // and avoid reading the position (in Engine?)
//    _positionInfoTimer.start(callback);
}


AvUserInterface::~AvUserInterface()
{
    _positionInfoTimer.stop();
}


int
AvUserInterface::rendererCount()
{
    return _pAvController->rendererCount();
}


AvRendererView*
AvUserInterface::rendererView(int numRenderer)
{
    return _pAvController->rendererView(numRenderer);
}


bool
AvUserInterface::isPlaying(AvRendererView* pRenderer)
{
    std::string transportState;
    std::string transportStatus;
    std::string speed;
    pRenderer->_pCtlMediaRenderer->AVTransport()->GetTransportInfo(0, transportState, transportStatus, speed);
    if (transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
        return true;
    }
    else {
        return false;
    }
}


int
AvUserInterface::serverCount()
{
    return _pAvController->serverCount();
}


CtlMediaObject*
AvUserInterface::serverRootObject(int numServer)
{
    return _pAvController->serverRootObject(numServer);
}


const std::string
AvUserInterface::serverUuid(int numServer)
{
    return _pAvController->serverDevice(numServer)->getUuid();
}


void
AvUserInterface::rendererSelected(AvRendererView* pRenderer)
{
    _pSelectedRendererView = pRenderer;
    _pSelectedRenderer = pRenderer->_pCtlMediaRenderer;
    std::string sourceInfo;
    std::string sinkInfo;
    _pSelectedRenderer->ConnectionManager()->GetProtocolInfo(sourceInfo, sinkInfo);
    ui4 instanceId = 0;
    std::string channel = AvChannel::MASTER;
    ui2 volume;
    _pSelectedRenderer->RenderingControl()->GetVolume(instanceId, channel, volume);
    newVolume(volume);
//    std::string volString = _pSelectedRenderer->getDevice()->getService(ServiceType::RC_1)->getStateVar<std::string>("Volume");
//    if (volString != "") {
//        newVolume(Poco::NumberParser::parse(volString));
//    }
}



void
AvUserInterface::mediaObjectSelected(CtlMediaObject* pObject)
{
    _pSelectedObject = pObject;
}


int
AvUserInterface::selectedRendererIndex()
{
    if (_pSelectedRenderer) {
        return _pAvController->rendererIndex(_pSelectedRendererView);
    }
    else {
        return -1;
    }
}


std::string
AvUserInterface::selectedRendererUuid()
{
    if (_pSelectedRenderer) {
        return _pSelectedRenderer->getDevice()->getUuid();
    }
    else {
        return "";
    }
}


void
AvUserInterface::playPressed()
{
    if (_pSelectedRenderer == 0 || _pSelectedObject == 0) {
        return;
    }
    Resource* pRes = _pSelectedObject->getResource();
    if (pRes) {
        std::string metaData;
        MediaObjectWriter writer(_pSelectedObject);
        writer.write(metaData);
//         _pSelectedObject->writeMetaData(metaData);
        try {
            _pSelectedRenderer->AVTransport()->SetAVTransportURI(0, pRes->getUri(), metaData);
            _pSelectedRenderer->AVTransport()->Play(0, "1");
        }
        catch (Poco::Exception e) {
            error(e.message());
            return;
        }
        Log::instance()->upnpav().debug("playing " + _pSelectedObject->getTitle());
    }
}


void
AvUserInterface::stopPressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Stop(0);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
    newPosition(0, 0);
}


void
AvUserInterface::pausePressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Pause(0);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
}


void
AvUserInterface::positionMoved(int position)
{
    Log::instance()->upnpav().debug("position moved to: " + Poco::NumberFormatter::format(position));
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_ABS_TIME, AvTypeConverter::writeTime(position * 1000000));
    }
    catch (Poco::Exception& e){
        error(e.message());
    }
}


void
AvUserInterface::volumeChanged(int value)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->RenderingControl()->SetVolume(0, "Master", value);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
}


void
AvUserInterface::pollPositionInfo(Poco::Timer& timer)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
//    Log::instance()->upnpav().debug("poll position info ...");
    // TODO: get TransportState and poll position info only, if it is PLAYING, RECORDING or TRANSITIONING
//     _pSelectedRenderer->AVTransport()->
    ui4 Track;
    std::string TrackDuration;
    std::string TrackMetaData;
    std::string TrackURI;
    std::string RelTime;
    std::string AbsTime;
    i4 RelCount;
    i4 AbsCount;
    _pSelectedRenderer->AVTransport()->GetPositionInfo(0, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
//    Log::instance()->upnpav().debug("TrackDuration: " + TrackDuration + ", TrackMetaData: " + TrackMetaData + ", TrackURI: " + TrackURI + ", RelTime: " + RelTime + ", AbsTime: " + AbsTime + ", RelCount: " + Poco::NumberFormatter::format(RelCount) + ", AbsCount: " + Poco::NumberFormatter::format(AbsCount));
    
    try {
        r8 trackDuration = AvTypeConverter::readDuration(TrackDuration);
        r8 absTime = AvTypeConverter::readDuration(AbsTime);
        newPosition(trackDuration, absTime);
    }
    catch (Poco::Exception& e) {
        //Log::instance()->upnpav().warning("could not read current track position: " + e.displayText());
    }
    
    if (TrackMetaData == "") {
        newTrack("", "", "");
    }
    else {
        CtlMediaObject object;
        try {
            object.readMetaData(TrackMetaData);
//            Log::instance()->upnpav().debug("new track title: " + object.getTitle());
//            Log::instance()->upnpav().debug("new track artist: " + object.getProperty(AvProperty::ARTIST));
//            Log::instance()->upnpav().debug("new track album: " + object.getProperty(AvProperty::ALBUM));
            newTrack(object.getTitle(), object.getProperty(AvProperty::ARTIST), object.getProperty(AvProperty::ALBUM));
        }
        catch (Poco::Exception& e) {
            newTrack("", "", "");
            Log::instance()->upnpav().error("could not read current track meta data: " + e.displayText());
        }
    }
    
    // FIXME: don't poll volume, use eventing through LastChange state variable.
    ui2 vol;
    _pSelectedRenderer->RenderingControl()->GetVolume(0, "Master", vol);
//    Log::instance()->upnpav().debug("volume of renderer is: " + Poco::NumberFormatter::format(vol));
    newVolume(vol);
}

} // namespace Av
} // namespace Omm

