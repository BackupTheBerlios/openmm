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
#include "QtNavigator.h"
#include "QtStdMediaContainer.h"

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
    QRect textRect2(option.rect);
    textRect2.setLeft(textRect.right() + padding);

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
_pMediaServer(pMediaServer),
_pItemDelegate(0)
{
    setModel(pMediaServer);
    setUniformRowHeights(true);
    setHeaderHidden(true);
    if (!_pMediaServer->_treeView) {
        _pItemDelegate = new QtStdMediaContainerItem;
        setItemDelegate(_pItemDelegate);
        setRootIsDecorated(false);
        setItemsExpandable(false);
    }
    // activated() is return, click or double click, selected() is click or double click on it.
    connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
}


void
QtStdMediaServerWidget::selectedModelIndex(const QModelIndex& index)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt standard media server widget selected index");

    Omm::Av::CtlMediaObject2* pObject = static_cast<Omm::Av::CtlMediaObject2*>(index.internalPointer());

    if (pObject->isContainer() && !_pMediaServer->_treeView) {
        QtStdMediaContainer* pQtObject = new QtStdMediaContainer;
        pQtObject->_pObject = pObject;
        pQtObject->_pServerWidget = this;
        pQtObject->_modelIndex = index;
        _pMediaServer->getNavigator()->push(pQtObject);
    }
    else {
        _pMediaServer->selectMediaObject(pObject);
    }
}


QtStdMediaServer::QtStdMediaServer() :
_pMediaServerWidget(0),
_treeView(false),
_pCharEncoding(0),
_pIconProvider(0)
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
QtStdMediaServer::show()
{
    _pMediaServerWidget->setRootIndex(index(0, 0, QModelIndex()));
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

    // creating a QObjects like QTreeView or QFileIconProvider and setting a model outside the GUI thread is not allowed,
    // so we do it on first selection and not on discovery of the device.
    _pCharEncoding = QTextCodec::codecForName("UTF-8");
    _pIconProvider = new QFileIconProvider;

    if (!_pMediaServerWidget) {
        _pMediaServerWidget = new QtStdMediaServerWidget(this);
    }
//    _pMediaServerWidget->setCurrentIndex(index(0, 0, QModelIndex()));
}


QIcon
QtStdMediaServer::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    Omm::Av::CtlMediaObject2* pObject = getObject(index);
    if (!pObject) {
        return QIcon();
    }
    std::string objectClass = pObject->getProperty(Omm::Av::AvProperty::CLASS)->getValue();
    if (pObject->isContainer()) {
        return _pIconProvider->icon(QFileIconProvider::Folder);
    }
    else if (objectClass.find(Omm::Av::AvClass::AUDIO_ITEM) != std::string::npos) {
        return _pIconProvider->icon(QFileIconProvider::Drive);
    }
    else if (objectClass.find(Omm::Av::AvClass::VIDEO_ITEM) != std::string::npos) {
        return _pIconProvider->icon(QFileIconProvider::Desktop);
    }
    else if (objectClass.find(Omm::Av::AvClass::IMAGE_ITEM) != std::string::npos) {
        return _pIconProvider->icon(QFileIconProvider::Computer);
    }
    return _pIconProvider->icon(QFileIconProvider::File);
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
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == 0) {
        Omm::Av::Log::instance()->upnpav().warning("UpnpBrowserModel::data() objectId reference is 0:");
        return QVariant();
    }
     Omm::Av::CtlMediaObject2* pObject = getObject(index);

    if (role == Qt::DisplayRole) {
        Omm::Av::AbstractProperty* pArtist = pObject->getProperty(Omm::Av::AvProperty::ARTIST);
//        std::string album = pObject->getProperty(Omm::Av::AvProperty::ALBUM)->getValue();
        if (!pArtist) {
            return _pCharEncoding->toUnicode(pObject->getTitle().c_str());
        }
        else {
            return _pCharEncoding->toUnicode((pArtist->getValue() + " - " + pObject->getTitle()).c_str());
        }
    }
    else if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            return icon(index);
        }
    }

    return QVariant();
}


QModelIndex
QtStdMediaServer::parent(const QModelIndex &index) const
{
    Omm::Av::CtlMediaObject2* pObject = getObject(index);
    if (!pObject) {
        return QModelIndex();
    }
    // FIXME: getParent() removed from CtlMediaObject2
//    Omm::Av::CtlMediaObject2* pParentObject = static_cast<Omm::Av::CtlMediaObject2*>(pObject->getParent());
    Omm::Av::CtlMediaObject2* pParentObject;
    if (!pParentObject) {
        return QModelIndex();
    }
    // FIXME: getParent() removed from CtlMediaObject2
//    Omm::Av::CtlMediaObject2* pGrandParentObject = static_cast<Omm::Av::CtlMediaObject2*>(pParentObject->getParent());
    Omm::Av::CtlMediaObject2* pGrandParentObject;
    if (!pGrandParentObject) {
        return createIndex(0, 0, getRootObject());
    }
    std::string parentObjectId = pParentObject->getId();
    for (Omm::ui4 row = 0; row < pGrandParentObject->getChildCount(); row++) {
        if (pGrandParentObject->getChildForRow(row)->getId() == parentObjectId) {
            return createIndex(row, 0, pParentObject);
        }
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
    if (row > int(pParentObject->getChildCount()) - 1) {
        return QModelIndex();
    }
    return createIndex(row, column, pParentObject->getChildForRow(row));
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
