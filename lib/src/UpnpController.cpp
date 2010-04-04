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

#include "UpnpController.h"
#include "UpnpAvCtrlImpl.h"

using namespace Omm;
using namespace Omm::Av;

ServerController::ServerController(MediaServerController* pServerController) :
_pServerController(pServerController)
{
    _pRoot = new Omm::Av::MediaObject();
    _pRoot->setObjectId("0");
    _pRoot->setTitle(_pServerController->getDevice()->getFriendlyName());
    _pRoot->setServerController(_pServerController);
    _pRoot->setFetchedAllChildren(false);
    // TODO: browse root object "0" here
    // TODO: this should depend on the browse result for root object "0"
    _pRoot->setIsContainer(true);
}


UpnpController::~UpnpController()
{
//     std::clog << "UpnpController::~UpnpController()";
}


// void
// UpnpController::start()
// {
//     init();
// //     JSignal::connectNodes(&_pollPositionInfoTimer.fire, this);
//     
//     // poll for current position every second
// //     _pollPositionInfoTimer.startTimer(1000);
// }



void
UpnpController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information(Poco::format("device added, friendly name: %s ,uuid: %s", pDevice->getFriendlyName(), pDevice->getUuid()));
//     std::clog << "UpnpController::deviceAdded()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    _pUserInterface->beginAddDevice(_devices.position(pDevice->getUuid()));
    _devices.append(pDevice->getUuid(), pDevice);
    _pUserInterface->endAddDevice();
}


void
UpnpController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information(Poco::format("device removed, friendly name: %s ,uuid: %s", pDevice->getFriendlyName(), pDevice->getUuid()));
//     std::clog << "UpnpController::deviceRemoved()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    _pUserInterface->beginRemoveDevice(_devices.position(pDevice->getUuid()));
    _devices.remove(pDevice);
    _pUserInterface->endRemoveDevice();
}


RendererView::RendererView(MediaRendererController* rendererController) :
_pRendererController(rendererController)
{
}


const std::string&
RendererView::getName()
{
    return _pRendererController->getDevice()->getFriendlyName();
}


void
UpnpAvController::setUserInterface(UpnpAvUserInterface* pUserInterface)
{
    _pAvUserInterface = pUserInterface;
    _pAvUserInterface->_pRenderers = &_renderers;
    _pAvUserInterface->_pServers = &_servers;
}


void
UpnpAvController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information(Poco::format("device added, friendly name: %s ,uuid: %s", pDevice->getFriendlyName(), pDevice->getUuid()));
//     std::clog << "UpnpAvController::deviceAdded()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        MediaRendererController* pRenderer = new MediaRendererController(
            pDevice,
            new RenderingControlControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl);
        _pAvUserInterface->beginAddRenderer(_renderers.size());
//         std::clog << "UpnpAvController::deviceAdded() number of renderers: " << _renderers.size() << std::endl;
        _renderers.append(pDevice->getUuid(), new RendererView(pRenderer));
//         std::clog << "UpnpAvController::deviceAdded() number of renderers: " << _renderers.size() << std::endl;
        _pAvUserInterface->endAddRenderer();
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        ServerController* pServer = new ServerController(new Omm::Av::MediaServerController(
            pDevice,
            new ContentDirectoryControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl));
        _pAvUserInterface->beginAddServer(_servers.size());
//         std::clog << "UpnpAvController::deviceAdded() number of servers: " << _servers.size() << std::endl;
        _servers.append(pDevice->getUuid(), pServer);
//         std::clog << "UpnpAvController::deviceAdded() number of servers: " << _servers.size() << std::endl;
        _pAvUserInterface->endAddServer();
    }
}


void
UpnpAvController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information(Poco::format("device removed, friendly name: %s ,uuid: %s", pDevice->getFriendlyName(), pDevice->getUuid()));
//     std::clog << "UpnpAvController::deviceRemoved()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        // TODO: delete renderer controller
        _pAvUserInterface->beginRemoveRenderer(_renderers.position(pDevice->getUuid()));
//         std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << _renderers.size() << std::endl;
        _renderers.remove(pDevice->getUuid());
//         std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << _renderers.size() << std::endl;
        _pAvUserInterface->endRemoveRenderer();
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        // TODO: delete server controller
        _pAvUserInterface->beginRemoveServer(_servers.position(pDevice->getUuid()));
//         std::clog << "UpnpAvController::deviceRemoved() number of servers: " << _servers.size() << std::endl;
        _servers.remove(pDevice->getUuid());
//         std::clog << "UpnpAvController::deviceRemoved() number of servers: " << _servers.size() << std::endl;
        _pAvUserInterface->endRemoveServer();
    }
}


UpnpAvUserInterface::UpnpAvUserInterface() :
_pRenderers(NULL),
_pServers(NULL),
_pSelectedRenderer(NULL),
_pSelectedObject(NULL)
{
}


int
UpnpAvUserInterface::rendererCount()
{
    return _pRenderers->size();
}


RendererView*
UpnpAvUserInterface::rendererView(int numRenderer)
{
    return &_pRenderers->get(numRenderer);
}


int
UpnpAvUserInterface::serverCount()
{
    return _pServers->size();
}


MediaObject*
UpnpAvUserInterface::serverRootObject(int numServer)
{
    return _pServers->get(numServer).root();
}


void
UpnpAvUserInterface::rendererSelected(RendererView* pRenderer)
{
    _pSelectedRenderer = pRenderer->_pRendererController;
    std::string sourceInfo;
    std::string sinkInfo;
    _pSelectedRenderer->ConnectionManager()->GetProtocolInfo(sourceInfo, sinkInfo);
}


void
UpnpAvUserInterface::mediaObjectSelected(Omm::Av::MediaObject* pObject)
{
    _pSelectedObject = pObject;
}


void
UpnpAvUserInterface::playPressed()
{
    if (_pSelectedRenderer == 0 || _pSelectedObject == 0) {
        return;
    }
    Resource* pRes = _pSelectedObject->getResource();
    if (pRes) {
        std::string metaData;
        _pSelectedObject->writeMetaData(metaData);
        _pSelectedRenderer->AVTransport()->SetAVTransportURI(0, pRes->_uri, metaData);
        _pSelectedRenderer->AVTransport()->Play(0, "1");
    }
}


void
UpnpAvUserInterface::stopPressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->AVTransport()->Stop(0);
}


void
UpnpAvUserInterface::pausePressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->AVTransport()->Pause(0);
}


void
UpnpAvUserInterface::positionMoved(int position)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
        // TODO: need to support UPnP time format in Variant
//     _pSelectedRenderer->AVTransport()->Seek(0, "ABS_TIME", );
}


void
UpnpAvUserInterface::volumeChanged(int value)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->RenderingControl()->SetVolume(0, "Master", value);
}


// 
// void
// UpnpController::sliderMoved(int position)
// {
//     std::clog << "UpnpController::sliderMoved() to:" << position;
//     if (_curMediaRenderer.IsNull()) {
//         return;
//     }
//     
//     NPT_String timestamp;
//     PLT_Didl::FormatTimeStamp(timestamp, position);
//     _mediaController->Seek(_curMediaRenderer, 0, "ABS_TIME", timestamp, NULL);
// }
// 
// 
// 
// 
// void
// UpnpController::OnGetPositionInfoResult(
//                                       NPT_Result               /* res */,
//                                       PLT_DeviceDataReference& /*device*/,
//                                       PLT_PositionInfo*        info,
//                                       void*                    /* userdata */)
// {
//     std::clog << "UpnpController::OnGetPositionInfoResult() duration:" << info->track_duration << "position:" << info->abs_time;
//     emit setSlider(info->track_duration, info->abs_time);
// }


// void
// UpnpController::onSignalReceived()
// {
// //     std::clog << "UpnpController::pollPositionInfo()";
//     if (!_curMediaRenderer.IsNull()) {
//         _mediaController->GetPositionInfo(_curMediaRenderer, 0, NULL);
//     }
// }



