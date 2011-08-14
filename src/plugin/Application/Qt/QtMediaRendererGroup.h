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

#include <QtGui>

#include <Omm/UpnpGui.h>

class QtMediaRenderer;
class QtWidgetList;
class QtWidgetCanvas;
class QtMediaRendererControlPanel;

class QtMediaRendererGroup : public QObject, public Omm::Gui::DeviceGroupModel
{
    Q_OBJECT

public:
    QtMediaRendererGroup();
 
    // WidgetListModel interface
    virtual Omm::Gui::ListWidget* createWidget();
    virtual Omm::Gui::ListWidget* getChildWidget(int row);
    virtual void attachWidget(int row, Omm::Gui::ListWidget* pWidget);
    virtual void detachWidget(int row);

    // Omm::DeviceGroup interface
    virtual Omm::Device* createDevice();
    virtual void show();

    // Gui::DeviceGroupModel interface
//    virtual Omm::Gui::Widget* getDeviceGroupWidget();

private slots:
    void playButtonPressed();
    void stopButtonPressed();
    void volumeSliderMoved(int value);
    void positionSliderMoved(int value);

private:
    virtual void init();

    QtWidgetList*                   _pWidgetList;
    QtWidgetCanvas*                 _pWidgetCanvas;
    QtMediaRendererControlPanel*    _pControlPanel;
};


#endif

