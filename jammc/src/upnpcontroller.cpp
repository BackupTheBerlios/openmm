#include <platinum/PltDidl.h>
#include <QtDebug>

#include "upnpcontroller.h"

UpnpController::UpnpController()
{
    qDebug() << "UpnpController::UpnpController()";

    m_upnp = new PLT_UPnP(1900, true);
    m_ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
    m_upnp->AddCtrlPoint(m_ctrlPoint);
    m_mediaBrowser = new UpnpSyncMediaBrowser(m_ctrlPoint, true);
    m_upnpBrowserModel = new UpnpBrowserModel(m_mediaBrowser);
    m_mediaController = new PLT_MediaController(m_ctrlPoint, this);
    m_upnpRendererListModel = new UpnpRendererListModel(this);
    m_renderingController = new JRenderingController(m_ctrlPoint);
    
    m_mainWindow = new ControllerGui();
    
    m_mainWindow->setBrowserTreeItemModel(m_upnpBrowserModel);
    m_mainWindow->setRendererListItemModel(m_upnpRendererListModel);
    qRegisterMetaType<string>("string");
    qRegisterMetaType<QItemSelection>("QItemSelection");  // TODO: why's that needed?
    connect(this, SIGNAL(rendererAddedRemoved(string, bool)),
            m_upnpRendererListModel, SLOT(rendererAddedRemoved(string, bool)));
    connect(m_mediaBrowser, SIGNAL(serverAddedRemoved(string, bool)),
            m_upnpBrowserModel, SLOT(serverAddedRemoved(string, bool)));
    connect(m_mainWindow->getBrowserTreeSelectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(m_mainWindow->getRendererListSelectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
    
    connect(m_mainWindow, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(m_mainWindow, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(m_mainWindow, SIGNAL(pauseButtonPressed()), this, SLOT(pauseButtonPressed()));
    connect(m_mainWindow, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), m_mainWindow, SLOT(setSlider(int, int)));
    connect(m_mainWindow, SIGNAL(volSliderMoved(int)), this, SLOT(volSliderMoved(int)));
    
    JSignal::connectNodes(&m_pollPositionInfoTimer.fire, this);
    
    qDebug() << "UpnpController::UpnpController() starting threads ...";
    // now start up all threads and let the show begin ...
    m_upnp->Start();
    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
    m_ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
    m_ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);
    // poll for current position every second
    m_pollPositionInfoTimer.startTimer(1000);
}


UpnpController::~UpnpController()
{
    // TODO: UPnP isn't stopped properly, segfaults when attempting to delete some members?
    qDebug() << "UpnpController::~UpnpController()";
    m_upnp->Stop();
/*    delete m_mediaController;
    delete m_mediaBrowser;
    delete m_controllerListener;
    delete m_upnp;*/
}


// TODO: update BrowserModel when MediaServer is added / removed.
//       don't do a complete reset() on the BrowserModel.

void
UpnpController::OnMRAddedRemoved(PLT_DeviceDataReference& device, int added)
{
    NPT_AutoLock lock(m_mediaRenderers);
    
    NPT_String uuid = device->GetUUID();
    string name = (char*) device->GetFriendlyName();
    qDebug() << "UpnpController::OnMRAddedRemoved()" << (added?"added":"removed") << "renderer:" << name.c_str() << (char*) uuid;

    if (added) {
        m_mediaRenderers.Put(uuid, device);
    } else {
        m_mediaRenderers.Erase(uuid);
    }
    emit rendererAddedRemoved(string((char*)uuid), added);
}


void
UpnpController::showMainWindow()
{
    m_mainWindow->show();
}


void
UpnpController::rendererSelectionChanged(const QItemSelection &selected,
                                 const QItemSelection &/*deselected*/)
{
    NPT_AutoLock lock(m_mediaRenderers);
    NPT_AutoLock lockCurRenderer(m_curMediaRendererLock);
    
    if (selected.count() > 1) {
        return;
    }
    if (selected.empty()) {
        qDebug() << "UpnpController::rendererSelectionChanged() nothing selected";
        m_curMediaRenderer = NULL;
        return;
    }
    QModelIndex index = selected.indexes().first();
    
    string* uuidRef = static_cast<string*>(index.internalPointer());
    m_curMediaRenderer = m_mediaRenderers[uuidRef->c_str()];
    
    qDebug() << "UpnpController::rendererSelectionChanged() row:" << index.row();
    qDebug() << "UpnpController::rendererSelectionChanged() selected renderer:" << 
        (char*) m_curMediaRenderer->GetFriendlyName() << "," << (char*) m_curMediaRenderer->GetUUID();
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
    
    ObjectReference* objectRef = static_cast<ObjectReference*>(selected.internalPointer());
    
    qDebug() << "UpnpController::playButtonPressed() selected server:" << 
        (char*) objectRef->server->GetFriendlyName() << "," << (char*) objectRef->server->GetUUID();
    qDebug() << "UpnpController::playButtonPressed() selected objectId:" << (char*) objectRef->objectId;
    
    // send SetAVTransportURI packet
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());
    m_mediaController->SetAVTransportURI(m_curMediaRenderer, 0, object->m_Resources[0].m_Uri, object->m_Didl, NULL);
    
}


void
UpnpController::stopButtonPressed()
{
    qDebug() << "UpnpController::stopButtonPressed()"; 
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_mediaController->Stop(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::pauseButtonPressed()
{
    qDebug() << "UpnpController::pauseButtonPressed()"; 
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_mediaController->Pause(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::sliderMoved(int position)
{
    qDebug() << "UpnpController::sliderMoved() to:" << position;
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
    qDebug() << "UpnpController::OnSetAVTransportURIResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
    // send Play packet ...
    m_mediaController->Play(m_curMediaRenderer, 0, "1", NULL);
}


void
UpnpController::OnPlayResult(
                           NPT_Result               res,
                           PLT_DeviceDataReference& device,
                           void*                    userdata)
{
    qDebug() << "UpnpController::OnPlayResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName() << res << userdata;
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
    qDebug() << "UpnpController::OnStopResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
    m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
}


void
UpnpController::OnPauseResult(
                            NPT_Result               /* res */,
                            PLT_DeviceDataReference& device,
                            void*                    /* userdata */)
{
    qDebug() << "UpnpController::OnPauseResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
}


void
UpnpController::OnSeekResult(
                           NPT_Result               /* res */,
                           PLT_DeviceDataReference& device,
                           void*                    /* userdata */)
{
    qDebug() << "UpnpController::OnSeekResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
}


void
UpnpController::OnGetPositionInfoResult(
                                      NPT_Result               /* res */,
                                      PLT_DeviceDataReference& /*device*/,
                                      PLT_PositionInfo*        info,
                                      void*                    /* userdata */)
{
    qDebug() << "UpnpController::OnGetPositionInfoResult() duration:" << info->track_duration << "position:" << info->abs_time;
    emit setSlider(info->track_duration, info->abs_time);
}


void
UpnpController::onSignalReceived()
{
//     qDebug() << "UpnpController::pollPositionInfo()";
    if (!m_curMediaRenderer.IsNull()) {
        m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
    }
}


void
UpnpController::volSliderMoved(int position)
{
    qDebug() << "UpnpController::volSliderMoved() to:" << position;
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    m_renderingController->setVolume(m_curMediaRenderer, 0, position);
}

