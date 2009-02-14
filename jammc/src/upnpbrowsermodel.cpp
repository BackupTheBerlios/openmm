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
#include "upnpbrowsermodel.h"

#include <QtDebug>

UpnpBrowserModel::UpnpBrowserModel(QObject *parent)
: QAbstractItemModel(parent)
{
    m_charEncoding = QTextCodec::codecForName("UTF-8");
    m_root = new UpnpObject();
}


UpnpBrowserModel::~UpnpBrowserModel()
{
}


int
UpnpBrowserModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << "UpnpBrowserModel::rowCount() parent objectId:" << getObject(parent)->m_objectId.c_str();
    return getObject(parent)->m_children.size();

}


int
UpnpBrowserModel::columnCount(const QModelIndex& /*parent*/) const
{;
    return 1;
}


bool
UpnpBrowserModel::hasChildren(const QModelIndex &parent) const
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::hasChildren() parent objectId:" << object->m_objectId.c_str();
    if (!parent.isValid())
        return (object->m_children.size() > 0);
    return object->isContainer();
}


bool
UpnpBrowserModel::canFetchMore(const QModelIndex &parent) const
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::canFetchMore() parent objectId:" << object->m_objectId.c_str();
    if (object == m_root && m_root->m_children.size() == 0) {
        return false;
    }
    return (!object->m_fetchedChildren);
}


void
UpnpBrowserModel::fetchMore(const QModelIndex &parent)
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::fetchMore() parent objectId:" << object->m_objectId.c_str();
    if (object->m_fetchedChildren)
        return;
    object->fetchChildren();
    object->m_fetchedChildren = true;
}


QVariant
UpnpBrowserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
    if (role != Qt::DisplayRole)
        return QVariant();
    
    UpnpObject* object = getObject(index);
//     qDebug() << "UpnpBrowserModel::data() objectId reference:" << object;
    return m_charEncoding->toUnicode(object->getTitle().c_str());
}


QModelIndex
UpnpBrowserModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    UpnpObject* object = getObject(index);
    if (!object->m_parent || object->m_parent == m_root)
        return QModelIndex();

    UpnpObject* grandp = object->m_parent->m_parent;
    if (!grandp)
        return QModelIndex();

    vector<UpnpObject*>::iterator row;
    row = find(grandp->m_children.begin(), grandp->m_children.end(), object->m_parent);
    if (row != grandp->m_children.end()) {
        return createIndex(row - grandp->m_children.begin(), 0, object->m_parent);
    }

    return QModelIndex();
}


QModelIndex
UpnpBrowserModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    UpnpObject* object = getObject(parent);
    return createIndex(row, column, (void*)(object->m_children[row]));
}


Qt::ItemFlags
UpnpBrowserModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
UpnpBrowserModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                             int /*role*/) const
{
//     if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//     }
    return "";
}


void
UpnpBrowserModel::serverAddedRemoved(UpnpServer* server, bool add)
{
    qDebug() << "UpnpBrowserModel::serverAddedRemoved()" << (add?"add":"remove") << "server:" << server->getFriendlyName().c_str();
    
    if (add) {
        UpnpObject* d = new UpnpObject();
        d->m_objectId = "0";
        d->m_server = server;
        m_root->m_children.push_back(d);
    }
    else {
    // TODO: delete the UpnpObject tree of server recursively to avoid a memory leak
        vector<UpnpObject*>::iterator i = m_root->m_children.begin();
        while ((*i)->m_server != server && i != m_root->m_children.end()) {
            ++i;
        }
        m_root->m_children.erase(i);
    }
    // TODO: do a more selective update of the BrowserModel and don't reset the whole model.
    reset();
}

/*
---------------------------------------------------------------------------------------
*/




// TODO: optimize: data should use the browse result of previous browse(directChildren)
//       and not browse each item again.
// QVariant
// UpnpBrowserModel::data(const QModelIndex &index, int role) const
// {
// //     qDebug() << "UpnpBrowserModel::data() role:" << role;
//     
//     if (!index.isValid())
//         return QVariant();
// 
//     if (index.internalPointer() == NULL) {
//         qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
//         return QVariant();
//     }
//     
// //     qDebug() << "UpnpBrowserModel::data() objectId reference:" << index.internalPointer();
//     if (role != Qt::DisplayRole)
//         return QVariant();
//     
//     ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
//     qDebug() << "UpnpBrowserModel::data() objectId:" << (char*) objectRef->objectId;
//     if (objectRef->objectId == "0") {
// //         qDebug() << "UpnpBrowserModel::data() presentation url:" << (char*) objectRef->server->m_PresentationURL;
//         return m_charEncoding->toUnicode(objectRef->server->GetFriendlyName());
//     }
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
//     PLT_MediaObject* object = (*browseResults->GetFirstItem());
// 
// //     qDebug() << "UpnpBrowserModel::data() returns:" << object->m_Title.GetChars();
//     return m_charEncoding->toUnicode(object->m_Title.GetChars());
// }


// Qt::ItemFlags
// UpnpBrowserModel::flags(const QModelIndex &index) const
// {
// //     qDebug() << "UpnpBrowserModel::flags()";
//     if (!index.isValid())
//         return 0;
// 
//     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
// }


// QVariant
// UpnpBrowserModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
//                              int /*role*/) const
// {
// //     if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
// //     }
//     return "";
// }


// QModelIndex
// UpnpBrowserModel::index(int row, int column,
//                         const QModelIndex &parent) const
// {
// //     qDebug() << "UpnpBrowserModel::index()";
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         PLT_DeviceMap mediaServers = m_mediaBrowser->getMediaServers();
//         PLT_DeviceDataReference server = (*mediaServers.GetEntries().GetItem(row))->GetValue();
//         return createIndex(row, column, m_mediaBrowser->getInternalPointer(server, "0"));
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     qDebug() << "UpnpBrowserModel::index() objectId:" << (char*) objectRef->objectId << "row:" << row << "col:" << column;
//     
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, false, browseResults);
//     if(!browseResults.IsNull()) {
//         PLT_MediaObject* childObject = (*browseResults->GetItem(row));
//         if (childObject) {
//             return createIndex(row, column, m_mediaBrowser->getInternalPointer(objectRef->server, childObject->m_ObjectID));
//         }
//     }
//     return QModelIndex();
// }


// QModelIndex
// UpnpBrowserModel::parent(const QModelIndex &index) const
// {
// //     qDebug() << "UpnpBrowserModel::parent()";
//     
//     // TODO: optimize this, as we need currently three (!) browse requests to determine parent's row.
//     //       the difficulty is to determine the row of the parent (for index creation).
//     if (!index.isValid())
//         return QModelIndex();
// 
//     if (index.internalPointer() == NULL) {
//         qWarning() << "UpnpBrowserModel::parent() objectId reference is NULL:";
//         return QModelIndex();
//     }
// //     qDebug() << "UpnpBrowserModel::parent() objectId reference:" << index.internalPointer();
//     
//     ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
//     qDebug() << "UpnpBrowserModel::parent() objectId:" << (char*) objectRef->objectId;
//     if (objectRef->objectId == "0") {
//         return QModelIndex();
//     }
//     
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
//     PLT_MediaObject* object = *browseResults->GetFirstItem();
//     NPT_String parentObjectId = object->m_ParentID;
//     
//     if (parentObjectId == "0") {
//         typedef NPT_Map<NPT_String, PLT_DeviceDataReference>::Entry NptMapEntry;
//         
//         PLT_DeviceMap mediaServers = m_mediaBrowser->getMediaServers();
//         NPT_List<NptMapEntry*>::Iterator server = mediaServers.GetEntries().GetFirstItem();
//         int row = 0;
//         while (server) {
//             PLT_DeviceDataReference serverDevice = (*server)->GetValue();
//             if (serverDevice == objectRef->server) {
//                     break;
//             }
//             ++server; ++row;
//         }
//         return createIndex(row, 0, m_mediaBrowser->getInternalPointer(objectRef->server, "0"));
//     }
//     
//     m_mediaBrowser->syncBrowse(objectRef->server, parentObjectId, true, browseResults);
//     object = (*browseResults->GetFirstItem());
//     NPT_String grandParentObjectId = object->m_ParentID;
//     
//     // this is the crucial part: we need to obtain the row of our parameter QModelIndex index.
//     // it needs a full blown directChildren browse request, fetchMore() is not possible here ...?
//     // 1. do a while (canFetchMore()) { ... fetchMore() ..,} until row is found
//     // 2. build a complete Objectreference* tree with fetchMore()
//     m_mediaBrowser->syncBrowse(objectRef->server, grandParentObjectId, false, browseResults);
//     NPT_List<PLT_MediaObject*>::Iterator item = browseResults->GetFirstItem();
//     int row = 0;
//     while (item) {
//         if ((*item)->m_ObjectID == parentObjectId) {
//             break;
//         }
//         ++item; ++row;
//     }
//     
//     return createIndex(row, 0, m_mediaBrowser->getInternalPointer(objectRef->server, parentObjectId));
// }


// int
// UpnpBrowserModel::rowCount(const QModelIndex &parent) const
// {
// //     qDebug() << "UpnpBrowserModel::rowCount()";
//     
//     // TODO: optimize this to browse only one (or first slice) of objects and ask for "TotalMatches"
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         return m_mediaBrowser->getMediaServers().GetEntryCount();
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     qDebug() << "UpnpBrowserModel::rowCount() objectId:" << (char*) objectRef->objectId;
//     
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
//     if(!browseResults.IsNull()) {
//         // get number of chidren
//         PLT_MediaObject* object = (*browseResults->GetFirstItem());
//         if (object->IsContainer()) {
//             // non lazy fetch:
//             PLT_MediaContainer* container = static_cast<PLT_MediaContainer*>(object);
//             return container->m_ChildrenCount;
// 
//             // lazy fetch:
//             //return 0;
//         }
//         else { // object is MediaItem and not a MediaContainer
//             return 0;
//         }
//     } 
//     return 0;
// }


// int
// UpnpBrowserModel::columnCount(const QModelIndex &parent) const
// {
// //     qDebug() << "UpnpBrowserModel::columnCount()";
//         // TODO: put these lines in a helper method ...
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         return 1;
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     
// //     qDebug() << "UpnpBrowserModel::columnCount() objectId:" << (char*) objectId;
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
//     // ... up to here.
//     PLT_MediaObject* object = (*browseResults->GetFirstItem());
//     
//     if (object->IsContainer()) {
// //         qDebug() << "UpnpBrowserModel::columnCount() columns: 1";
//         return 1;
//     }
//     else {
// //         qDebug() << "UpnpBrowserModel::columnCount() columns: 0";
//         return 0;
//     }
// }

/*
In your model overload hasChildren(), canFetchMore() and fetchMore().
hasChildren() should return true for those index's that have children even if the rowCount() is currently 0.
The default implementation will return true if rowCount() is greater than 0.
QTreeView uses hasChildren() and not rowCount() to determine if it should draw the little triangle/plus symbol to expand that index.
When the user clicks to expand the index QTreeView will call "if (canFetchMore(parent)) fetchMore(parent);"
Taking advantage of this inside the model's fetchMore() implementation, the node can now be populated.
*/

// bool
// UpnpBrowserModel::hasChildren(const QModelIndex & parent) const
// {
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         return m_mediaBrowser->getMediaServers().GetEntryCount() > 0;
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     qDebug() << "UpnpBrowserModel::+++++++++++++++++ hasChildren() objectId:" << (char*) objectRef->objectId;
//     
//     PLT_MediaObjectListReference browseResults;
//     m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
//     if(!browseResults.IsNull()) {
//         PLT_MediaObject* object = (*browseResults->GetFirstItem());
//         if (object->IsContainer()) {
//             return true;
//         }
//     }
//     return false;
// }


// TODO: replace all syncBrowse(..., false, browseResults) above with fetchMore() calls
//       and store somewhere the information if data was all fetched or not for this index.
//       browse must be performed for the specified index "parent".
// void
// UpnpBrowserModel::fetchMore(const QModelIndex & parent)
// {
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         return;
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     qDebug() << "UpnpBrowserModel::+++++++++++++++++ fetchMore() objectId:" << (char*) objectRef->objectId;
// }


// bool
// UpnpBrowserModel::canFetchMore(const QModelIndex & parent) const
// {
//     ObjectReference* objectRef;
//     if (!parent.isValid()) {
//         return false;
//     }
//     else {
//         objectRef = static_cast<ObjectReference*>(parent.internalPointer());
//     }
//     qDebug() << "UpnpBrowserModel::+++++++++++++++++ canFetchMore() objectId:" << (char*) objectRef->objectId;
//     return false;
// }


// void
// UpnpBrowserModel::serverAddedRemoved(string uuid, bool add)
// {
//     qDebug() << "UpnpBrowserModel::serverAddedRemoved()" << (add?"add":"remove") << "server:" << uuid.c_str();
//     
//     // TODO: do a more selective update of the BrowserModel and don't reset the whole model.
//     m_mediaBrowser->clearCache();
//     reset();
// }
