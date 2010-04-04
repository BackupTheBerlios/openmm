/*****************************************************************
|
|   Platinum - Device Host
|
|   Copyright (c) 2004-2008 Sylvain Rebaud
|   Author: Sylvain Rebaud (sylvain@rebaud.com)
|
 ****************************************************************/

#ifndef _PLT_DEVICE_HOST_H_
#define _PLT_DEVICE_HOST_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "PltDeviceData.h"
#include "PltHttpServerListener.h"
#include "PltSsdpListener.h"
#include "PltTaskManager.h"
#include "PltAction.h"

/*----------------------------------------------------------------------
|   forward declarations
+---------------------------------------------------------------------*/
class PLT_HttpServer;
class PLT_HttpServerHandler;
class PLT_SsdpDeviceAnnounceTask;
class PLT_SsdpListenTask;

/*----------------------------------------------------------------------
|   PLT_DeviceHost class
+---------------------------------------------------------------------*/
class PLT_DeviceHost : public PLT_DeviceData,
                       public PLT_SsdpPacketListener
{
public:
    PLT_DeviceHost(const char*  description_path = "/",
                   const char*  uuid = "",
                   const char*  device_type = "",
                   const char*  friendly_name = "",
                   bool         show_ip = false,
                   NPT_UInt16   port = 0);

    virtual void SetBroadcast(bool broadcast) { m_Broadcast = broadcast; }

    // Overridables
    virtual NPT_Result OnAction(PLT_ActionReference&          action, 
                                const NPT_HttpRequestContext& context);

    // NPT_HttpRequestHandler forward for control/event requests
    virtual NPT_Result ProcessHttpRequest(NPT_HttpRequest&              request,
                                          const NPT_HttpRequestContext& context,
                                          NPT_HttpResponse&             response);
    
    // PLT_SsdpDeviceAnnounceTask & PLT_SsdpDeviceAnnounceUnicastTask
    virtual NPT_Result Announce(PLT_DeviceData*  device, 
                                NPT_HttpRequest& request, 
                                NPT_UdpSocket&   socket, 
                                bool             byebye);

    virtual NPT_Result Announce(NPT_HttpRequest& request, 
                                NPT_UdpSocket&   socket, 
                                bool             byebye) {
        return Announce(this, request, socket, byebye);
    }

    // PLT_SsdpPacketListener method
    virtual NPT_Result OnSsdpPacket(NPT_HttpRequest&              request, 
                                    const NPT_HttpRequestContext& context);

    // PLT_SsdpDeviceSearchListenTask
    virtual NPT_Result ProcessSsdpSearchRequest(NPT_HttpRequest&              request, 
                                                const NPT_HttpRequestContext& context);

    // PLT_SsdpDeviceSearchResponseTask
    virtual NPT_Result SendSsdpSearchResponse(PLT_DeviceData*   device, 
                                              NPT_HttpResponse& response, 
                                              NPT_UdpSocket&    socket, 
                                              const char*       st,
                                              const NPT_SocketAddress* addr  = NULL);
    virtual NPT_Result SendSsdpSearchResponse(NPT_HttpResponse& response, 
                                              NPT_UdpSocket&    socket, 
                                              const char*       ST,
                                              const NPT_SocketAddress* addr = NULL) {
        return SendSsdpSearchResponse(this, response, socket, ST, addr);
    }
    
protected:
    virtual ~PLT_DeviceHost();
    
    virtual NPT_Result Start(PLT_SsdpListenTask* task);
    virtual NPT_Result Stop(PLT_SsdpListenTask* task);

    virtual NPT_Result ProcessHttpPostRequest(NPT_HttpRequest&              request,
                                              const NPT_HttpRequestContext& context,
                                              NPT_HttpResponse&             response);

    virtual NPT_Result ProcessHttpSubscriberRequest(NPT_HttpRequest&              request,
                                                    const NPT_HttpRequestContext& context,
                                                    NPT_HttpResponse&             response);

protected:
    friend class PLT_UPnP;
    friend class PLT_UPnP_DeviceStartIterator;
    friend class PLT_UPnP_DeviceStopIterator;
    friend class PLT_Service;
    friend class NPT_Reference<PLT_DeviceHost>;

private:
    PLT_TaskManager                   m_TaskManager;
    PLT_HttpServer*                   m_HttpServer;
    bool                              m_Broadcast;
    NPT_UInt16                        m_Port;
    NPT_List<NPT_HttpRequestHandler*> m_RequestHandlers;
};

typedef NPT_Reference<PLT_DeviceHost> PLT_DeviceHostReference;

#endif /* _PLT_DEVICE_HOST_H_ */
