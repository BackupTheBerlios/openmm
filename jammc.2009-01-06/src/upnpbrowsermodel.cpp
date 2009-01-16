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

UpnpBrowserModel::UpnpBrowserModel(UpnpSyncMediaBrowser* mediaBrowser, QObject *parent)
: QAbstractItemModel(parent), m_mediaBrowser(mediaBrowser)
{
    qDebug() << "UpnpBrowserModel::UpnpBrowserModel()";
}


UpnpBrowserModel::~UpnpBrowserModel()
{
    qDebug() << "UpnpBrowserModel::~UpnpBrowserModel()";
}


QVariant
UpnpBrowserModel::data(const QModelIndex &index, int role) const
{
//     qDebug() << "UpnpBrowserModel::data() role:" << role;
    
    if (!index.isValid())
        return QVariant();

    if (index.internalPointer() == NULL) {
        qDebug() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
    
//     qDebug() << "UpnpBrowserModel::data() objectId reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
    NPT_String objectId = (*static_cast<NPT_String*>(index.internalPointer()));
    PLT_MediaObjectListReference browseResults = doBrowse(objectId, true);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());

//     qDebug() << "UpnpBrowserModel::data() returns:" << object->m_Title.GetChars();
    return object->m_Title.GetChars();
}


Qt::ItemFlags
UpnpBrowserModel::flags(const QModelIndex &index) const
{
//     qDebug() << "UpnpBrowserModel::flags()";
    
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// static const char* title = "Media Object"; 
// 
// QVariant
// UpnpBrowserModel::headerData(int section, Qt::Orientation orientation,
//                              int role) const
// {
//     return &title;
// }


QModelIndex
UpnpBrowserModel::index(int row, int column,
                        const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::index()";
    
    if (!hasIndex(row, column, parent)) {
        qDebug() << "UpnpBrowserModel::index() row" << row << "column" << column << "doesn't have a valid index";
        return QModelIndex();
    }
    
    NPT_String objectId;
    if (!parent.isValid()) {
        objectId = "0";
    }
    else {
        objectId = (*static_cast<NPT_String*>(parent.internalPointer()));
    }
    
    PLT_MediaObjectListReference browseResults = doBrowse(objectId, false);
    if(!browseResults.IsNull()) {
        PLT_MediaObject* childObject = (*browseResults->GetItem(row));
        if (childObject) {
            return createIndex(row, column, m_mediaBrowser->getInternalPointer(childObject->m_ObjectID));
        }
    }
    return QModelIndex();
}


QModelIndex
UpnpBrowserModel::parent(const QModelIndex &index) const
{
//     qDebug() << "UpnpBrowserModel::parent()";
    
    // TODO: optimize this, as we need currently three (!) browse requests to determine parent's row.
    if (!index.isValid())
        return QModelIndex();

    if (index.internalPointer() == NULL) {
        qDebug() << "UpnpBrowserModel::parent() objectId reference is NULL:";
        return QModelIndex();
    }
//     qDebug() << "UpnpBrowserModel::parent() objectId reference:" << index.internalPointer();
    
    NPT_String objectId = (*static_cast<NPT_String*>(index.internalPointer()));
    PLT_MediaObjectListReference browseResults = doBrowse(objectId, true);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());
    NPT_String parentObjectId = object->m_ParentID;
    
    if (parentObjectId == "0")
        return QModelIndex();
    
    browseResults = doBrowse(parentObjectId, true);
    object = (*browseResults->GetFirstItem());
    NPT_String grandParentObjectId = object->m_ParentID;
    
    browseResults = doBrowse(grandParentObjectId, false);
    NPT_List<PLT_MediaObject*>::Iterator item = browseResults->GetFirstItem();
    int row = 0;
    while (item) {
        if ((*item)->m_ObjectID == parentObjectId) {
            break;
        }
        ++item; ++row;
    }
    
    return createIndex(row, 0, m_mediaBrowser->getInternalPointer(parentObjectId));
}


int
UpnpBrowserModel::rowCount(const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::rowCount()";
    
    // TODO: optimize this to browse only one object and ask for "TotalMatches"
    
/*    if (parent.column() > 0)  // TODO: why's that?
        return 0;*/
    
    NPT_String objectId;
    if (!parent.isValid()) {
        objectId = "0";
    }
    else {
        objectId = (*static_cast<NPT_String*>(parent.internalPointer()));
    }
//     qDebug() << "UpnpBrowserModel::rowCount() objectId:" << (char*) objectId;
    PLT_MediaObjectListReference browseResults = doBrowse(objectId, false);
    if(!browseResults.IsNull()) {
//         qDebug() << "UpnpBrowserModel::rowCount() rows:" << browseResults->GetItemCount();
        return browseResults->GetItemCount();
    } 
    else {
//         qDebug() << "UpnpBrowserModel::rowCount() rows: 0";
        return 0;
    }
}


int
UpnpBrowserModel::columnCount(const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::columnCount()";
    
    // TODO: put these lines in a helper method ...
    NPT_String objectId;
    if (!parent.isValid()) {
        objectId = "0";
    }
    else {
        objectId = (*static_cast<NPT_String*>(parent.internalPointer()));
    }
    
//     qDebug() << "UpnpBrowserModel::columnCount() objectId:" << (char*) objectId;
    PLT_MediaObjectListReference browseResults = doBrowse(objectId, true);
    // ... up to here.
    PLT_MediaObject* object = (*browseResults->GetFirstItem());
    
    if (object->IsContainer()) {
//         qDebug() << "UpnpBrowserModel::columnCount() columns: 1";
        return 1;
    }
    else {
//         qDebug() << "UpnpBrowserModel::columnCount() columns: 0";
        return 0;
    }
    // TODO: always return 1 seems also to be correct ...?
}


PLT_MediaObjectListReference
UpnpBrowserModel::doBrowse(NPT_String objectId, bool itemOnly, int index, int count) const
{
    // TODO: put this in UpnpSyncMediaBrowser
//     qDebug() << "UpnpBrowserModel::doBrowse() objectId:" << (char*) objectId;
    
    // TODO: optimize browsing on same item in sequence (e.g. for different rows)
    NPT_Result res = NPT_FAILURE;
    PLT_DeviceDataReference device;
    PLT_MediaObjectListReference browseResults;
    PLT_DeviceMap mediaServers;


    // TODO: get current MediaServer device properly. For now, we take the first in the list.
    mediaServers = m_mediaBrowser->GetMediaServers();
    device = (*mediaServers.GetEntries().GetFirstItem())->GetValue();
    if (device.IsNull()) {
        qDebug() << "UpnpBrowserModel::doBrowse() no mediaServer found";
        
        browseResults = NULL;
        return browseResults;
    }

//     const char *objectId;

//     objectId = static_cast<const char*>(item.internalPointer());
    res = m_mediaBrowser->syncBrowse(device, objectId, browseResults, itemOnly, index, count);

//     qDebug() << "UpnpBrowserModel::doBrowse() number of objects found:" << browseResults->GetItemCount();
    return browseResults;
}
