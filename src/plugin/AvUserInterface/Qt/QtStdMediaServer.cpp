/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#include "QtStdMediaServer.h"

class QtStdMediaContainerItem : public QStyledItemDelegate
{
public:
    QtStdMediaContainerItem(QObject* parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


QtStdMediaContainerItem::QtStdMediaContainerItem(QObject* parent) :
QStyledItemDelegate(parent)
{
}


void
QtStdMediaContainerItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int padding = 4;
    QRect iconRect(option.rect);
    iconRect.setLeft(padding);
    iconRect.setWidth(option.rect.height());
    QRect textRect(option.rect);
    textRect.setLeft(iconRect.right() + padding);

//    Omm::Av::Log::instance()->upnpav().debug(
//        + "list item painter, font size points: " + Poco::NumberFormatter::format(option.font.pointSize())
//        + ", font size pixels: " + Poco::NumberFormatter::format(option.font.pixelSize())
//    );

    if (qVariantCanConvert<QString>(index.data(Qt::DisplayRole))) {
        QString title = qvariant_cast<QString>(index.data(Qt::DisplayRole));
        painter->save();
//        painter->setRenderHint(QPainter::Antialiasing, true);
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(option.palette.highlightedText().color());
        }
//        painter->setFont(option.font);  // painter is style aware, don't need to set this.
        painter->drawText(textRect, Qt::AlignVCenter, title);
        painter->restore();
    }
    if (qVariantCanConvert<QIcon>(index.data(Qt::DecorationRole))) {
//        Omm::Av::Log::instance()->upnpav().debug("DRAWING ICON");
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        painter->save();
        icon.paint(painter, iconRect);
        painter->restore();
    }
    // use QApplication::style()->drawControl() to draw Qt widgets on the painter

//    else {
//        // default painter
//        QStyledItemDelegate::paint(painter, option, index);
//    }
}


QSize
QtStdMediaContainerItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize itemSize = QStyledItemDelegate::sizeHint(option, index);
    itemSize.rheight() *= 2;
    return itemSize;
}


QtStdMediaServerWidget::QtStdMediaServerWidget(QtStdMediaServer* pMediaServer) :
_pMediaServer(pMediaServer)
{
    setModel(pMediaServer);
    setUniformRowHeights(true);
    setHeaderHidden(true);
//        _pMediaServerTreeView->setRootIsDecorated(false);
//        _pMediaServerTreeView->setItemsExpandable(false);
    // activated() is return, click or double click, selected() is click or double click on it.
    connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));    
}


void
QtStdMediaServerWidget::selectedModelIndex(const QModelIndex& index)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt standard media server widget selected index");

    Omm::Av::CtlMediaObject2* pObject = static_cast<Omm::Av::CtlMediaObject2*>(index.internalPointer());
    _pMediaServer->selectedMediaObject(pObject);
}


QtStdMediaServer::QtStdMediaServer() :
_pMediaServerWidget(0),
_charEncoding(QTextCodec::codecForName("UTF-8"))
{
}


QtStdMediaServer::~QtStdMediaServer()
{
}


QString
QtStdMediaServer::getBrowserTitle()
{
    return QString(getFriendlyName().c_str());
}


QWidget*
QtStdMediaServer::getWidget()
{
    return _pMediaServerWidget;
}


void
QtStdMediaServer::initController()
{
    Omm::Av::Log::instance()->upnpav().debug("init Qt standard media server (controller)");
    // TODO: check if root object is an item or container and change the visual appearance accordingly.
    browseRootObject();
}


void
QtStdMediaServer::selected()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt standard media server selected (controller)");
    // creating a QTreeView and setting the model outside the GUI thread is not allowed,
    // so we do it on first selection and not on discovery of the device.
    if (!_pMediaServerWidget) {
        _pMediaServerWidget = new QtStdMediaServerWidget(this);
    }
//    _pMediaServerWidget->setCurrentIndex(index(0, 0, QModelIndex()));
}


void
QtStdMediaServer::selectedMediaObject(Omm::Av::CtlMediaObject2* pObject)
{
    selectMediaObject(pObject);
}


QIcon
QtStdMediaServer::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    Omm::Av::CtlMediaObject2* pObject = getObject(index);
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


void
QtStdMediaServer::selectedModelIndex(const QModelIndex& index)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt standard media server selected index");

    Omm::Av::CtlMediaObject2* pObject = static_cast<Omm::Av::CtlMediaObject2*>(index.internalPointer());
    selectMediaObject(pObject);
}


Omm::Av::CtlMediaObject2*
QtStdMediaServer::getObject(const QModelIndex &index) const
{
     Omm::Av::CtlMediaObject2* res = index.isValid() ? static_cast<Omm::Av::CtlMediaObject2*>(index.internalPointer()) : 0;
//    Omm::Av::Log::instance()->upnpav().debug("media server model get object: " + Poco::NumberFormatter::format(res));
    return res;
}


int
QtStdMediaServer::rowCount(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model row count");

     Omm::Av::CtlMediaObject2* pParentObject = getObject(parent);

    if (!pParentObject) {
//        Omm::Av::Log::instance()->upnpav().debug("media server model parent object: NULL, row count: 1");
        return 1; // root object has no parent and is the only row.
    }

//    Omm::Av::Log::instance()->upnpav().debug("media server model parent object: " + pParentObject->getObjectId() + " , row count: " + Poco::NumberFormatter::format(pParentObject->getChildCount()));
    return pParentObject->getChildCount();
}


int
QtStdMediaServer::columnCount(const QModelIndex& parent) const
{
    return 1;
}


bool
QtStdMediaServer::hasChildren(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model has children");

     Omm::Av::CtlMediaObject2* pParentObject = getObject(parent);
    if (!pParentObject) {
        return getRootObject()->isContainer();
    }
    return pParentObject->isContainer();
}


bool
QtStdMediaServer::canFetchMore(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model can fetch more");

     Omm::Av::CtlMediaObject2* pParentObject = getObject(parent);
    if (!pParentObject) {
        return !getRootObject()->fetchedAllChildren();
    }
    return (!pParentObject->fetchedAllChildren());
}


void
QtStdMediaServer::fetchMore(const QModelIndex &parent)
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model fetch more");

     Omm::Av::CtlMediaObject2* pParentObject = getObject(parent);
    if (!pParentObject) {
        getRootObject()->fetchChildren();
        return;
    }
    pParentObject->fetchChildren();
    emit layoutChanged();
}


QVariant
QtStdMediaServer::data(const QModelIndex &index, int role) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model data");

    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == 0) {
        Omm::Av::Log::instance()->upnpav().warning("UpnpBrowserModel::data() objectId reference is 0:");
        return QVariant();
    }
     Omm::Av::CtlMediaObject2* pObject = getObject(index);
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
QtStdMediaServer::parent(const QModelIndex &index) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model parent");

    Omm::Av::CtlMediaObject2* pObject = getObject(index);
    if (!pObject) {
        return QModelIndex();
    }
    Omm::Av::MediaObject* pParentObject = pObject->parent();
    if (!pParentObject) {
        return QModelIndex();
    }
    Omm::Av::CtlMediaObject2* pGrandParentObject = pObject->parent()->parent();
    if (!pGrandParentObject) {
        return createIndex(0, 0, getRootObject());
    }
    Omm::Av::CtlMediaObject2::ChildIterator row;
    row = find(pGrandParentObject->beginChildren(), pGrandParentObject->endChildren(), pObject->parent());
    if (row != pGrandParentObject->endChildren()) {
        return createIndex(row - pGrandParentObject->beginChildren(), 0, pObject->parent());
    }
    return QModelIndex();
}


QModelIndex
QtStdMediaServer::index(int row, int column, const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model index");

    // no index has been created yet, so we must be at the root of the tree or need to fetch more data.
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
     Omm::Av::CtlMediaObject2* pParentObject = getObject(parent);
    if (!pParentObject) {
        return createIndex(0, 0, getRootObject());
    }

    // if we can't deliver an index, because _children.size()-1 < row
    // then fetchMore() is triggered -> return QModelIndex()
    if (row > int(pParentObject->childCount()) - 1) {
        return QModelIndex();
    }
    return createIndex(row, column, pParentObject->getChild(row));
}


Qt::ItemFlags
QtStdMediaServer::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtStdMediaServer::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "";
}
