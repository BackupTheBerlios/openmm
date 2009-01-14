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
// #include <platinum/NptMap.h>

UpnpBrowserModel::UpnpBrowserModel(UpnpSyncMediaBrowser* mediaBrowser, QObject *parent)
: QAbstractItemModel(parent), m_mediaBrowser(mediaBrowser)
{
    m_charEncoding = QTextCodec::codecForName("UTF-8");
}


UpnpBrowserModel::~UpnpBrowserModel()
{
}


QVariant
UpnpBrowserModel::data(const QModelIndex &index, int role) const
{
//     qDebug() << "UpnpBrowserModel::data() role:" << role;
    
    if (!index.isValid())
        return QVariant();

    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
    
//     qDebug() << "UpnpBrowserModel::data() objectId reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
    ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
    if (objectRef->objectId == "0") {
//         qDebug() << "UpnpBrowserModel::data() presentation url:" << (char*) objectRef->server->m_PresentationURL;
        return m_charEncoding->toUnicode(objectRef->server->GetFriendlyName());
    }
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());

//     qDebug() << "UpnpBrowserModel::data() returns:" << object->m_Title.GetChars();
    return m_charEncoding->toUnicode(object->m_Title.GetChars());
}


Qt::ItemFlags
UpnpBrowserModel::flags(const QModelIndex &index) const
{
//     qDebug() << "UpnpBrowserModel::flags()";
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


QModelIndex
UpnpBrowserModel::index(int row, int column,
                        const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::index()";
    ObjectReference* objectRef;
    if (!parent.isValid()) {
        PLT_DeviceMap mediaServers = m_mediaBrowser->GetMediaServers();
        PLT_DeviceDataReference server = (*mediaServers.GetEntries().GetItem(row))->GetValue();
//         PLT_DeviceDataReference server = (*mediaServers.GetEntries().GetFirstItem())->GetValue();
        return createIndex(row, column, m_mediaBrowser->getInternalPointer(server, "0"));
    }
    else {
        objectRef = static_cast<ObjectReference*>(parent.internalPointer());
    }
    
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, false, browseResults);
    if(!browseResults.IsNull()) {
        PLT_MediaObject* childObject = (*browseResults->GetItem(row));
        if (childObject) {
            return createIndex(row, column, m_mediaBrowser->getInternalPointer(objectRef->server, childObject->m_ObjectID));
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
        qWarning() << "UpnpBrowserModel::parent() objectId reference is NULL:";
        return QModelIndex();
    }
//     qDebug() << "UpnpBrowserModel::parent() objectId reference:" << index.internalPointer();
    
    ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
    if (objectRef->objectId == "0") {
        return QModelIndex();
    }
    
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());
    NPT_String parentObjectId = object->m_ParentID;
    
    if (parentObjectId == "0") {
        typedef NPT_Map<NPT_String, PLT_DeviceDataReference>::Entry NptMapEntry;
        
        PLT_DeviceMap mediaServers = m_mediaBrowser->GetMediaServers();
        NPT_List<NptMapEntry*>::Iterator server = mediaServers.GetEntries().GetFirstItem();
        int row = 0;
        while (server) {
            PLT_DeviceDataReference serverDevice = (*server)->GetValue();
            if (serverDevice == objectRef->server) {
                    break;
            }
            ++server; ++row;
        }
        return createIndex(row, 0, m_mediaBrowser->getInternalPointer(objectRef->server, "0"));
    }
    
    m_mediaBrowser->syncBrowse(objectRef->server, parentObjectId, true, browseResults);
    object = (*browseResults->GetFirstItem());
    NPT_String grandParentObjectId = object->m_ParentID;
    
    m_mediaBrowser->syncBrowse(objectRef->server, grandParentObjectId, false, browseResults);
    NPT_List<PLT_MediaObject*>::Iterator item = browseResults->GetFirstItem();
    int row = 0;
    while (item) {
        if ((*item)->m_ObjectID == parentObjectId) {
            break;
        }
        ++item; ++row;
    }
    
    return createIndex(row, 0, m_mediaBrowser->getInternalPointer(objectRef->server, parentObjectId));
}


int
UpnpBrowserModel::rowCount(const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::rowCount()";
    
    // TODO: optimize this to browse only one (or first slice) of objects and ask for "TotalMatches"
    
    ObjectReference* objectRef;
    if (!parent.isValid()) {
        return m_mediaBrowser->GetMediaServers().GetEntryCount();
    }
    else {
        objectRef = static_cast<ObjectReference*>(parent.internalPointer());
    }
//     qDebug() << "UpnpBrowserModel::rowCount() objectId:" << (char*) objectId;
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, false, browseResults);
    if(!browseResults.IsNull()) {
        return browseResults->GetItemCount();
    } 
    return 0;
}


int
UpnpBrowserModel::columnCount(const QModelIndex &parent) const
{
//     qDebug() << "UpnpBrowserModel::columnCount()";
        // TODO: put these lines in a helper method ...
    ObjectReference* objectRef;
    if (!parent.isValid()) {
        return 1;
    }
    else {
        objectRef = static_cast<ObjectReference*>(parent.internalPointer());
    }
    
//     qDebug() << "UpnpBrowserModel::columnCount() objectId:" << (char*) objectId;
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
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
}
