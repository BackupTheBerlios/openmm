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

#include "QtMediaServerModel.h"


QtMediaServerModel::QtMediaServerModel(Omm::Av::CtlMediaObject* pRootObject, QObject *parent) :
QAbstractItemModel(parent),
_pRootObject(pRootObject),
_charEncoding(QTextCodec::codecForName("UTF-8"))
// FIXME: QPixmap: it is not safe to use pixmaps outside the gui thread ... crash in ctor off QFileIconProvider
//_iconProvider(new QFileIconProvider())
{
}


QtMediaServerModel::~QtMediaServerModel()
{
}


QtMediaObject*
QtMediaServerModel::getObject(const QModelIndex &index) const
{
    return index.isValid() ? static_cast<QtMediaObject*>(index.internalPointer()) : 0;
}


int
QtMediaServerModel::rowCount(const QModelIndex &parent) const
{
    QtMediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::rowCount() parent objectId:" << object->_objectId.c_str() << "return rows:" << object->_children.size();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::rowCount() number of servers: " << _pServers->size() << std::endl;
        return _pRootObject->getChildCount();
    }
    
//     std::clog << "UpnpBrowserModel::rowCount() number of child objects: " << object->getChildCount() << std::endl;
    return object->getChildCount();
}


int
QtMediaServerModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}


bool
QtMediaServerModel::hasChildren(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::hasChildren()" << std::endl;
    
    QtMediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::hasChildren() parent objectId:" << object->_objectId.c_str();
    if (object == 0) {
//         std::clog << "UpnpBrowserModel::hasChildren() there are servers: " << ((_pServers->size() > 0) ? "yes" : "nope") << std::endl;
        return _pRootObject->isContainer();
    }
//     std::clog << "UpnpBrowserModel::hasChildren() object has children: " << (object->isContainer() ? "yes" : "nope") << std::endl;
    return object->isContainer();
}


bool
QtMediaServerModel::canFetchMore(const QModelIndex &parent) const
{
//     std::clog << "UpnpBrowserModel::canFetchMore()" << std::endl;
    
    QtMediaObject* object = getObject(parent);
    if (object == 0) {
//        return false;
        return !_pRootObject->fetchedAllChildren();
    }
//     qDebug() << "UpnpBrowserModel::canFetchMore() parent objectId:" << object->_objectId.c_str();
    return (!object->fetchedAllChildren());
}


void
QtMediaServerModel::fetchMore(const QModelIndex &parent)
{
//     std::clog << "UpnpBrowserModel::fetchMore()" << std::endl;
    
    QtMediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::fetchMore() parent objectId:" << object->_objectId.c_str();
    if (object == 0) {
        _pRootObject->fetchChildren();
        return;
    }
    object->fetchChildren();
//     qDebug() << "UpnpBrowserModel::fetchMore() number of children:" << object->_children.size();
    emit layoutChanged();
}


QVariant
QtMediaServerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == 0) {
        Omm::Av::Log::instance()->upnpav().warning("UpnpBrowserModel::data() objectId reference is 0:");
        return QVariant();
    }
    QtMediaObject* object = getObject(index);
    std::string artist = object->getProperty(Omm::Av::AvProperty::ARTIST);
    std::string album = object->getProperty(Omm::Av::AvProperty::ALBUM);
    bool titleOnly = (artist == "");
    
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (titleOnly) {
//            return QString::fromStdString(object->getTitle());
            return _charEncoding->toUnicode(object->getTitle().c_str());
        }
        else {
            return _charEncoding->toUnicode(artist.c_str());
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
QtMediaServerModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QtMediaObject* object = getObject(index);
//     qDebug() << "UpnpBrowserModel::parent() index objectId:" << object->_objectId.c_str();
    if (object->parent() == 0) {
            return QModelIndex();
    }
    Omm::Av::MediaObject* grandp = object->parent()->parent();
    if (grandp == 0) {
        return QModelIndex();
    }
    Omm::Av::MediaObject::ChildIterator row;
    row = find(grandp->beginChildren(), grandp->endChildren(), object->parent());
    if (row != grandp->endChildren()) {
//         qDebug() << "UpnpBrowserModel::parent() return row:" << (row - grandp->_children.begin());
        return createIndex(row - grandp->beginChildren(), 0, (void*)(object->parent()));
    }
    return QModelIndex();
}


QModelIndex
QtMediaServerModel::index(int row, int column, const QModelIndex &parent) const
{
    // no index has been created yet, so we must be at the root of the tree ...?
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    QtMediaObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::index() parent objectId:" << object->_objectId.c_str() << "row:" << row;
    if (object == 0) {
        return createIndex(row, column, _pRootObject);
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
QtMediaServerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtMediaServerModel::headerData(int section, Qt::Orientation orientation,
                             int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}


QIcon
QtMediaServerModel::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    QtMediaObject* object = getObject(index);
    if (object == 0) {
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
