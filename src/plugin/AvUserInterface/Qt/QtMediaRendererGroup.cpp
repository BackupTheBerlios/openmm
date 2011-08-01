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

#include "QtMediaRendererGroup.h"
#include "QtMediaRendererControlPanel.h"
#include "QtMediaRenderer.h"
#include "QtController.h"



class QtMediaRendererItem : public QStyledItemDelegate
{
public:
    QtMediaRendererItem(QObject* parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


QtMediaRendererItem::QtMediaRendererItem(QObject* parent) :
QStyledItemDelegate(parent)
{
}


void
QtMediaRendererItem::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int padding = 4;
    QRect iconRect(option.rect);
    iconRect.setLeft(padding);
    iconRect.setWidth(option.rect.height());
    QRect textRect(option.rect);
    textRect.setLeft(iconRect.right() + padding);


    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(index.internalPointer());
    painter->save();
    Omm::Av::Log::instance()->upnpav().debug("media renderer widget: " + Poco::NumberFormatter::format(pRenderer->getDeviceWidget()));
//    pRenderer->getWidget()->render(painter);
    painter->restore();


//    Omm::Av::Log::instance()->upnpav().debug(
//        + "list item painter, font size points: " + Poco::NumberFormatter::format(option.font.pointSize())
//        + ", font size pixels: " + Poco::NumberFormatter::format(option.font.pixelSize())
//    );

//    if (qVariantCanConvert<QString>(index.data(Qt::DisplayRole))) {
//        QString title = qvariant_cast<QString>(index.data(Qt::DisplayRole));
//        painter->save();
////        painter->setRenderHint(QPainter::Antialiasing, true);
//        if (option.state & QStyle::State_Selected) {
//            painter->fillRect(option.rect, option.palette.highlight());
//            painter->setPen(option.palette.highlightedText().color());
//        }
////        painter->setFont(option.font);  // painter is style aware, don't need to set this.
//        painter->drawText(textRect, Qt::AlignVCenter, title);
//        painter->restore();
//    }
//    if (qVariantCanConvert<QIcon>(index.data(Qt::DecorationRole))) {
////        Omm::Av::Log::instance()->upnpav().debug("DRAWING ICON");
//        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
//        painter->save();
//        icon.paint(painter, iconRect);
//        painter->restore();
//    }
    // use QApplication::style()->drawControl() to draw Qt widgets on the painter

//    else {
//        // default painter
//        QStyledItemDelegate::paint(painter, option, index);
//    }
}


QSize
QtMediaRendererItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize itemSize = QStyledItemDelegate::sizeHint(option, index);
    itemSize.rheight() *= 2;
    return itemSize;
}


QtMediaRendererGroup::QtMediaRendererGroup() :
//QtStandardDeviceGroup(new Omm::Av::MediaRendererGroupDelegate, new QtMediaRendererItem),
QtWidgetDeviceGroup(new Omm::Av::MediaRendererGroupDelegate)
{
    for (int i = 0; i < 10; ++i) {
        QtMediaRendererWidget* pRendererWidget = new QtMediaRendererWidget;
        pRendererWidget->setText(QString::number(i));
//        pRendererWidget->setText(QString(10));
        Omm::Av::Log::instance()->upnpav().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pRendererWidget));
        addWidget(pRendererWidget);
    }
}


Omm::Device*
QtMediaRendererGroup::createDevice()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group create media renderer");

//    QtMediaRenderer* pRes = new QtMediaRenderer;
//    pRes->moveToThread(QApplication::instance()->thread());
    
    QtMediaRendererWidget* pWidget = static_cast<QtMediaRendererWidget*>(getWidget());
    QtMediaRenderer* pRes = new QtMediaRenderer;
    pWidget->_pMediaRenderer = pRes;
    pRes->setDeviceWidget(pWidget);
    connect(pWidget, SIGNAL(showWidget()), pWidget, SLOT(show()));
    connect(pWidget, SIGNAL(hideWidget()), pWidget, SLOT(hide()));
    return pRes;
}


void
QtMediaRendererGroup::addDevice(Omm::Device* pDevice, int position, bool begin)
{
    if (begin) {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group adds device at position:" + Poco::NumberFormatter::format(position));
    }
    else {
        QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(pDevice);
        Omm::Av::Log::instance()->upnpav().debug("get device: " + Poco::NumberFormatter::format(pDevice) + ", friendly name: " + pDevice->getFriendlyName());
        Omm::Av::Log::instance()->upnpav().debug("get renderer: " + Poco::NumberFormatter::format(pRenderer) + ", friendly name: " + pRenderer->getFriendlyName());
        emit pRenderer->getDeviceWidget()->showWidget();
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group finished adding device.");
    }
}


void
QtMediaRendererGroup::removeDevice(Omm::Device* pDevice, int position, bool begin)
{
    if (begin) {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group removes device at position:" + Poco::NumberFormatter::format(position));
        QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(pDevice);
        Omm::Av::Log::instance()->upnpav().debug("get device: " + Poco::NumberFormatter::format(pDevice) + ", friendly name: " + pDevice->getFriendlyName());
        Omm::Av::Log::instance()->upnpav().debug("get renderer: " + Poco::NumberFormatter::format(pRenderer) + ", friendly name: " + pRenderer->getFriendlyName());
        pRenderer->getDeviceWidget();
//        emit pRenderer->getDeviceWidget()->hideWidget();
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group finished removing device.");
    }
}


void
QtMediaRendererGroup::playButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->playPressed();
    }
}


void
QtMediaRendererGroup::stopButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->stopPressed();
    }
}


void
QtMediaRendererGroup::volumeSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->volumeChanged(value);
    }
}


void
QtMediaRendererGroup::positionSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->positionMoved(value);
    }
}


void
QtMediaRendererGroup::init()
{
    _pControlPanel = new QtMediaRendererControlPanel;
    static_cast<QtController*>(getController())->addPanel(_pControlPanel);

    connect(_pControlPanel, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(_pControlPanel, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(_pControlPanel, SIGNAL(volumeSliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_pControlPanel, SIGNAL(positionSliderMoved(int)), this, SLOT(positionSliderMoved(int)));
}
