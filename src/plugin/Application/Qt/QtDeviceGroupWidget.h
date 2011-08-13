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

#ifndef QtDeviceGroupWidget_INCLUDED
#define QtDeviceGroupWidget_INCLUDED

#include <QtGui>
#include <Omm/Upnp.h>

#include "QtNavigable.h"

class QtStandardDeviceGroup;
class QtDeviceListItem;


class QtDeviceGroupWidget : public QWidget , public QtNavigable
{
    Q_OBJECT

public:
    QtDeviceGroupWidget(QtStandardDeviceGroup* pDeviceGroupModel);

    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();

    virtual void selectDevice(Omm::Device* pDevice, int index);

private slots:
    void selectedModelIndex(const QModelIndex& index);

private:
    QVBoxLayout*                    _pLayout;
            
    QtStandardDeviceGroup*                  _pDeviceGroupModel;
    QTreeView*                      _pDeviceListView;
    QtDeviceListItem*               _pDeviceListItem;
};

#endif

