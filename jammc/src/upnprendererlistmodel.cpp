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

#include <algorithm>
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
//     qDebug() << "UpnpRendererListModel::data() role:" << role;
    
    if (!index.isValid())
        return QVariant();
    
    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpRendererListModel::data() objectId reference is NULL:";
        return QVariant();
    }
    
//     qDebug() << "UpnpRendererListModel::data() renderer reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
/*    QString renderer = *static_cast<QString*>(index.internalPointer());*/
//     qDebug() << "UpnpRendererListModel::data() returns:" << (char*) renderer->GetFriendlyName();
//     return m_charEncoding->toUnicode(renderer);
//     qRegisterMetaType<string>("string");
    return QString(static_cast<string*>(index.internalPointer())->c_str());
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
//     qDebug() << "UpnpRendererListModel::index()";
    return createIndex(row, column, m_rendererList.at(row));
}


QModelIndex
UpnpRendererListModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}


int
UpnpRendererListModel::rowCount(const QModelIndex &/*parent*/) const
{
//     qDebug() << "UpnpRendererListModel::rowCount()";
    
    return m_rendererList.size();
}


int
UpnpRendererListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}


void
UpnpRendererListModel::rendererAddedRemoved(string uuid, bool add)
{
    qDebug() << "UpnpRendererListModel::rendererAddedRemoved()" << (add?"add":"remove") << "renderer:" << uuid.c_str();
    
    if (add) {
        beginInsertRows(QModelIndex(), m_rendererList.size(), m_rendererList.size());
        m_rendererList.push_back(new string(uuid));
        endInsertRows();
    }
    else {
        vector<string*>::iterator i = m_rendererList.begin();
        while (**i != uuid && i != m_rendererList.end()) ++i;
        if (i == m_rendererList.end()) {
            qDebug() << "UpnpRendererListModel::rendererAddedRemoved() renderer not found";
            return;
        }
        int n = i - m_rendererList.begin();
        qDebug() << "UpnpRendererListModel::rendererAddedRemoved() remove nr:" << n;
        beginRemoveRows(QModelIndex(), n, n);
        qDebug() << "UpnpRendererListModel::rendererAddedRemoved() erase";
        m_rendererList.erase(i);
        qDebug() << "UpnpRendererListModel::rendererAddedRemoved() end remove";
        endRemoveRows();
    }
    
    // TODO: need to reset the whole model?
//     reset();
    
}
