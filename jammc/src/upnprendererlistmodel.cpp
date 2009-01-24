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
#include "upnprendererlistmodel.h"

#include <QtDebug>

UpnpRendererListModel::UpnpRendererListModel(UpnpController* mediaController, QObject *parent)
: QAbstractItemModel(parent), m_mediaController(mediaController)
{
    m_charEncoding = QTextCodec::codecForName("UTF-8");
}


UpnpRendererListModel::~UpnpRendererListModel()
{
}


QVariant
UpnpRendererListModel::data(const QModelIndex &index, int role) const
{
    qDebug() << "UpnpRendererListModel::data() role:" << role;
    
    if (!index.isValid())
        return QVariant();
    
    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpRendererListModel::data() objectId reference is NULL:";
        return QVariant();
    }
    
//     qDebug() << "UpnpRendererListModel::data() objectId reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
/*    ObjectReference* objectRef = static_cast<ObjectReference*>(index.internalPointer());
    if (objectRef->objectId == "0") {
//         qDebug() << "UpnpRendererListModel::data() presentation url:" << (char*) objectRef->server->m_PresentationURL;
        return m_charEncoding->toUnicode(objectRef->server->GetFriendlyName());
    }
    PLT_MediaObjectListReference browseResults;
    m_mediaBrowser->syncBrowse(objectRef->server, objectRef->objectId, true, browseResults);
    PLT_MediaObject* object = (*browseResults->GetFirstItem());
    
//     qDebug() << "UpnpRendererListModel::data() returns:" << object->m_Title.GetChars();
    return m_charEncoding->toUnicode(object->m_Title.GetChars());*/
}


Qt::ItemFlags
UpnpRendererListModel::flags(const QModelIndex &index) const
{
//     qDebug() << "UpnpRendererListModel::flags()";
    if (!index.isValid())
        return 0;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
UpnpRendererListModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                             int /*role*/) const
{
//     if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//     }
    return "";
}


QModelIndex
UpnpRendererListModel::index(int row, int column,
                        const QModelIndex &parent) const
{
    qDebug() << "UpnpRendererListModel::index()";

//     return createIndex(row, column, m_mediaController->m_mediaRenderers.);
//     return QModelIndex();
}


QModelIndex
UpnpRendererListModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}


int
UpnpRendererListModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << "UpnpRendererListModel::rowCount()";
    
    return m_mediaController->m_mediaRenderers.GetEntryCount();
}


int
UpnpRendererListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
