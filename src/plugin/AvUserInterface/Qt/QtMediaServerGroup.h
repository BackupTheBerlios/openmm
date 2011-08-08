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

#ifndef QtMediaServerGroup_INCLUDED
#define QtMediaServerGroup_INCLUDED

#include <QtGui>
#include <Omm/Upnp.h>
#include "QtNavigable.h"

class QtNavigator;
class QtWidgetCanvas;


class QtMediaServerGroup : public QObject, public QtNavigable, public Omm::DeviceGroup
{
public:
    QtMediaServerGroup();

    // QtNavigable interface
    virtual QWidget* getWidget();
    virtual QString getBrowserTitle();

    // Omm::DeviceGroup interface
    virtual Omm::Util::Widget* getDeviceGroupWidget();
    virtual Omm::Device* createDevice();
    virtual void selectDevice(Omm::Device* pDevice, int index);

    // WidgetListModel interface
    virtual Omm::Util::ListWidget* createWidget();
    // FIXME: getWidget() can move into Omm::DeviceGroup (introduce Omm::Device::getDeviceWidget()).
    virtual Omm::Util::ListWidget* getWidget(int row);
    virtual void attachWidget(int row, Omm::Util::ListWidget* pWidget);
    virtual void detachWidget(int row);

private:
    QtNavigator*                    _pNavigator;
    QtWidgetCanvas*                 _pWidgetCanvas;
};


#endif

