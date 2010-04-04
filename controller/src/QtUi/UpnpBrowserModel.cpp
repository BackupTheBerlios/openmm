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

#include "UpnpBrowserModel.h"

#include <QtDebug>

UpnpBrowserModel::UpnpBrowserModel(Omm::Av::UpnpAvUserInterface* pUserInterface, QObject *parent)
: QAbstractItemModel(parent),
m_pUserInterface(pUserInterface),
m_charEncoding(QTextCodec::codecForName("UTF-8")),
m_iconProvider(new QFileIconProvider())
{
}


UpnpBrowserModel::~UpnpBrowserModel()
{
}


Omm::Av::MediaObject*
UpnpBrowserModel::getObject(const QModelIndex &index) const
{
    return index.isValid() ? static_cast<Omm::Av::MediaObject*>(index.internalPointer()) : NULL;
}


int
UpnpBrowserModel::rowCount(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::rowCount()" << std::endl;
    
    Omm::Av::MediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::rowCount() parent objectId:" << object->m_objectId.c_str() << "return rows:" << object->m_children.size();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::rowCount() number of servers: " << m_pServers->size() << std::endl;
//         return m_pServers->size();
        return m_pUserInterface->serverCount();
    }
    
//     std::clog << "UpnpBrowserModel::rowCount() number of child objects: " << object->m_children.size() << std::endl;
//     return object->m_children.size();
    return object->childCount();
}


int
UpnpBrowserModel::columnCount(const QModelIndex& /*parent*/) const
{;
    return 1;
}


bool
UpnpBrowserModel::hasChildren(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::hasChildren()" << std::endl;
    
    Omm::Av::MediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::hasChildren() parent objectId:" << object->m_objectId.c_str();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::hasChildren() there are servers: " << ((m_pServers->size() > 0) ? "yes" : "nope") << std::endl;
//         return (m_pServers->size() > 0);
        return (m_pUserInterface->serverCount() > 0);
    }
//     if (!parent.isValid()) {
//         return (object->m_children.size() > 0);
//     }
//     std::clog << "UpnpBrowserModel::hasChildren() object has children: " << (object->isContainer() ? "yes" : "nope") << std::endl;
    return object->isContainer();
}


bool
UpnpBrowserModel::canFetchMore(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::canFetchMore()" << std::endl;
    
    Omm::Av::MediaObject* object = getObject(parent);
    if (object == 0) {
        return false;
    }
//     qDebug() << "UpnpBrowserModel::canFetchMore() parent objectId:" << object->m_objectId.c_str();
//     return (!object->m_fetchedAllChildren);
    return (!object->fetchedAllChildren());
}


void
UpnpBrowserModel::fetchMore(const QModelIndex &parent)
{
//     std::clog << "UpnpBrowserModel::fetchMore()" << std::endl;
    
    Omm::Av::MediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::fetchMore() parent objectId:" << object->m_objectId.c_str();
    if (object == NULL) {
        return;
    }
    object->fetchChildren();
//     qDebug() << "UpnpBrowserModel::fetchMore() number of children:" << object->m_children.size();
    emit layoutChanged();
}


QVariant
UpnpBrowserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
/*    if (role != Qt::DisplayRole) {
        return QVariant();
    }*/
    Omm::Av::MediaObject* object = getObject(index);
//     std::string label = object->m_objectId + ": " + object->getTitle();
    std::string label = object->objectId() + ": " + object->getTitle();
    
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case 0:
            return m_charEncoding->toUnicode(label.c_str());
        case 1: 
            return QString("1:00");
//         case 2: return type(index);
//         case 3: return time(index);
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return icon(index);
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return Qt::AlignRight;
        break;
    }
    
    return QVariant();
}


QModelIndex
UpnpBrowserModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Omm::Av::MediaObject* object = getObject(index);
//     qDebug() << "UpnpBrowserModel::parent() index objectId:" << object->m_objectId.c_str();
//     if (object->m_parent == NULL) {
    if (object->parent() == 0) {
            return QModelIndex();
    }
    Omm::Av::MediaObject* grandp = object->parent()->parent();
    if (grandp == 0) {
        int i = 0;
        while (i < m_pUserInterface->serverCount() && m_pUserInterface->serverRootObject(i) != object->parent()) {
            ++i;
        }
        if (i < m_pUserInterface->serverCount()) {
            return createIndex(i, 0, (void*)(object->parent()));
        }
        
//         Omm::Container<ServerController>::Iterator server = m_pServers->begin();
//         while ((*server)->root() != object->m_parent) {
//             ++server;
//         }
//         if (server != m_pServers->end()) {
//             return createIndex(server - m_pServers->begin(), 0, (void*)(object->m_parent));
//         }
        return QModelIndex();
    }
//     std::vector<Omm::Av::MediaObject*>::iterator row;
    Omm::Av::MediaObject::childIterator row;
    row = find(grandp->beginChildren(), grandp->endChildren(), object->parent());
    if (row != grandp->endChildren()) {
//         qDebug() << "UpnpBrowserModel::parent() return row:" << (row - grandp->m_children.begin());
        return createIndex(row - grandp->beginChildren(), 0, (void*)(object->parent()));
    }
    return QModelIndex();
}


QModelIndex
UpnpBrowserModel::index(int row, int column, const QModelIndex &parent) const
{
    // no index has been created yet, so we must be at the root of the tree ...?
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    Omm::Av::MediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::index() parent objectId:" << object->m_objectId.c_str() << "row:" << row;
    if (object == NULL) {
//         return createIndex(row, 0, (void*)(m_pServers->get(row).root()));
        return createIndex(row, 0, (void*)(m_pUserInterface->serverRootObject(row)));
    }
    
    // if we can't deliver an index, because m_children.size()-1 < row
    // then fetchMore() is triggered -> return QModelIndex()
    if (row > int(object->childCount()) - 1) {
        return QModelIndex();
    }
//     return createIndex(row, 0, (void*)(object->m_children[row]));
    return createIndex(row, 0, (void*)(object->getChild(row)));
}


Qt::ItemFlags
UpnpBrowserModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
UpnpBrowserModel::headerData(int section, Qt::Orientation orientation,
                             int role) const
{
/*    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return QVariant("Title");
    }
    return QVariant();
*/
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Name");
            case 1: return tr("Length");
            case 2: return tr("Type");
            default : return QVariant();
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}


QIcon
UpnpBrowserModel::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    Omm::Av::MediaObject* object = getObject(index);
    if (object == NULL) {
        return QIcon();
    }
    if (object->isContainer())
        return m_iconProvider->icon(QFileIconProvider::Folder);
    
    return m_iconProvider->icon(QFileIconProvider::File);
}


void
UpnpBrowserModel::beginAddServer(int position)
{
//     std::clog << "UpnpBrowserModel::beginAddServer() at position: " << position << std::endl;
    
    beginInsertRows(QModelIndex(), position, position);
}


void
UpnpBrowserModel::endAddServer()
{
//     std::clog << "UpnpBrowserModel::endAddServer()" << std::endl;
    
    endInsertRows();
    emit layoutChanged();
}

void
UpnpBrowserModel::beginRemoveServer(int position)
{
//     qDebug() << "UpnpBrowserModel::beginRemoveServer() at position: " << position;
    
    beginRemoveRows(QModelIndex(), position, position);
}


void
UpnpBrowserModel::endRemoveServer()
{
//     qDebug() << "UpnpBrowserModel::endRemoveServer()";
    
    endRemoveRows();
    emit layoutChanged();
}
