/*****************************************************************
|
|   Platinum - Micro Media Controller
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

#include "upnpcontroller.h"

#include <qthread.h>

#include <platinum/Platinum.h>
#include <platinum/PltMediaController.h>
#include <platinum/PltMediaControllerListener.h>
#include <platinum/PltSyncMediaBrowser.h>
#include <platinum/NptMap.h>
#include <platinum/NptStack.h>

#include <string>
using namespace std;

/*----------------------------------------------------------------------
 |   definitions
 +---------------------------------------------------------------------*/
typedef NPT_Map<NPT_String, NPT_String>        StringMap;
typedef NPT_Lock<StringMap>                    LockStringMap;
typedef NPT_Map<NPT_String, NPT_String>::Entry StringMapEntry;

/*----------------------------------------------------------------------
 |   MediaItemIdFinder
 +---------------------------------------------------------------------*/
class MediaItemIdFinder
{
public:
    // methods
    MediaItemIdFinder(const char* object_id) : m_ObjectID(object_id) {}

    bool operator()(const PLT_MediaObject* const & item) const {
        return item->m_ObjectID.Compare(m_ObjectID, true) ? false : true;
    }

private:
    // members
    NPT_String m_ObjectID;
};


// class UpnpControllerListener : public PLT_MediaControllerListener
// {
//     // PLT_MediaControllerListener
//     void OnMRAddedRemoved(PLT_DeviceDataReference& device, int added);
//     void OnMRStateVariablesChanged(PLT_Service* /* service */, NPT_List<PLT_StateVariable*>* /* vars */) {};
// };

class UpnpController;

class UpnpControllerThread : public QThread
{
    Q_OBJECT
        
public:
        UpnpControllerThread(QObject *parent = 0);
    
    UpnpController* getController() {return m_controller;}

/*signals:
    void rendererAddedRemoved(QString name, QString uuid, bool add);*/
    
protected:
    virtual void run();

private:
    UpnpController *m_controller;
    
};


/*----------------------------------------------------------------------
 |   PLT_MicroMediaController
 +---------------------------------------------------------------------*/
class UpnpController : public QObject, /*QThread,*/ PLT_MediaControllerListener
{
    Q_OBJECT
public:
    UpnpController(/*PLT_CtrlPointReference& ctrlPoint*/);
    virtual ~UpnpController();
    
// PLT_MediaControllerListener
    void OnMRAddedRemoved(PLT_DeviceDataReference& device, int added);
    void OnMRStateVariablesChanged(PLT_Service* /* service */, NPT_List<PLT_StateVariable*>* /* vars */) {};
    
    void ProcessCommandLoop();

signals:
    // TODO: can this reasonably be implemented without singals but with an interface 
    // of pure virtual functions?
    // -> naaa, this signal is emited from a different thread than the main gui thread!!
    // so: handle this with an event ...?
    // a signal connected with Qt::QueuedConnection still segfaults ...
//     void rendererAddedRemoved(QString name, QString uuid, bool add);
    void rendererAddedRemoved(string name, string uuid, bool add);
//     void rendererAddedRemoved();
    
    
    
/*protected:
    virtual void run();*/
    
private:
    const char* ChooseIDFromTable(StringMap& table);
    void        PopDirectoryStackToRoot(void);
    NPT_Result  DoBrowse();

    void        GetCurMediaServer(PLT_DeviceDataReference& server);
    void        GetCurMediaRenderer(PLT_DeviceDataReference& renderer);

    PLT_DeviceDataReference ChooseDevice(const NPT_Lock<PLT_DeviceMap>& deviceList);

    // Command Handlers
    void    HandleCmd_scan(const char* ip);
    void    HandleCmd_getms();
    void    HandleCmd_setms();
    void    HandleCmd_ls();
    void    HandleCmd_cd();
    void    HandleCmd_cdup();
    void    HandleCmd_pwd();
    void    HandleCmd_help();
    void    HandleCmd_getmr();
    void    HandleCmd_setmr();
    void    HandleCmd_open();
    void    HandleCmd_play();
    void    HandleCmd_stop();

private:
    /* The tables of known devices on the network.  These are updated via the
     * OnMSAddedRemoved and OnMRAddedRemoved callbacks.  Note that you should first lock
     * before accessing them using the NPT_Map::Lock function.
     */
    NPT_Lock<PLT_DeviceMap> m_MediaServers;
    NPT_Lock<PLT_DeviceMap> m_MediaRenderers;
    
    PLT_UPnP*               m_upnp;
    PLT_CtrlPointReference  m_ctrlPoint;

    /* The UPnP MediaServer control point. */
    PLT_SyncMediaBrowser*   m_MediaBrowser;
    PLT_MediaControllerListener* m_controllerListener;
    /* The UPnP MediaRenderer control point. */
    PLT_MediaController*    m_MediaController;

    /* The currently selected media server as well as 
     * a lock.  If you ever want to hold both the m_CurMediaRendererLock lock and the 
     * m_CurMediaServerLock lock, make sure you grab the server lock first.
     */
    PLT_DeviceDataReference m_CurMediaServer;
    NPT_Mutex               m_CurMediaServerLock;

    /* The currently selected media renderer as well as 
     * a lock.  If you ever want to hold both the m_CurMediaRendererLock lock and the 
     * m_CurMediaServerLock lock, make sure you grab the server lock first.
     */
    PLT_DeviceDataReference m_CurMediaRenderer;
    NPT_Mutex               m_CurMediaRendererLock;

    /* the most recent results from a browse request.  The results come back in a 
     * callback instead of being returned to the calling function, so this 
     * global variable is necessary in order to give the results back to the calling 
     * function.
     */
    PLT_MediaObjectListReference m_MostRecentBrowseResults;

    /* When browsing through the tree on a media server, this is the stack 
     * symbolizing the current position in the tree.  The contents of the 
     * stack are the object ID's of the nodes.  Note that the object id: "0" should
     * always be at the bottom of the stack.
     */
    NPT_Stack<NPT_String> m_CurBrowseDirectoryStack;

    /* the semaphore on which to block when waiting for a response from over
     * the network 
     */
    NPT_SharedVariable m_CallbackResponseSemaphore;
};

#endif /* UPNPCONTROLLER_H */

