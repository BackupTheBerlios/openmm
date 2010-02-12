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

#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

// #include <QObject>
// #include <QItemSelection>

// #include <platinum/Platinum.h>
// #include <platinum/PltMediaController.h>

#include <Jamm/UpnpAvTypes.h>

#include "UpnpAvCtrlImpl.h"

// #include "upnprenderingcontrol.h"
// #include "upnpsyncmediabrowser.h"
// #include "QtUi/UpnpBrowserModel.h"
// #include "QtUi/UpnpRendererListModel.h"
// #include "QtUi/ControllerGui.h"



// class UpnpBrowserModel;
// class UpnpRendererListModel;
// class ControllerGui;

// class RendererListItem
// {
//     RendererListItem(MediaRendererController* pRendererController)
//         : m_pRendererController(pRendererController) {}
//     
//     int index();
//     int rendererCount();
//     std::string displayName();
//     
// private:
//     MediaRendererController*    m_pRendererController;
// };


class UpnpUserInterface
{
    friend class UpnpController;
    
public:
    // TODO: pass command line arguments to user interface toolkit
    virtual int eventLoop() = 0;
    
    virtual void initGui() = 0;
    virtual void showMainWindow() = 0;
    
protected:
    virtual void beginAddDevice(int position) = 0;
    virtual void beginRemoveDevice(int position) = 0;
    virtual void endAddDevice() = 0;
    virtual void endRemoveDevice() = 0;
    
    virtual void deviceAdded(Device* pDevice);
    virtual void deviceRemoved(Device* pDevice);
    
    Jamm::Container<Jamm::Device>*    m_pDevices;
};


class UpnpAvUserInterface : UpnpUserInterface
{
    friend class UpnpAvController;
    
protected:
    virtual void beginAddRenderer(int position) = 0;
    virtual void beginAddServer(int position) = 0;
    virtual void beginRemoveRenderer(int position) = 0;
    virtual void beginRemoveServer(int position) = 0;
    virtual void endAddRenderer() = 0;
    virtual void endAddServer() = 0;
    virtual void endRemoveRenderer() = 0;
    virtual void endRemoveServer() = 0;
    
    void playPressed();
    void stopPressed();
    void pausePressed();
    void positionMoved(int position);
    void volumeChanged(int value);
    
    void rendererSelected(MediaRendererController* pRenderer);
    void MediaObjectSelected(Jamm::Av::MediaObject* pObject);
    
private:
    MediaRendererController*                     m_pSelectedRenderer;
    Jamm::Container<MediaRendererController>*    m_pRenderers;
    Jamm::Container<MediaServerController>*      m_pServers;
};


class UpnpController : public Jamm::Controller
{
public:
    ~UpnpController();
    
    void start();
    
protected:
    virtual void deviceAdded(Jamm::DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(Jamm::DeviceRoot* pDeviceRoot);
    
private:
    Jamm::Container<Jamm::Device>               m_devices;
    UpnpUserInterface*                          m_pUserInterface;
};


class UpnpAvController : public UpnpController
{
    
private:
    virtual void deviceAdded(Jamm::DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(Jamm::DeviceRoot* pDeviceRoot);
    
    Jamm::Container<MediaRendererController>    m_renderers;
    Jamm::Container<MediaServerController>      m_servers;
    UpnpAvUserInterface*                        m_pUserInterface;
};

    
    //     UpnpBrowserModel* getBrowserModel() const { return m_upnpBrowserModel; }
    
// signals:
//     void serverAddedRemoved(UpnpServer* server, bool add);
//     void rendererAddedRemoved(Jamm::Device* renderer, bool add);
//     void rendererAddedRemoved(string uuid, bool add);
//     void setSlider(int max, int val);
    
// private slots:
//     void rendererSelectionChanged(const QItemSelection &selected,
//                           const QItemSelection &deselected);
    
//     void playButtonPressed();
//     void stopButtonPressed();
//     void pauseButtonPressed();
//     void sliderMoved(int position);
//     void volSliderMoved(int position);
    
//     void modelIndexActivated(const QModelIndex& index);
//     void modelIndexExpanded(const QModelIndex& index);
    
    
//     std::vector<Jamm::Device*>                          m_pDevices;
//     std::map<Jamm::Device*,MediaRendererController*>    m_pRenderers;
//     std::map<Jamm::Device*,MediaServerController*>      m_pServers;
    

//     virtual void onSignalReceived();
    
    
//     UpnpBrowserModel*                                   m_upnpBrowserModel;
//     UpnpRendererListModel*                              m_upnpRendererListModel;
//     ControllerGui*                                      m_mainWindow;
    
//     JTimer                          m_pollPositionInfoTimer;
    
    
    
    // --------------------------------------------------------------
// protected:
//     virtual void OnMRAddedRemoved(PLT_DeviceDataReference& device, int added);
//     virtual void OnMRStateVariablesChanged(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars);
    
//     virtual void OnMSAddedRemoved(PLT_DeviceDataReference& device, int added);
//     virtual void OnMSStateVariablesChanged(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars);
//     virtual void OnMSBrowseResult(NPT_Result res, PLT_DeviceDataReference& device, PLT_BrowseInfo* info, void* userdata);
//     
//     
//     virtual void OnGetPositionInfoResult(
//                                           NPT_Result               /* res */,
//                                           PLT_DeviceDataReference& /* device */,
//                                           PLT_PositionInfo*        /* info */,
//                                           void*                    /* userdata */);
//     
// 
//     
//     virtual void OnPauseResult(
//                                 NPT_Result               /* res */,
//                                 PLT_DeviceDataReference& /* device */,
//                                 void*                    /* userdata */); 
//     
//     virtual void OnPlayResult(
//                                NPT_Result               /* res */,
//                                PLT_DeviceDataReference& /* device */,
//                                void*                    /* userdata */);
//     
//     
//     virtual void OnSeekResult(
//                                NPT_Result               /* res */,
//                                PLT_DeviceDataReference& /* device */,
//                                void*                    /* userdata */);
//     
//     virtual void OnSetAVTransportURIResult(
//                                             NPT_Result               /* res */,
//                                             PLT_DeviceDataReference& /* device */,
//                                             void*                    /* userdata */);
//     
// 
//     
//     virtual void OnStopResult(
//                                NPT_Result               /* res */,
//                                PLT_DeviceDataReference& /* device */,
//                                void*                    /* userdata */);
//     
// 
// private:
//     
//     PLT_UPnP*                       m_upnp;
//     PLT_CtrlPointReference          m_ctrlPoint;
// 
//     NPT_Lock<PLT_DeviceMap>         m_mediaServers;
//     NPT_Lock<PLT_DeviceMap>         m_mediaRenderers;
// 
//     PLT_MediaBrowser*               m_mediaBrowser;
//     JRenderingController*           m_renderingController;
//     PLT_MediaController*            m_mediaController;
// 
//     PLT_DeviceDataReference         m_curMediaRenderer;
//     NPT_Mutex                       m_curMediaRendererLock;
//     
// };

#endif /* UPNPCONTROLLER_H */

