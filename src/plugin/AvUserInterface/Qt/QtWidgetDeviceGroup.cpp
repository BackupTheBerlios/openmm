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

#include "QtWidgetDeviceGroup.h"


QtWidgetDeviceGroup::QtWidgetDeviceGroup(Omm::DeviceGroupDelegate* pDeviceGroupDelegate) :
QtDeviceGroup(pDeviceGroupDelegate),
_lastWidget(0)
{
//    _pScrollArea = new QScrollArea;
    _pGraphicsScene = new QGraphicsScene;
    _pGrahpicsView = new QGraphicsView(_pGraphicsScene);
}


QtWidgetDeviceGroup::~QtWidgetDeviceGroup()
{
}


QWidget*
QtWidgetDeviceGroup::getDeviceGroupWidget()
{
    return _pGrahpicsView;
}


void
QtWidgetDeviceGroup::addWidget(QWidget* pWidget)
{
    _widgetPool.push_back(pWidget);
    _pGraphicsScene->addWidget(pWidget);
    pWidget->hide();
//    pWidget->show();
}


QWidget*
QtWidgetDeviceGroup::getWidget()
{
    Omm::Av::Log::instance()->upnpav().debug("get device widget[" + Poco::NumberFormatter::format(_lastWidget) + "]: " + Poco::NumberFormatter::format(_widgetPool[_lastWidget]));

    QWidget* pRes = _widgetPool[_lastWidget];
    _lastWidget++;

    return pRes;
}


void
QtWidgetDeviceGroup::showWidget(QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt widget device group show device widget");

    pWidget->show();
//    _pGraphicsScene->addWidget(pWidget);
}
