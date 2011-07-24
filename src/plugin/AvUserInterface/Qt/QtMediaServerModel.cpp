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
    QtMediaObject* res = index.isValid() ? static_cast<QtMediaObject*>(index.internalPointer()) : 0;
    Omm::Av::Log::instance()->upnpav().debug("media server model get object: " + Poco::NumberFormatter::format(res));
    return res;
}


int
QtMediaServerModel::rowCount(const QModelIndex &parent) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model row count");
    
    QtMediaObject* pParentObject = getObject(parent);

    if (!pParentObject) {
        Omm::Av::Log::instance()->upnpav().debug("media server model parent object: NULL, row count: 1");
        return 1; // root object has no parent and is the only row.
    }
    
    Omm::Av::Log::instance()->upnpav().debug("media server model parent object: " + pParentObject->getObjectId() + " , row count: " + Poco::NumberFormatter::format(pParentObject->getChildCount()));
    return pParentObject->getChildCount();
}


int
QtMediaServerModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}


bool
QtMediaServerModel::hasChildren(const QModelIndex &parent) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model has children");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        return _pRootObject->isContainer();
    }
    return pParentObject->isContainer();
}


bool
QtMediaServerModel::canFetchMore(const QModelIndex &parent) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model can fetch more");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        return !_pRootObject->fetchedAllChildren();
    }
    return (!pParentObject->fetchedAllChildren());
}


void
QtMediaServerModel::fetchMore(const QModelIndex &parent)
{
    Omm::Av::Log::instance()->upnpav().debug("media server model fetch more");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        _pRootObject->fetchChildren();
        return;
    }
    pParentObject->fetchChildren();
    emit layoutChanged();
}


QVariant
QtMediaServerModel::data(const QModelIndex &index, int role) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model data");

    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == 0) {
        Omm::Av::Log::instance()->upnpav().warning("UpnpBrowserModel::data() objectId reference is 0:");
        return QVariant();
    }
    QtMediaObject* pObject = getObject(index);
//    std::string artist = object->getProperty(Omm::Av::AvProperty::ARTIST);
//    std::string album = object->getProperty(Omm::Av::AvProperty::ALBUM);
//    bool titleOnly = (artist == "");
    
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
//        if (titleOnly) {
//            return QString::fromStdString(object->getTitle());
            return _charEncoding->toUnicode(pObject->getTitle().c_str());
//        }
//        else {
//            return _charEncoding->toUnicode(artist.c_str());
//        }
        break;
    case Qt::DecorationRole:
//        if (index.column() == 0) {
//            return icon(index);
//        }
//     case Qt::TextAlignmentRole:
//         if (index.column() == 1) {
//             return Qt::AlignRight;
//         }
         return QVariant();
    }
    
    return QVariant();
}


QModelIndex
QtMediaServerModel::parent(const QModelIndex &index) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model parent");

    QtMediaObject* pObject = getObject(index);
    if (!pObject) {
        return QModelIndex();
    }
    Omm::Av::MediaObject* pParentObject = pObject->parent();
    if (!pParentObject) {
        return QModelIndex();
    }
    Omm::Av::MediaObject* pGrandParentObject = pObject->parent()->parent();
    if (!pGrandParentObject) {
        return createIndex(0, 0, _pRootObject);
    }
    Omm::Av::MediaObject::ChildIterator row;
    row = find(pGrandParentObject->beginChildren(), pGrandParentObject->endChildren(), pObject->parent());
    if (row != pGrandParentObject->endChildren()) {
        return createIndex(row - pGrandParentObject->beginChildren(), 0, pObject->parent());
    }
    return QModelIndex();
}


QModelIndex
QtMediaServerModel::index(int row, int column, const QModelIndex &parent) const
{
    Omm::Av::Log::instance()->upnpav().debug("media server model index");

    // no index has been created yet, so we must be at the root of the tree or need to fetch more data.
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        return createIndex(0, 0, _pRootObject);
    }

    // if we can't deliver an index, because _children.size()-1 < row
    // then fetchMore() is triggered -> return QModelIndex()
    if (row > int(pParentObject->childCount()) - 1) {
        return QModelIndex();
    }
    return createIndex(row, column, pParentObject->getChild(row));
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
    return "";
}


QIcon
QtMediaServerModel::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    QtMediaObject* pObject = getObject(index);
    if (pObject == 0) {
        return QIcon();
    }
    std::string objectClass = pObject->getProperty(Omm::Av::AvProperty::CLASS);
    if (pObject->isContainer()) {
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
