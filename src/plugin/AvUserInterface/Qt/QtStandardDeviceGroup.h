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

#ifndef QtStandardDeviceGroup_INCLUDED
#define QtStandardDeviceGroup_INCLUDED

#include <QtGui>

#include <Omm/Upnp.h>

#include "QtNavigable.h"
#include "QtDeviceGroup.h"

class QtNavigator;
class QtDeviceListItem;


class QtStandardDeviceGroup : public QtDeviceGroup, public QtNavigable
{
    Q_OBJECT
        
public:
    QtStandardDeviceGroup(Omm::DeviceGroupDelegate* pDeviceGroupDelegate, QStyledItemDelegate* pItemDelegate = 0);
    ~QtStandardDeviceGroup();

    // QtDeviceGroup interface
    virtual QWidget* getDeviceGroupWidget();

    // QtNavigable interface
    virtual QWidget* getWidget();
    virtual QString getBrowserTitle();

private slots:
    void selectedModelIndex(const QModelIndex& index);

protected:
    QtNavigator*                    _pNavigator;

private:
    void initGui();

    QListView*                      _pDeviceListView;
    QStyledItemDelegate*            _pItemDelegate;
};


#endif

