#include <platinum/PltDidl.h>
#include <QtDebug>

#include "upnpcontroller.h"

UpnpController::UpnpController()
{
    qDebug() << "UpnpController::UpnpController()";

    m_upnp = new PLT_UPnP(1900, true);
    m_ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
//     m_controllerListener = new PLT_MediaControllerListener();
    m_upnp->AddCtrlPoint(m_ctrlPoint);
    m_mediaBrowser = new UpnpSyncMediaBrowser(m_ctrlPoint, true);
    m_upnpBrowserModel = new UpnpBrowserModel(m_mediaBrowser);
    m_mediaController = new PLT_MediaController(m_ctrlPoint, this);
    m_upnp->Start();
    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
    m_ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
    m_ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);
    // TODO: get rid of this sleep and do it proper
    sleep(1);
    
    m_mainWindow = new ControllerGui();
    
    m_mainWindow->setBrowserTreeItemModel(m_upnpBrowserModel);
    connect(this, SIGNAL(rendererAddedRemoved(QString, QString, bool)),
            m_mainWindow, SLOT(rendererAddedRemoved(QString, QString, bool)));
    connect(m_mainWindow->getBrowserTreeSelectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(m_mainWindow, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(m_mainWindow, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(m_mainWindow, SIGNAL(pauseButtonPressed()), this, SLOT(pauseButtonPressed()));
    connect(m_mainWindow, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), m_mainWindow, SLOT(setSlider(int, int)));
    
    JSignal::connectNodes(&m_pollPositionInfoTimer.fire, this);
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


// QMap<QString, QString>
// UpnpController::getServers()
// {
//     NPT_AutoLock lock(m_mediaServers);
//     NPT_AutoLock lockCurServer(m_curMediaServerLock);
//     QMap<QString, QString> result;
// 
//     m_mediaServers = m_mediaBrowser->GetMediaServers();
//     const NPT_List<PLT_DeviceMapEntry*>& entries = m_mediaServers.GetEntries();
//     NPT_List<PLT_DeviceMapEntry*>::Iterator entry = entries.GetFirstItem();
//     qDebug() << "UpnpController::getServers() found" << m_mediaServers.GetEntryCount() << "server(s)";
//     while (entry) {
//         PLT_DeviceDataReference device = (*entry)->GetValue();
//         NPT_String name = device->GetFriendlyName();
//         NPT_String uuid = device->GetUUID();
//         result.insert((char*)uuid, (char*)name);
//         qDebug() << "UpnpController::getServers() add server:" << (char*)name << (char*)uuid;
//         ++entry;
//     }
// /*    if (m_mediaServers.GetEntryCount() == 1) {
//         m_curMediaServer = (*entries.GetFirstItem())->GetValue();
//     }*/
//     return result;
// }


void
UpnpController::OnMRAddedRemoved(PLT_DeviceDataReference& device, int added)
{
    NPT_AutoLock lock(m_mediaRenderers);
    NPT_AutoLock lockCurRenderer(m_curMediaRendererLock);
    NPT_String uuid = device->GetUUID();
    QString name = (char*) device->GetFriendlyName();
    qDebug() << "UpnpController::OnMRAddedRemoved()" << (added?"added":"removed") << "renderer:" << name << (char*) uuid;

    if (added) {
        m_mediaRenderers.Put(uuid, device);
        if (m_mediaRenderers.GetEntryCount() == 1) {
            m_curMediaRenderer = device;
        }
        emit rendererAddedRemoved((char*) uuid, name, true);
    } else { /* removed */
        m_mediaRenderers.Erase(uuid);
        // if it's the currently selected one, we have to get rid of it
        if (!m_curMediaRenderer.IsNull() && m_curMediaRenderer == device) {
            m_curMediaRenderer = NULL;
        }
        emit rendererAddedRemoved((char*) uuid, name, false);
    }
}


void
UpnpController::showMainWindow()
{
    m_mainWindow->show();
}


void
UpnpController::selectionChanged(const QItemSelection &selected,
                                const QItemSelection &/*deselected*/)
{
    if (selected.count() > 1) {
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
        
    qDebug() << "UpnpController::selectionChanged() row:" << index.row() << "col:" << index.column();
    qDebug() << "UpnpController::selectionChanged() selected server:" << 
        (char*) objectRef->server->GetFriendlyName() << "," << (char*) objectRef->server->GetUUID();
    qDebug() << "UpnpController::selectionChanged() selected objectId:" << (char*) objectRef->objectId;
    
    
}

void
UpnpController::playButtonPressed()
{
    if (m_curMediaRenderer.IsNull()) {
        return;
    }
    
    QModelIndex selected = m_mainWindow->getBrowserTreeSelectionModel()->currentIndex();
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
//     qDebug() << "UpnpController::OnGetPositionInfoResult() device:" << (char*) device->GetUUID() << (char*) device->GetFriendlyName();
    qDebug() << "UpnpController::OnGetPositionInfoResult() duration:" << info->track_duration << "position:" << info->abs_time;
//         << "track:" << info->track << endl
//         << "track_duration:" << info->track_duration << endl
//         << "rel_time:" << info->rel_time << endl
//         << "abs_time:" << info->abs_time << endl;
//         << "rel_count:" << info->rel_count << endl
//         << "abs_count:" << info->abs_count 
    emit setSlider(info->track_duration, info->abs_time);
//     emit setSlider(222, info->abs_time);
}


void
UpnpController::onSignalReceived()
{
//     qDebug() << "UpnpController::pollPositionInfo()";
    if (!m_curMediaRenderer.IsNull()) {
        m_mediaController->GetPositionInfo(m_curMediaRenderer, 0, NULL);
    }
}
