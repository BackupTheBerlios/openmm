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
#include <Omm/UpnpAv.h>
#include <Omm/UpnpAvController.h>

#include "QtDeviceGroup.h"
#include "QtNavigator.h"
#include "QtMediaServer.h"


class QtDeviceListItem : public QStyledItemDelegate
{
public:
    QtDeviceListItem(QObject* parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


QtDeviceListItem::QtDeviceListItem(QObject* parent) :
QStyledItemDelegate(parent)
{
}


void
QtDeviceListItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
QtDeviceListItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize itemSize = QStyledItemDelegate::sizeHint(option, index);
    itemSize.rheight() *= 2;
    return itemSize;
}


QtDeviceGroup::QtDeviceGroup(const std::string& deviceType, const std::string& shortName) :
DeviceGroup(deviceType, shortName),
_iconProvider(new QFileIconProvider())
{
    _charEncoding = QTextCodec::codecForName("UTF-8");

    _pDeviceListView = new QTreeView;
    _pDeviceListItem = new QtDeviceListItem(_pDeviceListView);
    _pDeviceListView->setItemDelegate(_pDeviceListItem);
    _pDeviceListView->setModel(this);
    _pDeviceListView->setUniformRowHeights(true);
    _pDeviceListView->setHeaderHidden(true);
    _pDeviceListView->setRootIsDecorated(false);
    _pDeviceListView->setItemsExpandable(false);
    // activated() is return, click or double click, selected() is click or double click on it.
    connect(_pDeviceListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectedModelIndex(const QModelIndex&)));

    _pNavigator = new QtNavigator;
    // push this Navigable on the Navigator, the actual widget pushed is _pDeviceListView (returned by getWidget()).
    _pNavigator->push(this);
}


QtDeviceGroup::~QtDeviceGroup()
{
}


QWidget*
QtDeviceGroup::getDeviceGroupWidget()
{
    return _pNavigator;
}


QVariant
QtDeviceGroup::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    if (index.internalPointer() == 0) {
        Omm::Log::instance()->upnp().warning("QtDeviceGroupModel::data() reference to device is 0:");
        return QVariant();
    }

    Omm::Device* pDevice = static_cast<Omm::Device*>(index.internalPointer());
    switch (role) {
        case Qt::DisplayRole:
            return QString(pDevice->getFriendlyName().c_str());
        case Qt::DecorationRole:
            return _iconProvider->icon(QFileIconProvider::Folder);
        default:
            return QVariant();
    }
}


Qt::ItemFlags
QtDeviceGroup::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
QtDeviceGroup::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "";
}


QModelIndex
QtDeviceGroup::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, getDevice(row));
}


QModelIndex
QtDeviceGroup::parent(const QModelIndex& index) const
{
    return QModelIndex();
}


int
QtDeviceGroup::rowCount(const QModelIndex& parent) const
{
    return getDeviceCount();
}


int
QtDeviceGroup::columnCount(const QModelIndex& parent) const
{
    return 1;
}


Omm::Device*
QtDeviceGroup::createDevice()
{
    if (_deviceType == Omm::Av::DeviceType::MEDIA_RENDERER_1) {
        return new Omm::Av::CtlMediaRenderer;
    }
    else if (_deviceType == Omm::Av::DeviceType::MEDIA_SERVER_1) {
        return new QtMediaServer;
    }
}


void
QtDeviceGroup::addDevice(Omm::Device* pDevice, int position, bool begin)
{
    Omm::Log::instance()->upnp().debug("Qt device group model adds device at position:" + Poco::NumberFormatter::format(position));

    if (begin) {
        beginInsertRows(QModelIndex(), position, position);
    }
    else {
        Omm::Log::instance()->upnp().debug("Qt device group model finished adding device.");
        endInsertRows();
        emit layoutChanged();
        if (rowCount() == 1) {
            emit setCurrentIndex(index(0, 0));
        }
    }
}


void
QtDeviceGroup::removeDevice(Omm::Device* pDevice, int position, bool begin)
{
    Omm::Log::instance()->upnp().debug("Qt device group model removes device at position:" + Poco::NumberFormatter::format(position));

    if (begin) {
        beginRemoveRows(QModelIndex(), position, position);
    }
    else {
        Omm::Log::instance()->upnp().debug("Qt device group model finished adding device.");
        endRemoveRows();
        emit layoutChanged();
    }
}


QString
QtDeviceGroup::getBrowserTitle()
{
    return ">";
}


QWidget*
QtDeviceGroup::getWidget()
{
    return _pDeviceListView;
}


void
QtDeviceGroup::selectDevice(Omm::Device* pDevice, int index)
{
    if (_deviceType == Omm::Av::DeviceType::MEDIA_SERVER_1) {
        _pNavigator->push(static_cast<QtMediaServer*>(pDevice));
        connect(static_cast<QtMediaServer*>(pDevice), SIGNAL(fakeSignal()), this, SLOT(fakeSlot()));
    }
}


void
QtDeviceGroup::selectedModelIndex(const QModelIndex& index)
{
    Omm::Device* pDevice = static_cast<Omm::Device*>(index.internalPointer());
//    static_cast<QtMediaServer*>(pDevice)->createServerModel();
    DeviceGroup::selectDevice(pDevice);
}