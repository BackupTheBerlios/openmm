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

#include "QtRendererListModel.h"

#include <algorithm>

QtRendererListModel::QtRendererListModel(Omm::Av::AvUserInterface* pUserInterface, QObject *parent) :
QAbstractItemModel(parent),
_pUserInterface(pUserInterface)
{
    _charEncoding = QTextCodec::codecForName("UTF-8");
}


QtRendererListModel::~QtRendererListModel()
{
}


QVariant
QtRendererListModel::data(const QModelIndex &index, int role) const
{
//     qDebug() << "UpnpRendererListModel::data() role:" << role;
    
    if (!index.isValid())
        return QVariant();
    
    if (index.internalPointer() == 0) {
        Omm::Av::Log::instance()->upnpav().warning("UpnpRendererListModel::data() reference to renderer device is 0:");
        return QVariant();
    }
    
//     qDebug() << "UpnpRendererListModel::data() renderer reference:" << index.internalPointer();
    if (role != Qt::DisplayRole)
        return QVariant();
    
    Omm::Av::AvRendererView* pRenderer = static_cast<Omm::Av::AvRendererView*>(index.internalPointer());
    return QString(pRenderer->getName().c_str());
//     return QString(pRenderer->getDevice()->getFriendlyName().c_str());
}


Qt::ItemFlags
QtRendererListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtRendererListModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                             int /*role*/) const
{
//     if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//     }
    return "";
}


QModelIndex
QtRendererListModel::index(int row, int column,
                        const QModelIndex &/*parent*/) const
{
//     qDebug() << "UpnpRendererListModel::index()";
//     return createIndex(row, column, _rendererList.at(row));
//     return createIndex(row, column, &_pRenderers->get(row));
    return createIndex(row, column, _pUserInterface->rendererView(row));
}


QModelIndex
QtRendererListModel::parent(const QModelIndex &/*index*/) const
{
    return QModelIndex();
}


int
QtRendererListModel::rowCount(const QModelIndex &/*parent*/) const
{
//     qDebug() << "UpnpRendererListModel::rowCount()";
    
//     return _rendererList.size();
//     return _pRenderers->size();
    return _pUserInterface->rendererCount();
}


int
QtRendererListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}


void
QtRendererListModel::beginAddRenderer(int position)
{
    Omm::Av::Log::instance()->upnpav().debug("UpnpRendererListModel::beginAddRenderer() at position:" + Poco::NumberFormatter::format(position));
    
    beginInsertRows(QModelIndex(), position, position);
}


void
QtRendererListModel::endAddRenderer()
{
    Omm::Av::Log::instance()->upnpav().debug("UpnpRendererListModel::endAddRenderer()");
    
    endInsertRows();
    emit layoutChanged();
    if (rowCount() == 1) {
        emit setCurrentIndex(index(0, 0));
    }
}

void
QtRendererListModel::beginRemoveRenderer(int position)
{
    Omm::Av::Log::instance()->upnpav().debug("UpnpRendererListModel::beginRemoveRenderer() at position:" + Poco::NumberFormatter::format(position));
    
    beginRemoveRows(QModelIndex(), position, position);
}


void
QtRendererListModel::endRemoveRenderer()
{
    Omm::Av::Log::instance()->upnpav().debug("UpnpRendererListModel::endRemoveRenderer()");
    
    endRemoveRows();
    emit layoutChanged();
}

