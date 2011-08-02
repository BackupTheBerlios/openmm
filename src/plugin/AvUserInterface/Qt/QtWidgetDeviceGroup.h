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

#ifndef QtWidgetDeviceGroup_INCLUDED
#define QtWidgetDeviceGroup_INCLUDED

#include <QtGui>

#include <Omm/Upnp.h>

#include "QtDeviceGroup.h"


class QtWidgetDeviceGroup : public QtDeviceGroup
{
//    Q_OBJECT
        
public:
    QtWidgetDeviceGroup(Omm::DeviceGroupDelegate* pDeviceGroupDelegate);
    ~QtWidgetDeviceGroup();

    void addWidget(QWidget* pWidget);
    QWidget* getWidget();

    void showWidget(QWidget* pWidget);

    // QtDeviceGroup interface
    virtual QWidget* getDeviceGroupWidget();

protected:
    std::vector<QWidget*>           _widgetPool;
    int                             _lastWidget;

private:
//    QScrollArea*                    _pScrollArea;
    QGraphicsScene*                 _pGraphicsScene;
    QGraphicsView*                  _pGrahpicsView;
};


#endif

