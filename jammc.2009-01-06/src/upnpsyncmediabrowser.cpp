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
PLT_SyncMediaBrowser(ctrlPoint, use_cache, listener)
{
}


NPT_Result
UpnpSyncMediaBrowser::syncBrowse(PLT_DeviceDataReference& device,
                      const char* object_id,
                      PLT_MediaObjectListReference& list,
                      bool                     browse_metadata,
                      NPT_Int32                index,
                      NPT_Int32                count,
                      const char*              filter,
                      const char*              sort)
{
//     qDebug() << "UpnpSyncMediaBrowser::syncBrowse() object_id:" << object_id;
    
    NPT_Result res = NPT_FAILURE;
    
        // reset output params
    list = NULL;
    
        // look into cache first
//     if (m_UseCache && NPT_SUCCEEDED(m_Cache.Get(device, object_id, list))) return NPT_SUCCESS;
    
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
            qDebug() << "UpnpSyncMediaBrowser::syncBrowse() failed";
            break;
        }
        
        if (NPT_FAILED(browse_data->res)) {
            qDebug() << "UpnpSyncMediaBrowser::syncBrowse() failed on browse_data";
            return browse_data->res;
        }
        
        if (browse_data->info.items->GetItemCount() == 0) {
            qDebug() << "UpnpSyncMediaBrowser::syncBrowse() failed, no objects found";
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
//     if (m_UseCache && NPT_SUCCEEDED(res) & !list.IsNull()) {
//         m_Cache.Put(device, object_id, list);
// }

    return res;
}


NPT_String*
UpnpSyncMediaBrowser::getInternalPointer(NPT_String objectId)
{
    NPT_String* res;
    if (!m_objectIdMap.HasKey(objectId)) {
        res = new NPT_String(objectId);
        m_objectIdMap.Put(objectId, res);
    }
    else {
        res = m_objectIdMap[objectId];
    }
    return res;
}
