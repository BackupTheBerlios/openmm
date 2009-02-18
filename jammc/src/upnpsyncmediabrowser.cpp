/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
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


UpnpDevice::UpnpDevice()
: m_pltDevice(NULL)
{
}


string
UpnpDevice::getFriendlyName()
{
    return string((const char*)m_pltDevice->GetFriendlyName());
}


UpnpServer::UpnpServer()
: UpnpDevice(),
m_pltBrowser(NULL)
{
}


UpnpBrowseResult
UpnpServer::browseChildren(UpnpObject* object, int index, int count, string filter, string sort)
{
    qDebug() << "UpnpServer::browseChildren() objectId:" << object->m_objectId.c_str();
    UpnpBrowseResult res;
    
    if (m_pltDevice.IsNull()) {
        qWarning() << "UpnpServer::browseChildren() no mediaServer found";
        return res;
    }
    
    qDebug() << "UpnpServer::browseChildren() send browse packet";
    // TODO: send out a singal when starting browsing and one when finished
    //       so that ControlerGui can inform the user that browsing activity
    //       takes place.
    // FIXME: shouldn't the server reply with only one answer? Is do-while-loop necessary?
//     do {	
        PLT_BrowseDataReference pltBrowseData(new PLT_BrowseData());
        pltBrowseData->shared_var.SetValue(0);
        // send off the browse packet.  Note that this will
        // not block.  There is a call to WaitForResponse in order
        // to block until the response comes back.
        NPT_Result nptRes = m_pltBrowser->Browse(m_pltDevice,
                                                 object->m_objectId.c_str(),
                                                 NPT_UInt32(index),
                                                 NPT_UInt32(count),
                                                 false,
                                                 filter.c_str(),
                                                 sort.c_str(),
                                                 new PLT_BrowseDataReference(pltBrowseData));
        pltBrowseData->shared_var.WaitUntilEquals(1, 30000);
        if (NPT_FAILED(nptRes)) {
            qWarning() << "UpnpServer::browseChildren() **FAILED**, parent objectId:" << object->m_objectId.c_str();
//             break;
            return res;
        }
        if (NPT_FAILED(pltBrowseData->res)) {
            qWarning() << "UpnpServer::browseChildren() **FAILED** to browse_data, parent objectId:" << object->m_objectId.c_str();
            return res;
        }
        if (pltBrowseData->info.items->GetItemCount() == 0) {
            qWarning() << "UpnpServer::browseChildren() **FAILED**, no objects found, parent objectId:" << object->m_objectId.c_str();
//             break;
            return res;
        }
        
        qDebug() << "UpnpServer::browseChildren() number of objects found:" << pltBrowseData->info.items->GetItemCount();
        // build a vector<UpnpObject*> out of pltBrowseData->info.items
        // info.items is of type PLT_MediaObjectListReference
        NPT_List<PLT_MediaObject*>::Iterator i = pltBrowseData->info.items->GetFirstItem();
        while (i) {
            UpnpObject* o = new UpnpObject();
            o->m_objectId = string((const char*)(*i)->m_ObjectID);
            o->m_server = object->m_server;
            o->m_parent = object;
            o->m_pltObject = (*i);
            res.m_result.push_back(o);
            ++i;
        }
        // clear the list items so that the data inside is not
        // cleaned up by PLT_MediaItemList dtor since we copied
        // each pointer into the new list.
        pltBrowseData->info.items->Clear();
        // stop now if our list contains exactly what the server said it had
        // FIXME: info.tm should == res.size only if parameter count == 0
        //        if count != 0 it should be res.size() == count
/*        if (pltBrowseData->info.tm && pltBrowseData->info.tm == res.m_result.size())
            break;*/
        // ask for the next chunk of entries
//         index = res.m_result.size();
        // TODO: put the last if clause into the while loop condition
//        } while(1);
    
    res.m_numberReturned = pltBrowseData->info.nr;
    res.m_totalMatches = pltBrowseData->info.tm;
    res.m_updateId = pltBrowseData->info.uid;
    return res;
}


UpnpObject::UpnpObject()
: m_server(NULL),
m_objectId(""),
m_parent(NULL),
m_children(vector<UpnpObject*>()),
m_fetchedAllChildren(false),
m_childCount(0),
m_pltObject(NULL)
{
}


bool
UpnpObject::isContainer()
{
    qDebug() << "UpnpObject::isContainer() objectId:" << m_objectId.c_str();
    if (m_objectId == "0") {
        qDebug() << "UpnpObject::isContainer() return: true";
        return true;
    }
    else {
        qDebug() << "UpnpObject::isContainer() return:" << (m_pltObject->IsContainer() ? "true" : "false");
        return m_pltObject->IsContainer();
    }
}


string
UpnpObject::getTitle()
{
    qDebug() << "UpnpObject::getTitle() objectId:" << m_objectId.c_str();
    if (m_objectId == "0") {
        return m_server->getFriendlyName();
    }
    else {
        return string((const char*)m_pltObject->m_Title.GetChars());
    }
}


int
UpnpObject::fetchChildren()
{
    qDebug() << "UpnpObject::fetchChildren() objectId:" << m_objectId.c_str();
    if (m_server && !m_fetchedAllChildren) {
        UpnpBrowseResult res = m_server->browseChildren(this, m_children.size(), UpnpServer::m_sliceSize);
        // append the list of browsed objects to our children list.
        m_children.insert(m_children.end(), res.m_result.begin(), res.m_result.end());
        // m_totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        if (m_children.size() >= res.m_totalMatches) {
            m_fetchedAllChildren = true;
        }
        m_childCount = res.m_totalMatches;
    }
    return m_childCount;
}


/*bool
UpnpObject::fetchedAllChildren()
{*/
//     return m_fetchedChildren;
//     return m_children.size() >= m_childCount;
    /*
    qDebug() << "UpnpObject::fetchedAllChildren() begin";
    // this is m_root, children are server that are added by 
    // UpnpBrowserModel::serverAddedRemoved(), so nothing to browse here
    if (m_parent == NULL) return true;
    // this is a root of a device, objectId == "0" and m_pltObject == NULL
    if (m_objectId == "0") ;
    bool b = IsContainer();
    if (!b) return true;
    int s = m_children.size();
    NPT_Int32 c = ((PLT_MediaContainer*)m_pltObject)->m_ChildrenCount;
    bool res = (c <= (NPT_Int32)s);
    qDebug() << "UpnpObject::fetchedAllChildren() end";
    return res;
    //    (static_cast<PLT_MediaContainer*>(m_pltObject)->m_ChildrenCount <= (NPT_Int32)m_children.size());
*/
// }
