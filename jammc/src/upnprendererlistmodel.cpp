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
//     NPT_AutoLock lock(m_mediaController->m_mediaRenderers);
    
    if (!index.isValid())
        return QVariant();
    
/*    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpRendererListModel::data() objectId reference is NULL:";
        return QVariant();
    }*/
    
//     qDebug() << "UpnpRendererListModel::data() objectId reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
/*    PLT_DeviceMap::Entry* rendererP = static_cast<PLT_DeviceMap::Entry*>(index.internalPointer());
//     qDebug() << "UpnpRendererListModel::data() returns:" << (char*) renderer->GetFriendlyName();
    return m_charEncoding->toUnicode(rendererP->GetValue()->GetFriendlyName());*/
/*    QString renderer = *static_cast<QString*>(index.internalPointer());*/
//     qDebug() << "UpnpRendererListModel::data() returns:" << (char*) renderer->GetFriendlyName();
//     return m_charEncoding->toUnicode(renderer);
//     qRegisterMetaType<string>("string");
    return m_rendererList.at(index.row());
}


Qt::ItemFlags
UpnpRendererListModel::flags(const QModelIndex &index) const
{
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
                        const QModelIndex &/*parent*/) const
{
    qDebug() << "UpnpRendererListModel::index()";
//     NPT_AutoLock lock(m_mediaController->m_mediaRenderers);
    
//     PLT_DeviceMap::Entry* rendererP = *(m_mediaController->m_mediaRenderers.GetEntries().GetItem(row));
//     return createIndex(row, column, rendererP/*.AsPointer()*/);
//     const QString* rendererP = &(m_rendererList.at(row));
    return createIndex(row, column, NULL);
    
}


QModelIndex
UpnpRendererListModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}


int
UpnpRendererListModel::rowCount(const QModelIndex &/*parent*/) const
{
    qDebug() << "UpnpRendererListModel::rowCount()";
    
//     NPT_AutoLock lock(m_mediaController->m_mediaRenderers);
    
//     return m_mediaController->m_mediaRenderers.GetEntryCount();
    return m_rendererList.size();
}


int
UpnpRendererListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}


// bool
// UpnpRendererListModel::insertRows(int position, int rows, const QModelIndex &/*parent*/)
// {
//     beginInsertRows(QModelIndex(), position, position+rows-1);
//     // data should be inserted here, but this is already done by CtrlPointListener thread ...?
//     endInsertRows();
//     return true;
// }
// 
// 
// bool
// UpnpRendererListModel::removeRows(int position, int rows, const QModelIndex &/*parent*/)
// {
//     beginRemoveRows(QModelIndex(), position, position+rows-1);
//    // data should be removed here, but this is already done by CtrlPointListener thread ...?
//     endRemoveRows();
//     return true;
// }


void
UpnpRendererListModel::rendererAddedRemoved(string uuid, bool add)
{
//     NPT_AutoLock lock(m_mediaController->m_mediaRenderers);
//     qDebug() << "UpnpRendererListModel::rendererAddedRemoved()" << (add?"add":"remove") << "renderer:" << (char*) (*device)->GetUUID();
    qDebug() << "UpnpRendererListModel::rendererAddedRemoved()" << (add?"add":"remove") << "renderer:" << uuid.c_str();
    
//     unsigned int nr;
    
    
    if (add) {
//         nr = m_mediaController->m_mediaRenderers.GetEntryCount();
//         qDebug() << "UpnpRendererListModel::rendererAddedRemoved() add nr:" << nr;
        beginInsertRows(QModelIndex(), 0, 0);
//         m_mediaController->m_mediaRenderers.Put((*device)->GetUUID(), (*device));
        m_rendererList.push_back(QString(uuid.c_str()));
        endInsertRows();
    }
    else {
/*        for (nr = 0; nr < m_mediaController->m_mediaRenderers.GetEntryCount(); ++nr) {
            if ((*m_mediaController->m_mediaRenderers.GetEntries().GetItem(nr))->GetKey() == (*device)->GetUUID()) {
                break;
            }
        }*/
//         nr = 0;
//         qDebug() << "UpnpRendererListModel::rendererAddedRemoved() remove nr:" << nr;
        beginRemoveRows(QModelIndex(), 0, 0);
//         m_mediaController->m_mediaRenderers.Erase((*device)->GetUUID());
//         reset();
        m_rendererList.pop_back();
        endRemoveRows();
    }
    
    // TODO: need to reset the whole model?
//     reset();
    
}
