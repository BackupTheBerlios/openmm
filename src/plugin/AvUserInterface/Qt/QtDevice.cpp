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

#include "QtDevice.h"
#include "QtMediaServerModel.h"


QtMediaServer::QtMediaServer()
{
}


QtMediaServer::~QtMediaServer()
{

}


void
QtMediaServer::initController()
{
    browseRootObject();
    _pMediaServerModel = new QtMediaServerModel(getRootObject());
}


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