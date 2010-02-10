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

#include <platinum/PltDidl.h>
#include <QtDebug>

#include "UpnpController.h"
// #include "modeltest.h"


UpnpController::~UpnpController()
{
    std::clog << "UpnpController::~UpnpController()";
}


void
UpnpController::initGui()
{
    m_upnpBrowserModel = new UpnpBrowserModel(this);
    m_upnpRendererListModel = new UpnpRendererListModel(this);
    
    m_mainWindow = new ControllerGui();
    m_mainWindow->setBrowserTreeItemModel(m_upnpBrowserModel);
    m_mainWindow->setRendererListItemModel(m_upnpRendererListModel);
    
    qRegisterMetaType<string>("string");
    qRegisterMetaType<QItemSelection>("QItemSelection");  // TODO: why's that needed?
    
    connect(this, SIGNAL(rendererAddedRemoved(string, bool)),
            m_upnpRendererListModel, SLOT(rendererAddedRemoved(string, bool)));
    connect(this, SIGNAL(serverAddedRemoved(UpnpServer*, bool)),
            m_upnpBrowserModel, SLOT(serverAddedRemoved(UpnpServer*, bool)));
    connect(m_mainWindow, SIGNAL(activated(const QModelIndex&)), this, SLOT(modelIndexActivated(const QModelIndex&)));
//     connect(m_mainWindow, SIGNAL(expanded(const QModelIndex&)), this, SLOT(modelIndexExpanded(const QModelIndex&)));
    connect(m_mainWindow->getRendererListSelectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
    
    connect(m_mainWindow, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(m_mainWindow, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(m_mainWindow, SIGNAL(pauseButtonPressed()), this, SLOT(pauseButtonPressed()));
    connect(m_mainWindow, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), m_mainWindow, SLOT(setSlider(int, int)));
    connect(m_mainWindow, SIGNAL(volSliderMoved(int)), this, SLOT(volSliderMoved(int)));
}


void
UpnpController::showMainWindow()
{
    m_mainWindow->show();
}


void
UpnpController::volSliderMoved(int position)
{
    std::clog << "UpnpController::volSliderMoved() to:" << position << std::endl;
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_renderingController->setVolume(m_curMediaRenderer, 0, position);
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
    std::clog << "MyController::deviceAdded()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//         std::clog << "friendly name: " << device->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        m_renderers[pDevice->getUuid()] = new Jamm::Av::MediaRendererController(
            pDevice,
            new RenderingControlControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl);
        emit rendererAddedRemoved(pDevice->getUuid(), true);
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        m_servers[pDevice->getUuid()] = new Jamm::Av::MediaServerController(
            pDevice,
            new ContentDirectoryControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl);
//         emit serverAddedRemoved(s, true);
    }
}

void
UpnpController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    std::clog << "MyController::deviceRemoved()" << std::endl;
    std::clog << "uuid: " << pDevice->getUuid() << std::endl;
    std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//         std::clog << "friendly name: " << device->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        m_renderers.erase(pDevice->getUuid());
        emit rendererAddedRemoved(pDevice->getUuid(), false);
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        m_servers.erase(pDevice->getUuid());
//         emit serverAddedRemoved(s, true);
    }
}



void
UpnpController::rendererSelectionChanged(const QItemSelection& selected,
                                         const QItemSelection& /*deselected*/)
{
    if (selected.count() > 1) {
        return;
    }
    if (selected.empty()) {
        std::clog << "UpnpController::rendererSelectionChanged() nothing selected" << std::endl;
        m_selectedRenderer = NULL;
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    string* pUuid = static_cast<string*>(index.internalPointer());
    m_selectedRenderer = m_renderers[*pUuid];
    
    if (m_selectedRenderer != NULL) {
        std::clog << "UpnpController::rendererSelectionChanged() row:" << index.row() << std::endl;
        std::clog << "UpnpController::rendererSelectionChanged() selected renderer:" << 
            m_selectedRenderer->getDevice()->getUuid() << std::endl;
        
        m_mainWindow->setVolumeSlider(100, 50);
    }
}


void
UpnpController::modelIndexActivated(const QModelIndex& index)
{
    std::clog << "++++++++ Object activated:" << m_upnpBrowserModel->getObject(index)->m_objectId.c_str() << std::endl;
}


void
UpnpController::modelIndexExpanded(const QModelIndex& index)
{
    std::clog << "++++++++ Object expanded:" << m_upnpBrowserModel->getObject(index)->m_objectId.c_str() << std::endl;
}


// void
// UpnpController::OnMRAddedRemoved(PLT_DeviceDataReference& device, int added)
// {
//     NPT_AutoLock lock(m_mediaRenderers);
//     
//     NPT_String uuid = device->GetUUID();
//     string name = (char*) device->GetFriendlyName();
//     std::clog << "UpnpController::OnMRAddedRemoved()" << (added?"added":"removed") << "renderer:" << name.c_str() << (char*) uuid;
// 
//     if (added) {
//         m_mediaRenderers.Put(uuid, device);
//     } else {
//         m_mediaRenderers.Erase(uuid);
//     }
//     emit rendererAddedRemoved(string((char*)uuid), added);
//     
//     PLT_Service* serviceAVRC;
//     NPT_String type;
//     type = "urn:schemas-upnp-org:service:RenderingControl:1";
//     if (NPT_FAILED(device->FindServiceByType(type, serviceAVRC))) {
//         NPT_LOG_FINE_1("Service %s not found", (const char*)type);
//     }
//     m_ctrlPoint->Subscribe(serviceAVRC);
// }


// void
// UpnpController::OnMRStateVariablesChanged(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars)
// {
//     std::clog << "UpnpController::OnMRStateVariablesChanged() on service:" << (char*)service->GetServiceType();
//     for (unsigned int i = 0; i < vars->GetItemCount(); ++i) {
//         std::clog << "UpnpController::OnMRStateVariablesChanged():" 
//             << (char*)(*vars->GetItem(i))->GetName()
//             << (char*)(*vars->GetItem(i))->GetValue()
//             << (char*)(*vars->GetItem(i))->GetDataType();
//     }
//     // TODO: handle change of StateVariables
//     // for example: react on volume change from another controller
//     // how should our volume slider differ between own changes and other controller's changes?
// }


void
UpnpController::OnMSAddedRemoved(PLT_DeviceDataReference& device, int added)
{
    NPT_String uuid = device->GetUUID();
    string name = (char*) device->GetFriendlyName();
    std::clog << "UpnpController::OnMSAddedRemoved()" << (added?"added":"removed") << "server:" << name.c_str() << (char*) uuid;
    
    PLT_Service* serviceAVCD;
    NPT_String type;
    type = "urn:schemas-upnp-org:service:ContentDirectory:1";
    if (NPT_FAILED(device->FindServiceByType(type, serviceAVCD))) {
        NPT_LOG_FINE_1("Service %s not found", (const char*)type);
        return;
    }
    m_ctrlPoint->Subscribe(serviceAVCD);
    
    UpnpServer* s = new UpnpServer();
    s->m_pltDevice = device;
    s->m_pltBrowser = m_mediaBrowser;
//     s->m_pltBrowser = new PLT_MediaBrowser(m_ctrlPoint, s);
    emit serverAddedRemoved(s, added);
}


void
UpnpController::OnMSStateVariablesChanged(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars)
{
    std::clog << "UpnpController::OnMSStateVariablesChanged() on service:" << (char*)service->GetServiceType();
    for (unsigned int i = 0; i < vars->GetItemCount(); ++i) {
        std::clog << "UpnpController::OnMSStateVariablesChanged():" 
            << (char*)(*vars->GetItem(i))->GetName()
            << (char*)(*vars->GetItem(i))->GetValue()
            << (char*)(*vars->GetItem(i))->GetDataType();
    }
    // TODO: handle change of StateVariables
}

// TODO: avoid this nasty trick to synchronize browsing
void
UpnpController::OnMSBrowseResult(NPT_Result res, PLT_DeviceDataReference& /*device*/, PLT_BrowseInfo* info, void* userdata)
{
    if (!userdata) return;
    
    PLT_BrowseDataReference* data = (PLT_BrowseDataReference*) userdata;
    (*data)->res = res;
    if (NPT_SUCCEEDED(res) && info) {
        (*data)->info = *info;
    }
    (*data)->shared_var.SetValue(1);
    delete data;
}





void
UpnpController::playButtonPressed()
{
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    QModelIndex selected = m_mainWindow->getBrowserTreeSelectionModel()->currentIndex();
    if (selected == QModelIndex()) {
        return;
    }
    
/*    ObjectReference* objectRef = static_cast<ObjectReference*>(selected.internalPointer());
    
    std::clog << "UpnpController::playButtonPressed() selected server:" << 
        (char*) objectRef->server->GetFriendlyName() << "," << (char*) objectRef->server->GetUUID();
    std::clog << "UpnpController::playButtonPressed() selected objectId:" << (char*) objectRef->objectId;
    */
/*    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());*/
    
    UpnpObject* object = m_upnpBrowserModel->getObject(selected);
    PLT_MediaObject* pltObject = object->m_pltObject;
    // send SetAVTransportURI packet
    m_mediaController->SetAVTransportURI(m_curMediaRenderer, 0, pltObject->m_Resources[0].m_Uri, pltObject->m_Didl, NULL);
    
}


void
UpnpController::stopButtonPressed()
{
    std::clog << "UpnpController::stopButtonPressed()"; 
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_mediaController->Stop(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::pauseButtonPressed()
{
    std::clog << "UpnpController::pauseButtonPressed()"; 
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_mediaController->Pause(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::sliderMoved(int position)
{
    std::clog << "UpnpController::sliderMoved() to:" << position;
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    NPT_String timestamp;
    PLT_Didl::FormatTimeStamp(timestamp, position);
    m_mediaController->Seek(m_curMediaRenderer, 0, "ABS_TIME", timestamp, NULL);
}


void
UpnpController::OnSetAVTransportURIResult(
                                        NPT_Result                /*res*/ ,
                                        PLT_DeviceDataReference&  device ,
                                        void*                     /*userdata*/ )
{
    std::clog << "UpnpController::OnSetAVTransportURIResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
    // send Play packet ...
    m_mediaController->Play(m_curMediaRenderer, 0, "1", NULL);
}


void
UpnpController::OnPlayResult(
                           NPT_Result               res,
                           PLT_DeviceDataReference& device,
                           void*                    userdata)
{
    std::clog << "UpnpController::OnPlayResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName() << res << userdata;
    // check if playing
    if (res == NPT_SUCCESS) {
        // then do something
    }
}


void
UpnpController::OnStopResult(
                           NPT_Result               /* res */,
                           PLT_DeviceDataReference& device,
                           void*                    /* userdata */)
{
    std::clog << "UpnpController::OnStopResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
    m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::OnPauseResult(
                            NPT_Result               /* res */,
                            PLT_DeviceDataReference& device,
                            void*                    /* userdata */)
{
    std::clog << "UpnpController::OnPauseResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
}


void
UpnpController::OnSeekResult(
                           NPT_Result               /* res */,
                           PLT_DeviceDataReference& device,
                           void*                    /* userdata */)
{
    std::clog << "UpnpController::OnSeekResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
}


void
UpnpController::OnGetPositionInfoResult(
                                      NPT_Result               /* res */,
                                      PLT_DeviceDataReference& /*device*/,
                                      PLT_PositionInfo*        info,
                                      void*                    /* userdata */)
{
    std::clog << "UpnpController::OnGetPositionInfoResult() duration:" << info->track_duration << "position:" << info->abs_time;
    emit setSlider(info->track_duration, info->abs_time);
}


// void
// UpnpController::onSignalReceived()
// {
// //     std::clog << "UpnpController::pollPositionInfo()";
//     if (!m_curMediaRenderer.IsNull()) {
//         m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
//     }
// }



