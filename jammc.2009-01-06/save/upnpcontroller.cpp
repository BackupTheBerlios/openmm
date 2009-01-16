/*****************************************************************
|
|   Platinum - Miccro Media Controller
|
|   Copyright (c) 2004-2008, Plutinosoft, LLC.
|   Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
****************************************************************/

#include "upnpcontroller.h"

#include <platinum/PltLeaks.h>

#include <cstdio>
#include <cstdlib>
#include <string>

#include "debug.h"

/*----------------------------------------------------------------------
|   UpnpController::UpnpController
+---------------------------------------------------------------------*/
UpnpController::UpnpController(/*PLT_CtrlPointReference& ctrlPoint*/)
{
    // create the stack that will be the directory where the
    // user is currently browsing. 
    // push the root directory onto the directory stack.
    m_CurBrowseDirectoryStack.Push("0");
    
    m_upnp = new PLT_UPnP(1900, true);
    m_ctrlPoint = PLT_CtrlPointReference(new PLT_CtrlPoint());
    m_MediaBrowser = new PLT_SyncMediaBrowser(m_ctrlPoint, true);
//     m_controllerListener = new PLT_MediaControllerListener();
    m_MediaController = new PLT_MediaController(m_ctrlPoint, this);
    m_upnp->AddCtrlPoint(m_ctrlPoint);
    m_upnp->Start();
    // tell control point to perform extra broadcast discover every secs
    // in case our device doesn't support multicast
    m_ctrlPoint->Discover(NPT_HttpUrl("255.255.255.255", 1900, "*"), "upnp:rootdevice", 1, 6000);
    m_ctrlPoint->Discover(NPT_HttpUrl("239.255.255.250", 1900, "*"), "upnp:rootdevice", 1, 6000);
    
//     start();
}

/*----------------------------------------------------------------------
|   UpnpController::UpnpController
+---------------------------------------------------------------------*/
UpnpController::~UpnpController()
{
//     delete m_ctrlPoint;
    delete m_MediaBrowser;
    delete m_MediaController;
//     delete m_controllerListener;
    // put this in "on thread exit"?
    m_upnp->Stop();
    delete m_upnp;
}

UpnpControllerThread::UpnpControllerThread(QObject *parent)
: QThread(parent)
{
    start();
}

void
UpnpControllerThread::run()
{
    m_controller = new UpnpController(); // TODO: delete m_controller before UpnpControllerThread is deleted.
//     sleep(2);
//     TRACE("UpnpControllerThread::run() emit rendererAddedRemoved()");
//     emit rendererAddedRemoved("foo", "bar", true);
//     sleep(5);
    exec();
}


/*
*  Remove trailing while space from a string
*/
void strchomp(char* str)
{
    if (!str) return;

    while (*str) {
        if ((*str == ' ')  ||
            (*str == '\t') ||
            (*str == '\r') ||
            (*str == '\n'))
        {
            *str = '\0';
            break;
        }
        ++str;
    }
}

/*----------------------------------------------------------------------
|   UpnpController::ChooseIDFromTable
+---------------------------------------------------------------------*/
/* 
 * Presents a list to the user, allows the user to choose one item.
 *
 * Parameters:
 *		StringMap: A map that contains the set of items from
 *                        which the user should choose.  The key should be a unique ID,
 *						 and the value should be a string describing the item. 
 *       returns a NPT_String with the unique ID. 
 */
const char*
UpnpController::ChooseIDFromTable(StringMap& table)
{
    printf("Select one of the following:\n");

    NPT_List<StringMapEntry*> entries = table.GetEntries();
    if (entries.GetItemCount() == 0) {
        printf("None available\n"); 
    } else {
        // display the list of entries
        NPT_List<StringMapEntry*>::Iterator entry = entries.GetFirstItem();
        int count = 0;
        while (entry) {
            printf("%d)\t%s (%s)\n", ++count, (const char*)(*entry)->GetValue(), (const char*)(*entry)->GetKey());
            ++entry;
        }

        int index, watchdog = 3;
        char buffer[1024];

        // wait for input
        while (watchdog > 0) {
            fgets(buffer, 1024, stdin);
            strchomp(buffer);

            if (1 != sscanf(buffer, "%d", &index)) {
                printf("Please enter a number\n");
            } else if (index < 0 || index > count)	{
                printf("Please choose one of the above, or 0 for none\n");
                watchdog--;
                index = 0;
            } else {	
                watchdog = 0;
            }
        }

        // find the entry back
        if (index != 0) {
            entry = entries.GetFirstItem();
            while (entry && --index) {
                ++entry;
            }
            if (entry) {
                return (*entry)->GetKey();
            }
        }
    }

    return NULL;
}

/*----------------------------------------------------------------------
|   UpnpController::PopDirectoryStackToRoot
+---------------------------------------------------------------------*/
void
UpnpController::PopDirectoryStackToRoot(void)
{
    NPT_String val;
    while (NPT_SUCCEEDED(m_CurBrowseDirectoryStack.Peek(val)) && val.Compare("0")) {
        m_CurBrowseDirectoryStack.Pop(val);
    }
}

/*----------------------------------------------------------------------
|   UpnpController::OnMRAddedRemoved
+---------------------------------------------------------------------*/
void
UpnpController::OnMRAddedRemoved(PLT_DeviceDataReference& device, int added)
{
    NPT_String uuid = device->GetUUID();

    if (added) {
        NPT_AutoLock lock(m_MediaRenderers);

        // test if it's a media renderer
        PLT_Service* service;
        if (NPT_SUCCEEDED(device->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service))) {
            m_MediaRenderers.Put(uuid, device);
            emit rendererAddedRemoved((char*) device->GetFriendlyName(), (char*) uuid, true);
        }
    } else { /* removed */
        {
            NPT_AutoLock lock(m_MediaRenderers);
            m_MediaRenderers.Erase(uuid);
            emit rendererAddedRemoved((char*) device->GetFriendlyName(), (char*) uuid, false);
        }

        {
            NPT_AutoLock lock(m_CurMediaRendererLock);

            // if it's the currently selected one, we have to get rid of it
            if (!m_CurMediaRenderer.IsNull() && m_CurMediaRenderer == device) {
                m_CurMediaRenderer = NULL;
            }
        }
    }
}

/*----------------------------------------------------------------------
|   UpnpController::ChooseIDGetCurMediaServerFromTable
+---------------------------------------------------------------------*/
void
UpnpController::GetCurMediaServer(PLT_DeviceDataReference& server)
{
    NPT_AutoLock lock(m_CurMediaServerLock);

    if (m_CurMediaServer.IsNull()) {
        printf("No server selected, select one with setms\n");
    } else {
        server = m_CurMediaServer;
    }
}

/*----------------------------------------------------------------------
|   UpnpController::GetCurMediaRenderer
+---------------------------------------------------------------------*/
void
UpnpController::GetCurMediaRenderer(PLT_DeviceDataReference& renderer)
{
    NPT_AutoLock lock(m_CurMediaRendererLock);

    if (m_CurMediaRenderer.IsNull()) {
        printf("No renderer selected, select one with setmr\n");
    } else {
        renderer = m_CurMediaRenderer;
    }
}

/*----------------------------------------------------------------------
|   UpnpController::DoBrowse
+---------------------------------------------------------------------*/
NPT_Result
UpnpController::DoBrowse()
{
    NPT_Result res = NPT_FAILURE;
    PLT_DeviceDataReference device;
    GetCurMediaServer(device);
    if (!device.IsNull()) {
        NPT_String object_id;
        m_CurBrowseDirectoryStack.Peek(object_id);

        // send off the browse packet.  Note that this will
        // not block.  There is a call to WaitForResponse in order
        // to block until the response comes back.
        res = m_MediaBrowser->Browse(device, (const char*)object_id, m_MostRecentBrowseResults);		
    }

    return res;
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_getms
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_getms()
{
    PLT_DeviceDataReference device;
    GetCurMediaServer(device);
    if (!device.IsNull()) {
        printf("Current media server: %s\n", (const char*)device->GetFriendlyName());
    } else {
        // this output is taken care of by the GetCurMediaServer call
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_getmr
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_getmr()
{
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        printf("Current media renderer: %s\n", (const char*)device->GetFriendlyName());
    } else {
        // this output is taken care of by the GetCurMediaRenderer call
    }
}

/*----------------------------------------------------------------------
|   UpnpController::ChooseDevice
+---------------------------------------------------------------------*/
PLT_DeviceDataReference
UpnpController::ChooseDevice(const NPT_Lock<PLT_DeviceMap>& deviceList)
{
    StringMap            namesTable;
    PLT_DeviceDataReference* result = NULL;
    NPT_String               chosenUUID;
    NPT_AutoLock             lock(m_MediaServers);

    // create a map with the device UDN -> device Name 
    const NPT_List<PLT_DeviceMapEntry*>& entries = deviceList.GetEntries();
    NPT_List<PLT_DeviceMapEntry*>::Iterator entry = entries.GetFirstItem();
    while (entry) {
        PLT_DeviceDataReference device = (*entry)->GetValue();
        NPT_String              name   = device->GetFriendlyName();
        namesTable.Put((*entry)->GetKey(), name);

        ++entry;
    }

    // ask user to choose
    chosenUUID = ChooseIDFromTable(namesTable);
    if (chosenUUID.GetLength()) {
        deviceList.Get(chosenUUID, result);
    }

    return result?*result:PLT_DeviceDataReference(); // return empty reference if not device was selected
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_setms
+---------------------------------------------------------------------*/
void 
UpnpController::HandleCmd_setms()
{
    NPT_AutoLock lock(m_CurMediaServerLock);

    PopDirectoryStackToRoot();
    m_CurMediaServer = ChooseDevice(m_MediaBrowser->GetMediaServers());
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_setmr
+---------------------------------------------------------------------*/
void 
UpnpController::HandleCmd_setmr()
{
    NPT_AutoLock lock(m_CurMediaRendererLock);

    m_CurMediaRenderer = ChooseDevice(m_MediaRenderers);
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_ls
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_ls()
{
    DoBrowse();

    if (!m_MostRecentBrowseResults.IsNull()) {
        printf("There were %d results\n", m_MostRecentBrowseResults->GetItemCount());

        NPT_List<PLT_MediaObject*>::Iterator item = m_MostRecentBrowseResults->GetFirstItem();
        while (item) {
            if ((*item)->IsContainer()) {
                printf("Container: %s (%s)\n", (*item)->m_Title.GetChars(), (*item)->m_ObjectID.GetChars());
            } else {
                printf("Item: %s (%s)\n", (*item)->m_Title.GetChars(), (*item)->m_ObjectID.GetChars());
            }
            ++item;
        }

        m_MostRecentBrowseResults = NULL;
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_cd
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_cd()
{
    NPT_String      newObjID;
    StringMap   containers;

    DoBrowse();

    if (!m_MostRecentBrowseResults.IsNull()) {
        NPT_List<PLT_MediaObject*>::Iterator item = m_MostRecentBrowseResults->GetFirstItem();
        while (item) {
            if ((*item)->IsContainer()) {
                containers.Put((*item)->m_ObjectID, (*item)->m_Title);
            }
            ++item;
        }

        newObjID = ChooseIDFromTable(containers);
        if (newObjID.GetLength()) {
            m_CurBrowseDirectoryStack.Push(newObjID);
        }

        m_MostRecentBrowseResults = NULL;
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_cdup
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_cdup()
{
    // we don't want to pop the root off now....
    NPT_String val;
    m_CurBrowseDirectoryStack.Peek(val);
    if (val.Compare("0")) {
        m_CurBrowseDirectoryStack.Pop(val);
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_pwd
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_pwd()
{
    NPT_Stack<NPT_String> tempStack;
    NPT_String val;

    while (NPT_SUCCEEDED(m_CurBrowseDirectoryStack.Peek(val))) {
        m_CurBrowseDirectoryStack.Pop(val);
        tempStack.Push(val);
    }

    while (NPT_SUCCEEDED(tempStack.Peek(val))) {
        tempStack.Pop(val);
        printf("%s/", (const char*)val);
        m_CurBrowseDirectoryStack.Push(val);
    }
    printf("\n");
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_open
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_open()
{
    NPT_String              objID;
    StringMap           tracks;
    PLT_DeviceDataReference device;

    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        // get the protocol info to try to see in advance if a track would play on the device

        // issue a browse
        DoBrowse();

        if (!m_MostRecentBrowseResults.IsNull()) {
            // create a map item id -> item title
            NPT_List<PLT_MediaObject*>::Iterator item = m_MostRecentBrowseResults->GetFirstItem();
            while (item) {
                if (!(*item)->IsContainer()) {
                    tracks.Put((*item)->m_ObjectID, (*item)->m_Title);
                }
                ++item;
            }

            // let the user choose which one
            objID = ChooseIDFromTable(tracks);
            if (objID.GetLength()) {
                // look back for the PLT_MediaItem in the results
                PLT_MediaObject* track = NULL;
                if (NPT_SUCCEEDED(NPT_ContainerFind(*m_MostRecentBrowseResults, MediaItemIdFinder(objID), track))) {
                    if (track->m_Resources.GetItemCount() > 0) {
                        // invoke the setUri
                        m_MediaController->SetAVTransportURI(device, 0, track->m_Resources[0].m_Uri, track->m_Didl, NULL);
                        m_MediaController->Play(device, 0, "1", NULL);                        
                    } else {
                        printf("Couldn't find the proper resource\n");
                    }

                } else {
                    printf("Couldn't find the track\n");
                }
            }

            m_MostRecentBrowseResults = NULL;
        }
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_play
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_play()
{
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        m_MediaController->Play(device, 0, "1", NULL);
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_stop
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_stop()
{
    PLT_DeviceDataReference device;
    GetCurMediaRenderer(device);
    if (!device.IsNull()) {
        m_MediaController->Stop(device, 0, NULL);
    }
}

/*----------------------------------------------------------------------
|   UpnpController::HandleCmd_help
+---------------------------------------------------------------------*/
void
UpnpController::HandleCmd_help()
{
    printf("\n\nNone of the commands take arguments.  The commands with a * \n");
    printf("signify ones that will prompt the user for more information once\n");
    printf("the command is called\n\n");
    printf("The available commands are:\n\n");
    printf(" quit    -   shutdown the Control Point\n");
    printf(" exit    -   same as quit\n");

    printf(" setms   - * select a media server to become the active media server\n");
    printf(" getms   -   print the friendly name of the active media server\n");
    printf(" ls      -   list the contents of the current directory on the active \n");
    printf("             media server\n");
    printf(" cd      - * traverse down one level in the content tree on the active\n");
    printf("             media server\n");
    printf(" cdup    -   traverse up one level in the content tree on the active\n");
    printf("             media server\n");
    printf(" pwd     -   print the path from the root to your current position in the \n");
    printf("             content tree on the active media server\n");
    printf(" setmr   - * select a media renderer to become the active media renderer\n");
    printf(" getmr   -   print the friendly name of the active media renderer\n");
    printf(" open    -   set the uri on the active media renderer\n");
    printf(" play    -   play the active uri on the active media renderer\n");
    printf(" stop    -   stop the active uri on the active media renderer\n");

    printf(" help    -   print this help message\n\n");
}

/*----------------------------------------------------------------------
|   UpnpController::ProcessCommandLoop
+---------------------------------------------------------------------*/
void 
UpnpController::ProcessCommandLoop()
{
    char command[2048];
    bool abort = false;

    command[0] = '\0';
    while (!abort) {
        printf("command> ");
        fflush(stdout);
        fgets(command, 2048, stdin);
        strchomp(command);

        if (0 == strcmp(command, "quit") || 0 == strcmp(command, "exit")) {
            abort = true;
        } else if (0 == strcmp(command, "setms")) {
            HandleCmd_setms();
        } else if (0 == strcmp(command, "getms")) {
            HandleCmd_getms();
        } else if (0 == strcmp(command, "ls")) {
            HandleCmd_ls();
        } else if (0 == strcmp(command, "cd")) {
            HandleCmd_cd();
        } else if (0 == strcmp(command, "cdup")) {
            HandleCmd_cdup();
        } else if (0 == strcmp(command, "pwd")) {
            HandleCmd_pwd();
        } else if (0 == strcmp(command, "setmr")) {
            HandleCmd_setmr();
        } else if (0 == strcmp(command, "getmr")) {
            HandleCmd_getmr();
        } else if (0 == strcmp(command, "open")) {
            HandleCmd_open();
        } else if (0 == strcmp(command, "play")) {
            HandleCmd_play();
        } else if (0 == strcmp(command, "stop")) {
            HandleCmd_stop();
        } else if (0 == strcmp(command, "help")) {
            HandleCmd_help();
        } else if (0 == strcmp(command, "")) {
            // just prompt again
        } else {
            printf("Unrecognized command: %s\n", command);
            HandleCmd_help();
        }
    }
}

