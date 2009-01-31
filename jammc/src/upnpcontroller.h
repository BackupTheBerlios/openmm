#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

#include <QObject>
#include <QItemSelection>

#include <jamm/thread.h>
#include <jamm/signode.h>

#include <platinum/Platinum.h>
#include <platinum/PltMediaController.h>
#include <platinum/PltMediaControllerListener.h>

#include "upnprenderingcontrol.h"
// #include "upnpmediacontroller.h"
#include "upnpsyncmediabrowser.h"
#include "upnpbrowsermodel.h"
#include "upnprendererlistmodel.h"
#include "controllergui.h"

using namespace Jamm;

class UpnpRendererListModel;

class UpnpController : public QObject, PLT_MediaControllerListener, JSlot
{
    Q_OBJECT

public:
    UpnpController();
    ~UpnpController();

    UpnpBrowserModel* getBrowserModel() { return m_upnpBrowserModel; }
    void showMainWindow();

signals:
    void rendererAddedRemoved(string uuid, bool add);
    void setSlider(int max, int val);

private slots:
    void rendererSelectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    
    void playButtonPressed();
    void stopButtonPressed();
    void pauseButtonPressed();
    void sliderMoved(int position);
    void volSliderMoved(int position);
    
private:
    virtual void onSignalReceived();
    
protected:
    /* PLT_MediaControllerListener
     * these callbacks are called from the listener thread
     */
    virtual void OnMRAddedRemoved(PLT_DeviceDataReference& device, int added);
    virtual void OnMRStateVariablesChanged(PLT_Service* service, NPT_List<PLT_StateVariable*>* vars);
    
    
        // AVTransport
//     virtual void OnGetCurrentTransportActionsResult(
//         NPT_Result               /* res */, 
//         PLT_DeviceDataReference& /* device */,
//         PLT_StringList*          /* actions */, 
//         void*                    /* userdata */);
    
//     virtual void OnGetDeviceCapabilitiesResult(
//                                                 NPT_Result               /* res */, 
//                                                 PLT_DeviceDataReference& /* device */,
//                                                 PLT_DeviceCapabilities*  /* capabilities */,
//                                                 void*                    /* userdata */);
    
//     virtual void OnGetMediaInfoResult(
//                                        NPT_Result               /* res */,
//                                        PLT_DeviceDataReference& /* device */,
//                                        PLT_MediaInfo*           /* info */,
//                                        void*                    /* userdata */);
    
    virtual void OnGetPositionInfoResult(
                                          NPT_Result               /* res */,
                                          PLT_DeviceDataReference& /* device */,
                                          PLT_PositionInfo*        /* info */,
                                          void*                    /* userdata */);
    
//     virtual void OnGetTransportInfoResult(
//                                            NPT_Result               /* res */,
//                                            PLT_DeviceDataReference& /* device */,
//                                            PLT_TransportInfo*       /* info */,
//                                            void*                    /* userdata */);
    
//     virtual void OnGetTransportSettingsResult(
//                                                NPT_Result               /* res */,
//                                                PLT_DeviceDataReference& /* device */,
//                                                PLT_TransportSettings*   /* settings */,
//                                                void*                    /* userdata */);
    
//     virtual void OnNextResult(
//                                NPT_Result               /* res */,
//                                PLT_DeviceDataReference& /* device */,
//                                void*                    /* userdata */);
    
    virtual void OnPauseResult(
                                NPT_Result               /* res */,
                                PLT_DeviceDataReference& /* device */,
                                void*                    /* userdata */); 
    
    virtual void OnPlayResult(
                               NPT_Result               /* res */,
                               PLT_DeviceDataReference& /* device */,
                               void*                    /* userdata */);
    
//     virtual void OnPreviousResult(
//                                    NPT_Result               /* res */,
//                                    PLT_DeviceDataReference& /* device */,
//                                    void*                    /* userdata */);
    
    virtual void OnSeekResult(
                               NPT_Result               /* res */,
                               PLT_DeviceDataReference& /* device */,
                               void*                    /* userdata */);
    
    virtual void OnSetAVTransportURIResult(
                                            NPT_Result               /* res */,
                                            PLT_DeviceDataReference& /* device */,
                                            void*                    /* userdata */);
    
//     virtual void OnSetPlayModeResult(
//                                       NPT_Result               /* res */,
//                                       PLT_DeviceDataReference& /* device */,
//                                       void*                    /* userdata */);
    
    virtual void OnStopResult(
                               NPT_Result               /* res */,
                               PLT_DeviceDataReference& /* device */,
                               void*                    /* userdata */);
    
    // ConnectionManager
//     virtual void OnGetCurrentConnectionIDsResult(
//                                                   NPT_Result               /* res */,
//                                                   PLT_DeviceDataReference& /* device */,
//                                                   PLT_StringList*          /* ids */,
//                                                   void*                    /* userdata */);
    
//     virtual void OnGetCurrentConnectionInfoResult(
//         NPT_Result               /* res */,
//         PLT_DeviceDataReference& /* device */,
//         PLT_ConnectionInfo*      /* info */,
//         void*                    /* userdata */);
    
//     virtual void OnGetProtocolInfoResult(
//                                           NPT_Result               /* res */,
//                                           PLT_DeviceDataReference& /* device */,
//                                           PLT_StringList*          /* sources */,
//                                           PLT_StringList*          /* sinks */,
//                                           void*                    /* userdata */);
    
    // AVRenderingControl
    // TODO: implement AVRendering Control

private:
    PLT_UPnP*                       m_upnp;
    PLT_CtrlPointReference          m_ctrlPoint;

    NPT_Lock<PLT_DeviceMap>         m_mediaServers;
    NPT_Lock<PLT_DeviceMap>         m_mediaRenderers;

    /* The UPnP MediaServer control point (a synchronous one)
     */
    UpnpSyncMediaBrowser*           m_mediaBrowser;
    JRenderingController*           m_renderingController;
    /* The UPnP MediaRenderer control point (an asynchronous one)
     */
    PLT_MediaController*            m_mediaController;

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
    
//     NPT_SharedVariable              m_AVTransportLock;
    
    UpnpBrowserModel*               m_upnpBrowserModel;
    UpnpRendererListModel*          m_upnpRendererListModel;
    ControllerGui*                  m_mainWindow;
    JTimer                          m_pollPositionInfoTimer;
};

#endif /* UPNPCONTROLLER_H */

