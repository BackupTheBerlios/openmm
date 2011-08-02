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

#ifndef QtMediaRendererGroup_INCLUDED
#define QtMediaRendererGroup_INCLUDED

#include "QtDeviceGroup.h"

class QtMediaRenderer;
class QtWidgetList;
class QtMediaRendererControlPanel;

class QtMediaRendererGroup : public QtDeviceGroup
{
    Q_OBJECT

public:
    QtMediaRendererGroup();

    // Omm::DeviceGroup interface
    virtual Omm::Device* createDevice();
    virtual void addDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void removeDevice(Omm::Device* pDevice, int index, bool begin);

    // QtDeviceGroup interface
    virtual QWidget* getDeviceGroupWidget();

    // QtWidgetListModel interface
//    virtual QWidget* createWidget(const QModelIndex& parent = QModelIndex());
//    virtual QWidget* getWidget(const QModelIndex& index);
//    virtual void attachWidget(const QModelIndex& index, QWidget* pWidget);
//    virtual void detachWidget(const QModelIndex& index);

    // QtWidgetListModel interface
    virtual QWidget* createWidget();
    virtual QWidget* getWidget(int row);
    virtual void attachWidget(int row, QWidget* pWidget);
    virtual void detachWidget(int row);

private slots:
    void playButtonPressed();
    void stopButtonPressed();
    void volumeSliderMoved(int value);
    void positionSliderMoved(int value);
    void selectedRenderer(int row);

private:
    virtual void init();

    QtWidgetList*                   _pWidgetList;
    QtMediaRendererControlPanel*    _pControlPanel;
};


#endif

