/*****************************************************************
|
|   Platinum - Service
|
|   Copyright (c) 2004-2008 Sylvain Rebaud
|   Author: Sylvain Rebaud (sylvain@rebaud.com)
|
 ****************************************************************/

#ifndef _PLT_SERVICE_H_
#define _PLT_SERVICE_H_

/*----------------------------------------------------------------------
|    includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "PltEvent.h"
#include "PltArgument.h"
#include "PltStateVariable.h"
#include "PltAction.h"

/*----------------------------------------------------------------------
|    forward declarations
+---------------------------------------------------------------------*/
class PLT_DeviceData;

/*----------------------------------------------------------------------
|    PLT_Service class
+---------------------------------------------------------------------*/
class PLT_Service
{
public:
    // methods
    PLT_Service(PLT_DeviceData* device,
                const char*     type = NULL, 
                const char*     id = NULL);
    ~PLT_Service();
    
    // class methods
    NPT_Result  InitURLs(const char* service_name, const char* device_uuid);
    bool        IsInitted() {
        return (m_ActionDescs.GetItemCount() > 0);
    }

    // static methods
    static bool IsTrue(NPT_String& value) {
        if (value.Compare("1", true)    && 
            value.Compare("true", true) && 
            value.Compare("yes", true)) {
            return false;
        }
        return true;
    }

    // accessor methods
    const NPT_String&   GetSCPDURL()      const   { return m_SCPDURL;     }
    const NPT_String&   GetControlURL()   const   { return m_ControlURL;  }
    const NPT_String&   GetEventSubURL()  const   { return m_EventSubURL; }
    const NPT_String&   GetServiceID()    const   { return m_ServiceID;   }
    const NPT_String&   GetServiceType()  const   { return m_ServiceType; }    
    PLT_DeviceData*     GetDevice()               { return m_Device; }

    // XML
    NPT_Result          GetSCPDXML(NPT_String& xml);
    NPT_Result          SetSCPDXML(const char* xml);
    NPT_Result          GetDescription(NPT_XmlElementNode* parent, NPT_XmlElementNode** service = NULL);

    // State Variables
    NPT_Result          SetStateVariable(const char* name, const char* value, bool publish = true);
    NPT_Result          SetStateVariableRate(const char* name, NPT_TimeInterval rate);
    NPT_Result          IncStateVariable(const char* name, bool publish = true);
    PLT_StateVariable*  FindStateVariable(const char* name);
    bool                IsSubscribable();

    // Actions
    PLT_ActionDesc*     FindActionDesc(const char* name);

private:    
    void                Cleanup();
    NPT_Result          AddChanged(PLT_StateVariable* var);
    NPT_Result          UpdateLastChange(NPT_List<PLT_StateVariable*>& vars);
    NPT_Result          NotifyChanged();


    /*----------------------------------------------------------------------
    |    PLT_ServiceEventTask
    +---------------------------------------------------------------------*/
    class PLT_ServiceEventTask : public PLT_ThreadTask
    {
    public:
        PLT_ServiceEventTask(PLT_Service* service) : m_Service(service) {}

        void DoRun() { 
            while (!IsAborting(10)) m_Service->NotifyChanged();
        }

    private:
        PLT_Service* m_Service;
    };

    // Events
    NPT_Result ProcessNewSubscription(
        PLT_TaskManager*         task_manager,
        const NPT_SocketAddress& addr, 
        const NPT_String&        callback_urls, 
        int                      timeout, 
        NPT_HttpResponse&        response);

    NPT_Result ProcessRenewSubscription(
        const NPT_SocketAddress& addr, 
        const NPT_String&        sid, 
        int                      timeout,
        NPT_HttpResponse&        response);
    
    NPT_Result ProcessCancelSubscription(
        const NPT_SocketAddress& addr, 
        const NPT_String&        sid, 
        NPT_HttpResponse&        response);


protected:
    friend class PLT_StateVariable; // so that we can call AddChanged from StateVariable
    friend class PLT_DeviceHost;
    friend class PLT_DeviceData;
    
    //members
    PLT_DeviceData* m_Device;
    NPT_String      m_ServiceType;
    NPT_String      m_ServiceID;
    NPT_String      m_SCPDURL;
    NPT_String      m_ControlURL;
    NPT_String      m_EventSubURL;
    PLT_ServiceEventTask* m_EventTask;
    
    NPT_Array<PLT_ActionDesc*>      m_ActionDescs;
    NPT_List<PLT_StateVariable*>    m_StateVars;
    NPT_Mutex                       m_Lock;
    NPT_List<PLT_StateVariable*>    m_StateVarsChanged;
    NPT_List<PLT_StateVariable*>    m_StateVarsToPublish;
    NPT_List<PLT_EventSubscriber*>  m_Subscribers;
};

/*----------------------------------------------------------------------
|    PLT_ServiceDescriptionURLFinder
+---------------------------------------------------------------------*/
class PLT_ServiceSCPDURLFinder
{
public:
    // methods
    PLT_ServiceSCPDURLFinder(const char* url) : m_URL(url) {}
    virtual ~PLT_ServiceSCPDURLFinder() {}
    bool operator()(PLT_Service* const & service) const;

private:
    // members
    NPT_String m_URL;
};

/*----------------------------------------------------------------------
|    PLT_ServiceControlURLFinder
+---------------------------------------------------------------------*/
class PLT_ServiceControlURLFinder
{
public:
    // methods
    PLT_ServiceControlURLFinder(const char* url) : m_URL(url) {}
    virtual ~PLT_ServiceControlURLFinder() {}
    bool operator()(PLT_Service* const & service) const;

private:
    // members
    NPT_String m_URL;
};

/*----------------------------------------------------------------------
|    PLT_ServiceEventSubURLFinder
+---------------------------------------------------------------------*/
class PLT_ServiceEventSubURLFinder
{
public:
    // methods
    PLT_ServiceEventSubURLFinder(const char* url) : m_URL(url) {}
    virtual ~PLT_ServiceEventSubURLFinder() {}
    bool operator()(PLT_Service* const & service) const;

private:
    // members
    NPT_String m_URL;
};

/*----------------------------------------------------------------------
|    PLT_ServiceIDFinder
+---------------------------------------------------------------------*/
class PLT_ServiceIDFinder
{
public:
    // methods
    PLT_ServiceIDFinder(const char* id) : m_Id(id) {}
    virtual ~PLT_ServiceIDFinder() {}
    bool operator()(PLT_Service* const & service) const;

private:
    // members
    NPT_String m_Id;
};

/*----------------------------------------------------------------------
|    PLT_ServiceTypeFinder
+---------------------------------------------------------------------*/
class PLT_ServiceTypeFinder
{
public:
    // methods
    PLT_ServiceTypeFinder(const char* type) : m_Type(type) {}
    virtual ~PLT_ServiceTypeFinder() {}
    bool operator()(PLT_Service* const & service) const;

private:
    // members
    NPT_String m_Type;
};

/*----------------------------------------------------------------------
|    PLT_LastChangeXMLIterator
+---------------------------------------------------------------------*/
class PLT_LastChangeXMLIterator
{
public:
    // methods
    PLT_LastChangeXMLIterator(NPT_XmlElementNode* node) : m_Node(node) {}
    virtual ~PLT_LastChangeXMLIterator() {}

    NPT_Result operator()(PLT_StateVariable* const & var) const;

private:
    NPT_XmlElementNode* m_Node;
};

#endif /* _PLT_SERVICE_H_ */
