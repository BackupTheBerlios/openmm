/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef UPNPSYNCMEDIABROWSER_H
#define UPNPSYNCMEDIABROWSER_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

#include <QObject>
#include <platinum/PltSyncMediaBrowser.h>

class UpnpObject;

class UpnpDevice
{
public:
    UpnpDevice();
    string getFriendlyName();

// protected:
    PLT_DeviceDataReference m_pltDevice;
};


class UpnpServer : public UpnpDevice
{
public:
    UpnpServer();
    // TODO: return reference or pointer to vector to avoid calling vector's copy constructor
    vector<UpnpObject*> browseChildren(UpnpObject* object, int index = 0, int count = 0, string filter = "*", string sort = "");
    
// private:
    PLT_MediaBrowser* m_pltBrowser;
};


class UpnpObject
{
public:
    UpnpObject();

    bool isContainer();
    string getTitle();
    void fetchChildren();

    UpnpServer*         m_server;
    string              m_objectId;
    UpnpObject*         m_parent;
    vector<UpnpObject*> m_children;
    //    int                 m_lastSliceFetched;
    bool                m_fetchedChildren;
    
// private:
    PLT_MediaObject*    m_pltObject;
};


/*
-----------------------------------------------------------------------------------
*/


// class UpnpMediaCache
// {
// public:
//     UpnpMediaCache();
//     virtual ~UpnpMediaCache();
//     
//     NPT_Result Put(PLT_DeviceDataReference& device, const char* item_id, const char* meta_data,
//                    PLT_MediaObjectListReference& list);
//     NPT_Result Get(PLT_DeviceDataReference& device, const char* item_id, const char* meta_data,
//                    PLT_MediaObjectListReference& list);
//     //    NPT_Result Clear(PLT_DeviceData* device = NULL);
//     NPT_Result clear();
//     
// private:
//     NPT_String GenerateKey(const char* device_uuid, const char* item_id, const char* meta_data);
//     map<NPT_String, PLT_MediaObjectListReference> m_items;
// };
// 
// 
// 
// 
// class UpnpSyncMediaBrowser : public QObject, PLT_SyncMediaBrowser
// {
//     Q_OBJECT
//         
// public:
//     UpnpSyncMediaBrowser(PLT_CtrlPointReference& ctrlPoint, bool use_cache = false, PLT_MediaContainerChangesListener* listener = NULL);
//     
//     NPT_Result syncBrowse(PLT_DeviceDataReference& device,
//                           const char*              object_id,
//                           bool                     browse_metadata,
//                           PLT_MediaObjectListReference& list,
//                           NPT_Int32                index = 0, 
//                           NPT_Int32                count = 0, // 0 means "get all" ...
//                           const char*              filter = "*", 
//                           const char*              sort = "");
//     
//     virtual void OnMSAddedRemoved(PLT_DeviceDataReference& device, int added);
//     const PLT_DeviceMap& getMediaServers() const { return GetMediaServers(); }
//     
//     // we need a pointer to server and object_id, to store it in the internal data model of UpnpBrowserModel
//     // see: createIndex(row, col, void*) ...
//     ObjectReference* getInternalPointer(PLT_DeviceDataReference server, NPT_String objectId);
//     
//     void clearCache() { m_objectIdMap.clear(); m_Cache.clear(); }
//     
// signals:
//     void serverAddedRemoved(string uuid, bool add);
//     
// private:
//     map<NPT_String, ObjectReference*>     m_objectIdMap;
//     bool                                  m_UseCache;
//     UpnpMediaCache                        m_Cache;
// };

#endif //UPNPSYNCMEDIABROWSER_H