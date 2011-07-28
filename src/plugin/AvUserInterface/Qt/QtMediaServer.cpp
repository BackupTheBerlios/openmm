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

#include "QtMediaServer.h"
#include "QtEventFilter.h"

class QtMediaContainerItem : public QStyledItemDelegate
{
public:
    QtMediaContainerItem(QObject* parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


QtMediaContainerItem::QtMediaContainerItem(QObject* parent) :
QStyledItemDelegate(parent)
{
}


void
QtMediaContainerItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
QtMediaContainerItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize itemSize = QStyledItemDelegate::sizeHint(option, index);
    itemSize.rheight() *= 2;
    return itemSize;
}


QtMediaServerWidget::QtMediaServerWidget(QAbstractItemModel* pModel)
{
    setModel(pModel);
    setUniformRowHeights(true);
    setHeaderHidden(true);
//        _pMediaServerTreeView->setRootIsDecorated(false);
//        _pMediaServerTreeView->setItemsExpandable(false);
//         activated() is return, click or double click, selected() is click or double click on it.
    connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));    
}


void
QtMediaServerWidget::selectedModelIndex(const QModelIndex& index)
{
    Omm::Av::Log::instance()->upnpav().debug("media server selected index");

//    Omm::Av::CtlMediaObject* pObject = static_cast<Omm::Av::CtlMediaObject*>(index.internalPointer());
//    selectMediaObject(pObject);
}


QtMediaServer::QtMediaServer() :
_pMediaServerTreeView(0),
_pFakeButton(0),
_charEncoding(QTextCodec::codecForName("UTF-8"))
{
}


QtMediaServer::~QtMediaServer()
{

}


QString
QtMediaServer::getBrowserTitle()
{
    return QString(getFriendlyName().c_str());
}


QWidget*
QtMediaServer::getWidget()
{
    return _pMediaServerTreeView;
//    return _pFakeButton;
//    return _pFakeWidget;
}


void
QtMediaServer::initController()
{
    Omm::Av::Log::instance()->upnpav().debug("init qt media server (controller)");
    // TODO: check if root object is an item or container and change the visual appearance accordingly.
    browseRootObject();
}


void
QtMediaServer::selected()
{
    Omm::Av::Log::instance()->upnpav().debug("qt media server selected (controller)");
    // creating a QTreeView and setting the model outside the GUI thread is not allowed,
    // so we do it on first selection and not on discovery of the device.
    if (!_pMediaServerTreeView) {
//        _pMediaServerTreeView = new QTreeView;
        _pMediaServerTreeView = new QtMediaServerWidget(this);
//        _pMediaServerTreeView->setModel(this);
//        _pMediaServerTreeView->setUniformRowHeights(true);
//        _pMediaServerTreeView->setHeaderHidden(true);
//        _pMediaServerTreeView->setRootIsDecorated(false);
//        _pMediaServerTreeView->setItemsExpandable(false);
//         activated() is return, click or double click, selected() is click or double click on it.
//        connect(_pMediaServerTreeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(selected(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(entered(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(expanded(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    }
    if (!_pFakeButton) {
        Omm::Av::Log::instance()->upnpav().debug("qt media server creating fake button");
        _pFakeWidget = new QWidget;
        QHBoxLayout* pLayout = new QHBoxLayout(_pFakeWidget);
        _pFakeButton = new QPushButton;
        pLayout->addWidget(_pFakeButton);
        Omm::Av::Log::instance()->upnpav().debug("qt media server connecting fake button");
        if (connect(_pFakeButton, SIGNAL(pressed()), this, SLOT(fakeButtonPushed()))) {
            Omm::Av::Log::instance()->upnpav().debug("qt media server fake button connected");
        };
        connect(_pFakeButton, SIGNAL(pressed()), this, SIGNAL(fakeSignal()));
        _pSignalSpy = new QSignalSpy(_pFakeButton, SIGNAL(pressed()));

        
        // following gives: Object::connect: No such slot QtMediaServer::QtMediaServer::fakeButtonPushed()
//        connect(_pFakeButton, SIGNAL(pressed()), this, SLOT(QtMediaServer::fakeButtonPushed()));
//        connect(_pFakeButton, SIGNAL(pressed()), this, SLOT(fakeButtonPushed()), Qt::QueuedConnection);
        _pFakeButton->setText("c'mon sucker, push me");
        QtEventFilter* pEventFilter = new QtEventFilter;
        _pFakeButton->installEventFilter(pEventFilter);
        connect(pEventFilter, SIGNAL(fakeSignal()), this, SLOT(fakeButtonPushed()));
    }
//    _pMediaServerTreeView->setCurrentIndex(index(0, 0, QModelIndex()));
    Omm::Av::Log::instance()->upnpav().debug("number of fake signals recorded: " + Poco::NumberFormatter::format(_pSignalSpy->count()));
}


void
QtMediaServer::fakeButtonPushed()
{
    Omm::Av::Log::instance()->upnpav().debug("qt media server fake button pushed");
    _pFakeButton->setText("darn, you pushed me ...");
}


void
QtMediaServer::selectionChanged(const QItemSelection&, const QItemSelection&)
{
    Omm::Av::Log::instance()->upnpav().debug("qt media server selection changed");
}


//void
//QtMediaServer::createServerModel()
//{
//    Omm::Av::Log::instance()->upnpav().debug("qt media server createServerModel()");
//    // creating a QTreeView and setting the model outside the GUI thread is not allowed,
//    // so we do it on first selection and not on discovery of the device.
//    if (!_pMediaServerTreeView) {
//        _pMediaServerTreeView = new QTreeView;
//        _pMediaServerTreeView->setModel(this);
//        _pMediaServerTreeView->setUniformRowHeights(true);
//        _pMediaServerTreeView->setHeaderHidden(true);
////        _pMediaServerTreeView->setRootIsDecorated(false);
////        _pMediaServerTreeView->setItemsExpandable(false);
//        // activated() is return, click or double click, selected() is click or double click on it.
//        connect(_pMediaServerTreeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
////        connect(_pMediaServerTreeView, SIGNAL(selected(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
////        connect(_pMediaServerTreeView, SIGNAL(entered(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
////        connect(_pMediaServerTreeView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//        connect(_pMediaServerTreeView, SIGNAL(expanded(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//    }
//}

//void
//QtMediaServer::exposed()
//{
//    connect(_pMediaServerTreeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));
//}


QIcon
QtMediaServer::icon(const QModelIndex &index) const
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


void
QtMediaServer::selectedModelIndex(const QModelIndex& index)
{
    Omm::Av::Log::instance()->upnpav().debug("media server selected index");

    Omm::Av::CtlMediaObject* pObject = static_cast<Omm::Av::CtlMediaObject*>(index.internalPointer());
    selectMediaObject(pObject);
}


QtMediaObject*
QtMediaServer::getObject(const QModelIndex &index) const
{
    QtMediaObject* res = index.isValid() ? static_cast<QtMediaObject*>(index.internalPointer()) : 0;
//    Omm::Av::Log::instance()->upnpav().debug("media server model get object: " + Poco::NumberFormatter::format(res));
    return res;
}


int
QtMediaServer::rowCount(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model row count");

    QtMediaObject* pParentObject = getObject(parent);

    if (!pParentObject) {
//        Omm::Av::Log::instance()->upnpav().debug("media server model parent object: NULL, row count: 1");
        return 1; // root object has no parent and is the only row.
    }

//    Omm::Av::Log::instance()->upnpav().debug("media server model parent object: " + pParentObject->getObjectId() + " , row count: " + Poco::NumberFormatter::format(pParentObject->getChildCount()));
    return pParentObject->getChildCount();
}


int
QtMediaServer::columnCount(const QModelIndex& parent) const
{
    return 1;
}


bool
QtMediaServer::hasChildren(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model has children");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        return getRootObject()->isContainer();
    }
    return pParentObject->isContainer();
}


bool
QtMediaServer::canFetchMore(const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model can fetch more");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        return !getRootObject()->fetchedAllChildren();
    }
    return (!pParentObject->fetchedAllChildren());
}


void
QtMediaServer::fetchMore(const QModelIndex &parent)
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model fetch more");

    QtMediaObject* pParentObject = getObject(parent);
    if (!pParentObject) {
        getRootObject()->fetchChildren();
        return;
    }
    pParentObject->fetchChildren();
    emit layoutChanged();
}


QVariant
QtMediaServer::data(const QModelIndex &index, int role) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model data");

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
QtMediaServer::parent(const QModelIndex &index) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model parent");

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
        return createIndex(0, 0, getRootObject());
    }
    Omm::Av::MediaObject::ChildIterator row;
    row = find(pGrandParentObject->beginChildren(), pGrandParentObject->endChildren(), pObject->parent());
    if (row != pGrandParentObject->endChildren()) {
        return createIndex(row - pGrandParentObject->beginChildren(), 0, pObject->parent());
    }
    return QModelIndex();
}


QModelIndex
QtMediaServer::index(int row, int column, const QModelIndex &parent) const
{
//    Omm::Av::Log::instance()->upnpav().debug("media server model index");

    // no index has been created yet, so we must be at the root of the tree or need to fetch more data.
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    QtMediaObject* pParentObject = getObject(parent);
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
QtMediaServer::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtMediaServer::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "";
}
