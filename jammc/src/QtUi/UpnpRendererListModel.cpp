/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include "UpnpRendererListModel.h"

#include <algorithm>
#include <QtDebug>

UpnpRendererListModel::UpnpRendererListModel(/*UpnpController* mediaController, */QObject *parent)
: QAbstractItemModel(parent)/*, m_mediaController(mediaController)*/
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
        qWarning() << "UpnpRendererListModel::data() reference to renderer device is NULL:";
        return QVariant();
    }
    
//     qDebug() << "UpnpRendererListModel::data() renderer reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
    return QString(static_cast<Jamm::Device*>(index.internalPointer())->getUuid().c_str());
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
UpnpRendererListModel::beginAddRenderer(int position)
{
    qDebug() << "UpnpRendererListModel::beginAddRenderer() at position:" << position;
    
    beginInsertRows(QModelIndex(), position, position);
}


void
UpnpRendererListModel::endAddRenderer()
{
    qDebug() << "UpnpRendererListModel::endAddRenderer()";
    
    endInsertRows();
}

void
UpnpRendererListModel::beginRemoveRenderer(int position)
{
    qDebug() << "UpnpRendererListModel::beginRemoveRenderer() at position:" << position;
    
    beginRemoveRows(QModelIndex(), position, position);
}


void
UpnpRendererListModel::endRemoveRenderer()
{
    qDebug() << "UpnpRendererListModel::endRemoveRenderer()";
    
    endRemoveRows();
}

// void
// UpnpRendererListModel::rendererAdded(MediaRendererController* pRenderer)
// {
//     qDebug() << "UpnpRendererListModel::rendererAdded() renderer:" << pRenderer->getDevice()->getUuid().c_str();
//     
//     beginInsertRows(QModelIndex(), m_rendererList.size(), m_rendererList.size());
//     m_rendererList.push_back(renderer);
//     endInsertRows();
// }
// 
// 
// void
// UpnpRendererListModel::rendererRemoved(MediaRendererController* pRenderer)
// {
//     qDebug() << "UpnpRendererListModel::rendererRemoved() renderer:" << pRenderer->getDevice()->getUuid().c_str();
//     
//     vector<Jamm::Device*>::iterator i = find(m_rendererList.begin(), m_rendererList.end(), renderer);
//     int n = i - m_rendererList.begin();
//     beginRemoveRows(QModelIndex(), n, n);
//     m_rendererList.erase(i);
//     endRemoveRows();
// }
