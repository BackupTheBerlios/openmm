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
#include "QtNavigable.h"

class QtNavigator;
class QtDeviceListItem;


class QtDeviceGroup : public QAbstractItemModel, public QtNavigable, public Omm::DeviceGroup
{
    Q_OBJECT

public:
    QtDeviceGroup(Omm::DeviceGroupDelegate* pDeviceGroupDelegate, QStyledItemDelegate* pItemDelegate = 0);
    ~QtDeviceGroup();

    // QtNavigable interface
    virtual QWidget* getWidget();
    virtual QString getBrowserTitle();

   // QAbstractItemModel interface
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void showDeviceGroup();

    virtual void addDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void removeDevice(Omm::Device* pDevice, int index, bool begin);

private slots:
    void selectedModelIndex(const QModelIndex& index);

protected:
    QtNavigator*                    _pNavigator;

private:
    void initGui();

    QListView*                      _pDeviceListView;
    QStyledItemDelegate*            _pItemDelegate;

    QTextCodec*                     _pCharEncoding;
    QFileIconProvider*              _pIconProvider;
};


#endif

