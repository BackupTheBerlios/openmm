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

#include <platinum/PltSyncMediaBrowser.h>

class UpnpSyncMediaBrowser : public PLT_SyncMediaBrowser
{
public:
    UpnpSyncMediaBrowser(PLT_CtrlPointReference& ctrlPoint, bool use_cache = false, PLT_MediaContainerChangesListener* listener = NULL);
    
    NPT_Result syncBrowse(PLT_DeviceDataReference& device,
                          const char* object_id,
                          PLT_MediaObjectListReference& list,
                          bool                     browse_metadata = false,
                          NPT_Int32                index = 0, 
                          NPT_Int32                count = 1024, // TODO: 0 means "get all" ...
                          const char*              filter = "*", 
                          const char*              sort = "");
    
    NPT_String* getInternalPointer(NPT_String objectId);
    
private:
    NPT_Map<NPT_String, NPT_String*> m_objectIdMap;
};


#endif //UPNPSYNCMEDIABROWSER_H