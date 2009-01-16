#include <platinum/PltLog.h>
#include <QtDebug>

#include "upnpcontroller.h"

UpnpController::UpnpController()
{
    qDebug() << "UpnpController::UpnpController()";
//     m_CurBrowseDirectoryStack.Push("0");
    PLT_SetLogLevel(PLT_LOG_LEVEL_MAX);

    m_upnp = new PLT_UPnP(1900, true);
    m_ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
    m_controllerListener = new PLT_MediaControllerListener();
    m_upnp->AddCtrlPoint(m_ctrlPoint);
    m_mediaBrowser = new UpnpSyncMediaBrowser(m_ctrlPoint, true);
    m_upnpBrowserModel = new UpnpBrowserModel(m_mediaBrowser);
    m_mediaController = new PLT_MediaController(m_ctrlPoint, this);
    m_upnp->Start();
    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
    m_ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
    m_ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);
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


QMap<QString, QString>
UpnpController::getServers()
{
    NPT_AutoLock lock(m_mediaServers);
    NPT_AutoLock lockCurServer(m_curMediaServerLock);
    QMap<QString, QString> result;

    m_mediaServers = m_mediaBrowser->GetMediaServers();
    const NPT_List<PLT_DeviceMapEntry*>& entries = m_mediaServers.GetEntries();
    NPT_List<PLT_DeviceMapEntry*>::Iterator entry = entries.GetFirstItem();
    qDebug() << "UpnpController::getServers() found" << m_mediaServers.GetEntryCount() << "server(s)";
    while (entry) {
        PLT_DeviceDataReference device = (*entry)->GetValue();
        NPT_String name = device->GetFriendlyName();
        NPT_String uuid = device->GetUUID();
        result.insert((char*)uuid, (char*)name);
        qDebug() << "UpnpController::getServers() add server:" << (char*)name << (char*)uuid;
        ++entry;
    }
/*    if (m_mediaServers.GetEntryCount() == 1) {
        m_curMediaServer = (*entries.GetFirstItem())->GetValue();
    }*/
    return result;
}


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
