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

#ifndef QtDeviceGroup_INCLUDED
#define QtDeviceGroup_INCLUDED

#include <QtGui>
#include <Omm/Upnp.h>

class QtDeviceGroupModel;
class QtCrumbButton;
class QtCrumbPanel;
class QtBrowserModel;
class QtDeviceListItem;

class QtDeviceGroup : public QWidget, public Omm::DeviceGroupDelegate
{
    Q_OBJECT
public:
    QtDeviceGroup();

    virtual void addDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void removeDevice(Omm::Device* pDevice, int index, bool begin);

private slots:
    void selectedDevice(const QModelIndex& index);

private:
    QtDeviceGroupModel*             _pDeviceGroupModel;
    QTreeView*                      _pDeviceListView;
    QtDeviceListItem*               _pDeviceListItem;
    QVBoxLayout*                    _pLayout;
    QtCrumbPanel*                   _pCrumbPanel;
    QtCrumbButton*                  _pCrumbButton;
};

#endif

