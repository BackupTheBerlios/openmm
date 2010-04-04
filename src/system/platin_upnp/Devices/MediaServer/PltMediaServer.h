/*****************************************************************
|
|   Platinum - AV Media Server Device
|
|   Copyright (c) 2004-2008 Sylvain Rebaud
|   Author: Sylvain Rebaud (sylvain@rebaud.com)
|
 ****************************************************************/

#ifndef _PLT_MEDIA_SERVER_H_
#define _PLT_MEDIA_SERVER_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "PltDeviceHost.h"
#include "PltMediaItem.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define MAX_PATH_LENGTH 1024

/* BrowseFlags */
enum BrowseFlags {
    BROWSEMETADATA,
    BROWSEDIRECTCHILDREN
};

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/
extern const char* BrowseFlagsStr[];
class PLT_HttpFileServerHandler;

/*----------------------------------------------------------------------
|   PLT_MediaServer class
+---------------------------------------------------------------------*/
class PLT_MediaServer : public PLT_DeviceHost
{
public:
    // PLT_DeviceHost methods
    virtual NPT_Result OnAction(PLT_ActionReference&          action, 
                                const NPT_HttpRequestContext& context);

protected:
    PLT_MediaServer(const char*  friendly_name,
                    bool         show_ip = false,
                    const char*  uuid = NULL,
                    NPT_UInt16   port = 0);
    virtual ~PLT_MediaServer();

    // class methods
    static NPT_Result  GetBrowseFlag(const char* str, BrowseFlags& flag);

    // ConnectionManager
    virtual NPT_Result OnGetCurrentConnectionIDs(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnGetProtocolInfo(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnGetCurrentConnectionInfo(PLT_ActionReference& action, const NPT_HttpRequestContext& context);

    // ContentDirectory
    virtual NPT_Result OnGetSortCapabilities(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnGetSearchCapabilities(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnGetSystemUpdateID(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnBrowse(PLT_ActionReference& action, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnSearch(PLT_ActionReference& action, const NPT_HttpRequestContext& context);

    // overridable methods
    virtual NPT_Result OnBrowseMetadata(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnBrowseDirectChildren(PLT_ActionReference& action, const char* object_id, const NPT_HttpRequestContext& context);
    virtual NPT_Result OnSearch(PLT_ActionReference& action, const NPT_String& object_id, const NPT_String& searchCriteria, const NPT_HttpRequestContext& context);
};

#endif /* _PLT_MEDIA_SERVER_H_ */
