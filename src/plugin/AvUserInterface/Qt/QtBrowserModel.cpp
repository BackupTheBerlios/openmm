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

#include "QtBrowserModel.h"

#include <QtDebug>

QtBrowserModel::QtBrowserModel(Omm::Av::AvUserInterface* pUserInterface, QObject *parent)
: QAbstractItemModel(parent),
_pUserInterface(pUserInterface),
_charEncoding(QTextCodec::codecForName("UTF-8")),
_iconProvider(new QFileIconProvider())
{
}


QtBrowserModel::~QtBrowserModel()
{
}


Omm::Av::ControllerObject*
QtBrowserModel::getObject(const QModelIndex &index) const
{
    return index.isValid() ? static_cast<Omm::Av::ControllerObject*>(index.internalPointer()) : 0;
}


int
QtBrowserModel::rowCount(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::rowCount()" << std::endl;
    
    Omm::Av::ControllerObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::rowCount() parent objectId:" << object->_objectId.c_str() << "return rows:" << object->_children.size();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::rowCount() number of servers: " << _pServers->size() << std::endl;
//         return _pServers->size();
        return _pUserInterface->serverCount();
    }
    
//     std::clog << "UpnpBrowserModel::rowCount() number of child objects: " << object->getChildCount() << std::endl;
//     return object->_children.size();
    return object->getChildCount();
}


int
QtBrowserModel::columnCount(const QModelIndex& parent) const
{
//     Omm::Av::ControllerObject* object = getObject(parent);
//     if (!object) {
//         Omm::Av::Log::instance()->upnpav().debug("1 col");
//         return 1;
//     }
//     else {
//         Omm::Av::Log::instance()->upnpav().debug("2 cols");
//         return 2;
//     }
    return 3;
}


bool
QtBrowserModel::hasChildren(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::hasChildren()" << std::endl;
    
    Omm::Av::ControllerObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::hasChildren() parent objectId:" << object->_objectId.c_str();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::hasChildren() there are servers: " << ((_pServers->size() > 0) ? "yes" : "nope") << std::endl;
//         return (_pServers->size() > 0);
        return (_pUserInterface->serverCount() > 0);
    }
//     if (!parent.isValid()) {
//         return (object->_children.size() > 0);
//     }
//     std::clog << "UpnpBrowserModel::hasChildren() object has children: " << (object->isContainer() ? "yes" : "nope") << std::endl;
    return object->isContainer();
}


bool
QtBrowserModel::canFetchMore(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::canFetchMore()" << std::endl;
    
    Omm::Av::ControllerObject* object = getObject(parent);
    if (object == 0) {
        return false;
    }
//     qDebug() << "UpnpBrowserModel::canFetchMore() parent objectId:" << object->_objectId.c_str();
//     return (!object->_fetchedAllChildren);
    return (!object->fetchedAllChildren());
}


void
QtBrowserModel::fetchMore(const QModelIndex &parent)
{
//     std::clog << "UpnpBrowserModel::fetchMore()" << std::endl;
    
    Omm::Av::ControllerObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::fetchMore() parent objectId:" << object->_objectId.c_str();
    if (object == 0) {
        return;
    }
    object->fetchChildren();
//     qDebug() << "UpnpBrowserModel::fetchMore() number of children:" << object->_children.size();
    emit layoutChanged();
}


QVariant
QtBrowserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == 0) {
        qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
    Omm::Av::ControllerObject* object = getObject(index);
    std::string artist = object->getProperty(Omm::Av::AvProperty::ARTIST);
    std::string album = object->getProperty(Omm::Av::AvProperty::ALBUM);
    bool titleOnly = (artist == "");
    
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case 0:
            if (titleOnly) {
                return _charEncoding->toUnicode(object->getTitle().c_str());
            }
            else {
                return _charEncoding->toUnicode(artist.c_str());
            }
        case 1:
            if (titleOnly) {
                return QVariant();
            }
            else {
                return _charEncoding->toUnicode(album.c_str());
            }
        case 2:
            if (titleOnly) {
                return QVariant();
            }
            else {
                return _charEncoding->toUnicode(object->getTitle().c_str());
            }
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0) {
            return icon(index);
        }
//     case Qt::TextAlignmentRole:
//         if (index.column() == 1) {
//             return Qt::AlignRight;
//         }
//         return QVariant();
    }
    
    return QVariant();
}


QModelIndex
QtBrowserModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Omm::Av::ControllerObject* object = getObject(index);
//     qDebug() << "UpnpBrowserModel::parent() index objectId:" << object->_objectId.c_str();
//     if (object->_parent == NULL) {
    if (object->parent() == 0) {
            return QModelIndex();
    }
    Omm::Av::ControllerObject* grandp = object->parent()->parent();
    if (grandp == 0) {
        int i = 0;
        while (i < _pUserInterface->serverCount() && _pUserInterface->serverRootObject(i) != object->parent()) {
            ++i;
        }
        if (i < _pUserInterface->serverCount()) {
            return createIndex(i, 0, (void*)(object->parent()));
        }
        
//         Omm::Container<ServerController>::Iterator server = _pServers->begin();
//         while ((*server)->root() != object->_parent) {
//             ++server;
//         }
//         if (server != _pServers->end()) {
//             return createIndex(server - _pServers->begin(), 0, (void*)(object->_parent));
//         }
        return QModelIndex();
    }
//     std::vector<Omm::Av::MediaObject*>::iterator row;
    Omm::Av::ControllerObject::ChildIterator row;
    row = find(grandp->beginChildren(), grandp->endChildren(), object->parent());
    if (row != grandp->endChildren()) {
//         qDebug() << "UpnpBrowserModel::parent() return row:" << (row - grandp->_children.begin());
        return createIndex(row - grandp->beginChildren(), 0, (void*)(object->parent()));
    }
    return QModelIndex();
}


QModelIndex
QtBrowserModel::index(int row, int column, const QModelIndex &parent) const
{
    // no index has been created yet, so we must be at the root of the tree ...?
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    Omm::Av::ControllerObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::index() parent objectId:" << object->_objectId.c_str() << "row:" << row;
    if (object == NULL) {
//         return createIndex(row, 0, (void*)(_pServers->get(row).root()));
        return createIndex(row, column, (void*)(_pUserInterface->serverRootObject(row)));
    }
    
    // if we can't deliver an index, because _children.size()-1 < row
    // then fetchMore() is triggered -> return QModelIndex()
    if (row > int(object->childCount()) - 1) {
        return QModelIndex();
    }
//     return createIndex(row, 0, (void*)(object->_children[row]));
    return createIndex(row, column, (void*)(object->getChild(row)));
}


Qt::ItemFlags
QtBrowserModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtBrowserModel::headerData(int section, Qt::Orientation orientation,
                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Album");
            case 2:
                return tr("Title");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}


QIcon
QtBrowserModel::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    Omm::Av::ControllerObject* object = getObject(index);
    if (object == NULL) {
        return QIcon();
    }
    std::string objectClass = object->getProperty(Omm::Av::AvProperty::CLASS);
    if (object->isContainer()) {
        return _iconProvider->icon(QFileIconProvider::Folder);
    }
    else if (objectClass.find(Omm::Av::AvClass::AUDIO_ITEM) != std::string::npos) {
        return _iconProvider->icon(QFileIconProvider::Drive);
    }
    else if (objectClass.find(Omm::Av::AvClass::VIDEO_ITEM) != std::string::npos) {
        return _iconProvider->icon(QFileIconProvider::Desktop);
    }
    else if (objectClass.find(Omm::Av::AvClass::IMAGE_ITEM) != std::string::npos) {
        return _iconProvider->icon(QFileIconProvider::Computer);
    }
    return _iconProvider->icon(QFileIconProvider::File);
}


void
QtBrowserModel::beginAddServer(int position)
{
//     std::clog << "UpnpBrowserModel::beginAddServer() at position: " << position << std::endl;
    
    beginInsertRows(QModelIndex(), position, position);
}


void
QtBrowserModel::endAddServer()
{
//     std::clog << "UpnpBrowserModel::endAddServer()" << std::endl;
    
    endInsertRows();
    emit layoutChanged();
}

void
QtBrowserModel::beginRemoveServer(int position)
{
//     qDebug() << "UpnpBrowserModel::beginRemoveServer() at position: " << position;
    
    beginRemoveRows(QModelIndex(), position, position);
}


void
QtBrowserModel::endRemoveServer()
{
//     qDebug() << "UpnpBrowserModel::endRemoveServer()";
    
    endRemoveRows();
    emit layoutChanged();
}
