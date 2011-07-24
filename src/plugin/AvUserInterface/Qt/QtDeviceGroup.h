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
    QtDeviceGroup(const std::string& deviceType, const std::string& shortName);
    ~QtDeviceGroup();

    QWidget* getDeviceGroupWidget();
    
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual Omm::Device* createDevice();
    virtual void addDevice(Omm::Device* pDevice, int index, bool begin);
    virtual void removeDevice(Omm::Device* pDevice, int index, bool begin);
    
    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();

    virtual void selectDevice(Omm::Device* pDevice, int index);

private slots:
    void selectedModelIndex(const QModelIndex& index);

signals:
    void setCurrentIndex(const QModelIndex& index);

//protected:
//    QtNavigator* getNavigator();

private:
    QVBoxLayout*                    _pLayout;
    QWidget*                        _pDeviceGroupWidget;
    QtNavigator*                    _pNavigator;
    QTreeView*                      _pDeviceListView;
    QtDeviceListItem*               _pDeviceListItem;

    QTextCodec*                     _charEncoding;
    QFileIconProvider*              _iconProvider;
};


class QtMediaServerGroup : public QtDeviceGroup
{

};


#endif

