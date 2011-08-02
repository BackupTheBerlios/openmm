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
#include "QtWidgetList.h"



QtMediaRendererGroup::QtMediaRendererGroup() :
QtDeviceGroup(new Omm::Av::MediaRendererGroupDelegate)
{
    _pWidgetList = new QtWidgetList;
    _pWidgetList->setModel(this);
}


Omm::Device*
QtMediaRendererGroup::createDevice()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group create media renderer");
    return new QtMediaRenderer;
}


void
QtMediaRendererGroup::addDevice(Omm::Device* pDevice, int position, bool begin)
{
    if (begin) {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group adds device at position: " + Poco::NumberFormatter::format(position));
    }
    else {
        QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(pDevice);
        Omm::Av::Log::instance()->upnpav().debug("get device: " + Poco::NumberFormatter::format(pDevice) + ", friendly name: " + pDevice->getFriendlyName());
        Omm::Av::Log::instance()->upnpav().debug("get renderer: " + Poco::NumberFormatter::format(pRenderer) + ", friendly name: " + pRenderer->getFriendlyName());
//        emit pRenderer->getDeviceWidget()->showWidget();
        insertWidget(position);
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group finished adding device.");
    }
}


void
QtMediaRendererGroup::removeDevice(Omm::Device* pDevice, int position, bool begin)
{
    if (begin) {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group removes device at position: " + Poco::NumberFormatter::format(position));
        QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(pDevice);
        Omm::Av::Log::instance()->upnpav().debug("get device: " + Poco::NumberFormatter::format(pDevice) + ", friendly name: " + pDevice->getFriendlyName());
        Omm::Av::Log::instance()->upnpav().debug("get renderer: " + Poco::NumberFormatter::format(pRenderer) + ", friendly name: " + pRenderer->getFriendlyName());
//        emit pRenderer->getDeviceWidget()->hideWidget();
        removeWidget(position);
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group finished removing device.");
    }
}


QWidget*
QtMediaRendererGroup::getDeviceGroupWidget()
{
    return _pWidgetList;
}


//QWidget*
//QtMediaRendererGroup::createWidget(const QModelIndex& parent)
//{
//    return new QtMediaRendererWidget;
//}
//
//
//QWidget*
//QtMediaRendererGroup::getWidget(const QModelIndex& index)
//{
//    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(index.internalPointer());
//    return pRenderer->getDeviceWidget();
//}
//
//
//void
//QtMediaRendererGroup::attachWidget(const QModelIndex& index, QWidget* pWidget)
//{
//    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget");
//    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(index.internalPointer());
//    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget: " + pRenderer->getFriendlyName());
//    QtMediaRendererWidget* pRendererWidget = static_cast<QtMediaRendererWidget*>(pWidget);
//    pRendererWidget->_pMediaRenderer = pRenderer;
//    pRenderer->setDeviceWidget(pRendererWidget);
//    connect(pRendererWidget, SIGNAL(showWidget()), pRendererWidget, SLOT(show()));
//    connect(pRendererWidget, SIGNAL(hideWidget()), pRendererWidget, SLOT(hide()));
//}
//
//
//void
//QtMediaRendererGroup::detachWidget(const QModelIndex& index)
//{
//    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(index.internalPointer());
//    QtMediaRendererWidget* pRendererWidget = pRenderer->getDeviceWidget();
//    pRendererWidget->_pMediaRenderer = 0;
//    pRenderer->setDeviceWidget(0);
//    disconnect(pRendererWidget, SIGNAL(showWidget()), pRendererWidget, SLOT(show()));
//    disconnect(pRendererWidget, SIGNAL(hideWidget()), pRendererWidget, SLOT(hide()));
//}


QWidget*
QtMediaRendererGroup::createWidget()
{
    return new QtMediaRendererWidget;
}


QWidget*
QtMediaRendererGroup::getWidget(int row)
{
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    return pRenderer->getDeviceWidget();
}


void
QtMediaRendererGroup::attachWidget(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget");
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget: " + pRenderer->getFriendlyName());
    QtMediaRendererWidget* pRendererWidget = static_cast<QtMediaRendererWidget*>(pWidget);
    pRendererWidget->_pMediaRenderer = pRenderer;
    pRenderer->setDeviceWidget(pRendererWidget);

//    pRendererWidget->setRendererName(pRenderer->getFriendlyName());
    
    connect(pRendererWidget, SIGNAL(showWidget()), pRendererWidget, SLOT(show()));
    connect(pRendererWidget, SIGNAL(hideWidget()), pRendererWidget, SLOT(hide()));
    connect(pRendererWidget, SIGNAL(configureWidget()), pRendererWidget, SLOT(configure()));

    emit pRendererWidget->configureWidget();
    emit pRendererWidget->showWidget();
}


void
QtMediaRendererGroup::detachWidget(int row)
{
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    QtMediaRendererWidget* pRendererWidget = pRenderer->getDeviceWidget();

    emit pRendererWidget->hideWidget();

    disconnect(pRendererWidget, SIGNAL(showWidget()), pRendererWidget, SLOT(show()));
    disconnect(pRendererWidget, SIGNAL(hideWidget()), pRendererWidget, SLOT(hide()));
    disconnect(pRendererWidget, SIGNAL(configureWidget()), pRendererWidget, SLOT(configure()));

    pRendererWidget->_pMediaRenderer = 0;
    pRenderer->setDeviceWidget(0);
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
