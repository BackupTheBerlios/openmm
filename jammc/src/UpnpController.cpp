/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include <QtDebug>

#include "UpnpController.h"

ServerController::ServerController(MediaServerController* pServerController) :
m_pServerController(pServerController)
{
    m_pRoot = new Jamm::Av::MediaObject();
    m_pRoot->m_objectId = "0";
    m_pRoot->m_properties.append("dc:title",
                                 new Variant(m_pServerController->getDevice()->getFriendlyName()));
    m_pRoot->m_parent = NULL;
    m_pRoot->m_server = m_pServerController;
    m_pRoot->m_fetchedAllChildren = false;
    // TODO: browse root object "0" here
    // TODO: this should depend on the browse result for root object "0"
    m_pRoot->m_isContainer = true;
}


UpnpController::~UpnpController()
{
    std::clog << "UpnpController::~UpnpController()";
}


void
UpnpController::start()
{
    init();
//     JSignal::connectNodes(&m_pollPositionInfoTimer.fire, this);
    
    // poll for current position every second
//     m_pollPositionInfoTimer.startTimer(1000);
}



void
UpnpController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    std::clog << "UpnpController::deviceAdded()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
    std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    m_pUserInterface->beginAddDevice(m_devices.position(pDevice->getUuid()));
    m_devices.append(pDevice->getUuid(), pDevice);
    m_pUserInterface->endAddDevice();
}


void
UpnpController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    std::clog << "UpnpController::deviceRemoved()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
    std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    m_pUserInterface->beginRemoveDevice(m_devices.position(pDevice->getUuid()));
    m_devices.remove(pDevice);
    m_pUserInterface->endRemoveDevice();
}


RendererView::RendererView(MediaRendererController* rendererController) :
m_pRendererController(rendererController)
{
}


const std::string&
RendererView::getName()
{
    return m_pRendererController->getDevice()->getFriendlyName();
}


void
UpnpAvController::setUserInterface(UpnpAvUserInterface* pUserInterface)
{
    m_pAvUserInterface = pUserInterface;
    m_pAvUserInterface->m_pRenderers = &m_renderers;
    m_pAvUserInterface->m_pServers = &m_servers;
}


void
UpnpAvController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    std::clog << "UpnpAvController::deviceAdded()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
    std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        MediaRendererController* pRenderer = new MediaRendererController(
            pDevice,
            new RenderingControlControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl);
        m_pAvUserInterface->beginAddRenderer(m_renderers.size());
        std::clog << "UpnpAvController::deviceAdded() number of renderers: " << m_renderers.size() << std::endl;
        m_renderers.append(pDevice->getUuid(), new RendererView(pRenderer));
        std::clog << "UpnpAvController::deviceAdded() number of renderers: " << m_renderers.size() << std::endl;
        m_pAvUserInterface->endAddRenderer();
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        ServerController* pServer = new ServerController(new Jamm::Av::MediaServerController(
            pDevice,
            new ContentDirectoryControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl));
        m_pAvUserInterface->beginAddServer(m_servers.size());
        std::clog << "UpnpAvController::deviceAdded() number of servers: " << m_servers.size() << std::endl;
        m_servers.append(pDevice->getUuid(), pServer);
        std::clog << "UpnpAvController::deviceAdded() number of servers: " << m_servers.size() << std::endl;
        m_pAvUserInterface->endAddServer();
    }
}


void
UpnpAvController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    std::clog << "UpnpAvController::deviceRemoved()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
    std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        // TODO: delete renderer controller
        m_pAvUserInterface->beginRemoveRenderer(m_renderers.position(pDevice->getUuid()));
        std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << m_renderers.size() << std::endl;
        m_renderers.remove(pDevice->getUuid());
        std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << m_renderers.size() << std::endl;
        m_pAvUserInterface->endRemoveRenderer();
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        // TODO: delete server controller
        m_pAvUserInterface->beginRemoveServer(m_servers.position(pDevice->getUuid()));
        std::clog << "UpnpAvController::deviceRemoved() number of servers: " << m_servers.size() << std::endl;
        m_servers.remove(pDevice->getUuid());
        std::clog << "UpnpAvController::deviceRemoved() number of servers: " << m_servers.size() << std::endl;
        m_pAvUserInterface->endRemoveServer();
    }
}


int
UpnpAvUserInterface::rendererCount()
{
    return m_pRenderers->size();
}


RendererView*
UpnpAvUserInterface::rendererView(int numRenderer)
{
    return &m_pRenderers->get(numRenderer);
}


int
UpnpAvUserInterface::serverCount()
{
    return m_pServers->size();
}


MediaObject*
UpnpAvUserInterface::serverRootObject(int numServer)
{
    return m_pServers->get(numServer).root();
}


void
UpnpAvUserInterface::rendererSelected(RendererView* pRenderer)
{
    m_pSelectedRenderer = pRenderer->m_pRendererController;
}


void
UpnpAvUserInterface::mediaObjectSelected(Jamm::Av::MediaObject* pObject)
{
    m_pSelectedObject = pObject;
}


void
UpnpAvUserInterface::playPressed()
{
    if (m_pSelectedRenderer == NULL) {
        return;
    }
    m_pSelectedRenderer->AVTransport()->SetAVTransportURI(0, m_pSelectedObject->getProperty("res"), "");
    m_pSelectedRenderer->AVTransport()->Play(0, "1");
}


void
UpnpAvUserInterface::stopPressed()
{
    if (m_pSelectedRenderer == NULL) {
        return;
    }
    m_pSelectedRenderer->AVTransport()->Stop(0);
}


void
UpnpAvUserInterface::pausePressed()
{
    if (m_pSelectedRenderer == NULL) {
        return;
    }
    m_pSelectedRenderer->AVTransport()->Pause(0);
}


void
UpnpAvUserInterface::positionMoved(int position)
{
    if (m_pSelectedRenderer == NULL) {
        return;
    }
        // TODO: need to support UPnP time format in Variant
//     m_pSelectedRenderer->AVTransport()->Seek(0, "ABS_TIME", );
}


void
UpnpAvUserInterface::volumeChanged(int value)
{
    if (m_pSelectedRenderer == NULL) {
        return;
    }
    m_pSelectedRenderer->RenderingControl()->SetVolume(0, "Master", value);
}


// 
// void
// UpnpController::sliderMoved(int position)
// {
//     std::clog << "UpnpController::sliderMoved() to:" << position;
//     if (m_curMediaRenderer.IsNull()) {
//         return;
//     }
//     
//     NPT_String timestamp;
//     PLT_Didl::FormatTimeStamp(timestamp, position);
//     m_mediaController->Seek(m_curMediaRenderer, 0, "ABS_TIME", timestamp, NULL);
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
//     if (!m_curMediaRenderer.IsNull()) {
//         m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
//     }
// }



