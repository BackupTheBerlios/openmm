/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
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

UpnpRendererListModel::UpnpRendererListModel(Omm::Av::AvUserInterface* pUserInterface, QObject *parent) :
QAbstractItemModel(parent),
// _pRenderers(pRenderers)
_pUserInterface(pUserInterface)
{
    _charEncoding = QTextCodec::codecForName("UTF-8");
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
    
    Omm::Av::RendererView* pRenderer = static_cast<Omm::Av::RendererView*>(index.internalPointer());
    return QString(pRenderer->getName().c_str());
//     return QString(pRenderer->getDevice()->getFriendlyName().c_str());
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
//     return createIndex(row, column, _rendererList.at(row));
//     return createIndex(row, column, &_pRenderers->get(row));
    return createIndex(row, column, _pUserInterface->rendererView(row));
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
    
//     return _rendererList.size();
//     return _pRenderers->size();
    return _pUserInterface->rendererCount();
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
    emit layoutChanged();
    if (rowCount() == 1) {
        emit setCurrentIndex(index(0, 0));
    }
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
    emit layoutChanged();
}

