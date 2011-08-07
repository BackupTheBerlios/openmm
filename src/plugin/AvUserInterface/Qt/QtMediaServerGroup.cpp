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

#include "QtMediaServerGroup.h"
#include "QtMediaServer.h"
#include "QtMediaObject.h"
#include "QtNavigator.h"
#include "QtWidget.h"
#include "QtWidgetList.h"


QtMediaServerGroup::QtMediaServerGroup() :
DeviceGroup(new Omm::Av::MediaServerGroupDelegate)
{
    _pWidgetCanvas = new QtWidgetCanvas(true);
    _pWidgetCanvas->setModel(this);

    _pNavigator = new QtNavigator;
    // push this Navigable on the Navigator, the actual widget pushed is returned by getWidget().
    _pNavigator->push(this);
}


QWidget*
QtMediaServerGroup::getWidget()
{
    return _pWidgetCanvas;
}


QString
QtMediaServerGroup::getBrowserTitle()
{
    return ">";
}


Omm::Util::Widget*
QtMediaServerGroup::getDeviceGroupWidget()
{
    return _pNavigator;
}


Omm::Device*
QtMediaServerGroup::createDevice()
{
    return new QtMediaServer;
}


void
QtMediaServerGroup::selectDevice(Omm::Device* pDevice, int index)
{
    QtMediaServer* pMediaServer = static_cast<QtMediaServer*>(pDevice);

    Omm::Av::Log::instance()->upnpav().debug("Qt media server group select device");
    Omm::Av::CtlMediaObject* pRootObject = pMediaServer->getRootObject();
    if (pRootObject->isContainer()) {
        pRootObject->fetchChildren();
        Omm::Av::Log::instance()->upnpav().debug("Qt media server root object is container, creating container widget.");
        QtMediaObject* pRootWidget = new QtMediaObject;
        pRootWidget->_pObject = pRootObject;
        pRootObject->setWidgetFactory(pRootWidget);
        pRootWidget->_pContainerView = new QtWidgetList;
        Omm::Av::Log::instance()->upnpav().debug("Qt media server pushing root container widget ...");
        _pNavigator->push(pRootWidget);
        pRootWidget->_pContainerView->setModel(pRootObject);
    }
}


Omm::Util::Widget*
QtMediaServerGroup::createWidget()
{
    return new QtMediaServerWidget;
}


Omm::Util::Widget*
QtMediaServerGroup::getWidget(int row)
{
    QtMediaServer* pServer = static_cast<QtMediaServer*>(getDevice(row));
    return pServer->getDeviceWidget();
}


void
QtMediaServerGroup::attachWidget(int row, Omm::Util::Widget* pWidget)
{
//    Omm::Av::Log::instance()->upnpav().debug("media server group attach widget");
    QtMediaServer* pServer = static_cast<QtMediaServer*>(getDevice(row));
    Omm::Av::Log::instance()->upnpav().debug("media server group attach widget row: " + Poco::NumberFormatter::format(row) + ", name: " + pServer->getFriendlyName());
    QtMediaServerWidget* pListWidget = static_cast<QtMediaServerWidget*>(pWidget);
    pListWidget->_pMediaServer = pServer;
    pServer->setDeviceWidget(pListWidget);

    connect(pListWidget, SIGNAL(showWidgetSignal()), pListWidget, SLOT(show()));
    connect(pListWidget, SIGNAL(hideWidgetSignal()), pListWidget, SLOT(hide()));
    connect(pListWidget, SIGNAL(configureWidget()), pListWidget, SLOT(configure()));
    connect(pListWidget, SIGNAL(unconfigureWidget()), pListWidget, SLOT(unconfigure()));

    emit pListWidget->configureWidget();
    emit pListWidget->showWidgetSignal();
    Omm::Av::Log::instance()->upnpav().debug("media server group attach widget finished.");
}


void
QtMediaServerGroup::detachWidget(int row)
{
    QtMediaServer* pServer = static_cast<QtMediaServer*>(getDevice(row));
    Omm::Av::Log::instance()->upnpav().debug("media server group detach widget row: " + Poco::NumberFormatter::format(row) + ", name: " + pServer->getFriendlyName());
    QtMediaServerWidget* pListWidget = pServer->getDeviceWidget();

    emit pListWidget->hideWidgetSignal();
    emit pListWidget->unconfigureWidget();

    disconnect(pListWidget, SIGNAL(showWidgetSignal()), pListWidget, SLOT(show()));
    disconnect(pListWidget, SIGNAL(hideWidgetSignal()), pListWidget, SLOT(hide()));
    disconnect(pListWidget, SIGNAL(configureWidget()), pListWidget, SLOT(configure()));
    disconnect(pListWidget, SIGNAL(unconfigureWidget()), pListWidget, SLOT(unconfigure()));

    pListWidget->_pMediaServer = 0;
    pServer->setDeviceWidget(0);
    Omm::Av::Log::instance()->upnpav().debug("media server group detach widget finished.");
}
