#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

#include <QObject>
// #include <QStringList>
#include <QMap>

#include <platinum/Platinum.h>
#include <platinum/PltMediaController.h>
#include <platinum/PltMediaControllerListener.h>
// #include <platinum/PltSyncMediaBrowser.h>

#include "upnpsyncmediabrowser.h"
#include "upnpbrowsermodel.h"


class UpnpController : public QObject, PLT_MediaControllerListener
{
    Q_OBJECT

public:
    UpnpController();
    ~UpnpController();

    QMap<QString, QString> getServers();
    UpnpBrowserModel* getBrowserModel() {return m_upnpBrowserModel;}

signals:
    void rendererAddedRemoved(QString uuid, QString name, bool add);

protected:
    /* PLT_MediaControllerListener
     * these callbacks are called from the listener thread
     */
    virtual void OnMRAddedRemoved(PLT_DeviceDataReference& device, int added);
//     virtual void OnMRStateVariablesChanged(PLT_Service* /* service */, NPT_List<PLT_StateVariable*>* /* vars */) {};

private:
    PLT_UPnP*                       m_upnp;
    PLT_CtrlPointReference          m_ctrlPoint;

    NPT_Lock<PLT_DeviceMap>         m_mediaServers;
    NPT_Lock<PLT_DeviceMap>         m_mediaRenderers;

    /* The UPnP MediaServer control point (a synchronous one)
     */
    UpnpSyncMediaBrowser*           m_mediaBrowser;

    /* The UPnP MediaRenderer control point (an asynchronous one)
     */
    PLT_MediaController*            m_mediaController;
    PLT_MediaControllerListener*    m_controllerListener;

    /* The currently selected media server as well as 
     * a lock.  If you ever want to hold both the m_CurMediaRendererLock lock and the 
     * m_CurMediaServerLock lock, make sure you grab the server lock first.
     */
    PLT_DeviceDataReference         m_curMediaServer;
    NPT_Mutex                       m_curMediaServerLock;

    /* The currently selected media renderer as well as 
     * a lock.  If you ever want to hold both the m_CurMediaRendererLock lock and the 
     * m_CurMediaServerLock lock, make sure you grab the server lock first.
     */
    PLT_DeviceDataReference         m_curMediaRenderer;
    NPT_Mutex                       m_curMediaRendererLock;
    
    UpnpBrowserModel*               m_upnpBrowserModel;
};

#endif /* UPNPCONTROLLER_H */

