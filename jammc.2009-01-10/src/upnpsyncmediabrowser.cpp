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
#include "upnpsyncmediabrowser.h"

#include <QtDebug>

UpnpSyncMediaBrowser::UpnpSyncMediaBrowser(PLT_CtrlPointReference& ctrlPoint,
                                           bool                    use_cache /* = false */, 
                                           PLT_MediaContainerChangesListener* listener /* = NULL */) :
PLT_SyncMediaBrowser(ctrlPoint, use_cache, listener), m_UseCache(use_cache)
{
}


NPT_Result
UpnpSyncMediaBrowser::syncBrowse(PLT_DeviceDataReference& device,
                      const char*              object_id,
                      bool                     browse_metadata,
                      PLT_MediaObjectListReference& list,
                      NPT_Int32                index,
                      NPT_Int32                count,
                      const char*              filter,
                      const char*              sort)
{
//     qDebug() << "UpnpSyncMediaBrowser::syncBrowse() object_id:" << object_id;
    
    NPT_Result res = NPT_FAILURE;
    
        // reset output params
    list = NULL;
    
    if (device.IsNull()) {
        qWarning() << "UpnpBrowserModel::syncBrowse() no mediaServer found";
        return res;
    }
    
    //    look into cache first
    if (m_UseCache && NPT_SUCCEEDED(m_Cache.Get(device, object_id, 
                                                browse_metadata?"meta":"children",
                                                list))) {
//         qDebug() << "UpnpSyncMediaBrowser::syncBrowse() cache hit";
        return NPT_SUCCESS;
    }
//     qDebug() << "UpnpSyncMediaBrowser::syncBrowse() send browse packet";
    
    do {	
        PLT_BrowseDataReference browse_data(new PLT_BrowseData());
            // send off the browse packet.  Note that this will
            // not block.  There is a call to WaitForResponse in order
            // to block until the response comes back.
        res = Browse(browse_data,
                    device,
                    (const char*)object_id,
                    index,
                    count,
                    browse_metadata,
                    filter,
                    sort);
        if (NPT_FAILED(res)) {
            qWarning() << "UpnpSyncMediaBrowser::syncBrowse() failed";
            break;
        }
        
        if (NPT_FAILED(browse_data->res)) {
            qWarning() << "UpnpSyncMediaBrowser::syncBrowse() failed on browse_data";
            return browse_data->res;
        }
        
        if (browse_data->info.items->GetItemCount() == 0) {
            qWarning() << "UpnpSyncMediaBrowser::syncBrowse() failed, no objects found";
            break;
        }
        
//         qDebug() << "UpnpSyncMediaBrowser::syncBrowse() number of objects found:" << browse_data->info.items->GetItemCount();
        
        if (list.IsNull()) {
            list = browse_data->info.items;
        } else {
            list->Add(*browse_data->info.items);
                // clear the list items so that the data inside is not
                // cleaned up by PLT_MediaItemList dtor since we copied
                // each pointer into the new list.
            browse_data->info.items->Clear();
        }
        
            // stop now if our list contains exactly what the server said it had
        if (browse_data->info.tm && browse_data->info.tm == list->GetItemCount())
            break;
        
            // ask for the next chunk of entries
        index = list->GetItemCount();
    } while(1);
    
        // cache the result
    if (m_UseCache && NPT_SUCCEEDED(res) & !list.IsNull()) {
        m_Cache.Put(device, object_id, browse_metadata?"meta":"children", list);
    }

    return res;
}


ObjectReference*
UpnpSyncMediaBrowser::getInternalPointer(PLT_DeviceDataReference server, NPT_String objectId)
{
    NPT_String key = server->GetUUID() + "/" + objectId;
    ObjectReference* res;
    if (!m_objectIdMap.HasKey(key)) {
        res = new ObjectReference();
        res->objectId = objectId;
        res->server = server;
        m_objectIdMap.Put(key, res);
    }
    else {
        res = m_objectIdMap[key];
    }
    return res;
}



/*----------------------------------------------------------------------
|   UpnpMediaCache::UpnpMediaCache
+---------------------------------------------------------------------*/
UpnpMediaCache::UpnpMediaCache()
{
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::~UpnpMediaCache
+---------------------------------------------------------------------*/
UpnpMediaCache::~UpnpMediaCache()
{
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::GenerateKey
+---------------------------------------------------------------------*/
NPT_String
UpnpMediaCache::GenerateKey(const char* device_uuid, 
                            const char* item_id,
                            const char* meta_data)
{
    NPT_String key = "upnp://";
    key += device_uuid;
    key += "/";
    key += item_id;
    key += meta_data;
    
    return key;
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::Clear
+---------------------------------------------------------------------*/
NPT_Result
UpnpMediaCache::Clear(PLT_DeviceDataReference& device, 
                      const char*              item_id,
                      const char* meta_data)
{
    NPT_String key = GenerateKey(device->GetUUID(), item_id, meta_data);
    if (key.GetLength() == 0) return NPT_ERROR_INVALID_PARAMETERS;
    
    NPT_List<UpnpMediaCacheEntry*>::Iterator entries = m_Items.GetEntries().GetFirstItem();
    NPT_List<UpnpMediaCacheEntry*>::Iterator entry;
    while (entries) {
        entry = entries++;
        if ((*entry)->GetKey() == (key)) {
            m_Items.Erase(key);
            return NPT_SUCCESS;
        }
    }
    
    return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::Clear
+---------------------------------------------------------------------*/
NPT_Result
UpnpMediaCache::Clear(PLT_DeviceData* device)
{
    if (!device) return m_Items.Clear();
    
    NPT_String key = GenerateKey(device->GetUUID(), "", "");
    NPT_List<UpnpMediaCacheEntry*>::Iterator entries = m_Items.GetEntries().GetFirstItem();
    NPT_List<UpnpMediaCacheEntry*>::Iterator entry;
    while (entries) {
        entry = entries++;
        NPT_String entry_key = (*entry)->GetKey();
        if (entry_key.StartsWith(key)) {
            m_Items.Erase(entry_key);
        }
    }
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::Put
+---------------------------------------------------------------------*/
NPT_Result
UpnpMediaCache::Put(PLT_DeviceDataReference&      device, 
                    const char*                   item_id, 
                    const char*                   meta_data,
                    PLT_MediaObjectListReference& list)
{
    NPT_String key = GenerateKey(device->GetUUID(), item_id, meta_data);
    if (key.GetLength() == 0) return NPT_ERROR_INVALID_PARAMETERS;
    
    m_Items.Erase(key);
    return m_Items.Put(key, list);
}

/*----------------------------------------------------------------------
|   UpnpMediaCache::Get
+---------------------------------------------------------------------*/
NPT_Result
UpnpMediaCache::Get(PLT_DeviceDataReference&      device, 
                    const char*                   item_id, 
                    const char*                   meta_data,
                    PLT_MediaObjectListReference& list)
{
    NPT_String key = GenerateKey(device->GetUUID(), item_id, meta_data);
    if (key.GetLength() == 0) return NPT_ERROR_INVALID_PARAMETERS;
    
    PLT_MediaObjectListReference* val = NULL;
    NPT_CHECK_FINE(m_Items.Get(key, val));
    
    list = *val;
    return NPT_SUCCESS;
}

